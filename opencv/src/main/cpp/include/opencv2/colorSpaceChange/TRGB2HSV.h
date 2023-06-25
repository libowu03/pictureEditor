/*************************************************************************
Copyright:   Trent.
Author:		 Trent1985
Date:		 2018-9-23
Mail:        dongtingyueh@163.com
Description: RGB2HSV
*************************************************************************/
#ifndef __RGB2HSI__
#define __RGB2HSI__

void RGBToHSV(unsigned char R, unsigned char G, unsigned char B, float *H, float *S, float *V);
void HSVToRGB(float H, float S, float V, unsigned char *R, unsigned char *G, unsigned char *B);
#endif