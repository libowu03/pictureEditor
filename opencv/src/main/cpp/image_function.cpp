//
// Created by libowu on 20-8-30.
// 该文件保存的是一些图片的功能性方法，比如抠图，旋转，重置大小等
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
#include <opencv2/match/SplineInterpolation.h>
#include "opencv2/imageFunction/image_function.h"
#include "include/opencv2/imageFunction/bean.h"
#include "include/opencv2/utils/utils.h"
#include "include/opencv2/utils/Commen.h"
#include "include/opencv2/match/SplineInterpolation.h"
#include "include/opencv2/hsl/HSL.hpp"

using namespace std;
using namespace cv;
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define SWAP(a, b, t)  do { t = a; a = b; b = t; } while(0)
#define CLIP_RANGE(value, min, max)  ( (value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)) )
#define COLOR_RANGE(value)  CLIP_RANGE(value, 0, 255)
#include <regex>
#include <opencv2/hsl/ColorSpace.hpp>

regex re("-?((\\d+|\\d+\\.\\d+)|(\\d+\\.\\d+)e[+-]\\d+)|-[0-9]+(.[0-9]+)?|[0-9]+(.[0-9]+)?");

void changeColor(float r, float g, float b, float black, int channelType, bool isRelative,int sourceRed,int sourceGreen,int sourceBlue,int &red,int &green, int &blue, int max, int mid, int min,bool is_relative);

/**
 * 替换证件照的背景色
 * @param env
 * @param clazz
 * @param bitmap
 * @return
 */
jobject imageIdPhotoReplaceBg(JNIEnv *env, jclass clazz, jobject bitmap) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        //原图
        cv::Mat src(info.height, info.width, CV_8UC4, pixels);
        cv::cvtColor(src, src, CV_RGBA2RGB);
        if (src.empty()) {
            printf("could not load image...\n");
            return bitmap;
        }
        // 1.将二维图像数据线性化
        Mat data;
        for (int i = 0; i < src.rows; i++)     //像素点线性排列
            for (int j = 0; j < src.cols; j++) {
                Vec3b point = src.at<Vec3b>(i, j);
                Mat tmp = (Mat_<float>(1, 3) << point[0], point[1], point[2]);
                data.push_back(tmp);
            }
        // 2.使用K-means聚类；分离出背景色
        int numCluster = 8;
        Mat labels;
        TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 10, 0.1);
        kmeans(data, numCluster, labels, criteria, 3, KMEANS_PP_CENTERS);

        // 3.背景与人物二值化
        Mat mask = Mat::zeros(src.size(), CV_8UC1);
        int index = src.rows * 2 + 2;  //获取点（2，2）作为背景色
        int cindex = labels.at<int>(index);
        /*  提取背景特征 */
        for (int row = 0; row < src.rows; row++) {
            for (int col = 0; col < src.cols; col++) {
                index = row * src.cols + col;
                int label = labels.at<int>(index);
                if (label == cindex) { // 背景
                    mask.at<uchar>(row, col) = 0;
                } else {
                    mask.at<uchar>(row, col) = 255;
                }
            }
        }
        // 4.腐蚀 + 高斯模糊：图像与背景交汇处高斯模糊化
        Mat k = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
        erode(mask, mask, k);
        //imshow("erode-mask", mask);
        GaussianBlur(mask, mask, Size(3, 3), 0, 0);
        //imshow("Blur Mask", mask);

        // 5.更换背景色以及交汇处融合处理
        RNG rng(12345);
        Vec3b color;  //设置的背景色
        color[0] = 00;//rng.uniform(0, 255);
        color[1] = 105;// rng.uniform(0, 255);
        color[2] = 35;// rng.uniform(0, 255);
        Mat result(src.size(), src.type());

        double w = 0.0;   //融合权重
        int b = 0, g = 0, r = 0;
        int b1 = 0, g1 = 0, r1 = 0;
        int b2 = 0, g2 = 0, r2 = 0;

        for (int row = 0; row < src.rows; row++) {
            for (int col = 0; col < src.cols; col++) {
                printf("%d\n", col);
                int m = mask.at<uchar>(row, col);
                if (m == 255) {
                    result.at<Vec3b>(row, col) = src.at<Vec3b>(row, col); // 前景
                } else if (m == 0) {
                    result.at<Vec3b>(row, col) = color; // 背景
                } else {
                    w = m / 255.0;
                    b1 = src.at<Vec3b>(row, col)[0];
                    g1 = src.at<Vec3b>(row, col)[1];
                    r1 = src.at<Vec3b>(row, col)[2];

                    b2 = color[0];
                    g2 = color[1];
                    r2 = color[2];

                    b = b1 * w + b2 * (1.0 - w);
                    g = g1 * w + g2 * (1.0 - w);
                    r = r1 * w + r2 * (1.0 - w);

                    result.at<Vec3b>(row, col)[0] = b;
                    result.at<Vec3b>(row, col)[1] = g;
                    result.at<Vec3b>(row, col)[2] = r;
                }
            }
        }
        //imshow("背景替换", result);
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 选择感兴趣区域
 */
