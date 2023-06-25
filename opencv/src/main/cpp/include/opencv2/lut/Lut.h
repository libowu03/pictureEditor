/*************************************************************************
Copyright:   Trent.
Author:		 Trent1985
Date:		 2018-9-23
Mail:        dongtingyueh@163.com
Description: LUT Filter
*************************************************************************/
#ifndef __TEST_LUT_FILTER__
#define __TEST_LUT_FILTER__

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/************************************************************
 *Function:  LUT Filter
 *Description: LUT Filter process
 *Params:
 *srcData:  image bgr data
 *width  :image width
 *height :image height
 *stride :image stride
 *Map: 512x512 LUT
 *Return :0-OK,or failed
 ************************************************************/
EXPORT int f_LUTFilter(unsigned char *srcData, int width, int height, int stride, unsigned char *Map);

#else

#ifdef __cplusplus
extern "C"
{
#endif
    int f_LUTFilter(unsigned char *srcData, int width, int height, int stride, unsigned char *Map);

////
#ifdef __cplusplus
}
#endif

#endif
#endif
