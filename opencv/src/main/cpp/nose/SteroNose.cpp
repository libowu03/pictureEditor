#include"opencv2/beauty/MakeupBase.h"
#include"SteroNose.h"
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include <opencv2/opencv.hpp>
#include <jni.h>
#include <android/bitmap.h>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/beauty/FaceLandmark.h>
#include <opencv2/utils/utils.h>

#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

int f_SteroNose(unsigned char* srcData, int width, int height, int stride, int srcFacePoints[212], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], int ratio)
{
	int srcKeyPoints[2 * 3] = {srcFacePoints[2 * 803], srcFacePoints[2 * 803 + 1], srcFacePoints[2 * 695], srcFacePoints[2 * 695 + 1], srcFacePoints[2 * 761], srcFacePoints[2 * 761 + 1]};
	return f_MakeupBaseSteronose(srcData, width, height, stride,srcKeyPoints, mskData, mWidth, mHeight, mStride, maskKeyPoints, ratio);
}

jobject StrengthNose(JNIEnv *env, jclass clazz, jobject bitmap,jobject noseMsk,jint strength){
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
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2BGRA);
    cv::Mat bgr = image.clone();
    cvtColor(bgr,bgr,COLOR_RGBA2BGRA);

    AndroidBitmapInfo msk;
    void *pixelsMsk;
    CV_Assert(AndroidBitmap_getInfo(env, noseMsk, &msk) >= 0);
    CV_Assert(msk.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              msk.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, noseMsk, &pixelsMsk) >= 0);
    CV_Assert(pixelsMsk);
    cv::Mat mskMat(msk.height, msk.width, CV_8UC4, pixelsMsk);
    cvtColor(image,image,CV_RGBA2BGRA);

    int point[973 * 2] = {0};
    //有可能识别到多张脸，只选取第一张脸
    for (int i = 0; i < 973; i++) {
        point[i * 2] = result[i].p.x;
        point[i * 2 + 1] = result[i].p.y;
    }
    int nosePoint[6] = {0};
    nosePoint[0] = 265;
    nosePoint[1] = 219;
    nosePoint[2] = 242;
    nosePoint[3] = 270;
    nosePoint[4] = 288;
    nosePoint[5] = 271;
    f_SteroNose(bgr.data,bgr.cols,bgr.rows,bgr.step,point,mskMat.data,mskMat.cols,mskMat.rows,mskMat.step,nosePoint,strength);
    jobject resultBitmap = matToBitmap(env, bgr, true, bitmap);
    bgr.release();
    image.release();
    mskMat.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, noseMsk);
    return resultBitmap;

}


