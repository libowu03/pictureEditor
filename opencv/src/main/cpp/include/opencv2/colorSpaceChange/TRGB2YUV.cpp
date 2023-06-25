
#include"TRGB2YUV.h"
#include"opencv2/utils/Commen.h"
#include<stdlib.h>
#include<math.h>

void RGB2YUV(unsigned char R, unsigned char G, unsigned char B, int *Y, int* U, int*V)
{
	*Y = (299 * R + 587 * G + 114 * B) / 1000;
	*U = (-147 * R - 289 * G + 436 * B) / 1000;
	*V = (615 * R - 515 * G - 100 * B) / 1000;
};
void YUV2RGB(int Y, int U, int V, unsigned char * R, unsigned char * G, unsigned char *B)
{
	*R = CLIP3((100 * Y + 114 * V) / 100, 0, 255);
	*G = CLIP3((1000 * Y - 395 * U - 581 * V) / 1000, 0, 255);
	*B = CLIP3((1000 * Y + 2032 * U) / 1000, 0, 255);
}; 