#include "myglheaders.h"
#include "SSBO.h"
#include <string.h>

SSBO::SSBO(void* ptr, size_t bytes, unsigned binding){
	glGenBuffers(1, &id);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, ptr, GL_DYNAMIC_COPY);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
SSBO::~SSBO(){
	glDeleteBuffers(1, &id);
}
void SSBO::upload(void* ptr, size_t bytes){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	GLvoid* gptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(gptr, ptr, bytes);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
void SSBO::download(void* ptr, size_t bytes){
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
	GLvoid* gptr = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	memcpy(ptr, gptr, bytes);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