jobject
imageChooseArea(JNIEnv *env, jclass clazz, jobject bitmap, jobjectArray point) {
    if (bitmap == nullptr/*true*/) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        //原图
        cv::Mat bgImge(info.height, info.width, CV_8UC4, pixels);
        Mat bgMask = Mat::zeros(bgImge.size(), CV_8UC1);
        Mat outBitmap(bgImge.size(), bgImge.type());
        jarray tempList;
        jint rows = env->GetArrayLength(point);
        LOGI("数组长度：%d", rows);
        //设置选择点的数组
        Point pts[1][rows];
        int index = 0;
        for (jint i = 0; i < rows; i++) {
            tempList = (jintArray) env->GetObjectArrayElement(point, i);
            jint cols = env->GetArrayLength(tempList);
            jint *coldata = env->GetIntArrayElements((jintArray) tempList, nullptr);
            for (jint j = 0; j < cols; j++) {
                if (j + 1 >= cols) {
                    break;
                }
                pts[0][index] = Point(coldata[j], coldata[j + 1]);
                index++;
            }
            env->ReleaseIntArrayElements((jintArray) tempList, coldata, 0);
        }

        const Point *ppts[] = {pts[0]};        //ppts指折线中拐点坐标指针
        int npt[] = {rows};                        //npt指折线中拐点个数指针
        Scalar color = Scalar(255, 0, 255);
        //获取感兴趣区域的区域坐标
        int left = -1;
        int right = -1;
        int top = -1;
        int bottom = -1;
        for (int i = 0; i < rows; i++) {
            Point p = pts[0][i];
            if (left == -1) {
                left = p.x;
            }
            if (right == -1) {
                right = p.x;
            }
            if (top == -1) {
                top = p.y;
            }
            if (bottom == -1) {
                bottom = p.y;
            }

            if (p.x >= right) {
                right = p.x;
            }
            if (p.x <= left) {
                left = p.x;
            }
            if (p.y >= bottom) {
                bottom = p.y;
            }
            if (p.y <= top) {
                top = p.y;
            }
        }
        //生成掩模
        fillPoly(bgMask, ppts, npt, 1, color, 8);
        //黑色区域设置为透明，白色区域设置为原图的像素信息
        int width = bgMask.cols * bgMask.channels();
        int height = bgMask.rows;
        for (int i = 0; i < height; i++) {
            const uchar *in = bgImge.ptr<uchar>(i);
            const uchar *inRoi = bgMask.ptr<uchar>(i);
            uchar *out = outBitmap.ptr<uchar>(i);
            for (int j = 0; j < width; j++) {
                if (inRoi[j] == 0) {
                    //黑色为不感兴趣区域
                    out[j * 4] = 0;
                    out[j * 4 + 1] = 0;
                    out[j * 4 + 2] = 0;
                    out[j * 4 + 3] = 0;
                } else {
                    //白色为感兴趣区域
                    out[j * 4] = in[j * 4];
                    out[j * 4 + 1] = in[j * 4 + 1];
                    out[j * 4 + 2] = in[j * 4 + 2];
                    out[j * 4 + 3] = in[j * 4 + 3];
                }
            }

        }
        Rect rect(left, top, right - left, bottom - top);
        Mat result = outBitmap(rect);
        //fillPoly(bgMask, ppts, npt, 1, color, 8);		//fillPoly接受的是一个二维矩阵
        jobject resultBitmap = matToBitmap(env, result, false, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 圆形裁剪
 */
jobject imageCircleArea(JNIEnv *env, jclass clazz, jobject bitmap, jobject pointSource, jint radius) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        cv::Mat bgImge(info.height, info.width, CV_8UC4, pixels);
        //1、建立圆形腌膜，2、将需要处理的地方复制出来，3、对整个图片进行处理后，贴到原始图像
        Mat resImg, roi;
        roi = Mat::zeros(bgImge.size(), CV_8UC1);
        resImg = Mat::zeros(bgImge.size(), CV_8UC4);

        jclass pointClazz = env->FindClass("android/graphics/Point");
        jfieldID javaPointX = env->GetFieldID(pointClazz, "x", "I");
        jfieldID javaPointY = env->GetFieldID(pointClazz, "y", "I");
        jint pointX = env->GetIntField(pointSource, javaPointX);
        jint pointY = env->GetIntField(pointSource, javaPointY);
        cv::Point point(pointX, pointY);
        cv::circle(roi, point, radius, CV_RGB(255, 255, 255), -1);//mask建立
        int x = 0;
        int y = 0;
        if (point.x - radius < 0) {
            x = 0;
        } else {
            x = point.x - radius;
        }
        if (point.y - radius < 0) {
            y = 0;
        } else {
            y = point.y - radius;
        }
        if (point.x < radius) {
            x = point.x - radius / 2;
            y = point.y - radius / 2;
        }
        Rect rect(x, y, x + radius, y + radius);
        int width = roi.cols * roi.channels();
        int height = roi.rows;
        for (int i = 0; i < height; i++) {
            const uchar *in = bgImge.ptr<uchar>(i);
            const uchar *inRoi = roi.ptr<uchar>(i);
            uchar *out = resImg.ptr<uchar>(i);
            for (int j = 0; j < width; j++) {
                if (inRoi[j] == 0) {
                    //黑色为不感兴趣区域
                    out[j * 4] = 0;
                    out[j * 4 + 1] = 0;
                    out[j * 4 + 2] = 0;
                    out[j * 4 + 3] = 0;
                } else {
                    //白色为感兴趣区域
                    out[j * 4] = in[j * 4];
                    out[j * 4 + 1] = in[j * 4 + 1];
                    out[j * 4 + 2] = in[j * 4 + 2];
                    out[j * 4 + 3] = in[j * 4 + 3];
                }
            }
        }
        Mat result = resImg(rect);
        /*  vector<int> compression_params;
          //CV_IMWRITE_PNG_COMPRESSION
          compression_params.push_back(16);
          compression_params.push_back(9);*/
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 *
 * 对图片进行锐化
 *      -1  -1  -1
 *  H1= -1   c  -1
 *      -1  -1  -1
 * 当c大于8的时候表示图像锐化，越接近8表示锐化效果越好。
 * 当c等于8的时候表示对图像进行高通滤波。
 * 当c值越大的时候，图像锐化效果在减弱，中心像素值的作用在提升
 *
 *       0  -1   0
 *  H2= -1   5  -1
 *       0  -1   0
 *
 * */
jobject imageSharpening(JNIEnv *env, jclass clazz, jobject bitmap, jdouble sharpeningSize) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        cv::Mat image(info.height, info.width, CV_8UC4, pixels);
        Mat h1_kernel = (Mat_<char>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
        Mat h2_kernel = (Mat_<char>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
        Mat h1_result, h2_result;
        filter2D(image, h1_result, CV_32F, h1_kernel);
        filter2D(image, h2_result, CV_32F, h2_kernel);
        convertScaleAbs(h1_result, h1_result);
        convertScaleAbs(h2_result, h2_result);
        jobject resultBitmap = matToBitmap(env, h2_result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 图像大小重置
 * @param env
 * @param clazz
 * @param bitmap
 * @param resizeValue
 * @return
 */
jobject
imageResize(JNIEnv *env, jclass clazz, jobject bitmap, jdouble resizeXValue, jdouble resizeYValue) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        cv::Mat image(info.height, info.width, CV_8UC4, pixels);
        Size dsize = Size(image.cols * resizeXValue, image.rows * resizeYValue);
        //Mat result = Mat(dsize, CV_32S);
        Mat result(image.size(),image.type());
        resize(image, result, dsize);
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        //callListener(listener,env,bitmap,nullptr, false,e.what());
        return bitmap;
    } catch (...) {
        //callListener(listener,env,bitmap,nullptr, false,"执行makeBitmapToPixels出现未知异常");
        return bitmap;
    }
}

/**
 * 图像通道设置
 * @param env
 * @param clazz
 * @param bitmap
 * @param useRedChannel
 * @param useGreenChannel
 * @param useBlureChannel
 * @return
 */
jobject imageStayColorChannel(JNIEnv *env, jclass clazz, jobject bitmap, jboolean useRedChannel,
                              jboolean useGreenChannel, jboolean useBlureChannel) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    //所有通道都保留，直接返回原图即可
    if (useRedChannel && useGreenChannel && useBlureChannel) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        cv::Mat image(info.height, info.width, CV_8UC4, pixels);
        Mat result(image.size(), image.type());
        int height = result.rows;
        int widht = result.cols;
        for (int h = 0; h < height; h++) {
            const uchar *in = image.ptr<uchar>(h);
            uchar *out = result.ptr<uchar>(h);
            for (int w = 0; w < widht; w++) {

                if (result.channels() == 3) {
                    if (useBlureChannel) {
                        out[w * 3 + 2] = in[w * 3 + 2];
                    } else {
                        out[w * 3 + 2] = 0;
                    }
                    if (useGreenChannel) {
                        out[w * 3 + 1] = in[w * 3 + 1];
                    } else {
                        out[w * 3 + 1] = 0;
                    }
                    if (useRedChannel) {
                        out[w * 3] = in[w * 3];
                    } else {
                        out[w * 3] = 0;
                    }
                } else if (result.channels() == 4) {
                    out[w * 4 + 3] = in[w * 4 + 3];
                    if (useBlureChannel) {
                        out[w * 4 + 2] = in[w * 4 + 2];
                    } else {
                        out[w * 4 + 2] = 0;
                    }
                    if (useGreenChannel) {
                        out[w * 4 + 1] = in[w * 4 + 1];
                    } else {
                        out[w * 4 + 1] = 0;
                    }
                    if (useRedChannel) {
                        out[w * 4] = in[w * 4];
                    } else {
                        out[w * 4] = 0;
                    }
                }
            }
        }
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        //callListener(listener,env,bitmap,nullptr, false,e.what());
        return bitmap;
    } catch (...) {
        //callListener(listener,env,bitmap,nullptr, false,"执行makeBitmapToPixels出现未知异常");
        return bitmap;
    }
}

/**
 * 调整亮度和对比度
 * @param env
 * @param clazz
 * @param bitmap
 * @param brightness
 * @param contrast
 * @return
 */
jobject changeBrightnessContrast(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,int brightness, int contrast)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cv::Mat outPixelout(image.size(), image.type());
    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    brightness = CLIP_RANGE(brightness, -255, 255);
    contrast = CLIP_RANGE(contrast, -255, 255);
    double B = brightness / 255.;
    double c = contrast / 255. ;
    double k = tan( (45 + 44 * c) / 180 * M_PI );
    Mat lookupTable(1, 256, CV_8U);
    uchar *p = lookupTable.data;
    for (int i = 0; i < 256; i++)
        p[i] = COLOR_RANGE( (i - 127.5 * (1 - B)) * k + 127.5 * (1 + B) );
    LUT(image, lookupTable, outPixelout);

    cv::Mat tmp(image.size(),image.type());

    int channel = image.channels();
    int width = image.cols * channel;
    int height = image.rows;
    for (int i = 0; i < height; i++)
    {
        const uchar *input = image.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *output = tmp.ptr<uchar>(i);
        uchar *tmpInput = outPixelout.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel)
        {
            uchar r = input[j + 2];
            uchar g = input[j + 1];
            uchar b = input[j];

            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                output[j + 2] = r;
                output[j + 1] = g;
                output[j] = b;
                if (outPixelout.channels() == 4){
                    output[j+3] = 255;
                }
                continue;
            }

            output[j + 2] = r * (1 - maxMask / 255.0) + tmpInput[j + 2] * maxMask / 255.0;
            output[j + 1] = g * (1 - maxMask / 255.0) + tmpInput[j + 1] * maxMask / 255.0;
            output[j] = b * (1 - maxMask / 255.0) + tmpInput[j] * maxMask / 255.0;
            if (outPixelout.channels() == 4){
                output[j+3] = 255;
            }
        }
    }

    jobject resultBitmap = matToBitmap(env, tmp, true, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    image.release();
    lookupTable.release();
    maskMt.release();
    tmp.release();
    outPixelout.release();
    return resultBitmap;
}

/**
 * 修改亮度和对比度
 * @param image
 * @param alpha
 * @param beta
 */
jobject
imageMakeBitmapLight(JNIEnv *env, jclass clazz, jobject bitmap, jdouble alpha, jdouble beta) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    try {
        AndroidBitmapInfo info;
        void *pixels;
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        cv::Mat image(info.height, info.width, CV_8UC4, pixels);
        cv::Mat outPixelout(image.size(), image.type());
        int rows = image.rows;
        //int cols = image.cols*image.channels();
        int cols = image.cols;
        if (alpha < 0) {
            alpha = alpha - 1;
        }
        //亮度和对比度分别为1和0时执行下去没意义，最终图片都是和原图一样，顾直接终止执行
        if (alpha == 1 && beta == 0) {
            return bitmap;
        }
        for (int height = 0; height < rows; height++) {
            const uchar *in = image.ptr<uchar>(height);
            uchar *outPixel = outPixelout.ptr<uchar>(height);
            for (int width = 0; width < cols; width++) {
                if (image.channels() == 3) {
                    uchar oneChannel = in[width * 3] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 3] * alpha + beta >= 255) {
                            oneChannel = 255;
                        } else if (in[width * 3] * alpha + beta < 0) {
                            oneChannel = 0;
                        }
                    } else {
                        if (in[width * 3] / (alpha * -1) + beta < 0) {
                            oneChannel = 0;
                        } else if (in[width * 3] / (alpha * -1) + beta > 255) {
                            oneChannel = 255;
                        } else {
                            oneChannel = in[width * 3] / (alpha * -1) + beta;
                        }

                    }
                    uchar twoChannel = in[width * 3 + 1] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 3 + 1] * alpha + beta >= 255) {
                            twoChannel = 255;
                        } else if (in[width * 3 + 1] * alpha + beta < 0) {
                            twoChannel = 0;
                        }
                    } else {
                        if (in[width * 3 + 1] / (alpha * -1) + beta < 0) {
                            twoChannel = 0;
                        } else if (in[width * 3 + 1] / (alpha * -1) + beta > 255) {
                            twoChannel = 255;
                        } else {
                            twoChannel = in[width * 3 + 1] / (alpha * -1) + beta;
                        }
                    }
                    uchar threeChannel = in[width * 3 + 2] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 3 + 2] * alpha + beta >= 255) {
                            threeChannel = 255;
                        } else if (in[width * 3 + 2] * alpha + beta < 0) {
                            threeChannel = 0;
                        }
                    } else {
                        if (in[width * 3 + 2] / (alpha * -1) + beta < 0) {
                            threeChannel = 0;
                        } else if (in[width * 3 + 2] / (alpha * -1) + beta > 255) {
                            threeChannel = 255;
                        } else {
                            threeChannel = in[width * 3 + 2] / (alpha * -1) + beta;
                        }

                    }
                    outPixel[width * 3] = oneChannel;
                    outPixel[width * 3 + 1] = twoChannel;
                    outPixel[width * 3 + 2] = threeChannel;
                } else if (image.channels() == 4) {
                    uchar oneChannel = in[width * 4] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 4] * alpha + beta >= 255) {
                            oneChannel = 255;
                        } else if (in[width * 4] * alpha + beta < 0) {
                            oneChannel = 0;
                        }
                    } else {
                        if (in[width * 4] / (alpha * -1) + beta < 0) {
                            oneChannel = 0;
                        } else if (in[width * 4] / (alpha * -1) + beta > 255) {
                            oneChannel = 255;
                        } else {
                            oneChannel = in[width * 4] / (alpha * -1) + beta;
                        }

                    }
                    uchar twoChannel = in[width * 4 + 1] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 4 + 1] * alpha + beta >= 255) {
                            twoChannel = 255;
                        } else if (in[width * 4 + 1] * alpha + beta < 0) {
                            twoChannel = 0;
                        }
                    } else {
                        if (in[width * 4 + 1] / (alpha * -1) + beta < 0) {
                            twoChannel = 0;
                        } else if (in[width * 4 + 1] / (alpha * -1) + beta > 255) {
                            twoChannel = 255;
                        } else {
                            twoChannel = in[width * 4 + 1] / (alpha * -1) + beta;
                        }
                    }
                    uchar threeChannel = in[width * 4 + 2] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 4 + 2] * alpha + beta >= 255) {
                            threeChannel = 255;
                        } else if (in[width * 4 + 2] * alpha + beta < 0) {
                            threeChannel = 0;
                        }
                    } else {
                        if (in[width * 4 + 2] / (alpha * -1) + beta < 0) {
                            threeChannel = 0;
                        } else if (in[width * 4 + 2] / (alpha * -1) + beta > 255) {
                            threeChannel = 255;
                        } else {
                            threeChannel = in[width * 4 + 2] / (alpha * -1) + beta;
                        }

                    }
                    uchar fourChannel = in[width * 4 + 3] * alpha + beta;
                    if (alpha > 0) {
                        if (in[width * 4 + 3] * alpha + beta >= 255) {
                            fourChannel = 255;
                        } else if (in[width * 4 + 3] * alpha + beta < 0) {
                            fourChannel = 0;
                        }
                    } else {
                        if (in[width * 4 + 3] / (alpha * -1) + beta < 0) {
                            fourChannel = 0;
                        } else if (in[width * 4 + 3] / (alpha * -1) + beta > 255) {
                            fourChannel = 255;
                        } else {
                            fourChannel = in[width * 4 + 3] / (alpha * -1) + beta;
                        }

                    }
                    outPixel[width * 4] = oneChannel;
                    outPixel[width * 4 + 1] = twoChannel;
                    outPixel[width * 4 + 2] = threeChannel;
                    outPixel[width * 4 + 3] = fourChannel;
                }
            }
        }
        jobject resultBitmap = matToBitmap(env, outPixelout, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        //callListener(listener,env,bitmap,nullptr, false,e.what());
        return bitmap;
    } catch (...) {
        //callListener(listener,env,bitmap,nullptr, false,"执行makeBitmapToPixels出现未知异常");
        return bitmap;
    }
}

