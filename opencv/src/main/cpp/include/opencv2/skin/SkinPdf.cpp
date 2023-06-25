#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SkinPdf.h"
#include "opencv2/colorSpaceChange/TRGB2YCbCr.h"

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CLIP3(x, a, b) MIN2(MAX2(a, x), b)

float Gaussian(float x, float mean, float var)
{
    float t = -0.5f * (x - mean) * (x - mean) / var;
    return exp(t);
}
float GetPDF(int R, int G, int B, float meanCb, float varCb, float meanCr, float varCr)
{
    int Y, Cb, Cr;
    RGBToYCbCr2(R, G, B, &Y, &Cb, &Cr);
    float pcb = Gaussian(Cb, meanCb, varCb);
    float pcr = Gaussian(Cr, meanCr, varCr);
    return 2.0f * pcb * pcr;
};
/*************************************************************************
 *Function: Skin PDF
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *Return: 0-OK,other failed
 **************************************************************************/
int f_SkinPDF(unsigned char *srcData, int width, int height, int stride)
{
    int ret = 0;
    float sum = 0, mean = 0, variance = 0;
    unsigned char *pSrc = srcData;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            // default setting is computed using special include.opencv2.skin data.
            // meanCb-varCb:102-196
            // meanCr-varCr:143-196
            int gray = CLIP3(GetPDF(pSrc[2], pSrc[1], pSrc[0], 102, 196, 143, 196) * 255.0f, 0, 255);
            pSrc[0] = pSrc[1] = pSrc[2] = gray;
            pSrc += 4;
        }
    }
    return ret;
};
