#include <stdio.h>
#include <stdint.h>


#define WIDTH 128
#define HEIGHT 128
#define DEPTH 128

#define FILENAME "cloudtexture.ex5"

/********************************************
* Generate a completely white 3D texture	*
*											*
********************************************/
int main()
{
	FILE* fp;
	//fp = fopen(FILENAME, "w");
	/*
	if (fp == NULL)
	{
		return -1;
	}*/
	if (fopen_s(&fp, FILENAME, "w") != 0) {
		return -1;
	}

	fprintf(fp, "%d %d %d\n", WIDTH, HEIGHT, DEPTH);

	for (int d = 0; d < DEPTH; d++)
	{
		for (int h = 0; h < HEIGHT; h++)
		{
			for (int w = 0; w < WIDTH; w++)
			{
				int r = 255;
				int g = 255;
				int b = 255;
				int a = 255;

				uint32_t pixel = 0;

				pixel |= r << 24;
				pixel |= g << 16;
				pixel |= b << 8;
				pixel |= a << 0;

				fprintf(fp, "%u ", pixel);
			}
			fprintf(fp, "\n");
		}
	}
}