/**
 *
 * 读取3d lut预设方案文件
 **/
Lut3D open3DLUT(string cubePath) {
    ifstream in(cubePath);
    if (!in.is_open()) {
        return Lut3D();
    }
    stringstream str2digit;
    vector<vector<float>> result;
    int size = 0;
    string line;
    float rMin = 0.0f;
    float gMin = 0.0f;
    float bMin = 0.0f;
    float rMax = 0.0f;
    float gMax = 0.0f;
    float bMax = 0.0f;
    int index = 0;
    if (in) {
        while (getline(in, line)) {
            index++;
            //在此处读取预设方案的大小
            if (strstr(line.c_str(), "LUT_3D_SIZE")) {
                vector<string> res = split(line, " ");
                if (res.size() >= 2) {
                    str2digit << res[1], str2digit >> size;
                    str2digit.clear();
                }
            } else if (strstr(line.c_str(), "DOMAIN_MIN")) {
                //获取rgb各分量最小值
                vector<string> res = split(line, " ");
                //cout<< "存在最小值" << line << endl;
                if (res.size() >= 4) {
                    //将各分量文本字符转换成float类型
                    str2digit << res[1], str2digit >> rMin;
                    str2digit.clear();
                    str2digit << res[2], str2digit >> gMin;
                    str2digit.clear();
                    str2digit << res[3], str2digit >> bMin;
                    str2digit.clear();
                }
            } else if (strstr(line.c_str(), "DOMAIN_MAX")) {
                //获取rgb各分量最大值
                vector<string> res = split(line, " ");
                //cout<< "存在最大值" << line << endl;
                if (res.size() >= 4) {
                    //将各分量文本字符转换成float类型
                    str2digit << res[1], str2digit >> rMax;
                    str2digit.clear();
                    str2digit << res[2], str2digit >> gMax;
                    str2digit.clear();
                    str2digit << res[3], str2digit >> bMax;
                    str2digit.clear();
                }
            } else {
                //获取rgb各分量具体数值
                vector<string> res = split(line, " ");
                //正常rgb数值区应该是0.462280 0.787292 0.193878这样子的，通过分割可获取到三部分
                if (res.size() == 3) {
                    bool isR = strncmp(res[0].c_str(), "0", strlen("0")) == 0 ||
                               strncmp(res[0].c_str(), "1", strlen("1")) == 0 ||
                               strncmp(res[0].c_str(), "2", strlen("2")) == 0 ||
                               strncmp(res[0].c_str(), "3", strlen("3")) == 0 ||
                               strncmp(res[0].c_str(), "4", strlen("4")) == 0 ||
                               strncmp(res[0].c_str(), "5", strlen("5")) == 0 ||
                               strncmp(res[0].c_str(), "6", strlen("6")) == 0 ||
                               strncmp(res[0].c_str(), "7", strlen("7")) == 0 ||
                               strncmp(res[0].c_str(), "8", strlen("8")) == 0 ||
                               strncmp(res[0].c_str(), "9", strlen("9")) == 0 ||
                               strncmp(res[0].c_str(), "-0", strlen("-0")) == 0 ||
                               strncmp(res[0].c_str(), "-1", strlen("-1")) == 0 ||
                               strncmp(res[0].c_str(), "-2", strlen("-2")) == 0 ||
                               strncmp(res[0].c_str(), "-3", strlen("-3")) == 0 ||
                               strncmp(res[0].c_str(), "-4", strlen("-4")) == 0 ||
                               strncmp(res[0].c_str(), "-5", strlen("-5")) == 0 ||
                               strncmp(res[0].c_str(), "-6", strlen("-6")) == 0 ||
                               strncmp(res[0].c_str(), "-7", strlen("-7")) == 0 ||
                               strncmp(res[0].c_str(), "-8", strlen("-8")) == 0 ||
                               strncmp(res[0].c_str(), "-9", strlen("-9")) == 0;

                    bool isG = strncmp(res[1].c_str(), "0", strlen("0")) == 0 ||
                               strncmp(res[1].c_str(), "1", strlen("1")) == 0 ||
                               strncmp(res[1].c_str(), "2", strlen("2")) == 0 ||
                               strncmp(res[1].c_str(), "3", strlen("3")) == 0 ||
                               strncmp(res[1].c_str(), "4", strlen("4")) == 0 ||
                               strncmp(res[1].c_str(), "5", strlen("5")) == 0 ||
                               strncmp(res[1].c_str(), "6", strlen("6")) == 0 ||
                               strncmp(res[1].c_str(), "7", strlen("7")) == 0 ||
                               strncmp(res[1].c_str(), "8", strlen("8")) == 0 ||
                               strncmp(res[1].c_str(), "9", strlen("9")) == 0 ||
                               strncmp(res[1].c_str(), "-0", strlen("-0")) == 0 ||
                               strncmp(res[1].c_str(), "-1", strlen("-1")) == 0 ||
                               strncmp(res[1].c_str(), "-2", strlen("-2")) == 0 ||
                               strncmp(res[1].c_str(), "-3", strlen("-3")) == 0 ||
                               strncmp(res[1].c_str(), "-4", strlen("-4")) == 0 ||
                               strncmp(res[1].c_str(), "-5", strlen("-5")) == 0 ||
                               strncmp(res[1].c_str(), "-6", strlen("-6")) == 0 ||
                               strncmp(res[1].c_str(), "-7", strlen("-7")) == 0 ||
                               strncmp(res[1].c_str(), "-8", strlen("-8")) == 0 ||
                               strncmp(res[1].c_str(), "-9", strlen("-9")) == 0;

                    bool isB = strncmp(res[2].c_str(), "0", strlen("0")) == 0 ||
                               strncmp(res[2].c_str(), "1", strlen("1")) == 0 ||
                               strncmp(res[2].c_str(), "2", strlen("2")) == 0 ||
                               strncmp(res[2].c_str(), "3", strlen("3")) == 0 ||
                               strncmp(res[2].c_str(), "4", strlen("4")) == 0 ||
                               strncmp(res[2].c_str(), "5", strlen("5")) == 0 ||
                               strncmp(res[2].c_str(), "6", strlen("6")) == 0 ||
                               strncmp(res[2].c_str(), "7", strlen("7")) == 0 ||
                               strncmp(res[2].c_str(), "8", strlen("8")) == 0 ||
                               strncmp(res[2].c_str(), "9", strlen("9")) == 0 ||
                               strncmp(res[2].c_str(), "-0", strlen("-0")) == 0 ||
                               strncmp(res[2].c_str(), "-1", strlen("-1")) == 0 ||
                               strncmp(res[2].c_str(), "-2", strlen("-2")) == 0 ||
                               strncmp(res[2].c_str(), "-3", strlen("-3")) == 0 ||
                               strncmp(res[2].c_str(), "-4", strlen("-4")) == 0 ||
                               strncmp(res[2].c_str(), "-5", strlen("-5")) == 0 ||
                               strncmp(res[2].c_str(), "-6", strlen("-6")) == 0 ||
                               strncmp(res[2].c_str(), "-7", strlen("-7")) == 0 ||
                               strncmp(res[2].c_str(), "-8", strlen("-8")) == 0 ||
                               strncmp(res[2].c_str(), "-9", strlen("-9.")) == 0;
                    if (isR && isG && isB) {
                        //将各分量文本字符转换成float类型
                        float resultR = atof(res[0].c_str());
                        float resultG = atof(res[1].c_str());
                        float resultB = atof(res[2].c_str());
                        vector<float> tmpLineResult;
                        tmpLineResult.push_back(resultR);
                        tmpLineResult.push_back(resultG);
                        tmpLineResult.push_back(resultB);
                        result.push_back(tmpLineResult);
                    }
                }
            }
        }
        //cout<< "获取的结果长度为：" << result.size() << endl;
    } else {
        //cout << "不存在文件" << endl;
    }
    in.close();
    Lut3D lut;
    lut.minRColor = rMin;
    lut.maxRColor = rMax;
    lut.minGColor = gMin;
    lut.maxGColor = gMax;
    lut.minBColor = bMin;
    lut.maxBColor = bMax;
    lut.colorValue = result;
    lut.size = size;
    return lut;
}

/**
 *
 * 通过3d lut预设方案修改图片的显示风格
 *
 * */
