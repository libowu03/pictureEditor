#include"opencv2/utils/Commen.h"
#include"EyeShadow.h"
#include"MakeupBase.h"

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
int f_EyeShadow(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[202], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 4], bool isLeft, int mode, int ratio)
{
	if(isLeft)
	{
	    int srcKeyPoints[2 * 4] = {srcFacePoints[2 * 39], srcFacePoints[2 * 39 + 1], srcFacePoints[2 * 42], srcFacePoints[2 * 42 + 1], srcFacePoints[2 * 45], srcFacePoints[2 * 45 + 1], srcFacePoints[2 * 48], srcFacePoints[2 * 48 + 1]};
	    return f_MakeupBaseShadow(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, mode, ratio);
	}
	else
	{
		int srcKeyPoints[2 * 4] = {srcFacePoints[2 * 51], srcFacePoints[2 * 51 + 1], srcFacePoints[2 * 54], srcFacePoints[2 * 54 + 1], srcFacePoints[2 * 57], srcFacePoints[2 * 57 + 1], srcFacePoints[2 * 60], srcFacePoints[2 * 60 + 1]};
	    return f_MakeupBaseShadow(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, mode, ratio);
	}
};