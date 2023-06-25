#include"MakeupBase.h"
#include"EyeBrow.h"
#include "FaceLandmark.h"
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include <jni.h>
#include <opencv2/utils/utils.h>

using namespace std;
using namespace cv;


int f_EyeBrow(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[973*2], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], bool isLeft, int ratio)
{
	if(isLeft)
	{
	    int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 273], srcFacePoints[2 * 273 + 1], (srcFacePoints[2 * 290] + srcFacePoints[2 * 322]) / 2, (srcFacePoints[2 * 290 + 1] + srcFacePoints[2 * 322 + 1]) / 2, srcFacePoints[2 * 305], srcFacePoints[2 * 305 + 1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
	else
	{
		int srcKeyPoints[2 * 3] = { srcFacePoints[2 * 370], srcFacePoints[2 * 370 + 1], (srcFacePoints[2 * 354] + srcFacePoints[2 * 385]) / 2, (srcFacePoints[2 * 354 + 1] + srcFacePoints[2 * 385 + 1]) / 2,srcFacePoints[2 * 337], srcFacePoints[2 * 337 + 1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
}


jobject eyeBrow(JNIEnv *env, jclass clazz, jobject bitmap,jobject maskLeftBitmap,jobject maskRightBitmap,jint strength){
	std::vector<FaceKeyPoint>  result = FaceLandmark::detector->detect(env, bitmap);
	if (result.size() == 0){
		return bitmap;
	}
	AndroidBitmapInfo info;
	void *pixels;
	CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
	CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
	CV_Assert(pixels);
	cv::Mat img_src(info.height, info.width, CV_8UC4, pixels);
	cv::Mat bgr = img_src.clone();
	cvtColor(bgr,bgr,COLOR_RGBA2BGRA);


	AndroidBitmapInfo mskLeft;
	void *pixelsMskLeft;
	CV_Assert(AndroidBitmap_getInfo(env, maskLeftBitmap, &mskLeft) >= 0);
	CV_Assert(mskLeft.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  mskLeft.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, maskLeftBitmap, &pixelsMskLeft) >= 0);
	CV_Assert(pixelsMskLeft);
	cv::Mat mskMatLeft(mskLeft.height, mskLeft.width, CV_8UC4, pixelsMskLeft);
	cvtColor(mskMatLeft,mskMatLeft,COLOR_RGBA2BGRA);

	AndroidBitmapInfo mskRight;
	void *pixelsMskRight;
	CV_Assert(AndroidBitmap_getInfo(env, maskRightBitmap, &mskRight) >= 0);
	CV_Assert(mskRight.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  mskRight.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, maskRightBitmap, &pixelsMskRight) >= 0);
	CV_Assert(pixelsMskRight);
	cv::Mat mskMatRight(mskRight.height, mskRight.width, CV_8UC4, pixelsMskRight);
	cvtColor(mskMatRight,mskMatRight,COLOR_RGBA2BGRA);

	int point[973 * 2] = {0};
	int maskKeyPointLeft[] = {184,189,220,185,245,193};
	int maskKeyPointRight[] = {287,193,311,185,346,188};
	//有可能识别到多张脸，只选取第一张脸
	for (int i = 0; i < 973; i++) {
		point[i * 2] = result[i].p.x;
		point[i * 2 + 1] = result[i].p.y;
	}

	f_EyeBrow(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,maskKeyPointLeft,true,strength);
	f_EyeBrow(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatRight.data,mskMatRight.cols,mskMatRight.rows,mskMatRight.step,maskKeyPointRight,false,strength);
	cvtColor(bgr,bgr,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, bgr, true, bitmap);
	img_src.release();
	bgr.release();
	mskMatLeft.release();
	mskMatRight.release();
	AndroidBitmap_unlockPixels(env, bitmap);
	AndroidBitmap_unlockPixels(env, maskLeftBitmap);
	AndroidBitmap_unlockPixels(env, maskRightBitmap);
	return resultBitmap;
}
