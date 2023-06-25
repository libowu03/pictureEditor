//
// Created by libowu on 20-8-29.
//
#include <jni.h>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <android/log.h>
#include <opencv2/core.hpp>

#define ASSERT(status, ret)     if (!(status)) { return ret; }
#define ASSERT_FALSE(status)    ASSERT(status, false)
#define TAG "日志"
//定义TAG之后，我们可以在LogCat通过TAG过滤出NDK打印的日志
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
using namespace std;
using namespace cv;

/**
 * mat转bitmap
 * @param env
 * @param src
 * @param needPremultiplyAlpha
 * @param sourceBitmap
 * @return
 */
jobject matToBitmap(JNIEnv *env, Mat &src, bool needPremultiplyAlpha, jobject sourceBitmap) {
    jclass java_bitmap_clazz = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid_config = env->GetMethodID(java_bitmap_clazz, "getConfig",
                                            "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(sourceBitmap, mid_config);

    jclass java_bitmap_class = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid_create = env->GetStaticMethodID(java_bitmap_class, "createBitmap",
                                                  "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class, mid_create, src.cols, src.rows,
                                                 bitmap_config);
    AndroidBitmapInfo info;
    void *pixels = 0;
    try {
        //validate
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);

        //type mat
        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (src.type() == CV_8UC1) {
                cvtColor(src, tmp, CV_GRAY2RGBA);
                int height = tmp.rows;
                int width = tmp.cols;
                //LOGE("CV_8UC1");
            } else if (src.type() == CV_8UC3) {
                cvtColor(src, tmp, CV_RGB2RGBA);
                //LOGE("CV_8UC3");

            } else if (src.type() == CV_8UC4) {
                if (needPremultiplyAlpha) {
                    cvtColor(src, tmp, COLOR_RGBA2mRGBA);

                    //LOGE("CV_8UC4_1");

                } else {
                    src.copyTo(tmp);
                    //LOGE("CV_8UC4_2");

                }
            } else {
                //LOGE("CV_8UC4_其他");

            }
            //LOGI("0通道数量为%d", (tmp.rows));
        } else {
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if (src.type() == CV_8UC1) {
                cvtColor(src, tmp, CV_GRAY2BGR565);
            } else if (src.type() == CV_8UC3) {
                cvtColor(src, tmp, CV_RGB2BGR565);
            } else if (src.type() == CV_8UC4) {
                cvtColor(src, tmp, CV_RGBA2BGR565);
            }
            //LOGI("2通道数量为%d", (tmp.rows * tmp.channels()));
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}

bool matToBitmapPro(JNIEnv * env, cv::Mat & matrix, jobject obj_bitmap){
    void * bitmapPixels;                                            // Save picture pixel data
    AndroidBitmapInfo bitmapInfo;                                   // Save picture parameters
    ASSERT_FALSE( AndroidBitmap_getInfo(env, obj_bitmap, &bitmapInfo) >= 0);        // Get picture parameters
    ASSERT_FALSE( bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                  || bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565 );          // Only ARGB? 8888 and RGB? 565 are supported
    ASSERT_FALSE( matrix.dims == 2
                  && bitmapInfo.height == (uint32_t)matrix.rows
                  && bitmapInfo.width == (uint32_t)matrix.cols );                   // It must be a 2-dimensional matrix with the same length and width
    ASSERT_FALSE( matrix.type() == CV_8UC1 || matrix.type() == CV_8UC3 || matrix.type() == CV_8UC4 );
    ASSERT_FALSE( AndroidBitmap_lockPixels(env, obj_bitmap, &bitmapPixels) >= 0 );  // Get picture pixels (lock memory block)
    ASSERT_FALSE( bitmapPixels );

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2RGBA);     break;
            case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2RGBA);      break;
            case CV_8UC4:   matrix.copyTo(tmp);                                 break;
            default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
        }
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        switch (matrix.type()) {
            case CV_8UC1:   cv::cvtColor(matrix, tmp, cv::COLOR_GRAY2BGR565);   break;
            case CV_8UC3:   cv::cvtColor(matrix, tmp, cv::COLOR_RGB2BGR565);    break;
            case CV_8UC4:   cv::cvtColor(matrix, tmp, cv::COLOR_RGBA2BGR565);   break;
            default:        AndroidBitmap_unlockPixels(env, obj_bitmap);        return false;
        }
    }
    AndroidBitmap_unlockPixels(env, obj_bitmap);
    return true;
}

