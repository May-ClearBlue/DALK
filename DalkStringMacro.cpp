#include "MultiStdafx.h"

#ifdef _DALK_PS3_
#pragma comment(lib,"target/ppu/lib/libadec_stub.a")
#pragma comment(lib,"target/ppu/lib/libaudio_stub.a")
#pragma comment(lib,"target/ppu/lib/libcgb.a")
#pragma comment(lib,"target/ppu/lib/libdbgfont_gcm.a")
#pragma comment(lib,"target/ppu/lib/libgcm_cmd.a")
#pragma comment(lib,"target/ppu/lib/libgcm_sys_stub.a")
#pragma comment(lib,"target/ppu/lib/libl10n_stub.a")
#pragma comment(lib,"target/ppu/lib/libmixer.a")
#pragma comment(lib,"target/ppu/lib/libmstreamSPURSAT3.a")
#pragma comment(lib,"target/ppu/lib/libperf.a")
#pragma comment(lib,"target/ppu/lib/libpngdec_stub.a")
#pragma comment(lib,"target/ppu/lib/libresc_stub.a")
#pragma comment(lib,"target/ppu/lib/librtc_stub.a")
#pragma comment(lib,"target/ppu/lib/libspurs_jq_stub.a")
#pragma comment(lib,"target/ppu/lib/libspurs_stub.a")
#pragma comment(lib,"target/ppu/lib/libsync_stub.a")
#pragma comment(lib,"target/ppu/lib/libsysmodule_stub.a")
#pragma comment(lib,"target/ppu/lib/libsysutil_np_trophy_stub.a")
#pragma comment(lib,"target/ppu/lib/libsysutil_stub.a")
#pragma comment(lib,"target/ppu/lib/libusbd_stub.a")
#endif

#include "DalkStringMacro.h"

using namespace DalkString;

#if 0
char* ToUpper(char *str){
	for(int a = 0;*(str + a) != '\0'; a++)
		*(str + a) = toupper( *(str + a) );        /* p の指す中身を大文字に変換 */

	return (str);                  /* 文字列の先頭アドレスを返す */
}

char* GetChar(char* src, char* dest) {
	memset(dest,0,3);

	dest[0] = *src;

	if( IsSJIS( *(src++) ) )
		dest[1] = *(src++);

	return src;
}

bool IncChar(char*& str){
	if(IsSJIS(*str)) {
		str+=2;
		return false;
	}
	
	str ++;
	return true;
}

//文字検索単品。srcの先頭一文字がdestに存在しない場合false、する場合はtrue
bool  FindChar(char* dest, char* src, char* buf) {
	char t[3] = {0};

	if(IsSJIS(*src)){
		strncpy(t,src,2);

		if(buf != NULL)
			strncpy(buf,src,2);
	}
	else{
		t[0] = *src;

		if(buf != NULL)
			*buf = *src;
	}

	std::string g = dest;
	if( FindString(g, t) == -1)//g.find(t) == string::npos )
		return false;
	return true;
}

int FindString(std::string& s, char* src) {
	int r = 0;
	char* t = (char*)s.data();
	while(*t != '\0') {
		if(*t != *src)
			r += DalkString::IncChar(t) ? 1 : 2;
		else if(!IsSJIS(*t))
			return r;
		else if(IsSJIS(*src) && t[1] == src[1] )
			return r;
		else
			r+= DalkString::IncChar(t) ? 1 : 2;
	}
	return -1;
}

//itoa string仕様
void itoa(int num,std::string& str,int radix){
	char numstr[16] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
#ifdef _WIN32
	_itoa_s(num,numstr,radix);
#else
	sprintf(numstr,"%d",num);
#endif
	str += numstr;
}
#endif

//一文字を１６進数に変換
int ToHex(char code) {
	switch ( code ) {
		case '0':
			return 0;
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
	}
	return -1;
}
