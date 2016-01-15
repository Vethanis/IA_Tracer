#include "myglheaders.h"
#include "texture.h"
#include "debugmacro.h"


Texture::Texture(unsigned w, unsigned h, TEXTURETYPE t){
	width = w; height = h; type = t;
	
	glGenTextures(1, &tex_id);	MYGLERRORMACRO
	glBindTexture(GL_TEXTURE_2D, tex_id);	MYGLERRORMACRO
	
	if(type == COLOR){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	MYGLERRORMACRO
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO
		glGenerateMipmap(GL_TEXTURE_2D);	MYGLERRORMACRO
	}
	else{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	MYGLERRORMACRO
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	MYGLERRORMACRO
	
		switch(type){
		case COLOR:
			break;
		case UBYTE:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case UBYTE2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8UI, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case UBYTE3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8UI, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, width, height, 0, GL_RED, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8I, width, height, 0, GL_RG, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SBYTE3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8I, width, height, 0, GL_RGB, GL_BYTE, NULL);	MYGLERRORMACRO}
			break;
		case SINT:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case SINT2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32I, width, height, 0, GL_RG, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case SINT3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32I, width, height, 0, GL_RGB, GL_INT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT:{glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT2:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		case FLOAT3:{glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, NULL);	MYGLERRORMACRO}
			break;
		}
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);	MYGLERRORMACRO
}

Texture::~Texture(){
	glDeleteTextures(1, &tex_id);	MYGLERRORMACRO
}

void Texture::bind(unsigned channel){
	glActiveTexture(GL_TEXTURE0 + channel);MYGLERRORMACRO
	glBindTexture(GL_TEXTURE_2D, tex_id);MYGLERRORMACRO
}


void Texture::setCSBinding(unsigned binding){
	glBindImageTexture(0, tex_id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
}