jobject matToBitmapNew(JNIEnv *env, Mat &src, bool needPremultiplyAlpha, jobject sourceBitmap) {
    jclass java_bitmap_clazz = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid_config = env->GetMethodID(java_bitmap_clazz, "getConfig",
                                            "()Landroid/graphics/Bitmap$Config;");
    jobject bitmap_config = env->CallObjectMethod(sourceBitmap, mid_config);

    jclass java_bitmap_class = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid_create = env->GetStaticMethodID(java_bitmap_class, "createBitmap",
                                                  "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class, mid_create, src.cols, src.rows,
                                                 bitmap_config);
    AndroidBitmapInfo info;
    void *pixels = 0;
    try {
        //validate
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);
        //type mat
        Mat tmp(info.height, info.width, CV_8UC4, pixels);
        cvtColor(src, tmp, CV_BGR2RGBA);
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch (...) {
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}

/**
 * bitmap转mat
 * @param env
 * @param bitmap
 * @param matrix
 * @return
 */
bool BitmapToMatrix(JNIEnv *env, jobject bitmap, cv::Mat &matrix) {
    void *bitmapPixels;                                            // 保存图片像素数据
    AndroidBitmapInfo bitmapInfo;                                   // 保存图片参数

    ASSERT_FALSE(AndroidBitmap_getInfo(env, bitmap, &bitmapInfo) >= 0);        // 获取图片参数
    ASSERT_FALSE(bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888
                 || bitmapInfo.format ==
                    ANDROID_BITMAP_FORMAT_RGB_565);          // 只支持 ARGB_8888 和 RGB_565
    ASSERT_FALSE(AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels) >= 0);  // 获取图片像素（锁定内存块）
    ASSERT_FALSE(bitmapPixels);

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, bitmapPixels);    // 建立临时 mat
        tmp.copyTo(matrix);                                                         // 拷贝到目标 matrix
    } else {
        cv::Mat tmp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, bitmapPixels);
        cv::cvtColor(tmp, matrix, cv::COLOR_BGR5652RGB);
    }

    AndroidBitmap_unlockPixels(env, bitmap);            // 解锁
    return true;
}

/**
 * 执行成功失败的回调
 * @param listener
 * @param env
 * @param isSuccess 是否成功
 */
void callListener(jobject listener, JNIEnv *env, jobject sourceBitmap, jobject resultBitmap,
                  bool isSuccess, const char *msg) {
    if (listener == NULL) {
        return;
    }
    if (isSuccess) {
        jclass listenerClazz = env->GetObjectClass(listener);
        jmethodID methodId = env->GetMethodID(listenerClazz, "onSuccess",
                                              "(Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;)V");
        env->CallIntMethod(listener, methodId, sourceBitmap, resultBitmap);
    } else {
        jclass listenerClazz = env->GetObjectClass(listener);
        jmethodID methodId = env->GetMethodID(listenerClazz, "onError",
                                              "(Landroid/graphics/Bitmap;Ljava/lang/String;)V");
        env->CallIntMethod(listener, methodId, sourceBitmap, msg);
    }
}

Mat mul(Mat source, Mat target, double scale) {
    Mat result(source.size(), source.type());
    int cols = source.cols;
    int rows = source.rows;
    //相乘时必须保证行列相同，否则直接返回原数据
    if (cols != target.cols || rows != target.rows) {
        return source;
    }
    for (int h = 0; h < rows; ++h) {
        const uchar *s = source.ptr<uchar>(h);
        const uchar *t = target.ptr<uchar>(h);
        uchar *r = result.ptr<uchar>(h);
        for (int w = 0; w < cols; ++w) {
            if (source.channels() == 3) {
                uchar channelOne = 0;
                uchar channelTwo = 0;
                uchar channelThree = 0;
                channelOne = s[w] * t[w] * scale;
                channelTwo = s[w + 1] * t[w + 1] * scale;
                channelThree = s[w + 2] * t[w + 2] * scale;
                if (channelOne > 255) {
                    channelOne = 255;
                } else if (channelOne < 0) {
                    channelOne = 0;
                }
                if (channelTwo > 255) {
                    channelTwo = 255;
                } else if (channelTwo < 0) {
                    channelTwo = 0;
                }
                if (channelThree > 255) {
                    channelThree = 255;
                } else if (channelThree < 0) {
                    channelThree = 0;
                }
                r[w] = channelOne;
                r[w + 1] = channelTwo;
                r[w + 2] = channelThree;
            } else if (source.channels() == 4) {
                uchar channelOne = 0;
                uchar channelTwo = 0;
                uchar channelThree = 0;
                uchar channelFour = 0;
                channelOne = s[w] * t[w] * scale;
                channelTwo = s[w + 1] * t[w + 1] * scale;
                channelThree = s[w + 2] * t[w + 2] * scale;
                channelFour = s[w + 3] * t[w + 3] * scale;
                if (channelOne > 255) {
                    channelOne = 255;
                } else if (channelOne < 0) {
                    channelOne = 0;
                }
                if (channelTwo > 255) {
                    channelTwo = 255;
                } else if (channelTwo < 0) {
                    channelTwo = 0;
                }
                if (channelThree > 255) {
                    channelThree = 255;
                } else if (channelThree < 0) {
                    channelThree = 0;
                }
                if (channelFour > 255) {
                    channelFour = 255;
                } else if (channelFour < 0) {
                    channelFour = 0;
                }
                r[w] = channelOne;
                r[w + 1] = channelTwo;
                r[w + 2] = channelThree;
                r[w + 3] = channelFour;
            }
        }
        return result;
    }
}

