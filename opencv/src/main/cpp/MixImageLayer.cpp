//
// Created by libowu on 6/12/22.
//

#include "MixImageLayer.h"
#include "Mask.h"
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <iostream>
#include <fstream>
#include <opencv2/utils/utils.h>
#include <opencv2/hsl/ColorSpace.hpp>

using namespace std;
using namespace cv;
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

jobject darkCorner(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jboolean isBlack,
                   jfloat strength) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img, img, COLOR_RGBA2BGR);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGR);
    Mat result(img.size(), img.type());
    uchar color = 0;
    if (!isBlack) {
        color = 255;
    }
    Mat m(img.size(), img.type(), Scalar(color, color, color));
    uchar channel = img.channels();
    int width = channel * img.cols;
    int height = img.rows;
    if (strength == 1) {
        int a = 0;
    } else if (strength == 0) {
        int a = 1;
    }
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        const uchar *mIn = m.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);

        for (int j = 0; j < width; j = j + channel) {
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];
            int maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            maxMask = maxMask * (pow(strength, exp((255 - maxMask) / 255)));
            if (maxMask < 0) {
                maxMask = 0;
            }
            if (maxMask == 0) {
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            uchar resultR = mIn[j + 2];
            uchar resultG = mIn[j + 1];
            uchar resultB = mIn[j + 0];

            out[j + 2] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
            out[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
            out[j] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
        }
    }

    img.release();
    maskMt.release();
    cvtColor(result, result, COLOR_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    result.release();
    m.release();
    return resultBitmap;
}

jobject
normal(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jobject join, jint left, jint top,
       float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img, img, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, join, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, join, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat maskJoin(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(maskJoin, maskJoin, COLOR_RGBA2BGRA);

    Mat result(img.size(), CV_8UC4);
    uchar channel = img.channels();
    int width = channel * img.cols;
    int height = img.rows;
    int widthMix = maskJoin.cols * maskJoin.channels();
    int heightMix = maskJoin.rows;
    int heightIndex = top;
    int leftIndex = left;
    bool isNone = false;
    for (int i = 0; i < height; i++) {
        const uchar *in = img.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        uchar *out = result.ptr<uchar>(i);
        uchar *a = nullptr;
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                a = maskJoin.ptr<uchar>(i + abs(top));
            } else {
                a = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                a = maskJoin.ptr<uchar>(i - top);
            } else {
                a = nullptr;
            }
        }
        for (int j = 0; j < width; j = j + channel) {
            uchar r = in[j + 2];
            uchar g = in[j + 1];
            uchar b = in[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                out[j + 3] = alpha;
                out[j + 2] = r;
                out[j + 1] = g;
                out[j] = b;
                continue;
            }
            int resultR = r;
            int resultG = g;
            int resultB = b;
            if (a != nullptr) {
                if (left <= 0 && a != nullptr && j + abs(left * channel) + 2 < widthMix) {
                    float weight = a[j + abs(left * channel) + 3] / 255.0;
                    if (a[j + abs(left * channel) + 3] != 0) {
                        resultB = (1 - weight) * in[j + 0] + a[j + abs(left * channel) + 0];
                        resultG = (1 - weight) * in[j + 1] + a[j + abs(left * channel) + 1];
                        resultR = (1 - weight) * in[j + 2] + a[j + abs(left * channel) + 2];
                        if (resultB > 255) {
                            resultB = 255;
                        }
                        if (resultG > 255) {
                            resultG = 255;
                        }
                        if (resultR > 255) {
                            resultR = 255;
                        }
                    }
                } else if (left > 0 && a != nullptr && j >= left * channel &&
                           j < left * channel + widthMix) {
                    float weight = a[j - abs(left * channel) + 3] / 255.0;
                    if (a[j - abs(left * channel) + 3] != 0) {
                        resultB = (1 - weight) * in[j + 0] + a[j - abs(left * channel) + 0];
                        resultG = (1 - weight) * in[j + 1] + a[j - abs(left * channel) + 1];
                        resultR = (1 - weight) * in[j + 2] + a[j - abs(left * channel) + 2];
                        if (resultB > 255) {
                            resultB = 255;
                        }
                        if (resultG > 255) {
                            resultG = 255;
                        }
                        if (resultR > 255) {
                            resultR = 255;
                        }
                    }
                }
            }
            out[j + 3] = 255;
            out[j + 2] = r * (1 - addWeight) +
                         (r * (1 - maxMask / 255.0) + resultR * (maxMask / 255.0)) * addWeight;
            out[j + 1] = g * (1 - addWeight) +
                         (g * (1 - maxMask / 255.0) + resultG * (maxMask / 255.0)) * addWeight;
            out[j] = b * (1 - addWeight) +
                     (b * (1 - maxMask / 255.0) + resultB * (maxMask / 255.0)) * addWeight;
        }
    }
    cvtColor(img, img, CV_BGRA2RGBA);
    img.release();
    maskMt.release();
    maskJoin.release();
    cvtColor(result, result, CV_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, join);
    return resultBitmap;
}

