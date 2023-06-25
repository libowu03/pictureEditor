#include"opencv2/utils/Commen.h"
#include"FaceBlush.h"
#include"opencv2/beauty/MakeupBase.h"
#include "FaceLandmark.h"
#include <jni.h>
#include <opencv2/utils/utils.h>
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;


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
int f_FaceBlush(unsigned char* srcData, int width, int height, int stride, int srcKeyPoints[2*3], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], bool isLeft, int ratio)
{
	if(isLeft)
	{
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
	else
	{
	    return f_MakeupBase(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
	}
}

/**
 * 添加腮红
 * @param env
 * @param clazz
 * @param bitmap
 * @param filterBitmap
 * @param mskBitmap
 * @param strength
 * @return
 */
jobject addFaceBlush(JNIEnv *env, jclass clazz, jobject bitmap,jobject mskLeftBitmap,jobject mskRightBitmap,jint strength){
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
	CV_Assert(AndroidBitmap_getInfo(env, mskLeftBitmap, &mskLeft) >= 0);
	CV_Assert(mskLeft.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
					  mskLeft.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, mskLeftBitmap, &pixelsMskLeft) >= 0);
	CV_Assert(pixelsMskLeft);
	cv::Mat mskMatLeft(mskLeft.height, mskLeft.width, CV_8UC4, pixelsMskLeft);
	cvtColor(mskMatLeft,mskMatLeft,COLOR_RGBA2BGRA);

	AndroidBitmapInfo mskRight;
	void *pixelsMskRight;
	CV_Assert(AndroidBitmap_getInfo(env, mskRightBitmap, &mskRight) >= 0);
	CV_Assert(mskRight.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  mskRight.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, mskRightBitmap, &pixelsMskRight) >= 0);
	CV_Assert(pixelsMskRight);
	cv::Mat mskMatRight(mskRight.height, mskRight.width, CV_8UC4, pixelsMskRight);
	cvtColor(mskMatRight,mskMatRight,COLOR_RGBA2BGRA);

	int point[973 * 2] = {0};
	//有可能识别到多张脸，只选取第一张脸
	for (int i = 0; i < 973; i++) {
		point[i * 2] = result[i].p.x;
		point[i * 2 + 1] = result[i].p.y;
	}
	int leftPoint[6] = {0};
	leftPoint[0] = point[266];
	leftPoint[1] = point[267];
	leftPoint[2] = point[1338];
	leftPoint[3] = point[1339];
	leftPoint[4] = point[480];
	leftPoint[5] = point[481];

	int rightPoint[6] = {0};
	rightPoint[0] = point[16];
	rightPoint[1] = point[17];
	rightPoint[2] = point[1468];
	rightPoint[3] = point[1469];
	rightPoint[4] = point[352];
	rightPoint[5] = point[353];
	//circle(bgr,Point(leftPoint[0],leftPoint[1]),10,Scalar(255,0,0,255));
	//circle(bgr,Point(leftPoint[2],leftPoint[3]),10,Scalar(255,0,0,255));
	//circle(bgr,Point(leftPoint[4],leftPoint[5]),10,Scalar(255,0,0,255));

	int mskPointLeft[6] = {170,232,211,332,254,240};
	int mskPointRight[6] = {520-170,232,520-211,332,520-254,240};
	f_FaceBlush(bgr.data,bgr.cols,bgr.rows,bgr.step,leftPoint,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,mskPointLeft,true,strength);
	f_FaceBlush(bgr.data,bgr.cols,bgr.rows,bgr.step,rightPoint,mskMatRight.data,mskMatRight.cols,mskMatRight.rows,mskMatRight.step,mskPointRight,false,strength);

	cvtColor(bgr,bgr,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, bgr, true, bitmap);
	img_src.release();
	bgr.release();
	mskMatLeft.release();
	mskMatRight.release();
	AndroidBitmap_unlockPixels(env, bitmap);
	AndroidBitmap_unlockPixels(env, mskLeftBitmap);
	AndroidBitmap_unlockPixels(env, mskRightBitmap);
	return resultBitmap;
}