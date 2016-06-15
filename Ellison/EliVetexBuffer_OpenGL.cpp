#include "Multistdafx.h"
#include <Siglune/SigDirect3D.h>
#include <EliVertexBuffer.h>

CVertexBuffer::CVertexBuffer() :
	m_Num(-1),
	m_Type(VX_INVALID),
	m_ObjectID(0)
{ ; }

BOOL CVertexBuffer::Create(DWORD num, VERTEX_TYPE type) {
	Release();

	GLenum res;

	glGenBuffers(1, &m_ObjectID);
	SIG_ERRORRETURNBOOL(res,"Failed@glGenBuffers");

	glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
	SIG_ERRORRETURNBOOL(res,"Failed@glBindBuffer");

	glBufferData(GL_ARRAY_BUFFER, num * VERTEX_SIZE[type], NULL, GL_STATIC_DRAW);
	SIG_ERRORRETURNBOOL(res,"Failed@glBufferData");

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_Type = type;

	return TRUE;
}

BOOL CVertexBuffer::SetData(void* pData, DWORD num) {
	GLenum res;

	glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
	SIG_ERRORRETURNBOOL(res,"Failed@glBindBuffer");

	glBufferSubData(GL_ARRAY_BUFFER, 0, num * VERTEX_SIZE[m_Type], pData);
	SIG_ERRORRETURNBOOL(res,"Failed@glBufferSubData");

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_Num = num;

	return TRUE;
}

void CVertexBuffer::Release() {
	if(m_ObjectID != 0)
		glDeleteBuffers(1,&m_ObjectID);

	GLenum hr;
	
	if(SUCCEEDED(hr))
		m_ObjectID = 0;
}

void CVertexBuffer::SetStream(int num) {
	GLenum res;

	glBindBuffer(GL_ARRAY_BUFFER, m_ObjectID);
	SIG_ERRORCHECK(res,"Failed@glBindBuffer");

	GLuint totalsize = 0;
	GLuint index = 0;

	for(ATTRIB_TYPE* a = _FIXED_VERTEX_FORMAT_[m_Type];*a != SH_ATR_TERMINATE ;a++) {
		glEnableVertexAttribArray(index);
		SIG_ERRORCHECK(res,"Failed@glEnableVertexAttribArray");
		// index, num, type, normalized, datalen, bufp
		glVertexAttribPointer(index, ShaderAttributeRef[*a].size_count, GL_FLOAT, GL_FALSE, VERTEX_SIZE[m_Type], (void*)totalsize);
		SIG_ERRORCHECK(res,"Failed@glVertexAttribPointer");
#ifdef _DALK_DEBUG_BUILD_
		string Name = ShaderAttributeRef[*a].name;
		_DEBUGPRINTF("index = %d count = %d total = %d offset = %d ", *a, ShaderAttributeRef[*a].size_count, VERTEX_SIZE[m_Type], totalsize);
#endif
		totalsize += ShaderAttributeRef[*a].size_count * sizeof(float);
		index++;
	}

}