/**
 *
 * 变暗(A为上层，B为下层，C为输出图层)
 * B>=A 则 C=A B<=A 则 C=B
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject
toDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask, jint left,
       jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            int redBottom = inBottom[j + 2];
            int greenBottom = inBottom[j + 1];
            int blueBottom = inBottom[j];

            if (inR != -1) {
                int redTop = inR;
                int greenTop = inG;
                int blueTop = inB;
                if (redBottom >= redTop) {
                    redResult = redTop;
                } else if (redBottom <= redTop) {
                    redResult = redBottom;
                }
                if (greenBottom >= greenTop) {
                    greenResult = greenTop;
                } else if (greenBottom <= greenTop) {
                    greenResult = greenBottom;
                }
                if (blueBottom >= blueTop) {
                    blueResult = blueTop;
                } else if (blueBottom <= blueTop) {
                    blueResult = blueBottom;
                }
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}


/**
 *
 * 变亮(A为上层，B为下层，C为输出图层)
 * B<=A 则 C=A B>=A 则 C=B
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject
toLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask, jint left,
        jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            int redBottom = inBottom[j + 2];
            int greenBottom = inBottom[j + 1];
            int blueBottom = inBottom[j];

            if (inR != -1) {
                int redTop = inR;
                int greenTop = inG;
                int blueTop = inB;
                if (redBottom <= redTop) {
                    redResult = redTop;
                } else if (redBottom >= redTop) {
                    redResult = redBottom;
                }
                if (greenBottom <= greenTop) {
                    greenResult = greenTop;
                } else if (greenBottom >= greenTop) {
                    greenResult = greenBottom;
                }
                if (blueBottom <= blueTop) {
                    blueResult = blueTop;
                } else if (blueBottom >= blueTop) {
                    blueResult = blueBottom;
                }
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}


/**
 * 正片叠底(A为上层，B为下层，C为输出图层)
 * C=(A×B)/255
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject toMultiply(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                   jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            int redBottom = inBottom[j + 2];
            int greenBottom = inBottom[j + 1];
            int blueBottom = inBottom[j];

            if (inR != -1) {
                int redTop = inR;
                int greenTop = inG;
                int blueTop = inB;
                redResult = (redTop * redBottom) / 255;
                greenResult = (greenTop * greenBottom) / 255;
                blueResult = (blueTop * blueBottom) / 255;
            }

            if (redResult > 255) {
                redResult = 255;
            }
            if (greenResult > 255) {
                greenResult = 255;
            }
            if (blueResult > 255) {
                blueResult = 255;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}


/**
 *
 * 反相
 *
 **/
Mat change(const cv::Mat &src) {
    Mat result(src.size(), src.type());
    int channel = src.channels();
    int widht = channel * src.cols;
    int height = src.rows;
    for (int i = 0; i < height; i++) {
        const uchar *in = src.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);

        for (int j = 0; j < widht; j = j + channel) {
            int redResult = 255 - in[j + 2];
            int greenResult = 255 - in[j + 1];
            int blueResult = 255 - in[j];
            outResult[j + 2] = redResult;
            outResult[j + 1] = greenResult;
            outResult[j] = blueResult;
        }
    }
    return result;
}

/**
 * 颜色减淡(A为上层，B为下层，C为输出图层)
 * C=(B/(255−a)*255(这里使用四舍五入)
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject
toColorLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
             jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            if (inG != -1) {
                if ((255 - inR) != 0) {
                    redResult = ((inBottom[j + 2] / (255.0f - inR)) * 255.0f) + 0.5;
                }

                if ((255 - inG) != 0) {
                    greenResult = ((inBottom[j + 1] / (255.0f - inG)) * 255.0f) + 0.5;
                }

                if ((255 - inB) != 0) {
                    blueResult = ((inBottom[j] / (255.0f - inB)) * 255.0f) + 0.5;
                }
            }

            if (redResult > 255) {
                redResult = 255;
            }
            if (greenResult > 255) {
                greenResult = 255;
            }
            if (blueResult > 255) {
                blueResult = 255;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    cvtColor(bottom, bottom, CV_BGRA2RGBA);
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 线性颜色加深(A为上层，B为下层，C为输出图层)
 * C=B-A-255
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject
toLineColorDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            if (inR != -1) {
                redResult = inBottom[j + 2] + inR - 255;
                greenResult = inBottom[j + 1] + inG - 255;
                blueResult = inBottom[j] + inB - 255;
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 线性减淡添加(A为上层，B为下层，C为输出图层)
 * C=B+A
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject
toLineColorLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                 jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            if (inR != -1) {
                redResult = inBottom[j + 2] + inR;
                greenResult = inBottom[j + 1] + inG;
                blueResult = inBottom[j] + inB;
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 颜色加深(A为上层，B为下层，C为输出图层)
 * C= (1 - (255-B)/A)*255+0.5 加上0.5目的是四舍五入
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject
toColorDark(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
            jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            if (inR != -1) {
                if (inR != 0) {
                    redResult = (1.0f - (255 - inBottom[j + 2]) / (inR * 1.0f)) * 255 + 0.5f;
                }
                if (inG != 0) {
                    greenResult = (1.0f - (255 - inBottom[j + 1]) / (inG * 1.0f)) * 255 + 0.5f;
                }
                if (inB != 0) {
                    blueResult = (1.0f - (255 - inBottom[j]) / (inB * 1.0f)) * 255 + 0.5f;
                }
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 滤色(A为上层，B为下层，C为输出图层)
 * C=255-(B反相×A反相)/255
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 *
 * */
