#ifndef TEXTURE_H
#define TEXTURE_H

enum TEXTURETYPE{
	COLOR,
	UBYTE,
	UBYTE2,
	UBYTE3,
	SBYTE,
	SBYTE2,
	SBYTE3,
	SINT,
	SINT2,
	SINT3,
	FLOAT,
	FLOAT2,
	FLOAT3
};

class Texture{
	unsigned tex_id, width, height;
	TEXTURETYPE type;
public:
	Texture(unsigned w, unsigned h, TEXTURETYPE t);
	~Texture();
	void bind(unsigned channel);
	void setCSBinding(unsigned binding);
	unsigned getID(){return tex_id;};
	unsigned getWidth(){return width;};
	unsigned getHeight(){return height;};
};

#endif
