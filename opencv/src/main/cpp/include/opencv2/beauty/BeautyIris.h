
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-11-23
Mail:        dongtingyueh@163.com
Description: BeautyIris .
*************************************************************************/
#ifndef __T_BEAUTY_IRIS__
#define __T_BEAUTY_IRIS__


#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

/*************************************************************************
*Function: BeautyIris
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*srcPoints: 12 eyepoints and one center points
*maskData: 32BGRA mask image buffer
*mWidth: width of mask
*mHeight:height of mask
*mStride:Stride of mask
*maskPoints: 12 eyepoints and one center points of mask image
*ratio: intensity of iris, [0,100]
*Return: 0-OK,other failed
**************************************************************************/
EXPORT int f_BeautyIris(unsigned char* srcData, int width, int height, int stride, int srcPoints[], unsigned char* maskData, int mWidth, int mHeight, int mStride, int maskPoints[], int ratio);
#else

#ifdef __cplusplus
extern "C" {
#endif    

   int f_BeautyIris(unsigned char* srcData, int width, int height, int stride, int srcPoints[], unsigned char* maskData, int mWidth, int mHeight, int mStride, int ratio);
//   int f_BeautyIris(unsigned char* srcData, int width, int height, int stride, int srcPoints[], unsigned char* maskData, int mWidth, int mHeight, int mStride, int maskPoints[], int ratio);
extern jobject beautyIris(JNIEnv *env, jclass clazz, jobject bitmap,jobject maskBitmap,jint strength);
#ifdef __cplusplus
}
#endif


#endif



#endif