jobject
toFilterColor(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
              jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            if (inR != -1) {
                redResult = 255 - ((255 - inBottom[j + 2]) * (255 - inR)) / 255;
                greenResult = 255 - ((255 - inBottom[j + 1]) * (255 - inG)) / 255;
                blueResult = 255 - ((255 - inBottom[j]) * (255 - inB)) / 255;
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 叠加(A为上层，B为下层，C为输出图层)
 * B/255 <= 0.5: C = 2(A/255)(B/255)
 * B/255 > 0.5: C = 1 - 2(1-(A/255))(1-(B/255))
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject toOverlay(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jobject join, jint left,
                  jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img, img, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, join, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, join, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat maskJoin(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(maskJoin, maskJoin, COLOR_RGBA2BGRA);

    Mat result = img.clone();
    Mat ch = change(maskJoin);
    int channel = result.channels();
    int widht = channel * result.cols;
    int height = result.rows;
    int widthMix = maskJoin.cols * maskJoin.channels();
    int heightMix = maskJoin.rows;
    for (int i = 0; i < height; i++) {
        uchar *inTop = maskJoin.ptr<uchar>(i);
        const uchar *inBottom = img.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = maskJoin.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = maskJoin.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int redResult = 0;
            int greenResult = 0;
            int blueResult = 0;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    if (inBottom[j + 2] / 255.0f <= 0.5) {
                        redResult = (2 * (inBottom[j + 2] / 255.0f) *
                                     (inTop[j + abs(left * channel) + 2] / 255.0f)) * 255.0f + 0.5f;
                    } else {
                        redResult = (1.0f - 2.0f * (1 - (inBottom[j + 2] / 255.0f)) *
                                            (1 - (inTop[j + abs(left * channel) + 2] / 255.0f))) *
                                    255.0f + 0.5f;
                    }

                    if (inBottom[j + 1] / 255.0f <= 0.5) {
                        greenResult = (2 * (inBottom[j + 1] / 255.0f) *
                                       (inTop[j + abs(left * channel) + 1] / 255.0f)) * 255.0f +
                                      0.5f;
                    } else {
                        greenResult = (1.0f - 2.0f * (1 - (inBottom[j + 1] / 255.0f)) *
                                              (1 - (inTop[j + abs(left * channel) + 1] / 255.0f))) *
                                      255.0f + 0.5f;
                    }

                    if (inBottom[j] / 255.0f <= 0.5) {
                        blueResult = (2 * (inBottom[j] / 255.0f) *
                                      (inTop[j + abs(left * channel)] / 255.0f)) * 255.0f + 0.5f;
                    } else {
                        blueResult = (1.0f - 2.0f * (1 - (inBottom[j] / 255.0f)) *
                                             (1 - (inTop[j + abs(left * channel)] / 255.0f))) *
                                     255.0f + 0.5f;
                    }
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    if (inBottom[j + 2] / 255.0f <= 0.5) {
                        redResult = (2 * (inBottom[j + 2] / 255.0f) *
                                     (inTop[j - abs(left * channel) + 2] / 255.0f)) * 255.0f + 0.5f;
                    } else {
                        redResult = (1.0f - 2.0f * (1 - (inBottom[j + 2] / 255.0f)) *
                                            (1 - (inTop[j - abs(left * channel) + 2] / 255.0f))) *
                                    255.0f + 0.5f;
                    }

                    if (inBottom[j + 1] / 255.0f <= 0.5) {
                        greenResult = (2 * (inBottom[j + 1] / 255.0f) *
                                       (inTop[j - abs(left * channel) + 1] / 255.0f)) * 255.0f +
                                      0.5f;
                    } else {
                        greenResult = (1.0f - 2.0f * (1 - (inBottom[j + 1] / 255.0f)) *
                                              (1 - (inTop[j - abs(left * channel) + 1] / 255.0f))) *
                                      255.0f + 0.5f;
                    }

                    if (inBottom[j] / 255.0f <= 0.5) {
                        blueResult = (2 * (inBottom[j] / 255.0f) *
                                      (inTop[j - abs(left * channel)] / 255.0f)) * 255.0f + 0.5f;
                    } else {
                        blueResult = (1.0f - 2.0f * (1 - (inBottom[j] / 255.0f)) *
                                             (1 - (inTop[j - abs(left * channel)] / 255.0f))) *
                                     255.0f + 0.5f;
                    }
                }
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    img.release();
    maskMt.release();
    maskJoin.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, join);
    return resultBitmap;
}


/**
 *
 * 强光(A为上层，B为下层，C为输出图层)
 * A/255 <= 0.5:C=2*(B/255)*(A/255)
 * A/255 > 0.5:C = 1 - 2(1-(A/255))(1-(B/255))
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject
toHardLight(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jobject join, jint left,
            jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat img(info.height, info.width, CV_8UC4, pixels);
    cvtColor(img, img, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, join, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, join, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat maskJoin(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(maskJoin, maskJoin, COLOR_RGBA2BGRA);

    Mat result = img.clone();
    Mat ch = change(maskJoin);
    int channel = result.channels();
    int widht = channel * result.cols;
    int height = result.rows;
    int widthMix = maskJoin.cols * maskJoin.channels();
    int heightMix = maskJoin.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = result.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = maskJoin.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = maskJoin.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int redResult = 0;
            int greenResult = 0;
            int blueResult = 0;
            int inR = 0;
            int inG = 0;
            int inB = 0;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                float weight = inTop[j + abs(left * channel) + 3] / 255.0;
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = (1 - weight) * inBottom[j + 0] + inTop[j + abs(left * channel) + 0];
                    inG = (1 - weight) * inBottom[j + 1] + inTop[j + abs(left * channel) + 1];
                    inR = (1 - weight) * inBottom[j + 2] + inTop[j + abs(left * channel) + 2];
                    if (inB > 255) {
                        inB = 255;
                    }
                    if (inG > 255) {
                        inG = 255;
                    }
                    if (inR > 255) {
                        inR = 255;
                    }
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                float weight = inTop[j - abs(left * channel) + 3] / 255.0;
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = (1 - weight) * inBottom[j + 0] + inTop[j - abs(left * channel) + 0];
                    inG = (1 - weight) * inBottom[j + 1] + inTop[j - abs(left * channel) + 1];
                    inR = (1 - weight) * inBottom[j + 2] + inTop[j - abs(left * channel) + 2];
                    if (inB > 255) {
                        inB = 255;
                    }
                    if (inG > 255) {
                        inG = 255;
                    }
                    if (inR > 255) {
                        inR = 255;
                    }
                }
            }
            if (inR / 255.0f <= 0.5) {
                redResult = (2 * (inBottom[j + 2] / 255.0f) * (inR / 255.0f)) * 255.0f + 0.5f;
            } else {
                redResult =
                        (1.0f - 2.0f * (1 - (inBottom[j + 2] / 255.0f)) * (1 - (inR / 255.0f))) *
                        255.0f + 0.5f;
            }

            if (inG / 255.0f <= 0.5) {
                greenResult = (2 * (inBottom[j + 1] / 255.0f) * (inG / 255.0f)) * 255.0f + 0.5f;
            } else {
                greenResult =
                        (1.0f - 2.0f * (1 - (inBottom[j + 1] / 255.0f)) * (1 - (inG / 255.0f))) *
                        255.0f + 0.5f;
            }

            if (inB / 255.0f <= 0.5) {
                blueResult = (2 * (inBottom[j] / 255.0f) * (inB / 255.0f)) * 255.0f + 0.5f;
            } else {
                blueResult = (1.0f - 2.0f * (1 - (inBottom[j] / 255.0f)) * (1 - (inB / 255.0f))) *
                             255.0f + 0.5f;
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    cvtColor(img, img, CV_BGRA2RGBA);
    img.release();
    maskMt.release();
    maskJoin.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, join);
    return resultBitmap;
}

/**
 *
 * 柔光(A为上层，B为下层，C为输出图层)
 * A/255 <= 0.5:C = 2(A/255)(B/255) + (B/255)(B/255)(1-2(A/255))
 * A/255 > 0.5:C = 2(B/255)(1-(A/255)) + √(B/255)(2(A/255)-1)
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject
toSoftLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
            jint left, jint top, float topWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = result.channels();
    int widht = channel * result.cols;
    int height = result.rows;
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    for (int i = 0; i < height; i++) {
        uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int redResult = 0;
            int greenResult = 0;
            int blueResult = 0;
            float redTopPrecent = 0;
            float greenTopPrecent = 0;
            float blueTopPrecent = 0;
            float redBottomPrecent = inBottom[j + 2] / 255.0f;
            float greenBottomPrecent = inBottom[j + 1] / 255.0f;
            float blueBottomPrecent = inBottom[j] / 255.0f;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    redTopPrecent = inTop[j + abs(left * channel) + 2] / 255.0f;
                    greenTopPrecent = inTop[j + abs(left * channel) + 1] / 255.0f;
                    blueTopPrecent = inTop[j + abs(left * channel)] / 255.0f;
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    redTopPrecent = inTop[j - abs(left * channel) + 2] / 255.0f;
                    greenTopPrecent = inTop[j - abs(left * channel) + 1] / 255.0f;
                    blueTopPrecent = inTop[j - abs(left * channel)] / 255.0f;
                }
            }
            if (inTop != nullptr) {
                if (inTop[j + 2] / 255.0f <= 0.5) {
                    redResult = (2 * redBottomPrecent * redTopPrecent +
                                 redBottomPrecent * redBottomPrecent * (1 - 2 * redTopPrecent)) *
                                255 + 0.5;
                } else {
                    redResult = (2 * redBottomPrecent * (1 - redTopPrecent) +
                                 sqrt(redBottomPrecent) * (2 * redTopPrecent - 1)) * 255 + 0.5;
                }

                if (inTop[j + 1] / 255.0f <= 0.5) {
                    greenResult = (2 * greenBottomPrecent * greenTopPrecent +
                                   greenBottomPrecent * greenBottomPrecent *
                                   (1 - 2 * greenTopPrecent)) * 255 + 0.5;
                } else {
                    greenResult = (2 * greenBottomPrecent * (1 - greenTopPrecent) +
                                   sqrt(greenBottomPrecent) * (2 * greenTopPrecent - 1)) * 255 +
                                  0.5;
                }

                if (inTop[j] / 255.0f <= 0.5) {
                    blueResult = (2 * blueBottomPrecent * blueTopPrecent +
                                  blueBottomPrecent * blueBottomPrecent *
                                  (1 - 2 * blueTopPrecent)) * 255 + 0.5;
                } else {
                    blueResult = (2 * blueBottomPrecent * (1 - blueTopPrecent) +
                                  sqrt(blueBottomPrecent) * (2 * blueTopPrecent - 1)) * 255 + 0.5;
                }
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - topWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               topWeight;
            outResult[j + 1] = g * (1 - topWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               topWeight;
            outResult[j] = b * (1 - topWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * topWeight;
        }
    }
    cvtColor(result, result, CV_BGRA2RGBA);
    cvtColor(bottom, bottom, CV_BGRA2RGBA);
    cvtColor(topBt, topBt, CV_BGRA2RGBA);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, topBitmap);
    topBt.release();
    maskMt.release();
    bottom.release();
    return resultBitmap;
}


/**
 *
 * 亮光(A为上层，B为下层，C为输出图层)
 * A/255 <= 0.5:C = 1 + ((B/255)-1)/2(A/255)
 * A/255 > 0.5:C = (B/255)/2(1-(A/255))
 * 以底下图片为大小依据进行图层混合
 * @param top 上层图层
 * @param bottom 下层图层
 **/
jobject
toVividLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
             jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);

        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int redResult = 0;
            int greenResult = 0;
            int blueResult = 0;

            int inR = 0;
            int inG = 0;
            int inB = 0;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            if (inR != 0) {
                if (inR <= 128) {
                    redResult = 255 - (255 - inBottom[j + 2]) / (2.0f * inR) * 255;
                } else {
                    redResult = inBottom[j + 2] / (2.0f * (255 - inR)) * 255;
                }
            } else {
                redResult = 255;
            }

            if (inG != 0) {
                if (inG <= 128) {
                    greenResult = 255 - (255 - inBottom[j + 1]) / (2.0f * inG) * 255;
                } else {
                    greenResult = inBottom[j + 1] / (2.0f * (255 - inG)) * 255;
                }
            } else {
                greenResult = 255;
            }

            if (inB != 0) {
                if (inB <= 128) {
                    blueResult = 255 - (255 - inBottom[j + 1]) / (2.0f * inB) * 255;
                } else {
                    blueResult = inBottom[j + 1] / (2.0f * (255 - inB)) * 255;
                }
            } else {
                blueResult = 255;
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 线性光（Linear Light）
 * 2 * 混合色 + 基色 -255
 *
 **/
jobject
toLinearLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
              jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);
    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);

        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = 0;
            int inG = 0;
            int inB = 0;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            if (inR != -1) {
                redResult = 2 * inR + inBottom[j + 2] - 255;
                greenResult = 2 * inG + inBottom[j + 1] - 255;
                blueResult = 2 * inB + inBottom[j] - 255;
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 点光（Pin Light）
 * 计算公式：基色 < 2 * 混合色 - 255：结果色 = 2 * 混合色 - 255；
 * 2 * 混合色 - 255 < 基色 < 2 * 混合色 ：结果色 = 基色；
 * 基色 > 2 * 混合色：结果色 = 2 * 混合色。
 *
 **/
jobject toPinLight(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                   jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inBottom[j + 2];
            int greenResult = inBottom[j + 1];
            int blueResult = inBottom[j];

            if (inR != -1) {
                if (inBottom[j + 2] < 2 * inR - 255) {
                    redResult = 2 * inTop[j + 2] - 255;
                } else if (2 * inR - 255 < inBottom[j + 2] < 2 * inR) {
                    redResult = inBottom[j + 2];
                } else if (inBottom[j + 2] > 2 * inR) {
                    redResult = 2 * inR;
                }

                if (inBottom[j + 1] < 2 * inG - 255) {
                    greenResult = 2 * inG - 255;
                } else if (2 * inG - 255 < inBottom[j + 1] < 2 * inG) {
                    greenResult = inBottom[j + 1];
                } else if (inBottom[j + 1] > 2 * inG) {
                    greenResult = 2 * inG;
                }

                if (inBottom[j] < 2 * inB - 255) {
                    blueResult = 2 * inB - 255;
                } else if (2 * inB - 255 < inBottom[j] < 2 * inB) {
                    blueResult = inBottom[j];
                } else if (inBottom[j] > 2 * inB) {
                    blueResult = 2 * inB;
                }
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 实色混合（Hard Mix）
 * 计算公式：混合色 + 基色 < 255：结果色 = 0 ；
 * 混合色 + 基色 >= 255：结果色 = 255。
 *
 * */
jobject toHardMix(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                  jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inBottom[j + 2];
            int greenResult = inBottom[j + 1];
            int blueResult = inBottom[j];
            if (inR != -1) {
                if (inR + inBottom[j + 2] < 255) {
                    redResult = 0;
                } else if (inR + inBottom[j + 2] >= 255) {
                    redResult = 255;
                }
                if (inG + inBottom[j + 1] < 255) {
                    greenResult = 0;
                } else if (inG + inBottom[j + 1] >= 255) {
                    greenResult = 255;
                }
                if (inB + inBottom[j] < 255) {
                    blueResult = 0;
                } else if (inB + inBottom[j] >= 255) {
                    blueResult = 255;
                }
            }

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 *
 * 差值（Difference）
 * 结果色 = |混合色 - 基色|
 **/
jobject
toDifference(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
             jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;
    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            if (inR != -1 && inTop != nullptr) {
                redResult = abs(inTop[j + 2] - inBottom[j + 2]);
                greenResult = abs(inTop[j + 1] - inBottom[j + 1]);
                blueResult = abs(inTop[j] - inBottom[j]);
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }
            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 排除（Exclusion
 * 结果色 = (混合色 + 基色) - 混合色 * 基色 / 128
 **/
jobject
toExclusion(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
            jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }
            int redResult = r;
            int greenResult = g;
            int blueResult = b;

            if (inR != -1) {
                redResult = inR + inBottom[j + 2] - inR * inBottom[j + 2] / 128;
                greenResult = inG + inBottom[j + 1] - inG * inBottom[j + 1] / 128;
                blueResult = inB + inBottom[j] - inB * inBottom[j] / 128;
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 减去（Subtract）
 *结果色 = 基色 - 混合色。
 **/
jobject toSubtract(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                   jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = 0;
            int inG = 0;
            int inB = 0;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inBottom[j + 2] - inR;
            int greenResult = inBottom[j + 1] - inG;
            int blueResult = inBottom[j] - inB;

            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 * 划分（Divide）
 * 结果色 = (基色 / 混合色) * 255
 * */
jobject toDivide(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
                 jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat result = bottom.clone();
    Mat ch = change(topBt);
    int channel = bottom.channels();
    int widht = channel * bottom.cols;
    int height = bottom.rows;
    for (int i = 0; i < height; i++) {
        const uchar *inTop = topBt.ptr<uchar>(i);
        const uchar *inBottom = bottom.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topBt.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topBt.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maxMask == 0) {
                outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int  inR = -1;
            int  inG = -1;
            int  inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (inTop[j + abs(left * channel) + 3] != 0) {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (inTop[j - abs(left * channel) + 3] != 0) {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = r;
            int greenResult = g;
            int blueResult = b;
            if (inR != -1) {
                redResult = inBottom[j + 2] / (inR * 1.0f) * 255;
                greenResult = inBottom[j + 1] / (inG * 1.0f) * 255;
                blueResult = inBottom[j] / (inB * 1.0f) * 255;
            }


            if (redResult > 255) {
                redResult = 255;
            } else if (redResult < 0) {
                redResult = 0;
            }
            if (greenResult > 255) {
                greenResult = 255;
            } else if (greenResult < 0) {
                greenResult = 0;
            }
            if (blueResult > 255) {
                blueResult = 255;
            } else if (blueResult < 0) {
                blueResult = 0;
            }

            outResult[j + 3] = 255;
            outResult[j + 2] = r * (1 - addWeight) +
                               (r * (1 - maxMask / 255.0) + redResult * maxMask / 255.0) *
                               addWeight;
            outResult[j + 1] = g * (1 - addWeight) +
                               (g * (1 - maxMask / 255.0) + greenResult * maxMask / 255.0) *
                               addWeight;
            outResult[j] = b * (1 - addWeight) +
                           (b * (1 - maxMask / 255.0) + blueResult * maxMask / 255.0) * addWeight;
        }
    }
    bottom.release();
    maskMt.release();
    topBt.release();
    cvtColor(result, result, CV_BGRA2RGB);
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 *
 * 色相（Hue）
 * 混合图层色相替换基图层色相，其他不变
 *
 **/
jobject
toHue(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask, jint left,
      jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat topCp = topBt.clone();
    Mat bottomCp = bottom.clone();
    cv::cvtColor(topCp, topCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(bottomCp, bottomCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(maskMt, maskMt, COLOR_BGR2HSV_FULL);
    int channel = bottomCp.channels();
    int widht = channel * bottomCp.cols;
    int height = bottomCp.rows;
    Mat result = bottomCp.clone();
    //cv::cvtColor(result, result, COLOR_BGR2HSV_FULL);
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottomCp.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topCp.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topCp.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maskIn[j + 2] == 0) {
                //outResult[j + 3] = alpha;
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (channel == 4) {
                    if (inTop[j + abs(left * channel) + 3] != 0) {
                        inB = inTop[j + abs(left * channel)];
                        inG = inTop[j + abs(left * channel) + 1];
                        inR = inTop[j + abs(left * channel) + 2];
                    }
                } else {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (channel == 4) {
                    if (inTop[j - abs(left * channel) + 3] != 0) {
                        inB = inTop[j - abs(left * channel)];
                        inG = inTop[j - abs(left * channel) + 1];
                        inR = inTop[j - abs(left * channel) + 2];
                    }
                } else {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inR;
            int greenResult = inG;
            int blueResult = inB;
            if (inR == -1) {
                redResult = r;
            }
            if (inG == -1) {
                greenResult = g;
            }
            if (inB == -1) {
                blueResult = b;
            }
            outResult[j] = b;
            outResult[j + 1] = g * (1 - addWeight) + (g * (1 - maskIn[j + 2] / 255.0) +greenResult * (maskIn[j + 2] / 255.0)) *addWeight;
            outResult[j + 2] = r * (1 - addWeight) + (r * (1 - maskIn[j + 2] / 255.0) +redResult * (maskIn[j + 2] / 255.0)) *addWeight;
        }
    }
    cvtColor(result, result, COLOR_HSV2RGB_FULL);
    bottom.release();
    maskMt.release();
    topBt.release();
    topCp.release();
    bottomCp.release();
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 *
 * 饱和度（Saturation）
 * 混合图层饱和度替换替换基图层饱和度，其他不变
 *
 **/
jobject
toSaturation(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
             jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat topCp = topBt.clone();
    Mat bottomCp = bottom.clone();
    cv::cvtColor(topCp, topCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(bottomCp, bottomCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(maskMt, maskMt, COLOR_BGR2HSV_FULL);
    int channel = bottomCp.channels();
    int widht = channel * bottomCp.cols;
    int height = bottomCp.rows;
    Mat result = bottomCp.clone();
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottomCp.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topCp.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topCp.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maskIn[j + 2] == 0) {
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (channel == 4){
                    if (inTop[j + abs(left * channel) + 3] != 0) {
                        inB = inTop[j + abs(left * channel)];
                        inG = inTop[j + abs(left * channel) + 1];
                        inR = inTop[j + abs(left * channel) + 2];
                    }
                }else{
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (channel == 4){
                    if (inTop[j - abs(left * channel) + 3] != 0) {
                        inB = inTop[j - abs(left * channel)];
                        inG = inTop[j - abs(left * channel) + 1];
                        inR = inTop[j - abs(left * channel) + 2];
                    }
                }else{
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inR;
            int greenResult = inG;
            int blueResult = inB;
            if (inR == -1) {
                redResult = r;
            }
            if (inG == -1) {
                greenResult = g;
            }
            if (inB == -1) {
                blueResult = b;
            }
            outResult[j] = b;
            outResult[j + 1] = g * (1 - addWeight) + (g * (1 - maskIn[j + 2] / 255.0) +greenResult * ((maskIn[j + 2] / 255.0))) *addWeight;
            outResult[j + 2] = r;
        }
    }
    cvtColor(result, result, COLOR_HSV2RGB_FULL);
    bottom.release();
    maskMt.release();
    topBt.release();
    topCp.release();
    bottomCp.release();
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 *
 * 颜色（Saturation）
 * 混合图层饱和度与色相替换替换基图层饱和度与色相，其他不变
 *
 **/
jobject
toColor(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask, jint left,
        jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat topCp = topBt.clone();
    Mat bottomCp = bottom.clone();
    cv::cvtColor(topCp, topCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(bottomCp, bottomCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(maskMt, maskMt, COLOR_BGR2HSV_FULL);
    int channel = bottomCp.channels();
    int widht = channel * bottomCp.cols;
    int height = bottomCp.rows;
    Mat result = bottomCp.clone();
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottomCp.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topCp.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topCp.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maskIn[j + 2] == 0) {
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (channel == 4){
                    if (inTop[j + abs(left * channel) + 3] != 0) {
                        inB = inTop[j + abs(left * channel)];
                        inG = inTop[j + abs(left * channel) + 1];
                        inR = inTop[j + abs(left * channel) + 2];
                    }
                }else{
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (channel == 4){
                    if (inTop[j - abs(left * channel) + 3] != 0) {
                        inB = inTop[j - abs(left * channel)];
                        inG = inTop[j - abs(left * channel) + 1];
                        inR = inTop[j - abs(left * channel) + 2];
                    }
                }else{
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inR;
            int greenResult = inG;
            int blueResult = inB;
            if (inR == -1) {
                redResult = r;
            }
            if (inG == -1) {
                greenResult = g;
            }
            if (inB == -1) {
                blueResult = b;
            }
            outResult[j] = b * (1 - addWeight) + (b * (1 - maskIn[j + 2] / 255.0) +blueResult * ((maskIn[j + 2] / 255.0))) *addWeight;
            outResult[j + 1] = g * (1 - addWeight) + (g * (1 - maskIn[j + 2] / 255.0) +greenResult * ((maskIn[j + 2] / 255.0))) *addWeight;
            outResult[j + 2] = r;
        }
    }
    cvtColor(result, result, COLOR_HSV2RGB_FULL);
    bottom.release();
    maskMt.release();
    topCp.release();
    bottomCp.release();
    topBt.release();
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}

/**
 *
 * 明度（Luminosity）
 * 混合图层明度替换替换基图层明度，其他不变
 *
 **/
jobject
toLuminosity(JNIEnv *env, jclass clazz, jobject topBitmap, jobject bottomBitmap, jobject mask,
             jint left, jint top, float addWeight) {
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, topBitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, topBitmap, &pixels) >= 0);
    CV_Assert(pixels);
    //原图
    cv::Mat topBt(info.height, info.width, CV_8UC4, pixels);
    cvtColor(topBt, topBt, COLOR_RGBA2BGRA);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2BGRA);


    AndroidBitmapInfo infoJoin;
    void *pixelsMaskJoin;
    CV_Assert(AndroidBitmap_getInfo(env, bottomBitmap, &infoJoin) >= 0);
    CV_Assert(infoJoin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoJoin.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bottomBitmap, &pixelsMaskJoin) >= 0);
    CV_Assert(pixelsMaskJoin);
    cv::Mat bottom(infoJoin.height, infoJoin.width, CV_8UC4, pixelsMaskJoin);
    cvtColor(bottom, bottom, COLOR_RGBA2BGRA);

    int widthMix = topBt.cols * topBt.channels();
    int heightMix = topBt.rows;

    Mat topCp = topBt.clone();
    Mat bottomCp = bottom.clone();
    cv::cvtColor(topCp, topCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(bottomCp, bottomCp, COLOR_BGR2HSV_FULL);
    cv::cvtColor(maskMt, maskMt, COLOR_BGR2HSV_FULL);
    int channel = bottomCp.channels();
    int widht = channel * bottomCp.cols;
    int height = bottomCp.rows;
    Mat result = bottomCp.clone();
    for (int i = 0; i < height; i++) {
        const uchar *inTop = nullptr;
        const uchar *inBottom = bottomCp.ptr<uchar>(i);
        uchar *outResult = result.ptr<uchar>(i);
        const uchar *maskIn = maskMt.ptr<uchar>(i);
        if (top <= 0) {
            if (i + abs(top) < heightMix) {
                inTop = topCp.ptr<uchar>(i + abs(top));
            } else {
                inTop = nullptr;
            }
        } else {
            if (i >= top && i < top + heightMix) {
                inTop = topCp.ptr<uchar>(i - top);
            } else {
                inTop = nullptr;
            }
        }
        for (int j = 0; j < widht; j = j + channel) {
            uchar r = inBottom[j + 2];
            uchar g = inBottom[j + 1];
            uchar b = inBottom[j];
            uchar alpha = 255;
            uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
            if (maskIn[j + 2] == 0) {
                outResult[j + 2] = r;
                outResult[j + 1] = g;
                outResult[j] = b;
                continue;
            }
            int inR = -1;
            int inG = -1;
            int inB = -1;
            if (left <= 0 && inTop != nullptr && j + abs(left * channel) + 2 < widthMix) {
                if (channel == 4) {
                    if (inTop[j + abs(left * channel) + 3] != 0) {
                        inB = inTop[j + abs(left * channel)];
                        inG = inTop[j + abs(left * channel) + 1];
                        inR = inTop[j + abs(left * channel) + 2];
                    }
                } else {
                    inB = inTop[j + abs(left * channel)];
                    inG = inTop[j + abs(left * channel) + 1];
                    inR = inTop[j + abs(left * channel) + 2];
                }
            } else if (left > 0 && inTop != nullptr && j >= left * channel &&
                       j < left * channel + widthMix) {
                if (channel == 4) {
                    if (inTop[j - abs(left * channel) + 3] != 0) {
                        inB = inTop[j - abs(left * channel)];
                        inG = inTop[j - abs(left * channel) + 1];
                        inR = inTop[j - abs(left * channel) + 2];
                    }
                } else {
                    inB = inTop[j - abs(left * channel)];
                    inG = inTop[j - abs(left * channel) + 1];
                    inR = inTop[j - abs(left * channel) + 2];
                }
            }

            int redResult = inR;
            int greenResult = inG;
            int blueResult = inB;
            if (inR == -1) {
                redResult = r;
            }
            if (inG == -1) {
                greenResult = g;
            }
            if (inB == -1) {
                blueResult = b;
            }
            outResult[j] = b;
            outResult[j + 1] = g;
            outResult[j + 2] = r * (1 - addWeight) + (r * (1 - maskIn[j + 2] / 255.0) +redResult * ((maskIn[j + 2] / 255.0))) *addWeight;
        }
    }
    cvtColor(result, result, COLOR_HSV2RGB_FULL);
    bottom.release();
    maskMt.release();
    topCp.release();
    bottomCp.release();
    topBt.release();
    jobject resultBitmap = matToBitmap(env, result, false, bottomBitmap);
    result.release();
    AndroidBitmap_unlockPixels(env, topBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    AndroidBitmap_unlockPixels(env, bottomBitmap);
    return resultBitmap;
}
