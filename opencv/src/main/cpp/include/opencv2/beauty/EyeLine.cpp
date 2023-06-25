#include"MakeupBase.h"
#include"EyeLine.h"
#include "FaceLandmark.h"
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include <opencv2/utils/utils.h>

using namespace std;
using namespace cv;


int f_EyeLine(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[973*2], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 4], bool isLeft, int ratio,bool isDown)
{
	if(isLeft)
	{
		int center = 480;
		if (isDown){
			center = 513;
		}
	    int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 466], srcFacePoints[2 * 466 + 1], srcFacePoints[2 * center], srcFacePoints[2 * center + 1], srcFacePoints[2 * 496], srcFacePoints[2 * 496 +1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
	else
	{
		int center = 543;
		if (isDown){
			center = 642;
		}
		int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 626], srcFacePoints[2 * 626 + 1], srcFacePoints[2 * center], srcFacePoints[2 * center + 1], srcFacePoints[2 * 594], srcFacePoints[2 * 594 + 1]};
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
}

jobject eyeLine(JNIEnv *env, jclass clazz, jobject bitmap,jobject maskLeftBitmap,jobject maskRightBitmap,jint eyeLineUpStrength,jint eyeLineDownStrength){
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
	int maskKeyPointLeft[] = {201,217,219,209,238,221,218,226};
	int maskKeyPointRight[] = {292,221,311,209,329,217,314,225};
	//有可能识别到多张脸，只选取第一张脸
	for (int i = 0; i < 973; i++) {
		point[i * 2] = result[i].p.x;
		point[i * 2 + 1] = result[i].p.y;
	}
	if (eyeLineDownStrength != 0){
		f_EyeLine(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,maskKeyPointLeft,true,eyeLineDownStrength,true);
		f_EyeLine(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatRight.data,mskMatRight.cols,mskMatRight.rows,mskMatRight.step,maskKeyPointRight,false,eyeLineDownStrength,true);
	}

	if (eyeLineUpStrength != 0){
		f_EyeLine(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,maskKeyPointLeft,true,eyeLineUpStrength,false);
		f_EyeLine(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMatRight.data,mskMatRight.cols,mskMatRight.rows,mskMatRight.step,maskKeyPointRight,false,eyeLineUpStrength,false);
	}

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



