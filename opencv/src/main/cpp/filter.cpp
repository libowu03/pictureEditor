//
// Created by libowu on 20-8-29.
//
#include <opencv2/opencv.hpp>
#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include "include/opencv2/filter/filter.h"
#include "include/opencv2/utils/utils.h"
#include <opencv2/features2d.hpp>
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


/**
 * 图片像素化
 * 原理：老式的图片像素较低，表象为同一个部分，老式图片使用比较少的像素点去绘制该部分，只要模拟出这种情况即可。我们将一个比较细腻的像素点与周围的像素合并，就可以表现出像素低的情况。
 * @param env
 * @param clazz
 * @param bitmap
 * @param pixels_size 已像素核为中心到四个边的距离，加入值为1，则3*3的像素将被设置为同等色值
 * @return 处理好的图片
 */
jobject
makeBitmapToPixels(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jint pixels_size) {
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

        AndroidBitmapInfo infoMask;
        void *pixelsMask;
        CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
        CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
        CV_Assert(pixelsMask);
        cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
        cvtColor(maskMt, maskMt, COLOR_RGBA2RGB);


        cv::cvtColor(image, image, COLOR_RGBA2RGB);
        cv::Mat result = image.clone();
        int rows = result.rows;
        //int cols = image.cols*image.channels();
        int cols = result.cols;
        for (int row = pixels_size; row < rows; row = row + (pixels_size + 1)) {//遍历图像各个像素点的像素值
            for (int col = pixels_size; col < cols; col = col + (pixels_size + 1)) {
                //设置中点
                int r = image.at<Vec3b>(row, col)[0];//获取row行col列0通道的像素值,Vec3b可以用来读取三通道的RGB图像像素值
                int g = image.at<Vec3b>(row, col)[1];
                int b = image.at<Vec3b>(row, col)[2];

                result.at<Vec3b>(row, col)[0] = r;
                result.at<Vec3b>(row, col)[1] = g;
                result.at<Vec3b>(row, col)[2] = b;

                //这两个循环是设置矩阵的数据
                for (int k = row - pixels_size; k <= row; k++) {
                    for (int i = col - pixels_size; i <= col; i++) {
                        //将中心的像素点赋值给周边的像素，相当于把像素点放大，原本一个像素点的长宽为1*1，现在则为(（pixeSize*2）+1)*(（pixeSize*2）+1),这样就可以出现图片像素化的效果了，也可以用来打马赛克。
                        result.at<cv::Vec3b>(k, i)[0] = r;//
                        result.at<cv::Vec3b>(k, i)[1] = g;
                        result.at<cv::Vec3b>(k, i)[2] = b;
                    }
                }
                if (col + (pixels_size + 1) >= cols && col != cols - 1) {
                    col = cols - (pixels_size + 1) - 1;
                }
            }
            if (row + (pixels_size + 1) >= rows && row != rows - 1) {
                row = rows - (pixels_size + 1) - 1;
            }
        }
        int width = image.cols * image.channels();
        int height = image.rows;
        uchar channel = image.channels();
        for (int i = 0; i < height; i++) {
            uchar *in = image.ptr<uchar>(i);
            const uchar *maskIn = maskMt.ptr<uchar>(i);
            uchar *out = result.ptr<uchar>(i);
            for (int j = 0; j < width; j += channel) {
                uchar redValue = in[j + 0];
                uchar greenValue = in[j + 1];
                uchar blueValue = in[j + 2];

                uchar effectR = out[j + 0];
                uchar effectG = out[j + 1];
                uchar effectB = out[j + 2];

                uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
                if (maxMask == 0) {
                    out[j + 2] = blueValue;
                    out[j + 1] = greenValue;
                    out[j] = redValue;
                    if (channel == 4) {
                        out[j + 3] = in[j + 3];
                    }
                    continue;
                }

                out[j + 2] = blueValue * (1 - maxMask / 255.0) + effectB * maxMask / 255.0;
                out[j + 1] = greenValue * (1 - maxMask / 255.0) + effectG * maxMask / 255.0;
                out[j] = redValue * (1 - maxMask / 255.0) + effectR * maxMask / 255.0;
                if (channel == 4) {
                    out[j + 3] = in[j + 3];
                }
            }
        }
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        AndroidBitmap_unlockPixels(env, mask);
        image.release();
        maskMt.release();
        return resultBitmap;
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 反色滤镜
 * 原理：色值最大值255减去当前像素色值
 * @param env
 * @param clazz
 * @param bitmap
 * @return 处理好的图片
 */
jobject makeBitmapColorCounter(JNIEnv *env, jclass clazz, jobject bitmap) {
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
        cv::Mat src(info.height, info.width, CV_8UC4, pixels);
        cv::cvtColor(src, src, COLOR_RGBA2BGR);
        cv::Mat testMat = src.clone();
        int rows = src.rows;
        int cols = src.cols * src.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = src.ptr<uchar>(i);
            uchar *o = testMat.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                o[j] = 255 - p[j];
            }
        }
        jobject result = matToBitmap(env, testMat, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return result;
    } catch (Exception &e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 将图片改成灰度图像
 * 原理：获取三通道平均值
 * @param env
 * @param obj
 * @param buf
 * @param w
 * @param h
 * @return
 */
jobject makeBitmapGray(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out(image.size(), image.type());
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                o[j] = (p[j] + p[j + 1] + p[j + 2]) / 3;
            }
        }
        jobject result = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return result;
    } catch (Exception &e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 黑白滤镜
 * 原理：获取像素色值，假如色值大于100，认为是黑色，否则是白色
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapWhiteAndBlack(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out = image.clone();
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                uchar arv = (p[j] + p[j + 1] + p[j + 2]) / 3;
                if (arv >= 100) {
                    o[j] = 0;
                } else {
                    o[j] = 255;
                }
            }
        }
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }

}

