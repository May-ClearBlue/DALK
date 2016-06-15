#ifndef _RIM_HEAP_
#define _RIM_HEAP_

#include <MultiStdafx.h>

#include <assert.h>

#ifndef _DALK_WINDOWS_
# include <stdlib.h>
#endif

//VirtualAllocは4k、HeapAllocは64byte aligned
#define _RIM_LARGE_MEMORYALIGNMENT_ 4096
#define _RIM_SMALL_MEMORYALIGNMENT_ 64

#define ALIGN(N1,N2)			(((N1)+((N2)-1))&(~((N2)-1)))
//#define ALIGN(x,a)						(((x)+((a)-1))/(a)*(a))

//malloc系のトレース用（デバッグ時のみ）
#if defined( _DALK_DEBUG_BUILD_) || defined(_DALK_TEST_)

typedef string _HEAP_LOGSTRING;

struct Heap_Info {
	_HEAP_LOGSTRING	_function;
	_HEAP_LOGSTRING	_type;
};

static map<void*, Heap_Info> g_HeapInfo;

inline BOOL AddHeapInfo(void* pAddress, const char* funcname, const char* type) {
	if( g_HeapInfo.find(pAddress) == g_HeapInfo.end() ) {
		Heap_Info hi = { funcname, type };
		g_HeapInfo[pAddress] = hi;
		return TRUE;
	}
	return FALSE;
}

inline BOOL RemoveHeapInfo(void* pAddress, const char* type) {
	if( g_HeapInfo.find(pAddress) == g_HeapInfo.end() )
		return FALSE;

	if(g_HeapInfo[pAddress]._type != type)
		return FALSE;

	g_HeapInfo.erase(pAddress);
	return TRUE;
}

inline void _DEBUG_HEAP_INFO() {
	map<void*, Heap_Info>::iterator it = g_HeapInfo.begin();

	_DEBUGPRINTF(_TEXT("_MALLOC_STATE_\n"));

	int a = 0;
	while( it != g_HeapInfo.end() ) {
#if defined(_DALK_WINDOWS_)
		_HEAP_LOGSTRING __log = it->second._function + " method=" + it->second._type;
		_DEBUGPRINTF(_TEXT("%d:%s"), a, __log.data());
#elif defined(_DALK_IOS_)
		_DEBUGPRINTF("at %s method=%s ", it->second._function.data(), it->second._type.data());
#endif
		a++;
		it++;
	}
}

#define _VALLOC(a,b)\
	{\
	a = RimHeap::VirtualAlloc(b, _RIM_LARGE_MEMORYALIGNMENT_);\
	AddHeapInfo(a, _FUNC_NAME, "VirtualAlloc" );\
	}
#define _HALLOC(a,b)\
	{\
	a = RimHeap::HeapAlloc(b);\
	AddHeapInfo(a, _FUNC_NAME, "HeapAlloc" );\
	}
#define _GFREE(x)\
	{\
	RemoveHeapInfo(x, "GlobalAlloc" );\
	RimHeap::GlobalFree(x);\
	}
#define _VFREE(x)\
	{\
	RemoveHeapInfo(x, "VirtualAlloc" );\
	RimHeap::VirtualFree(x);\
	}
#define _HFREE(x)\
	{\
	RemoveHeapInfo(x, "HeapAlloc");\
	RimHeap::HeapFree(x);\
	}
#else
#define _DEBUG_HEAP_INFO() 
#define _GALLOC(a)   (RimHeap::GlobalAlloc(a))
#define _VALLOC(a,b) (a = RimHeap::VirtualAlloc(b, _RIM_LARGE_MEMORYALIGNMENT_))
#define _HALLOC(a,b) (a = RimHeap::HeapAlloc(b))
#define _GFREE(x) RimHeap::GlobalFree(x)
#define _VFREE(x) RimHeap::VirtualFree(x)
#define _HFREE(x) RimHeap::HeapFree(x)
#endif

#define _MALLOC(a) _GALLOC(a)
#define _FREE(a)   _GFREE(a)

