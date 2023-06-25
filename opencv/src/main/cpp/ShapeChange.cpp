//
// Created by libowu on 2021/11/4.
// 主要负责图像形态学变换的处理
//
#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include "include/opencv2/utils/utils.h"
#include "include/opencv2/shapeChange/ShapeChange.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

/**
 * 图片的透视，仿射缩放，旋转卷曲缩放等
 */
jobject transformImage(JNIEnv *env, jclass clazz, jobject bitmap,jobjectArray point)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    jint rows = env->GetArrayLength(point);
    jarray tempList;
    Point2f pts[rows];
    int index = 0;
    for (jint i = 0; i < rows; i++) {
        tempList = (jintArray) env->GetObjectArrayElement(point, i);
        jint cols = env->GetArrayLength(tempList);
        jint *coldata = env->GetIntArrayElements((jintArray) tempList, nullptr);
        for (jint j = 0; j < cols; j++) {
            if (j + 1 >= cols) {
                break;
            }
            pts[index] = Point(coldata[j], coldata[j + 1]);
            index++;
        }
        env->ReleaseIntArrayElements((jintArray) tempList, coldata, 0);
    }

    //原始图片的顶点数组
    /*cv::Point2f srcQuad[] = {
            cv::Point2f(0, 0),                       //左上角
            cv::Point2f(src.cols - 1, 0),            //右上角
            cv::Point2f(src.cols - 1, src.rows - 1), //右下角
            cv::Point2f(0, src.rows - 1)             //左下角
    };*/


    //需要变换目标图像的顶点数组
    cv::Point2f dstQuad[] = {
            cv::Point2f(0, 0),                       //左上角
            cv::Point2f(src.cols - 1, 0),            //右上角
            cv::Point2f(src.cols - 1, src.rows - 1), //右下角
            cv::Point2f(0, src.rows - 1)
    };
    //稀疏透视变换
    cv::Mat warp_mat = cv::getPerspectiveTransform(pts, dstQuad);
    cv::Mat dst;
    cv::warpPerspective(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
    jobject resultBitmap = matToBitmap(env, dst, true, bitmap);
    dst.release();
    src.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

