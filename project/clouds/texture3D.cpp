#include "texture3D.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>




GLuint load3DTexture(const char * file_path)
{
	int w, h, d;
	GLuint texture;
	FILE* fp;
	if (fopen_s(&fp, file_path, "r") != 0) {
		return -1;
	}

	/* Read header */
	fscanf(fp, "%d", &w);
	fscanf(fp, "%d", &h);
	fscanf(fp, "%d", &d);

	/* Create image */
	uint8_t* data = (uint8_t*)calloc(w * h * d * 4, sizeof *data);

	/* Read image */
	int i = 0;
	uint32_t pixel;
	while (fscanf(fp, "%d", &pixel) == 1)
	{
		data[i++] = pixel >> 24;
		data[i++] = pixel >> 16;
		data[i++] = pixel >> 8;
		data[i++] = pixel >> 0;
	}

	/* Load texture */
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_3D, texture);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, w, h, d, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	/* Free memory */
	free(data);

	return texture;
}