/**
 * 怀旧风格
 *
 * 原理：R = 0.93r + 0.769g + 0.189b
 *       G = 0.349r + 0.686g + 0.168b
 *       B = 0.272r + 0.534g + 0.131b
 *
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapNostalgia(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out = image.clone();
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                if (j * 3 > cols) {
                    break;
                }

                int r = p[j * 3];
                int g = p[j * 3 + 1];
                int b = p[j * 3 + 2];
                int newR = 0.393 * r + 0.769 * g + 0.189 * b;
                int newG = 0.349 * r + 0.686 * g + 0.168 * b;
                int newB = 0.272 * r + 0.534 * g + 0.131 * b;
                if (newR < 0) {
                    newR = 0;
                } else if (newR > 255) {
                    newR = 255;
                }
                if (newG < 0) {
                    newG = 0;
                } else if (newG > 255) {
                    newG = 255;
                }
                if (newB < 0) {
                    newB = 0;
                } else if (newB > 255) {
                    newB = 255;
                }
                o[j * 3] = newR;
                o[j * 3 + 1] = newG;
                o[j * 3 + 2] = newB;
            }
        }
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 熔铸滤镜
 *
 * 原理：R = r*128/(g+b+1)
 *       G = g*128/(r+b+1)
 *       B = b*128/(r+g+1)
 *
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapFounding(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out = image.clone();
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                uchar r = p[j * 3];
                uchar g = p[j * 3 + 1];
                uchar b = p[j * 3 + 2];
                int newR = r * 128 / (g + b + 1);
                int newG = g * 128 / (r + b + 1);
                int newB = b * 128 / (r + g + 1);
                if (newR < 0) {
                    newR = 0;
                } else if (newR > 255) {
                    newR = 255;
                }
                if (newG < 0) {
                    newG = 0;
                } else if (newG > 255) {
                    newG = 255;
                }
                if (newB < 0) {
                    newB = 0;
                } else if (newB > 255) {
                    newB = 255;
                }
                o[j * 3] = newR;
                o[j * 3 + 1] = newG;
                o[j * 3 + 2] = newB;
            }
        }
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 冰冻滤镜
 *
 * 原理：R = (r-g-b)*3/2
 *       G = (g-r-b)*3/2
 *       B = (b-g-r)*3/2
 *
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapFrozen(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out = image.clone();
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                uchar r = p[j * 3];
                uchar g = p[j * 3 + 1];
                uchar b = p[j * 3 + 2];
                int newR = (r - g - b) * 3 / 2;
                int newG = (g - r - b) * 3 / 2;
                int newB = (b - g - r) * 3 / 2;
                if (newR < 0) {
                    newR = 0;
                } else if (newR > 255) {
                    newR = 255;
                }
                if (newG < 0) {
                    newG = 0;
                } else if (newG > 255) {
                    newG = 255;
                }
                if (newB < 0) {
                    newB = 0;
                } else if (newB > 255) {
                    newB = 255;
                }
                o[j * 3] = newR;
                o[j * 3 + 1] = newG;
                o[j * 3 + 2] = newB;
            }
        }
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 连环画滤镜
 * 原理：R = |g-b+g+r|*r/256
 *       G = |b-g+b+r|*r/256
 *       B = |b-g+b+r|*g/256
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapComics(JNIEnv *env, jclass obj, jobject bitmap) {
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
        cv::cvtColor(image, image, CV_RGBA2RGB);
        cv::Mat out = image.clone();
        int rows = image.rows;
        int cols = image.cols * image.channels();
        for (int i = 0; i < rows; i++) {
            const uchar *p = image.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < cols; j++) {
                uchar r = p[j * 3];
                uchar g = p[j * 3 + 1];
                uchar b = p[j * 3 + 2];
                int newR = (g - b + g + r) * r / 256;
                int newG = (b - g + b + r) * r / 256;
                int newB = (b - g + b + r) * g / 256;
                if (newR < 0) {
                    newR = newR * -1;
                } else if (newR > 255) {
                    newR = 255;
                }
                if (newG < 0) {
                    newG = newG * -1;
                } else if (newG > 255) {
                    newG = 255;
                }
                if (newB < 0) {
                    newB = newB * -1;
                } else if (newB > 255) {
                    newB = 255;
                }
                o[j * 3] = newR;
                o[j * 3 + 1] = newG;
                o[j * 3 + 2] = newB;
            }
        }
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (const Exception &exception) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 高斯模糊
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapBlur(JNIEnv *env, jclass obj, jobject bitmap, jobject mask, int blurSize) {
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
        if (blurSize < 1) {
            return bitmap;
        }
        cv::Mat image(info.height, info.width, CV_8UC4, pixels);
        cvtColor(image, image, CV_RGBA2RGB);

        AndroidBitmapInfo infoMask;
        void *pixelsMask;
        CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
        CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
        CV_Assert(pixelsMask);
        cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
        cvtColor(maskMt, maskMt, CV_RGBA2RGB);

        cv::Mat out;
        //图像的高斯核模糊为5*5的大小，opencv中，高斯核的大小必须是奇数。因为高斯模糊的作用效果在高斯卷积覆盖范围的中间位置。
        cv::GaussianBlur(image, out, cv::Size(21, 21), 0, 0);
        //这里为什么进行循环100次呢？在上面调用GaussianBlur方法后，out已经被写入内存中了。
        //也就是out可作为输入对象，也可以作为输出对象，如果只执行一次GaussianBlur方法，高斯模糊效果有限，所以这里进行了一百次循环，每次模糊效果都是在上一次模糊的效果的基础上进行的，所以100次循环后，效果已经非常明显了了
        for (int i = 0; i < blurSize; i++) {
            cv::GaussianBlur(out, out, cv::Size(21, 21), 0, 0);
        }

        int width = image.cols * image.channels();
        int height = image.rows;
        int channel = image.channels();
        for (int i = 0; i < height; i++) {
            uchar *in = image.ptr<uchar>(i);
            const uchar *maskIn = maskMt.ptr<uchar>(i);
            uchar *o = out.ptr<uchar>(i);
            for (int j = 0; j < width; j += channel) {
                uchar r = in[j];
                uchar g = in[j + 1];
                uchar b = in[j + 2];

                uchar resultR = o[j];
                uchar resultG = o[j + 1];
                uchar resultB = o[j + 2];
                uchar maxMask = MAX(maskIn[j + 0], MAX(maskIn[j + 1], maskIn[j + 2]));
                if (maxMask == 0) {
                    o[j + 2] = b;
                    o[j + 1] = g;
                    o[j] = r;
                    if (channel == 4) {
                        o[j + 3] = 255;
                    }
                    continue;
                }
                o[j + 2] = b * (1 - maxMask / 255.0) + resultB * maxMask / 255.0;
                o[j + 1] = g * (1 - maxMask / 255.0) + resultG * maxMask / 255.0;
                o[j] = r * (1 - maxMask / 255.0) + resultR * maxMask / 255.0;
                if (channel == 4) {
                    o[j + 3] = 255;
                }
            }
        }
        jobject result = matToBitmap(env, out, false, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        AndroidBitmap_unlockPixels(env, mask);
        image.release();
        maskMt.release();
        out.release();
        return result;
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 图像侵蚀滤镜
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapErode(JNIEnv *env, jclass clazz, jobject bitmap) {
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
        cv::Mat temp(info.height, info.width, CV_8UC4, pixels);
        cv::Mat result(temp.size(), temp.type());
        erode(temp, result, getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 描边滤镜
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapEdge(JNIEnv *env, jclass clazz, jobject bitmap) {
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
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            cv::Mat temp(info.height, info.width, CV_8UC4, pixels);
            cv::Mat result(temp.size(), temp.type());
            cv::Mat gray;
            cvtColor(temp, gray, cv::COLOR_RGBA2GRAY);
            Canny(gray, gray, 45, 75);
            cvtColor(gray, result, cv::COLOR_GRAY2RGBA);
            jobject resultBitmap = matToBitmap(env, result, true, bitmap);
            return resultBitmap;
        } else {
            cv::Mat temp(info.height, info.width, CV_8UC2, pixels);
            cv::Mat result(temp.size(), temp.type());
            cv::Mat gray;
            cvtColor(temp, gray, cv::COLOR_RGB2GRAY);
            Canny(gray, gray, 45, 75);
            cvtColor(gray, result, cv::COLOR_GRAY2RGB);
            jobject resultBitmap = matToBitmap(env, result, true, bitmap);
            AndroidBitmap_unlockPixels(env, bitmap);
            return resultBitmap;
        }
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

/**
 * 膨胀滤镜
 * @param env
 * @param obj
 * @param bitmap
 * @param listener
 * @return
 */
