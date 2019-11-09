#pragma once
#include <GL/glew.h>
#include <cstdint>

#include "md5.h"

struct Color {
	uint8_t R, G, B;
};

class RenderImage {
private:
	GLuint textureID;
	int Width, Height;
	Color* imgData;

	bool changed = false;
public:
	RenderImage(int width, int height);
	~RenderImage();
	
	RenderImage(const RenderImage&) = delete;
	RenderImage& operator=(const RenderImage&) = delete;

	void Clear(Color col);
	void SetPixel(int x, int y, Color col);
	void Line(int x0, int y0, int x1, int y1, Color col);
	
	void BufferImage();

	md5 GetHashCode();
};
