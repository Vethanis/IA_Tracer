#ifndef TEXTURE_H
#define TEXTURE_H

enum TEXTURETYPE{
	COLOR,
	DEPTH,
	MATERIAL
};

class Texture{
	unsigned tex_id, width, height;
	TEXTURETYPE type;
public:
	Texture(unsigned w, unsigned h, TEXTURETYPE t);
	~Texture();
	void bind(unsigned channel);
	unsigned getID(){return tex_id;};
	unsigned getWidth(){return width;};
	unsigned getHeight(){return height;};
};

#endif
