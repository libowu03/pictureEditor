#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "opencv2/opencv.hpp"
#include "SkinDetect.h"
#include "opencv2/colorSpaceChange/TRGB2YCbCr.h"
#include "opencv2/colorSpaceChange/TRGB2HSV.h"
#include "opencv2/utils/Commen.h"

/*************************************************************************
 *Function: Skin detection in RGB color space
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *Return: 0-OK,other failed
 *Refference: Human include.opencv2.skin color clustering for face detection.
 **************************************************************************/
int f_SkindetectionRGB(unsigned char *srcData, int width, int height, int stride)
{
    int ret = 0;
    unsigned char *pSrc = srcData;
    int R, G, B;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            B = pSrc[0];
            G = pSrc[1];
            R = pSrc[2];
            if (!((R > 95) && (G > 40) && (B > 20) && (R > G) && (R > B) && (MAX(R, MAX(G, B)) - MIN(R, MIN(G, B)) > 15) && (abs(R - G) > 15)))
            {
                pSrc[0] = 0;
                pSrc[1] = 0;
                pSrc[2] = 0;
            }
            pSrc += 4;
        }
    }
    return ret;
}
/*************************************************************************
 *Function: Skin detection in hsv color space
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *Return: 0-OK,other failed
 *Refference: Skin color enhancement based on favorite include.opencv2.skin color in HSV color space.
 **************************************************************************/
int f_SkindetectionHSV(unsigned char *srcData, int width, int height, int stride)
{
    int ret = 0;
    unsigned char *pSrc = srcData;
    int R, G, B;
    float H, S, V;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            B = pSrc[0];
            G = pSrc[1];
            R = pSrc[2];
            RGBToHSV(R, G, B, &H, &S, &V);
            if (!(((S >= 0.1) && (S <= 0.68)) && ((V >= 0.13 && V <= 0.25 && H >= ((0.4 - V) / 0.014) && H <= ((V + 0.062) / 0.01)) || ((V > 0.25) && (V <= 0.38) && (H >= (0.4 - V) / 0.014) && (H <= (0.67 - V) / 0.014)) || ((V > 0.38) && (V <= 0.46) && (H >= (V - 0.34) / 0.03) && (H <= (0.67 - V) / 0.014)) ||
                                                  ((V > 0.46) && (V <= 0.6) && (H >= (V - 0.34) / 0.03) && (H <= (V - 0.31) / 0.009)) || ((V > 0.6) && (V <= 0.76) && (H >= (0.91 - V) / 0.14) && (H <= (V - 0.31) / 0.009)) || ((V > 0.76) && (V <= 0.91) && (H >= (0.91 - V) / 0.14) && (H <= (1.17 - V) / 0.0082)) || ((V > 0.91) && (V <= 1) && (H >= (V - 0.91) / 0.0041) && (H <= (1.17 - V) / 0.0082)))))
            {
                pSrc[0] = 0;
                pSrc[1] = 0;
                pSrc[2] = 0;
            }
            pSrc += 4;
        }
    }
    return ret;
}
/*************************************************************************
 *Function: Skin detection in ycgcr color space
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *Return: 0-OK,other failed
 *Refference: YCgCr颜色空间的肤色据类人脸检测法.
 **************************************************************************/
int f_SkindetectionYCgCr(unsigned char *srcData, int width, int height, int stride)
{
    int ret = 0;
    unsigned char *pSrc = srcData;
    int R, G, B;
    float Cr, Cg, Cb;
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            B = pSrc[0];
            G = pSrc[1];
            R = pSrc[2];
            Cg = 128 - 0.318f * R + 0.4392f * G - 0.1212f * B;
            Cr = 128 + 0.4392f * R - 0.3677f * G - 0.0714f * B;
            if (!((Cg >= 85) && (Cg <= 135) && ((Cr <= (280 - Cg)) && (Cr >= (260 - Cg)))))
            {
                pSrc[0] = 0;
                pSrc[1] = 0;
                pSrc[2] = 0;
            }
            pSrc += 4;
        }
    }
    return ret;
}