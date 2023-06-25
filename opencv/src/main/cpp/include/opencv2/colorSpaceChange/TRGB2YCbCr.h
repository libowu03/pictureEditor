/*************************************************
Copyright:  Copyright HZ.
Author:		Hu Yaowu
Date:		2015-04-21
Mail:       dongtingyueh@163.com
Description:RGB to YCbCr.
**************************************************/
#ifndef __RGB2YcbCr__
#define __RGB2YcbCr__

void RGBToYCbCr2(int R, int G, int B, int *Y, int *Cb, int *Cr);
void YCbCrToRGB2(int Y, int Cb, int Cr, int *R, int *G, int *B);
#endif