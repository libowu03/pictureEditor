
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-4-23
Mail:        dongtingyueh@163.com
Description: Skin detection.
*************************************************************************/
#ifndef __T_BEEPS_FILTER__
#define __T_BEEPS_FILTER__

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

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
EXPORT int f_SkindetectionRGB(unsigned char *srcData, int width, int height, int stride);

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
EXPORT int f_SkindetectionHSV(unsigned char *srcData, int width, int height, int stride);
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
EXPORT int f_SkindetectionYCgCr(unsigned char *srcData, int width, int height, int stride);
#else

#ifdef __cplusplus
extern "C"
{
#endif

    int f_SkindetectionRGB(unsigned char *srcData, int width, int height, int stride);
    int f_SkindetectionHSV(unsigned char *srcData, int width, int height, int stride);
    int f_SkindetectionYCgCr(unsigned char *srcData, int width, int height, int stride);
#ifdef __cplusplus
}
#endif

#endif

#endif