Mat addOrCut(Mat source, double addOrCut, double scale) {
    Mat result(source.size(), source.type());
    int cols = source.cols;
    int rows = source.rows;
    //相乘时必须保证行列相同，否则直接返回原数据
    for (int h = 0; h < rows; ++h) {
        const uchar *s = source.ptr<uchar>(h);
        uchar *r = result.ptr<uchar>(h);
        for (int w = 0; w < cols; ++w) {
            if (source.channels() == 3) {
                uchar channelOne = 0;
                uchar channelTwo = 0;
                uchar channelThree = 0;
                channelOne = (s[w] + addOrCut) * scale;
                channelTwo = (s[w + 1] + addOrCut) * scale;
                channelThree = (s[w + 2] + addOrCut) * scale;
                if (channelOne > 255) {
                    channelOne = 255;
                } else if (channelOne < 0) {
                    channelOne = 0;
                }
                if (channelTwo > 255) {
                    channelTwo = 255;
                } else if (channelTwo < 0) {
                    channelTwo = 0;
                }
                if (channelThree > 255) {
                    channelThree = 255;
                } else if (channelThree < 0) {
                    channelThree = 0;
                }
                r[w] = channelOne;
                r[w + 1] = channelTwo;
                r[w + 2] = channelThree;
            } else if (source.channels() == 4) {
                uchar channelOne = 0;
                uchar channelTwo = 0;
                uchar channelThree = 0;
                uchar channelFour = 0;
                channelOne = (s[w] + addOrCut) * scale;
                channelTwo = (s[w + 1] + addOrCut) * scale;
                channelThree = (s[w + 2] + addOrCut) * scale;
                channelFour = (s[w + 3] + addOrCut) * scale;
                if (channelOne > 255) {
                    channelOne = 255;
                } else if (channelOne < 0) {
                    channelOne = 0;
                }
                if (channelTwo > 255) {
                    channelTwo = 255;
                } else if (channelTwo < 0) {
                    channelTwo = 0;
                }
                if (channelThree > 255) {
                    channelThree = 255;
                } else if (channelThree < 0) {
                    channelThree = 0;
                }
                if (channelFour > 255) {
                    channelFour = 255;
                } else if (channelFour < 0) {
                    channelFour = 0;
                }
                r[w] = channelOne;
                r[w + 1] = channelTwo;
                r[w + 2] = channelThree;
                r[w + 3] = channelFour;
            }
        }
        return result;
    }
}

vector<string> split(const string& str, const string& delim) {
    vector<string> res;
    if("" == str) return res;
    //先将要切割的字符串从string类型转换为char*类型
    char * strs = new char[str.length() + 1] ; //不要忘了
    strcpy(strs, str.c_str());

    char * d = new char[delim.length() + 1];
    strcpy(d, delim.c_str());

    char *p = strtok(strs, d);
    while(p) {
        string s = p; //分割得到的字符串转换为string类型
        res.push_back(s); //存入结果数组
        p = strtok(NULL, d);
    }
    return res;
}


jstring str2jstring(JNIEnv* env,const char* pat)
{
    //定义java String类 strClass
    jclass strClass = (env)->FindClass("Ljava/lang/String;");
    //获取String(byte[],String)的构造器,用于将本地byte[]数组转换为一个新String
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    //建立byte数组
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    //将char* 转换为byte数组
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
    // 设置String, 保存语言类型,用于byte数组转换至String时的参数
    jstring encoding = (env)->NewStringUTF("GB2312");
    //将byte数组转换为java String,并输出
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
}


std::string jstring2str(JNIEnv* env, jstring jstr)
{
    char*   rtn   =   NULL;
    jclass   clsstring   =   env->FindClass("java/lang/String");
    jstring   strencode   =   env->NewStringUTF("GB2312");
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize   alen   =   env->GetArrayLength(barr);
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
    if(alen   >   0)
    {
        rtn   =   (char*)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return   stemp;
}




                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    