jobject makeBitmapLutChange(JNIEnv *env, jclass clazz, jobject bitmap, jstring cubePath) {
    if (bitmap == nullptr) {
        return bitmap;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    cvtColor(src,src,CV_RGBA2RGB);
    Mat result(src.size(), src.type());
    time_t start,ends;
    start = clock();
    Lut3D lut = open3DLUT(jstring2str(env, cubePath));
    ends = clock();
    LOGI("日志,读取文件间隔为：%d", (int)(ends - start));
    vector<vector<float>> lutArray = lut.colorValue;
    if (lutArray.size() == 0) {
        return bitmap;
    }
    //cout << "长度：" << lutArray.size() << endl;
    int height = src.rows;
    int widht = src.cols * src.channels();
    int channel = src.channels();
    start = clock();
    for (int i = 0; i < height; i++) {
        const uchar *in = src.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            float r = in[j];
            float g = in[j + 1];
            float b = in[j+2];
            //cout << "R值为：" << r << "g值为：" << g << ",b值为" << b << endl;
            //获取rgb在0~1内的范围
            float radioR = r / 255.0f;
            float radioG = g / 255.0f;
            float radioB = b / 255.0f;
            int tmpR = radioR * (lut.size - 1);
            int tmpG = radioG * (lut.size - 1);
            int tmpB = radioB * (lut.size - 1);
            //获取计算出来的数值的上边界数值和下边界数值
            int tmpRH = tmpR + 1;
            if (tmpRH > (lut.size - 1)) {
                tmpRH = tmpR;
            }
            int tmpRL = tmpR;
            int tmpGH = tmpG + 1;
            if (tmpGH > (lut.size - 1)) {
                tmpGH = tmpR;
            }
            int tmpGL = tmpG;
            int tmpBH = tmpB + 1;
            if (tmpBH > (lut.size - 1)) {
                tmpBH = tmpR;
            }
            int tmpBL = tmpB;
            //获取上下边界数据在cube文件中的位置
            int indexH = lut.size * lut.size * tmpBH + lut.size * tmpGH + tmpRH;
            int indexL = lut.size * lut.size * tmpBL + lut.size * tmpGL + tmpRL;
            try {
                //LOGI("行数：%d",indexH);
                //获取对应索引的位置
                vector<float> colorH = lutArray[indexH];
                vector<float> colorL = lutArray[indexL];
                //通过 结果=最低值+（最高值-最低值）*颜色数值小数点值 [这里是通过权重来计算对应的颜色的，比如6.6的颜色，则0.6说明该颜色更偏向于7，6.1中多出的0.1可以看出颜色是更偏向于6的]
                float resultR = ((colorH[0] - colorL[0])*(radioR * (lut.size-1) - tmpR) + colorL[0])*255;
                float resultG = ((colorH[1] - colorL[1])*(radioG * (lut.size-1) - tmpG) + colorL[1])*255;
                float resultB = ((colorH[2] - colorL[2])*(radioB * (lut.size-1) - tmpB) + colorL[2])*255;
                if(resultR > 255){
                    resultR = 255;
                }else if(resultR < 0){
                    resultR = 0;
                }
                if(resultG > 255){
                    resultG = 255;
                }else if(resultG < 0){
                    resultG = 0;
                }
                if(resultB > 255){
                    resultB = 255;
                }else if(resultB < 0){
                    resultB = 0;
                }
                //cout << "结果：" << resultR << "," << resultG << "," << resultB << endl;
                //最终赋值到新的图片上，形成新的滤镜结果图片
                out[j+0] = abs(resultR);
                out[j+1] = abs(resultG);
                out[j+2] = abs(resultB);
                if (channel == 4) {
                    out[j + 3] = in[j + 3];
                }
            }catch (...){
                //LOGI("错误日志:%d",indexH);
            }
        }
    }
    ends = clock();
    LOGI("日志,时间间隔%d",(int)(ends - start));
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    src.release();
    result.release();
    return resultBitmap;
}

/**
 *
 * 获取通道的rgb信息，用于制作直方图
 *
 * */
