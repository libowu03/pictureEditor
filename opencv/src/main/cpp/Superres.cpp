//
// Created by libowu on 5/30/22.
//

#include "Superres.h"
#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/utils/utils.h>
#include <opencv2/dnn_superres.hpp>

using namespace std;
using namespace cv;
using namespace dnn_superres;

Mat upscaleImage(JNIEnv *env,Mat img, jstring modelName, jstring modelPath, int scale);

/**
 * 超分辨率
 * @param env
 * @param clazz
 * @param bitmap
 * @param size
 * @return
 */
jobject startSuperres(JNIEnv *env, jclass clazz, jobject bitmap,jint size,jstring modelPath,jstring modelName){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img,img,COLOR_RGBA2BGR);
    Mat result = upscaleImage(env,img, modelName, modelPath, size);
    cvtColor(result,result,COLOR_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    img.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

Mat upscaleImage(JNIEnv *env,Mat img, jstring modelName, jstring modelPath, int scale){
    DnnSuperResImpl sr;
    sr.readModel(jstring2str(env,modelPath));
    sr.setModel(jstring2str(env,modelName),scale);
    // Output image
    Mat outputImage;
    sr.upsample(img, outputImage);
    return outputImage;
}