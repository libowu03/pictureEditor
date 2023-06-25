
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-11-23
Mail:        dongtingyueh@163.com
Description: FASH BLUSH .
*************************************************************************/
#ifndef __T_MAKEUP_FACE_BLUSH__
#define __T_MAKEUP_FACE_BLUSH__


#include <jni.h>

#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif
/*************************************************************************
*Function: Face Blush
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*srcFacePoints: 101 face points.
*mskData: blush image buffer with format bgra32
*mWidth: width of mask image 
*mHeight: height of mask image
*mStride: Stride of mask image
*maskKeyPoints�� 3 key points of blush mask.
*isLeft: left or right face.
*ratio: intensity of effect, [0, 100]
*Return: 0-OK,other failed
**************************************************************************/
EXPORT int f_FaceBlush(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[202], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], bool isLeft, int ratio);
#else

#ifdef __cplusplus
extern "C" {
#endif    

   /////////////////
	int f_FaceBlush(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[202], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], bool isLeft, int ratio);
	extern jobject addFaceBlush(JNIEnv *env, jclass clazz, jobject bitmap,jobject mskLeftBitmap,jobject mskRightBitmap,jint strength);
#ifdef __cplusplus
}
#endif


#endif



#endif