namespace RimHeap {

//VirtualAlloc
#if defined(_DALK_WINDOWS_)
inline void* GlobalAlloc(size_t size) {
	return (void*)::GlobalAlloc(GMEM_FIXED, size);
}

template <class P>inline BOOL GlobalFree( P*& pAddress ) {
	pAddress = (P*)::GlobalFree(pAddress);
	return pAddress == NULL ? TRUE : FALSE;
}

inline void* VirtualAlloc(size_t size, size_t alignment) {
	return ::VirtualAlloc( NULL , ALIGN(size , alignment), MEM_COMMIT,PAGE_EXECUTE_READWRITE);
}

template <class P>inline BOOL VirtualFree( P*& pAddress ) {
	if(::VirtualFree(pAddress, 0, MEM_RELEASE) != TRUE)
		return FALSE;

	pAddress = NULL;
	return TRUE;
}

inline void* HeapAlloc(size_t size){
	HANDLE handle = ::GetProcessHeap();
	void* res = ::HeapAlloc(handle, HEAP_ZERO_MEMORY, size);
	if(res == NULL)
	   ::OutputDebugString(_T("メモリの確保に失敗\n"));
	return res;
}

template <class P>inline BOOL HeapFree(P*& address) {
	if( address && ::HeapFree(::GetProcessHeap(), 0, address) )
		address = NULL;
	return address ? TRUE : FALSE;
}

#elif defined(_DALK_IOS_)

#include <mach/mach.h>

inline void* VirtualAlloc(size_t size, size_t alignment) {
	size_t*          pAddress;

//size_t分を足してallocate
	size += sizeof(size_t);
 
	kern_return_t  err = vm_allocate( mach_task_self(), (vm_address_t*) &pAddress, ALIGN(size , alignment) , VM_FLAGS_ANYWHERE);

	if(err != KERN_SUCCESS)
		return NULL;

//ヘッダにsizeを保存しておく
	*pAddress++ = size;	

//ヘッダの直後のアドレスを返す
	return pAddress;
}

template <class P>inline BOOL VirtualFree( P*& pAddress ) {
	if(pAddress == NULL)
		return TRUE;

	size_t* pSize = (size_t*)pAddress;
	pSize--;

	kern_return_t  err = vm_deallocate( mach_task_self(), (vm_address_t)pSize, *pSize);
	return err != KERN_SUCCESS ? FALSE : TRUE;
}

#elif defined(_DALK_ANDROID_)
inline void* VirtualAlloc(size_t size, size_t alignment) {
	void* pAddress = NULL;
	int r = posix_memalign (&pAddress, size, alignment);

	if(r != 0);
		return NULL;
	return  pAddress;
}

template <class P>inline BOOL VirtualFree( P*& pAddress ) {
	free(pAddress);
	pAddress = NULL;
	return TRUE;
}
#endif
	
#if defined(_DALK_IOS_) || defined(_DALK_ANDROID_)
inline void* GlobalAlloc(size_t size) {
	return malloc(size);
}

template <class P>inline BOOL GlobalFree( P*& pAddress ) {
	free(pAddress);
	pAddress = NULL;
}

inline void* HeapAlloc(size_t size) {
	void* pAddress = NULL;
	int r = posix_memalign(&pAddress, size, 64);

	if(r != 0)
		return NULL;
	return  pAddress;
}
template <class P>inline BOOL HeapFree( P* &pAddress ) {
	free(pAddress);
	pAddress = NULL;
	return TRUE;
}
#endif

    
#if defined(_DALK_PS3_) 
//EASTLをデバッグ、かつスタティックリンクする場合↓のnewのオーバーライドを実装する必要あり。たぶんデバッグ用

#if !EASTL_DLL
		// If building a regular library and not building EASTL as a DLL...
	    // It is expected that the application define the following
        // versions of operator new for the application. Either that or the
        // user needs to override the implementation of the allocator class.
inline void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line){
	return malloc(size);
}

inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
	return MemAlloc(size);
}
#endif
#endif

#if defined( _DALK_DEBUG_BUILD_) || defined(_DALK_TEST_)
inline void* _GALLOC(size_t a)
{
	void* temp = RimHeap::GlobalAlloc(a);
	if (temp != NULL)
		AddHeapInfo(temp, _FUNC_NAME, "GlobalAlloc");
	return temp;
}
#endif

};
#endif
