
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-11-23
Mail:        dongtingyueh@163.com
Description: Soft include.opencv2.skin .
*************************************************************************/
#ifndef __T_SOFT_SKIN_CHANNEL_METHOD__
#define __T_SOFT_SKIN_CHANNEL_METHOD__

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/*************************************************************************
 *Function: SOFT SKIN CHANNEL METHOD
 *Params:
 *srcData:32BGRA image buffer
 *width:  width of image
 *height: height of image
 *stride: Stride of image
 *lightMap: light curve map data with format BGRA32
 *ratio: Intensity of softskin,range [0,100]
 *Return: 0-OK,other failed
 **************************************************************************/
EXPORT int f_Softskin_ChannelMethod(unsigned char *srcData, int width, int height, int stride, unsigned char *lightMap, int ratio);

#else

#ifdef __cplusplus
extern "C"
{
#endif

extern int SoftskinChannelMethod(unsigned char *srcData, int width, int height, int stride, unsigned char *lightMap, int ratio,unsigned char *maskB);
extern jobject SoftSkin(JNIEnv *env, jclass clazz, jobject bitmap,jobject lutMap,jobject mask,int ratio);

#ifdef __cplusplus
}
#endif

#endif

#endif
