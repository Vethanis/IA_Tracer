#ifndef SSBO_H
#define SSBO_H

class SSBO{
	unsigned id;
public:
	SSBO(void* ptr, size_t bytes, unsigned binding=0);
	~SSBO();
	void upload(void* src, size_t bytes);
	void download(void* dest, size_t bytes);
};

#endif