jobject getHistogram(JNIEnv *env, jclass clazz, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    Histogram ch;
    int height = src.rows;
    int width = src.cols * src.channels();
    int channel = src.channels();
    jintArray redArray = env->NewIntArray(256);
    jintArray greenArray = env->NewIntArray(256);
    jintArray blurArray = env->NewIntArray(256);
    jintArray rgbArray = env->NewIntArray(256);
    jint r[256] = {0};
    jint g[256] = {0};
    jint b[256] = {0};
    jint rgb[256] = {0};
    for (int h = 0; h < height; h++) {
        const uchar *in = src.ptr<uchar>(h);
        for (int w = 0; w < width; w = w + src.channels()) {
            int rp = in[w + 0];
            int gp = in[w + 1];
            int bp = in[w + 2];
            r[rp] = r[rp] + 1;
            g[gp] = g[gp] + 1;
            b[bp] = b[bp] + 1;
            rgb[rp] = rgb[rp] + 1;
            rgb[gp] = rgb[gp] + 1;
            rgb[bp] = rgb[bp] + 1;
        }
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    src.release();

    char *methodName = "<init>";
    char *methodSign = "([I[I[I[I)V";
    //定义java String类 strClass
    jclass channelInfo = (env)->FindClass("com/image/library/opencv/bean/ImageChannelInfo");
    //构造方法
    jmethodID constructorMethodId = env->GetMethodID(channelInfo, methodName, methodSign);
    //获取java String类方法String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    env->SetIntArrayRegion(redArray, 0, 256, r);
    //env->ReleaseIntArrayElements(redArray, r, NULL);
    env->SetIntArrayRegion(greenArray, 0, 256, g);
    //env->ReleaseIntArrayElements(greenArray, g, NULL);
    env->SetIntArrayRegion(blurArray, 0, 256, b);
    //env->ReleaseIntArrayElements(blurArray, b, NULL);
    env->SetIntArrayRegion(rgbArray, 0, 256, rgb);
    //env->ReleaseIntArrayElements(rgbArray, rgb, NULL);
    auto result = (jobject) (env)->NewObject(channelInfo, constructorMethodId, redArray, greenArray,
                                             blurArray, rgbArray);
    //将byte数组转换为java String,并输出
    return result;
}

/**
 *
 * 颜色选择工具
 * @param r red通道的增长百分比
 * @param g green通道的增长百分比
 * @param b blue通道增长百分比
 * @param black 三通道增长百分比
 * @param channelType 调整通道的类型。1：红色，-1：黄色，2：绿色，-2：青色，3：蓝色，-3：洋红
 * @param isRelative 怎加的方式是相对的还是绝对的
 * */
jobject
chooseColor(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
            jfloat redRed, jfloat redGreen, jfloat redBlue,jfloat redBlack,
            jfloat greenRed, jfloat greenGreen, jfloat greenBlue,jfloat greenBlack,
            jfloat blueRed, jfloat blueGreen, jfloat blueBlue,jfloat blueBlack,
            jfloat cyanRed, jfloat cyanGreen, jfloat cyanBlue,jfloat cyanBlack,
            jfloat yellowRed, jfloat yellowGreen, jfloat yellowBlue,jfloat yellowBlack,
            jfloat carmineRed, jfloat carmineGreen, jfloat carmineBlue,jfloat carmineBlack,
            jfloat whiteRed, jfloat whiteGreen, jfloat whiteBlue,jfloat whiteBlack,
            jfloat blackRed, jfloat blackGreen, jfloat blackBlue,jfloat blackBlack,
            jboolean is_relative) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);


    Mat result(src.size(),src.type());
    int channel = src.channels();
    int widht = src.cols * channel;
    int height = src.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = src.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            //这里需要注意，读取bitmap生成的mat是BGRA类型的，而imread读取图片是RGBA类型的，现在同意采取读取bitmap类型的，所以r和b位置调换一下
            int blue = in[j + 2];
            int green = in[j + 1];
            int red= in[j];
            int redResult = red;
            int greenResult = green;
            int blueResult = blue;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {

                out[j + 2] = red;
                out[j + 1] = green;
                out[j] = blue;
                if (channel == 4)
                {
                    out[j + 3] = 255;
                }
                continue;
            }

            //最大通道值
            int max = MAX( MAX(red,green),blue);
            //最小通道值
            int min = MIN( MIN(red,green),blue);
            //中间通道值
            int mid = red+green+blue - max - min;
            //加入选中要改变的颜色是红色或黄色，实际上是改变三通道中red通道最大时的像素
            //加入选中要改变的颜色是红色或黄色，实际上是改变三通道中red通道最大时的像素
            if (max == red)
            {
                changeColor(redRed, redGreen, redBlue, 1, 1, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }
            if ((max == red || max == green) && min == blue)
            {
                changeColor(yellowRed, yellowGreen, yellowBlue, 1, -1, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            if (max == green)
            {
                changeColor(greenRed, greenGreen, greenBlue, 1, 2, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            if ( (max == red || max == blue) && min == green)
            {
                changeColor(carmineRed, carmineGreen, carmineBlue, 1, -2, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            if (max == blue)
            {
                changeColor(blueRed, blueGreen, blueBlue, 1, 3, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            if ((max == blue || max == green) && min == red)
            {
                changeColor(cyanRed, cyanGreen, cyanBlue, 1, -3, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            //白色
            if (min > 128)
            {
                changeColor(whiteRed, whiteGreen, whiteBlue, 1, 4, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            //黑色
            if (max < 128)
            {
                changeColor(blackRed, blackGreen, blackBlue, 1, 5, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            //中性色
            if ((red != 0 && red != 255) && (green != 0 && green != 255) && (blue != 0 && blue != 255))
            {
                //changeColor(middleRed, middleGreen, middleBlue, 1, 6, true, red, green, blue, redResult, greenResult, blueResult, max, mid, min, is_relative);
            }

            if(redResult > 255){
                redResult = 255;
            }else if(redResult < 0){
                redResult = 0;
            }
            if(greenResult > 255){
                greenResult = 255;
            }else if(greenResult < 0){
                greenResult = 0;
            }
            if(blueResult > 255){
                blueResult = 255;
            }else if(blueResult < 0){
                blueResult = 0;
            }

            out[j + 2] = red * (1 - maxMask / 255.0) + redResult * maxMask / 255.0;
            out[j + 1] = green * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0;
            out[j] = blue * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0;

            if(channel == 4){
                out[j+3] = in[j+3];
            }
        }
    }
    jobject resultBitmap = matToBitmapNew(env, result, true, bitmap);
    src.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 * 可选颜色修改色值
 * @param r
 * @param g
 * @param b
 * @param black
 * @param channelType
 * @param isRelative
 * @param sourceRed 原始像素值
 * @param sourceGreen 原始绿色通道像素值
 * @param sourceBlue 原始蓝色通道像素值
 * @param red
 * @param green
 * @param blue
 * @param max
 * @param mid
 * @param min
 */
void changeColor(float r, float g, float b, float black, int channelType, bool isRelative,int sourceRed,int sourceGreen,int sourceBlue,int &red,int &green, int &blue, int max, int mid, int min,bool is_relative) {
    float redPrecentUp = sourceRed / 256.0f;
    float redPrecentDown = 1 - redPrecentUp;
    float greenPrecentUp = sourceGreen / 256.0f;
    float greenPrecentDown = 1 - greenPrecentUp;
    float bluePrecentUp = sourceBlue / 256.0f;
    float bluePrecentDown = 1 - bluePrecentUp;
    float tmpR = 0.0f, tmpG = 0.0f, tmpB = 0.0f;
    if (r > 0 && r > redPrecentUp)
    {
        tmpR = redPrecentUp;
    }
    else if (r > 0 && r < redPrecentUp)
    {
        tmpR = r;
    }
    else if (r < 0 && abs(r) > redPrecentDown)
    {
        tmpR = -redPrecentDown;
    }
    else if (r < 0 && abs(r) < redPrecentDown)
    {
        tmpR = r;
    }
    if (g > 0 && g > greenPrecentUp)
    {
        tmpG = greenPrecentUp;
    }
    else if (g > 0 && g < greenPrecentUp)
    {
        tmpG = g;
    }
    else if (g < 0 && abs(g) > greenPrecentDown)
    {
        tmpG = -greenPrecentDown;
    }
    else if (g < 0 && abs(g) < greenPrecentDown)
    {
        tmpG = g;
    }
    if (b > 0 && b > bluePrecentUp)
    {
        tmpB = bluePrecentUp;
    }
    else if (b > 0 && b < bluePrecentUp)
    {
        tmpB = b;
    }
    else if (b < 0 && abs(b) > bluePrecentDown)
    {
        tmpB = -bluePrecentDown;
    }
    else if (b < 0 && abs(b) < bluePrecentDown)
    {
        tmpB = b;
    }
    if (!is_relative)
    {
        //计算可变范围
        int scope = max - mid;
        if (channelType == -1 || channelType == -2 || channelType == -3)
        {
            scope = mid - min;
        }
        else if (channelType == 4)
        {
            scope = min * 2 - 255;
        }
        else if (channelType == 5)
        {
            scope = 255 - min * 2;
        }
        else if (channelType == 6)
        {
            scope = abs(255 - max) * 2;
        }

        red = tmpR * scope * -1 + red + 0.5;
        green = tmpG * scope * -1 + green + 0.5;
        blue = tmpB * scope * -1 + blue + 0.5;
    }
    else
    {
        //计算可变范围
        int scope = max - mid;
        if (channelType == -1 || channelType == -2 || channelType == -3)
        {
            scope = mid - min;
        }
        else if (channelType == 4)
        {
            scope = min * 2 - 255;
        }
        else if (channelType == 5)
        {
            scope = 255 - max * 2;
        }
        else if (channelType == 6)
        {
            scope = abs(255 - max) * 2;
        }

        int maxRed = redPrecentDown * scope + red + 0.5;
        int maxGreen = greenPrecentDown * scope + green + 0.5;
        int maxBlue = bluePrecentDown * scope + blue + 0.5;
        int minRed = redPrecentUp * scope * -1 + red + 0.5;
        int minGreen = greenPrecentUp * scope * -1 + green + 0.5;
        int minBlue = bluePrecentUp * scope * -1 + blue + 0.5;

        if (sourceRed > 128)
        {
            if (r > 0)
            {
                scope = (maxRed - red) / redPrecentUp;
            }
        }
        if (r > 0)
        {
            red = (redPrecentUp * scope * -1) * r + red + 0.5;
        }
        else
        {
            red = (redPrecentDown * scope * -1) * r + red + 0.5;
        }

        if (sourceGreen > 128)
        {
            if (g > 0)
            {
                scope = (maxGreen - green) / greenPrecentUp;
            }
        }
        if (g > 0)
        {
            green = (greenPrecentUp * scope * -1) * g + green + 0.5;
        }
        else
        {
            green = (greenPrecentDown * scope * -1) * g + green + 0.5;
        }

        if (sourceBlue > 128)
        {
            if (b > 0)
            {
                scope = (maxBlue - blue) / bluePrecentUp;
            }
        }
        if (b > 0)
        {
            blue = (bluePrecentUp * scope * -1) * b + blue + 0.5;
        }
        else
        {
            blue = (bluePrecentDown * scope * -1) * b + blue + 0.5;
        }
    }
}

/**
 * 通道混合器
 * @param env
 * @param clazz
 * @param bitmap 输入图片
 * @param channelType 输出通道
 * @param redPrecent 红色通道百分比
 * @param greenPrecent 绿色通道百分比
 * @param bluePrecent 蓝色通道百分比
 * @param allPrecent 所有通道百分比
 */
jobject
mixColorChannel(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jfloat redChannelRed,
                jfloat redChannelGreen, jfloat redChannelBlue, jfloat greenChannelRed,
                jfloat greenChannelGreen, jfloat greenChannelBlue, jfloat blueChannelRed,
                jfloat blueChannelGreen, jfloat blueChannelBlue) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    Mat result(src.size(), src.type());
    int channel = src.channels();
    int widht = src.cols * channel;
    int height = src.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = src.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            int blue = in[j + 2];
            int green = in[j + 1];
            int red = in[j];

            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {
                out[j + 2] = blue;
                out[j + 1] = green;
                out[j] = red;
                if (channel == 4) {
                    out[j + 3] = in[j+3];
                }
                continue;
            }


            int redResult = red;
            int greenResult = green;
            int blueResult = blue;
            int max = MAX(MAX(red, green), blue);
            //设置百分比获取最终的三通道数值
            redResult = red * redChannelRed + green * redChannelGreen + blue * redChannelBlue;
            greenResult =
                    green * greenChannelGreen + red * greenChannelRed + blue * greenChannelBlue;
            blueResult = blue * blueChannelBlue + red * blueChannelRed + green * blueChannelGreen;

            redResult = red * (1 - maxMask / 255.0) + redResult * maxMask / 255.0;
            greenResult = green * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0;
            blueResult = blue * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0;

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0){
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0){
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0){
                blueResult = 0;
            }

            out[j + 2] = blueResult;
            out[j + 1] = greenResult;
            out[j + 0] = redResult;
            if (channel == 4) {
                out[j + 3] = in[j+3];
            }
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    src.release();
    maskMt.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}


/**
 *
 * @param input 期望输入值数组
 * @param output 期望输出值数组
 * @param pointSize 期望输入和输出值的个数，个数必须和inputArray，outputArray长度一致
 * @param RGB 需要修改的通道，0：全通道，1：红色通道，2：绿色通道，3：绿色通道
 *
 * */
jobject
curve(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,double *xArray, double *yArray, int pointSize,
      int RGB) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                      infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    Mat result(src.size(), src.type());
    CCubicSplineInterpolation cul(xArray, yArray, pointSize);
    int height = src.rows;
    int widht = src.cols * src.channels();
    int channel = src.channels();
    for (int i = 0; i < height; i++) {
        const uchar *in = src.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < widht; j = j + channel) {
            double r = in[j];
            double g = in[j + 1];
            double b = in[j + 2];
            double resultA = in[j + 3];
            double resultR = r;
            double resultG = g;
            double resultB = b;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0)
            {

                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                if (channel == 4)
                {
                    out[j + 3] = resultA;
                }
                continue;
            }

            if (RGB == 0) {
                cul.GetYByX(r, resultR);
                cul.GetYByX(g, resultG);
                cul.GetYByX(b, resultB);
            } else if (RGB == 1) {
                cul.GetYByX(r, resultR);
            } else if (RGB == 2) {
                cul.GetYByX(g, resultG);
            } else if (RGB == 3) {
                cul.GetYByX(b, resultB);
            }
            if (resultR > 255) {
                resultR = 255;
            } else if (resultR < 0) {
                resultR = 0;
            }
            if (resultG > 255) {
                resultG = 255;
            } else if (resultG < 0) {
                resultG = 0;
            }
            if (resultB > 255) {
                resultB = 255;
            } else if (resultB < 0) {
                resultB = 0;
            }
            out[j + 2] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
            out[j] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
            if (channel == 4) {
                out[j + 3] = resultA;
            }
        }
    }
    jobject resultBitmap = matToBitmapNew(env, result, true, bitmap);
    src.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 *
 * @param xRedArray 红色通道曲线x坐标
 * @param yRedArray 红色通道曲线y坐标
 * @param pointRedSize 红色通道坐标数组长度
 * @param xGreenArray 绿色通道曲线x坐标
 * @param yGreenArray 绿色通道曲线y坐标
 * @param pointGreenSize 绿色通道曲线坐标数组长度
 * @param xBlueArray 蓝色通道曲线x坐标
 * @param yBlueArray 蓝色通道曲线y坐标
 * @param pointBlueSize 蓝色通道曲线坐标数组长度
 * @param xRgbArray rgb三通道曲线x坐标
 * @param yRgbArray rgb三通道曲线y坐标
 * @param pointRgbSize rgb三通道曲线坐标数组长度
 * @param RGB 需要修改的通道，0：全通道，1：红色通道，2：绿色通道，3：绿色通道
 *
 * */
jobject
curve(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, double *xRedArray, double *yRedArray,
      int pointRedSize, double *xGreenArray, double *yGreenArray, int pointGreenSize,
      double *xBlueArray,
      double *yBlueArray, int pointBlueSize, double *xRgbArray,
      double *yRgbArray, int pointRgbSize) {
    //Mat src = imread("/storage/emulated/0/2.jpg", IMREAD_UNCHANGED);
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);
    cvtColor(src,src,CV_RGBA2RGB);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,CV_RGBA2RGB);

    Mat result = src.clone();
    cvtColor(result,result,CV_RGBA2RGB);

    CCubicSplineInterpolation culRed(xRedArray, yRedArray, pointRedSize);
    CCubicSplineInterpolation culGreen(xGreenArray, yGreenArray, pointGreenSize);
    CCubicSplineInterpolation culBlue(xBlueArray, yBlueArray, pointBlueSize);
    CCubicSplineInterpolation culRgb(xRgbArray, yRgbArray, pointRgbSize);
    int height = src.rows;
    int widht = src.cols ;
    int channel = src.channels();

    uchar pR[256];
    for (int i = 0; i < 256; i++)
    {
        double out = 0;
        culRgb.GetYByX(i, out);
        culRed.GetYByX(out, out);
        if (out > 255){
            out = 255;
        }else if (out < 0){
            out = 0;
        }
        pR[i] = out;
    }
    Mat lutR(1, 256, CV_8UC1, pR);

    uchar pG[256];
    for (int i = 0; i < 256; i++)
    {
        double out = 0;
        culRgb.GetYByX(i, out);
        culGreen.GetYByX(out, out);
        if (out > 255){
            out = 255;
        }else if (out < 0){
            out = 0;
        }
        pG[i] = out;
    }
    Mat lutG(1, 256, CV_8UC1, pG);

    uchar pB[256];
    for (int i = 0; i < 256; i++)
    {
        double out = 0;
        culRgb.GetYByX(i, out);
        culBlue.GetYByX(out, out);
        if (out > 255){
            out = 255;
        }else if (out < 0){
            out = 0;
        }
        pB[i] = out;
    }
    Mat lutB(1, 256, CV_8UC1, pB);

    vector<Mat> mergeMats;
    mergeMats.push_back(lutR);
    mergeMats.push_back(lutG);
    mergeMats.push_back(lutB);
    Mat lutResult(1,256,CV_8UC3);
    cvtColor(lutResult,lutResult,CV_BGR2RGB);
    merge(mergeMats, lutResult);
    LUT(result,lutResult,result);
    lutResult.release();
    lutR.release();
    lutG.release();
    lutB.release();

//    cvtColor(lutResult,lutResult,CV_RGB2BGR);
    for (int i = 0; i < height; i++) {
        const uchar *in = src.data + i * src.step;
        uchar *out = result.data + i * result.step;
        const uchar *maskIn = maskMt.data + i * maskMt.step;

        for (int j = 0; j < widht; j++) {
            uchar r = in[0];
            uchar g = in[1];
            uchar b = in[2];
            uchar resultR = out[0];
            uchar resultG = out[1];
            uchar resultB = out[2];
            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                out[2] = b;
                out[1] = g;
                out[0] = r;
                in = in+3;
                out = out+3;
                maskIn = maskIn+3;
                continue;
            }
            int aa = b * (1 - maxMask / 255.0) + resultB * (maxMask / 255.0);
            int bb = g * (1 - maxMask / 255.0) + resultG * (maxMask / 255.0);
            int cc = r * (1 - maxMask / 255.0) + resultR * (maxMask / 255.0);
            out[2] = b * (1 - maxMask / 255.0) + resultB * (maxMask / 255.0);
            out[1] = g * (1 - maxMask / 255.0) + resultG * (maxMask / 255.0);
            out[0] = r * (1 - maxMask / 255.0) + resultR * (maxMask / 255.0);
            in = in+3;
            out = out+3;
            maskIn = maskIn+3;
        }
    }
    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmapNew(env, result, true, bitmap);
    src.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}


/**
 *
 * 手动液化方案
 * @param centerPoint 液化的中心点,即鼠标或手指点击屏幕时候的位置
 * @param movePoint 液化终点位置，即鼠标或手指抬起时的位置
 * @param radio 液化半径
 * @param stregth 液化强度
 * 公式：液化后的点 = 当前点-(液化半径^2-|当前点-液化中心点|^2)/( (液化半径^2-|当前点-液化中心点|^2)+|移动终点-液化中心点|^2 )^2*(移动终点-液化中心点)
 *
 * */
jobject deformation(JNIEnv *env, jclass clazz, jobject bitmap, Point centerPoint, Point movePoint,
                    int radio, int str) {
    Mat src = imread("/storage/emulated/0/2.jpg", IMREAD_UNCHANGED);
    Mat result = src.clone();
    Mat read = src.clone();
    int height = src.rows;
    int width = src.cols * src.channels();
    int channel = src.channels();
    int centerX = centerPoint.x;
    int centerY = centerPoint.y;
    int moveX = movePoint.x;
    int moveY = movePoint.y;
    int rMax = radio;
    int strength = str;
    //最大强度设置为100
    if (strength > 100) {
        strength = 100;
    }
    const uchar *tempData = read.ptr<uchar>();
    centerX = centerX * channel;
    moveX = moveX * channel;
    for (int i = 0; i < height; i++) {
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + channel) {
            float dis = sqrt((float) (centerX - (j)) / channel * (centerX - (j)) / channel +
                             (centerY - i) * (centerY - i));
            if (dis < rMax) {

                float xc = pow(j / channel - centerX / channel, 2) + pow(i - centerY, 2);
                float mc = pow(moveX / channel - centerX / channel, 2) + pow(moveY - centerY, 2);
                float tx = (pow(rMax, 2) - xc);
                float d = tx / (tx + mc * 100.0f / strength);
                d = d * d;
                int px = CLIP3(j / channel -
                               (moveX / channel - centerX / channel) * d * (1.0f - dis / rMax), 0,
                               width - channel);
                int py = CLIP3(i - (moveY - centerY) * d * (1.0f - dis / rMax), 0, height - 1);
                int pos = py * width + px * channel;
                if (pos > width * height) {
                    continue;
                }
                out[j] = tempData[pos];
                out[j + 1] = tempData[pos + 1];
                out[j + 2] = tempData[pos + 2];
                if (channel == 4) {
                    out[j + 3] = tempData[pos + 3];
                }
            }
        }
    }
    jobject resultBitmap = matToBitmapNew(env, result, true, bitmap);
    src.release();
    result.release();
    //AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 * usm锐化
 * radius：锐化半径
 * number:锐化数量
 * threshold:锐化阈值
 *
 * */
jobject
usmFilter(JNIEnv *env, jclass clazz, jobject bitmap,jobject maskBitmap,int radius, int amount, int threshold) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat src(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, maskBitmap, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, maskBitmap, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    radius = radius * 5;
    int ret = 0;
    radius = CLIP3(radius, 0, 100);
    amount = CLIP3(amount, 0,500);
    threshold = CLIP3(threshold, 0,255);
    if(radius%2 == 0){
        radius = radius + 1;
    }
    Mat dst(src.size(),src.type());
    Mat mask(src.size(),src.type());
    int width = src.cols*src.channels();
    int height = src.rows;
    int channel = src.channels();
    Mat tmpScaleSrc;
    resize(src,tmpScaleSrc, Size(src.cols * 0.5, src.rows * 0.5));
    GaussianBlur(tmpScaleSrc, tmpScaleSrc, cv::Size(radius, radius), 0);
    resize(tmpScaleSrc,dst, Size(tmpScaleSrc.cols/0.5, tmpScaleSrc.rows/0.5));
    tmpScaleSrc.release();

    int i, j, r, g, b, offset;
    amount = amount * 128 / 100;
    for(j = 0; j < height; j++)
    {
        const uchar *in = src.ptr<uchar>(j);
        const uchar *ds = dst.ptr<uchar>(j);
        uchar *ms = mask.ptr<uchar>(j);
        for(i = 0; i < width; i = i + channel)
        {
            ms[i + 0] = abs(in[i + 0] - ds[i + 0]) < threshold ? 0 : 128;
            ms[i + 1] = abs(in[i + 1] - ds[i + 1]) < threshold ? 0 : 128;
            ms[i + 2] = abs(in[i + 2] - ds[i + 2]) < threshold ? 0 : 128;
            if(channel == 4){
                ms[i + 3] = in[i + 3];
            }
        }
    }

    Mat tmpMaskSrc;
    resize(mask,tmpMaskSrc, Size(src.cols * 0.5, src.rows * 0.5));
    GaussianBlur(tmpMaskSrc, tmpMaskSrc, cv::Size(radius, radius), 0.5, 0.5);
    resize(tmpMaskSrc,mask, Size(tmpMaskSrc.cols/0.5, tmpMaskSrc.rows/0.5));
    tmpMaskSrc.release();

    Mat tmp = src.clone();
    for(j = 0; j < height; j++)
    {
        uchar *in = tmp.ptr<uchar>(j);
        const uchar *ds = dst.ptr<uchar>(j);
        uchar *ms = mask.ptr<uchar>(j);
        const uchar *maskIn = maskMt.ptr<uchar>(j);
        for(i = 0; i < width; i = i + channel)
        {
            uchar maxMask = MAX(maskIn[i + 0], MAX(maskIn[i + 1], maskIn[i + 2]));
            uchar tmpR = in[i+2];
            uchar tmpG = in[i+1];
            uchar tmpB = in[i];
            if (maxMask == 0)
            {
                in[i + 2] = tmpR;
                in[i + 1] = tmpG;
                in[i] = tmpB;
                continue;
            }

            b = in[i + 0] - ds[i + 0];
            g = in[i + 1] - ds[i + 1];
            r = in[i + 2] - ds[i + 2];

            b = (in[i + 0] + ((b * amount) >> 7));
            g = (in[i + 1] + ((g * amount) >> 7));
            r = (in[i + 2] + ((r * amount) >> 7));

            b = (b * ms[i + 0] + in[i+ 0] * (128 - ms[i+ 0])) >> 7;
            g = (g * ms[i + 1] + in[i+ 1] * (128 - ms[i + 1])) >> 7;
            r = (r * ms[i + 2] + in[i+ 2] * (128 - ms[i + 2])) >> 7;

            in[i + 0] = CLIP3(b, 0, 255);
            in[i + 1] = CLIP3(g, 0, 255);
            in[i + 2] = CLIP3(r, 0, 255);
        }
    }
    jobject resultBitmap = matToBitmap(env, tmp, true, bitmap);
    src.release();
    dst.release();
    tmp.release();
    mask.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, maskBitmap);
    return resultBitmap;
}

/**
 *
 *修改图片饱和度
 **/
jobject toChangeHSV_H(JNIEnv *env, jclass clazz, jobject bitmap, int saturation) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    Mat result(image.size(), image.type());
    if (saturation == 0) {
        return bitmap;
    }
    int r, g, b, rgbMin, rgbMax;
    saturation = CLIP3(saturation, -100, 100);
    int k = saturation / 100.0f * 128;
    int alpha = 0;
    int height = image.rows;
    int width = image.cols * image.channels();
    for (int i = 0; i < height; i++) {
        const uchar *in = image.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j++) {
            b = in[j * image.channels()];
            g = in[j * image.channels() + 1];
            r = in[j * image.channels() + 2];
            rgbMin = MIN2(MIN2(r, g), b);
            rgbMax = MAX2(MAX2(r, g), b);
            int delta = (rgbMax - rgbMin);
            int value = (rgbMax + rgbMin);
            if (delta == 0) {
                out[j * image.channels()] = in[j * image.channels()];
                out[j * image.channels() + 1] = in[j * image.channels() + 1];
                out[j * image.channels() + 2] = in[j * image.channels() + 2];
                if (image.channels() == 4) {
                    out[j * image.channels() + 3] = in[j * image.channels() + 3];
                }
                continue;
            }
            int l = value >> 1;
            int s = l < 128 ? (delta << 7) / value : (delta << 7) / (510 - value);
            if (k >= 0) {
                alpha = k + s >= 128 ? s : 128 - k;
                alpha = 128 * 128 / alpha - 128;
            } else {
                alpha = k;
            }
            r = r + ((r - l) * alpha >> 7);
            g = g + ((g - l) * alpha >> 7);
            b = b + ((b - l) * alpha >> 7);
            out[j * image.channels()] = CLIP3(b, 0, 255);
            out[j * image.channels() + 1] = CLIP3(g, 0, 255);
            out[j * image.channels() + 2] = CLIP3(r, 0, 255);
            if (image.channels() == 4) {
                out[j * image.channels() + 3] = in[j * image.channels() + 3];
            }
        }
    }
    jobject resultBitmap = matToBitmapNew(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 * 修改色相、饱和度、明度
 * @param env
 * @param clazz
 * @param bitmap
 * @param hue
 * @param saturation
 * @param brightness
 * @return
 */
jobject toChangeHsl(JNIEnv *env, jclass clazz, jobject bitmap,int hue, int saturation,int brightness,int colorChannel){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    Mat result(image.size(),image.type());
    HSL hsl;
    hsl.channels[colorChannel].hue = hue;
    hsl.channels[colorChannel].saturation = saturation;
    hsl.channels[colorChannel].brightness = brightness;
    hsl.adjust(image,result);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 *
 * 获取某个位置上的颜色值
 *
 **/
jintArray getPixelColor(JNIEnv *env, jclass clazz, jobject bitmap,int x,int y){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    //x = 25;
    //y = 100;
    if (y >= image.rows){
        LOGI("超出范围");
    }
    uchar *in = image.ptr<uchar>(y);
    int red = in[x*image.channels()+0];
    int green = in[x*image.channels()+1];
    int blue = in[x*image.channels()+2];
    LOGI("输出%d",red);
    jintArray colorArray = env->NewIntArray(4);;
    jint *arr = env->GetIntArrayElements(colorArray, NULL);
    arr[0] = red;
    arr[1] = green;
    arr[2] = blue;
    arr[3] = 255;
    env->ReleaseIntArrayElements(colorArray, arr, 0);
    image.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return colorArray;
}

/**
 * 旋转图片
 * @param env
 * @param clazz
 * @param bitmap
 * @param x
 * @param y
 * @return
 */
jobject getRotationBitmap(JNIEnv *env, jclass clazz, jobject bitmap,int angle){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    Mat result;
    //设置旋转中心
    Point2f center(image.cols / 2, image.rows / 2);
    //设置旋转角度
    Mat M = getRotationMatrix2D(center, angle, 1);
    warpAffine(image, result, M, Size(image.cols, image.rows),INTER_CUBIC,BORDER_TRANSPARENT);
    //warpAffine(image, result, M, Size(image.cols, image.rows));

    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}


/**
 * 旋转图片
 * @param env
 * @param clazz
 * @param bitmap
 * @param x
 * @param y
 * @return
 */
jobject getRotationBitmapResize(JNIEnv *env, jclass clazz, jobject bitmap,int degree){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    Mat result;
    //设置旋转中心
    Point2f center(image.cols / 2, image.rows / 2);
    int newW = cos(degree*3.14159/180.0)*image.cols;
    int newH = tan(degree*3.14159/180.0)*newW;
    LOGI("原始宽高为：%d,%d",image.cols, image.rows);
    LOGI("修改宽高为：%d,%d",newW,newH);
    //设置旋转角度
    Mat M = getRotationMatrix2D(center, degree, 1);
    warpAffine(image, result, M, Size(newW, newH),INTER_CUBIC,BORDER_TRANSPARENT);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}


/**
 *
 * 翻转图像，以Y轴翻转或X轴翻转
 *
 **/
jobject makeBitmapRollingXY(JNIEnv *env, jclass clazz, jobject bitmap,jboolean isY)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cv::Mat out(image.size(), image.type());
    int height = image.rows;
    int width = image.cols * image.channels();
    if (isY){
        flip(image, out, 1);
    }else{
        flip(image, out, 0);
    }
    /*for (int i = 0; i < height; i++)
    {
        if (isY)
        {
            const uchar *in = image.ptr<uchar>(i);
            uchar *outPiex = out.ptr<uchar>(i);
            for (int j = 0; j < width; j++)
            {
                //这里需要注意，如果直接翻转，没有进行通道处理，那么最后输出的图片通道和原有图片是相反的，因为翻转过程中，通道也一并被翻转了
                if (image.channels() == 3)
                {
                    if ((width - 1) - j * 3 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 3] = in[j * 3 + 2];
                    if ((width - 1) - j * 3 - 1 < 0)
                    {
                        break;
                    }

                    outPiex[(width - 1) - j * 3 - 1] = in[j * 3 + 1];
                    if ((width - 1) - j * 3 - 1 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 3 - 2] = in[j * 3];
                }
                else if (image.channels() == 4)
                {
                    if ((width - 1) - j * 4 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 4] = in[j * 4 + 3];
                    if ((width - 1) - j * 4 - 1 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 4 - 1] = in[j * 4 + 2];
                    if ((width - 1) - j * 4 - 2 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 4 - 2] = in[j * 4 + 1];
                    if ((width - 1) - j * 4 - 3 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 4 - 3] = in[j * 4];
                }
                else if (image.channels() == 2)
                {
                    if ((width - 1) - j * 2 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 2] = in[j * 2 + 1];
                    if ((width - 1) - j * 2 - 1 < 0)
                    {
                        break;
                    }
                    outPiex[(width - 1) - j * 2 - 1] = in[j * 2];
                }
                else if (image.channels() == 1)
                {
                    outPiex[(width - 1) - j] = in[j];
                }
            }
            //flip(image, out, 1);
        }
        else
        {
            int index = (height - 1) - i;
            const uchar *in = image.ptr<uchar>(index);
            uchar *outPiex = out.ptr<uchar>(i);
            for (int j = 0; j < width; j++)
            {
                outPiex[j + 0] = in[j + 0];
                outPiex[j + 1] = in[j + 1];
                outPiex[j + 2] = in[j + 2];
            }
//            flip(image, out, 1);
        }
    }*/

    jobject resultBitmap = matToBitmap(env, out, true, bitmap);
    image.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

jobject readImage(JNIEnv *env, jclass clazz, char *path,jobject bitmap,jint maxWidth){
    //LOGE("执行ndk读入%s",path);
    Mat result = imread(path,cv::IMREAD_UNCHANGED);
    if (result.empty()){
        return nullptr;
    }
    cvtColor(result,result,COLOR_BGR2RGB);
    if (maxWidth > 0){
        float scale = maxWidth/(result.cols*1.0f);
        Size dsize = Size(result.cols * scale, result.rows * scale);
        resize(result,result,dsize);
    }
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    result.release();
    return resultBitmap;
}

/**
 * 自动对比度
 * @param env
 * @param clazz
 * @param bitmap
 * @return
 */
jobject autoContrast(JNIEnv *env, jclass clazz, jobject bitmap){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cv::Mat result(image.size(),image.type());
    //cv::equalizeHist(image,result);
    cv::Mat *list;
    cv::split(image,list);
    result = list[0];
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 * @brief 调整色温
 *
 */
jobject ChangeTemp(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint percent)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat input(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cv::Mat result = input.clone();
    cvtColor(input,input,CV_RGBA2RGB);
    cvtColor(maskMt,maskMt,CV_RGBA2RGB);
    cvtColor(result,result,CV_RGBA2RGB);
    int row = result.rows;
    int col = result.cols;
    int level = percent / 2;
    for (int i = 0; i < row; i++)
    {
        uchar *out = result.data + i * result.step;
        const uchar *maskIn = maskMt.data + i * maskMt.step;

        for (int j = 0; j < col; j++)
        {
            uchar rTmp = out[0];
            uchar gTmp = out[1];
            uchar bTmp = out[2];

            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                out[2] = bTmp;
                out[1] = gTmp;
                out[0] = rTmp;
                out = out +3 ;
                maskIn = maskIn +3 ;
                continue;
            }

            int r = rTmp*(1-maxMask/255.0)+(out[0] + level)*maxMask/255.0;
            int g = gTmp*(1-maxMask/255.0)+(out[1] + level)*maxMask/255.0;
            int b = bTmp*(1-maxMask/255.0)+(out[2] - level)*maxMask/255.0;

            if ( r > 255){
                r = 255;
            }else if (r < 0){
                r = 0;
            }

            if ( g > 255){
                g = 255;
            }else if (g < 0){
                g = 0;
            }

            if ( b > 255){
                b = 255;
            }else if (b < 0){
                b = 0;
            }

            out[2] = b;
            out[1] = g;
            out[0] = r;
            out = out +3 ;
            maskIn = maskIn +3 ;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    input.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}


/**
 * @brief 调整色温
 *
 */
jobject ChangeColorHue(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint percent)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat input(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cv::Mat result = input.clone();
    cvtColor(input,input,CV_RGBA2RGB);
    cvtColor(maskMt,maskMt,CV_RGBA2RGB);
    cvtColor(result,result,CV_RGBA2RGB);
    int row = result.rows;
    int col = result.cols * result.channels();
    int level = percent / 2;
    for (int i = 0; i < row; i++)
    {
        uchar *out = result.data + i * result.step;
        const uchar *maskIn = maskMt.data + i * maskMt.step;
        for (int j = 0; j < col; j = j + result.channels())
        {
            uchar rTmp = out[0];
            uchar gTmp = out[1];
            uchar bTmp = out[2];

            uchar maxMask = MAX(maskIn[0], MAX(maskIn[1], maskIn[2]));
            if (maxMask == 0)
            {
                out[2] = bTmp;
                out[1] = gTmp;
                out[0] = rTmp;
                out = out +3 ;
                maskIn = maskIn +3 ;
                continue;
            }

            int r = rTmp*(1-maxMask/255.0)+(out[0] + level)*maxMask/255.0;
            int g = gTmp*(1-maxMask/255.0)+(out[1] - level)*maxMask/255.0;
            int b = bTmp*(1-maxMask/255.0)+(out[2] + level)*maxMask/255.0;

            if ( r > 255){
                r = 255;
            }else if (r < 0){
                r = 0;
            }

            if ( g > 255){
                g = 255;
            }else if (g < 0){
                g = 0;
            }

            if ( b > 255){
                b = 255;
            }else if (b < 0){
                b = 0;
            }
            out[0] = r;
            out[1] = g;
            out[2] = b;
            out = out +3 ;
            maskIn = maskIn +3 ;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    input.release();
    result.release();
    maskMt.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}

/**
 * 降噪处理
 * @param env
 * @param clazz
 * @param bitmap
 * @param strength
 * @return
 */
jobject denoise(JNIEnv *env, jclass clazz, jobject bitmap,jfloat strength)
{
    if (strength <= 0){
        return bitmap;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat input(info.height, info.width, CV_8UC4, pixels);
    cv::Mat result = input.clone();
    cvtColor(result,result,CV_RGBA2RGB);
    if (strength > 0 && strength <=25)
    {
        fastNlMeansDenoisingColored(result, result, 0.55f*strength, 10, 3, 21);
    }
    else if (strength > 25 && strength <=55)
    {
        fastNlMeansDenoisingColored(result, result, 0.4f*strength, 10, 5, 35);
    }
    else
    {
        fastNlMeansDenoisingColored(result, result, 0.35f*strength, 10, 7, 35);
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    input.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

/**
 * 自然饱和度
 * @param env
 * @param clazz
 * @param bitmap
 * @param strength
 * @return
 */
jobject VibranceAlgorithm(JNIEnv *env, jclass clazz, jobject bitmap,jint strength)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat input(info.height, info.width, CV_8UC4, pixels);
    cvtColor(input,input,CV_RGBA2RGB);
    cv::Mat result = input.clone();
//    cvtColor(result,result,CV_RGBA2BGR);
    unsigned char *Src = input.data;
    unsigned char *Dest = result.data;
    int Height = result.rows;
    int Width = result.cols;
    int Stride = result.step;
    int Adjustment = strength;
    int VibranceAdjustment = -1.28 * Adjustment;
    for (int Y = 0; Y < Height; Y++)
    {
        unsigned char *LinePS = Src + Y * Stride;
        unsigned char *LinePD = Dest + Y * Stride;
        for (int X = 0; X < Width; X++)
        {
            int Blue, Green, Red, Max;
            Blue = LinePS[0], Green = LinePS[1], Red = LinePS[2];
            int Avg = (Blue + Green + Green + Red) >> 2;
            if (Blue > Green)
                Max = Blue;
            else
                Max = Green;
            if (Red > Max)
                Max = Red;
            int AmtVal = (Max - Avg) * VibranceAdjustment;
            if (Blue != Max)
                Blue += (((Max - Blue) * AmtVal) >> 14);
            if (Green != Max)
                Green += (((Max - Green) * AmtVal) >> 14);
            if (Red != Max)
                Red += (((Max - Red) * AmtVal) >> 14);
            if (Red < 0)
                Red = 0;
            else if (Red > 255)
                Red = 255;
            if (Green < 0)
                Green = 0;
            else if (Green > 255)
                Green = 255;
            if (Blue < 0)
                Blue = 0;
            else if (Blue > 255)
                Blue = 255;
            LinePD[0] = Blue;
            LinePD[1] = Green;
            LinePD[2] = Red;
            LinePS += 3;
            LinePD += 3;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    input.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

jobject getSigleColorPhoto(JNIEnv *env, jclass clazz, jobject bitmap, jint r,jint g,jint b){
    if (r > 255){
        r = 255;
    }else if (r < 0){
        r = 0;
    }
    if (g > 255){
        g = 255;
    }else if (g < 0){
        g = 0;
    }
    if (b > 255){
        b = 255;
    }else if (b < 0){
        b = 0;
    }
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2BGR);
    cv::Mat result = image.clone();
    cvtColor(result,result,CV_RGBA2BGR);
    int height = image.rows;
    int width = image.cols * image.channels();
    for (int i = 0; i < height; i++)
    {
        uchar *out = result.ptr<uchar>(i);
        for (int j = 0; j < width; j = j + result.channels())
        {
            out[j+0] = r;
            out[j+1] = g;
            out[j+2] = b;
        }
    }
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    image.release();
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

jobject fixImage(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    cvtColor(image,image,CV_RGBA2BGR);

    cv::Mat result(image.size(),image.type());
    cvtColor(result,result,CV_RGBA2BGR);


    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt,maskMt,CV_RGBA2GRAY);

    cv::inpaint(image,maskMt,result,9,INPAINT_NS);

    cvtColor(result,result,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    result.release();
    maskMt.release();
    image.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    return resultBitmap;
}


/**
 * @brief 去除紫边
 * @param borderCount 紫边数量，即检测核大小
 * @param colorH 色相范围
 * @param isPurple 是否是紫边，如果不是紫边就是绿边
 *
 */
jobject removeBorder(JNIEnv *env, jclass clazz, jobject bitmap,jint borderCount = 10,jint startH = 210,jint endH = 350)
{
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img,img,CV_RGBA2BGR);
    cv::Mat result = img.clone();
    cvtColor(result,result,CV_RGBA2BGR);
    Mat border = img.clone();
    cvtColor(border, border, COLOR_BGR2GRAY);
    Canny(border, border, 100, 200);
    int channel = img.channels();
    int widht = channel * img.cols;
    int height = img.rows;
    cvtColor(border, border, COLOR_GRAY2BGR);
    for (int i = 0; i < height; i++)
    {
        const uchar *inTop = img.ptr<uchar>(i);
        const uchar *inBorder = border.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);

        for (int j = 0; j < widht; j = j + channel)
        {
            uchar r = inTop[j + 2];
            uchar g = inTop[j + 1];
            uchar b = inTop[j + 0];

            uchar rB = inBorder[j + 2];
            uchar gB = inBorder[j + 1];
            uchar bB = inBorder[j + 0];

            float h, s, B;
            RGB2HSB(r, g, b, &h, &s, &B);
            if (h > startH && h < endH && s > 0.05 && s < 1 && B > 0.2 && B < 1 && (r != g && g != b) && rB != 0)
            {
                int leftRightCount = borderCount;
                int checkCount = 50;
                int checkBriness = 2;
                float sutableH = -1;
                float sutableS = -1;
                float sutableB = -1;

                int brinessLeft = -1;
                int brinessLeftTop = -1;
                int brinessRight = -1;
                int brinessRightTop = -1;
                int brinessTop = -1;
                int brinessBottom = -1;
                int brinessBottomLeft = -1;
                int brinessBottomRigt = -1;
                int maxBriness = -1;

                //获取上下左右点的亮度
                for (int iTmp = (checkBriness / 2 - checkBriness); iTmp < abs((checkBriness / 2 - checkBriness)); iTmp++)
                {
                    if (iTmp + i < 0 || iTmp + 1 > height)
                    {
                        break;
                    }
                    uchar *inTopTmp = img.ptr<uchar>(iTmp + i);
                    //左右取N列
                    for (int jTmp = checkBriness / 2 - checkBriness; jTmp < abs((checkBriness / 2 - checkBriness)); jTmp++)
                    {
                        uchar rTmp = inTopTmp[j + jTmp * 3 + 2];
                        uchar gTmp = inTopTmp[j + jTmp * 3 + 1];
                        uchar bTmp = inTopTmp[j + jTmp * 3 + 0];
                        if (iTmp == -1 && jTmp == -1)
                        {
                            //左上角
                            brinessLeftTop = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == -1 && jTmp == 0)
                        {
                            //上
                            brinessTop = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == -1 && jTmp == 1)
                        {
                            //右上
                            brinessRightTop = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == 0 && jTmp == -1)
                        {
                            //左
                            brinessLeft = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == 0 && jTmp == 1)
                        {
                            //右
                            brinessRight = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == 1 && jTmp == -1)
                        {
                            //左下
                            brinessBottomLeft = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == 1 && jTmp == 0)
                        {
                            //下
                            brinessBottom = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                        else if (iTmp == 1 && jTmp == 1)
                        {
                            //右下
                            brinessBottomRigt = MAX(rTmp, MAX(gTmp, bTmp));
                        }
                    }
                }
                maxBriness = MAX(brinessLeftTop, MAX(brinessTop, MAX(brinessRightTop, MAX(brinessLeft, MAX(brinessRight, MAX(brinessBottomLeft, MAX(brinessBottom, brinessBottomRigt)))))));

                //寻找合适的色相
                /*for (int iTmp = (checkCount / 2 - checkCount); iTmp < abs((checkCount / 2 - checkCount)); iTmp++)
                {
                    if (iTmp + i < 0 || iTmp + 1 > height)
                    {
                        break;
                    }
                    if (sutableH != -1)
                    {
                        break;
                    }
                    uchar *inTopTmp = img.ptr<uchar>(iTmp + i);
                    for (int jTmp = checkCount / 2 - checkCount; jTmp < abs((checkCount / 2 - checkCount)); jTmp++)
                    {
                        if (j + jTmp * 3 < 0 || j + jTmp * 3 > widht)
                        {
                            break;
                        }
                        uchar rTmp = inTopTmp[j + jTmp * 3 + 2];
                        uchar gTmp = inTopTmp[j + jTmp * 3 + 1];
                        uchar bTmp = inTopTmp[j + jTmp * 3 + 0];
                        RGB2HSB(rTmp, gTmp, bTmp, &h, &s, &B);
                        if ((h < 280 - 70 || h > 280 + 70) && B < maxBriness / 255.0 && (rTmp != gTmp && gTmp != bTmp))
                        {
                            sutableH = h;
                            sutableB = B;
                            sutableS = s;
                            break;
                        }
                    }
                }*/

                //上下取N行
                for (int iTmp = (leftRightCount / 2 - leftRightCount); iTmp < abs((leftRightCount / 2 - leftRightCount)); iTmp++)
                {
                    if (iTmp + i < 0 || iTmp + 1 > height)
                    {
                        break;
                    }
                    uchar *inTopTmp = img.ptr<uchar>(iTmp + i);
                    uchar *outResultTmp = result.ptr<uchar>(iTmp + i);
                    //左右取N列
                    for (int jTmp = leftRightCount / 2 - leftRightCount; jTmp < abs((leftRightCount / 2 - leftRightCount)); jTmp++)
                    {
                        if (j + jTmp * 3 < 0 || j + jTmp * 3 > widht)
                        {
                            break;
                        }
                        uchar rTmp = inTopTmp[j + jTmp * 3 + 2];
                        uchar gTmp = inTopTmp[j + jTmp * 3 + 1];
                        uchar bTmp = inTopTmp[j + jTmp * 3 + 0];
                        int mid = rTmp + gTmp + bTmp - MIN(rTmp, MIN(gTmp, bTmp)) - MAX(rTmp, MAX(gTmp, bTmp));
                        RGB2HSB(rTmp, gTmp, bTmp, &h, &s, &B);
                        if (h > startH && h < endH && s > 0.05 && s < 1 && B > 0.2 && B < 1 && (rTmp != gTmp && gTmp != bTmp))
                        {
                            /*outResultTmp[j + jTmp * 3] = mid;
                            outResultTmp[j + jTmp * 3 + 1] = mid;
                            outResultTmp[j + jTmp * 3 + 2] = mid;*/
                            s = 0;
                            float aaa[3] = {h,s,B};
                            uchar bgr[3] = {bTmp,gTmp,rTmp};
                            HSB2BGR(aaa,bgr);
                            outResultTmp[j + jTmp * 3] = bgr[0];
                            outResultTmp[j + jTmp * 3 + 1] = bgr[1];
                            outResultTmp[j + jTmp * 3 + 2] = bgr[2];
                        }
                    }
                }
            }
        }
    }

    //cvtColor(result,result,CV_BGR2RGB);
    cvtColor(img,img,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    result.release();
    img.release();
    border.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    return resultBitmap;
}

