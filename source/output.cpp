#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "output.h"

ColorImage::ColorImage()
{
	pPixel = 0;
}

ColorImage::~ColorImage()
{
	if (pPixel) delete[] pPixel;
	pPixel = 0;
}

void ColorImage::init(int xSize, int ySize)
{
	Pixel p = {0,0,0};
	xRes = xSize;
	yRes = ySize;
	pPixel = new Pixel[xSize*ySize];
	clear(p);
}

void ColorImage::clear(Pixel background)
{
	int i;

	if (! pPixel) return;
	for (i=0; i<xRes*yRes; i++) pPixel[i] = background;
}

Pixel ColorImage::readPixel(int x, int y)
{
	assert(pPixel); // die if image not initialized
	return pPixel[x + y*yRes];
}

void ColorImage::writePixel(int x, int y, Pixel p)
{
	assert(pPixel); // die if image not initialized
	pPixel[x + y*xRes] = p;
}

void ColorImage::outputPPM(const char *filename)
{
    FILE *outFile = fopen(filename, "wb");

	assert(outFile); // die if file can't be opened

	fprintf(outFile, "P6 %d %d 255\n", xRes, yRes);
	fwrite(pPixel, 1, 3*xRes*yRes, outFile );

	fclose(outFile);
}