jobject makeBitmapDilate(JNIEnv *env, jclass clazz, jobject bitmap) {
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
        cv::Mat temp(info.height, info.width, CV_8UC4, pixels);
        cv::Mat result(temp.size(), temp.type());
        dilate(temp, result, getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    } catch (Exception e) {
        return bitmap;
    } catch (...) {
        return bitmap;
    }
}

int getMax(Mat src) {
    int row = src.rows;
    int col = src.cols;
    int temp = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            temp = max((int) src.at<uchar>(i, j), temp);
        }
        if (temp == 255) return temp;
    }
    return temp;
}

jobject
makeBitmapRemoveHaze(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                     float value) {
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

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);
    cvtColor(maskMt, maskMt, COLOR_RGBA2RGB);

    double eps = 0;
    int row = src.rows;
    int col = src.cols;
    Mat M = Mat::zeros(row, col, CV_8UC1);
    Mat M_max = Mat::zeros(row, col, CV_8UC1);
    Mat M_ave = Mat::zeros(row, col, CV_8UC1);
    Mat L = Mat::zeros(row, col, CV_8UC1);
    Mat dst = Mat::zeros(row, col, CV_8UC3);
    double m_av = 0;
    double A = 0;
    //get M
    long sum = 0;
    for (int i = 0; i < row; i++) {
        uchar *in = src.data + i * src.step;
        uchar *inM = M.data + i * M.step;
        uchar *inMax = M_max.data + i * M_max.step;
        for (int j = 0; j < col; j++) {
            uchar r, g, b, temp1, temp2;
            b = in[0];
            g = in[1];
            r = in[2];
            temp1 = min(min(r, g), b);
            temp2 = max(max(r, g), b);
            inM[0] = temp1;
            inMax[0] = temp2;
            sum += temp1;
            in += 3;
            inM += 1;
            inMax += 1;
        }
    }
    m_av = sum / (row * col * 255.0);
    if (value > m_av) {
        value = m_av;
    }
    eps = value / m_av;
    boxFilter(M, M_ave, CV_8UC1, Size(51, 51));
    double delta = min(0.9, eps * m_av);
    for (int i = 0; i < row; i++) {
        uchar *inL = L.data + i * L.step;
        uchar *in_ave = M_ave.data + i * M_ave.step;
        uchar *in_m = M.data + i * M.step;
        for (int j = 0; j < col; j++) {
            inL[0] = min((int) (delta * in_ave[0]), (int) in_m[0]);
           /* inL[1] = min((int) (delta * in_ave[1]), (int) in_m[1]);
            inL[2] = min((int) (delta * in_ave[2]), (int) in_m[2]);*/
            inL = inL + 1;
            in_ave = in_ave + 1;
            in_m = in_m + 1;
        }
    }
    A = (getMax(M_max) + getMax(M_ave)) * 0.5;
    for (int i = 0; i < row; i++) {
        uchar *inL = L.data + i * L.step;
        uchar *inSrc = src.data + i * src.step;
        uchar *inMask = maskMt.data + i * maskMt.step;
        uchar *inDst = dst.data + i * dst.step;
        for (int j = 0; j < col; j++) {
            int temp = inL[0];
            uchar maxMask = MAX(inMask[0],MAX(inMask[1], inMask[2]));
            if (maxMask == 0) {
                inDst[0] = inSrc[0];
                inDst[1] = inSrc[1];
                inDst[2] = inSrc[2];
                inL = inL + 1;
                inSrc = inSrc + 3;
                inMask = inMask + 3;
                inDst = inDst + 3;
                continue;
            }

            int valR = A * (inSrc[0] - temp) / (A - temp);
            if (valR > 255) valR = 255;
            if (valR < 0) valR = 0;
            inDst[0] =(inSrc[0] * (1 - maxMask / 255.0)) + valR * maxMask / 255.0;

            int valG = A * (inSrc[1] - temp) / (A - temp);
            if (valG > 255) valG = 255;
            if (valG < 0) valG = 0;
            inDst[1] =(inSrc[1] * (1 - maxMask / 255.0)) + valG * maxMask / 255.0;

            int valB = A * (inSrc[2] - temp) / (A - temp);
            if (valB > 255) valB = 255;
            if (valB < 0) valB = 0;
            inDst[2] =(inSrc[2] * (1 - maxMask / 255.0)) + valB * maxMask / 255.0;

      /*      inDst[0] = inSrc[0];
            inDst[1] = inSrc[1];
            inDst[2] = inSrc[2];*/


            inL = inL + 1;
            inSrc = inSrc + 3;
            inMask = inMask + 3;
            inDst = inDst + 3;
        }
    }
    jobject resultBitmap = matToBitmap(env, dst, false, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, mask);
    dst.release();
    src.release();
    maskMt.release();
    M.release();
    M_ave.release();
    L.release();
    return resultBitmap;
}



