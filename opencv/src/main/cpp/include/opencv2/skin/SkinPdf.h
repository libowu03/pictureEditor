
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-4-23
Mail:        dongtingyueh@163.com
Description: Skin PDF .
*************************************************************************/
#ifndef __T_SKIN_PDF__
#define __T_SKIN_PDF__

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/*************************************************************************
 *Function: Skin PDF
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *Return: 0-OK,other failed
 **************************************************************************/
EXPORT int f_SkinPDF(unsigned char *srcData, int width, int height, int stride);

#else

#ifdef __cplusplus
extern "C"
{
#endif

    int f_SkinPDF(unsigned char *srcData, int width, int height, int stride);

#ifdef __cplusplus
}
#endif

#endif

#endif
