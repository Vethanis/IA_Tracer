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
};

#endif
