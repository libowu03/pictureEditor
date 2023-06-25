#include"MakeupBase.h"
#include"EyeLash.h"
#include<math.h>
#include<string.h>
#include<stdlib.h>



int f_EyeLash(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[202], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], bool isLeft, int ratio)
{
	if(isLeft)
	{
	    int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 39], srcFacePoints[2 * 39 + 1], srcFacePoints[2 * 42], srcFacePoints[2 * 42 + 1], srcFacePoints[2 * 45], srcFacePoints[2 * 45 + 1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
	else
	{
		int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 51], srcFacePoints[2 * 51 + 1], srcFacePoints[2 * 54], srcFacePoints[2 * 54 + 1], srcFacePoints[2 * 57], srcFacePoints[2 * 57 + 1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
};


