/*************************************************************************
Copyright:   Trent.
Author:		 Trent1985
Date:		 2018-9-23
Mail:        dongtingyueh@163.com
Description: Gauss Filter
*************************************************************************/
#ifndef __TEST_GAUSSFILTER__
#define __TEST_GAUSSFILTER__

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/************************************************************
 *Function:  Gauss Filter
 *Description: Gauss Filter process
 *Params:
 *srcData:  image bgra data
 *width  :image width
 *height :image height
 *stride :image stride
 *r: radius of gauss filter, range [0,]
 *Return :0-OK,or failed
 ************************************************************/
EXPORT int f_FastGaussFilter(unsigned char *srcData, int width, int height, int stride, float r);

#else

#ifdef __cplusplus
extern "C"
{
#endif
    int f_FastGaussFilter(unsigned char *srcData, int width, int height, int stride, float r);

////
#ifdef __cplusplus
}
#endif

#endif
#endif
