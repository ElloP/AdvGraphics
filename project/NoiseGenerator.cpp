#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "FastNoise.h"
#include <iostream>
#include "NoiseGenerator.h"


#define WIDTH 256
#define HEIGHT 256
#define DEPTH 256

#define FILENAME "../project/clouds/cloudtexture.ex5"

/********************************************
* Generate a completely white 3D texture	*
*											*
********************************************/
int NoiseGenerator::generateNoiseTexture()
{
	FastNoise fn;
	fn.SetFrequency(0.1);

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

	float a = 50.0f;
	float b = 4.0f;

	for (int d = 0; d < DEPTH; d++)
	{
		for (int h = 0; h < HEIGHT; h++)
		{
			for (int w = 0; w < WIDTH; w++)
			{
				/* generate secret number between 1 and 10: */
				float n = (fn.GetPerlin(w, h, d) + 1.0f) / 2.0f;
				float cel = (fn.GetCellular(w, h, d) + 1.0f) / 2.0f;
				float brow = (fn.GetPerlinFractal(w, h, d) + 1.0f) / 2.0f;
				n = fmax((cel - brow),0);
				//n = cel;
				float he = h / 256.0f;
				n *= (1.0 - exp(-a * he)) * exp(-b * he);
			
				//n = 0.0f;
				
				float c = 255 * n;
				int r = c;
				int g = c;
				int b = c;
				int a = c;

			//	std::cout << n << "\n";

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
	fcloseall();

	return 1;
}