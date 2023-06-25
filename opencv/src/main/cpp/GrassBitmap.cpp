//
// Created by libowu on 2021/10/28.
// 主要是模糊处理
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
#include "include/opencv2/hsl/ColorSpace.hpp"
#include "opencv2/grass/GrassBitmap.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

Mat makeBitmapJoin(cv::Mat& image,Mat addImg, double addImgWeight, cv::Point point,int type);
cv::Mat shapeCut(const cv::Mat& image,Shape shapeData);
Mat gaussianBlur(const cv::Mat& image, int count);
cv::Mat bitmapJoin(cv::Mat& image,cv::Mat addImg);
cv::Mat bitmapJoin(cv::Mat& image,cv::Mat addImg,cv::Mat mask);

//圆形
int mCircle = 0;
//矩形
int mRect = 1;

jobject grassBitmap(JNIEnv *env, jclass clazz, jobject bitmap,Shape data){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    Mat gauss = gaussianBlur(image, data.grassWeight);
    Mat out = shapeCut(image,data);

    if (data.shapeType == mCircle)
    {
        //圆形模糊
        cv::Point point(data.circleCenter.x - data.circleRadio, data.circleCenter.y - data.circleRadio);
        Mat result = makeBitmapJoin(gauss, out, 1, point,data.shapeType);
        jobject resultBitmap = matToBitmap(env, result, true, bitmap);
        out.release();
        gauss.release();
        result.release();
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    }
    else if (data.shapeType == mRect) {
        //矩形模糊
        cv::Point point(data.rectStartPosition.x, data.rectStartPosition.y);
        Mat result = makeBitmapJoin(gauss, out, 1, point,data.shapeType);
        jobject resultBitmap = matToBitmap(env, out, true, bitmap);
        out.release();
        gauss.release();
        result.release();
        AndroidBitmap_unlockPixels(env, bitmap);
        return resultBitmap;
    }
    return bitmap;
}

/**
 * 高斯模糊图片
 * image : 目标图片
 * count : 高斯模糊次数，次数越高，模糊程度越大
 */
Mat gaussianBlur(const cv::Mat& image, int count)
{
    cv::Mat out;
    //图像的高斯核模糊为5*5的大小，opencv中，高斯核的大小必须是奇数。因为高斯模糊的作用效果在高斯卷积覆盖范围的中间位置。
    cv::GaussianBlur(image, out, cv::Size(5, 5), 3, 3);
    //这里为什么进行循环100次呢？在上面调用GaussianBlur方法后，out已经被写入内存中了。
    //也就是out可作为输入对象，也可以作为输出对象，如果只执行一次GaussianBlur方法，高斯模糊效果有限，所以这里进行了一百次循环，每次模糊效果都是在上一次模糊的效果的基础上进行的，所以100次循环后，效果已经非常明显了了
    for (int i = 0; i < count; i++)
    {
        cv::GaussianBlur(out, out, cv::Size(11, 11), 15, 0, 4);
    }
    //cv::imshow("dealPicTwo", out);
    //cv::waitKey(0);
    return out;
}

/**
 * 裁剪图片
 * @param env
 * @param clazz
 * @param bitmap
 * @param shapeData
 * @return
 */
jobject cutoutBitmap(JNIEnv *env, jclass clazz, jobject bitmap,Shape shapeData){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);
    Rect rect(shapeData.rectStartPosition.x, shapeData.rectStartPosition.y, shapeData.rectWidth, shapeData.rectHeight);
    Mat result = image(rect);
    jobject resultBitmap = matToBitmap(env, result, true, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    image.release();
    result.release();
    return resultBitmap;
}

cv::Mat shapeCut(const cv::Mat& image,Shape shapeData) {
    cv::Mat tempImage;
    image.copyTo(tempImage);
    cv::cvtColor(tempImage, tempImage, COLOR_BGR2BGRA);
    //圆的半径
    int radio = shapeData.circleRadio;
    //圆心x坐标
    int centerX = shapeData.circleCenter.x;
    //圆心的y坐标
    int centerY = shapeData.circleCenter.y;
    cv::Point point(centerX-radio, centerY-radio);
    int channel = tempImage.channels();
    int height = tempImage.rows;
    int widht = tempImage.cols * channel;
    int index = 0;
    Mat result(Size(shapeData.rectWidth, shapeData.rectHeight), CV_8UC4);
    if (shapeData.shapeType == mCircle) {
        Mat tmp(Size(radio * 2, radio * 2), CV_8UC4);
        result = tmp;
    }
    else if (shapeData.shapeType == mRect)
    {
        Mat tmp(Size(shapeData.rectWidth, shapeData.rectHeight), CV_8UC4);
        result = tmp;
    }
    int addImgRows = 0;
    int addImgCols = 0;
    int resultIndex = 0;
    //标准圆的计算公式：(x-a)²+(y-b)²=r²
    for (int h = 0; h < height; h++) {
        const uchar* in = tempImage.ptr<uchar>(h);
        uchar* addIn = result.ptr<uchar>(0);
        if (shapeData.shapeType == mCircle) {
            if (h >= point.y && h < point.y + radio * 2)
            {
                //cout << "高度为：" << addImgRows << endl;
                addIn = result.ptr<uchar>(addImgRows);
                addImgRows++;
            }
        }
        else if (shapeData.shapeType == mRect) {
            if (h > shapeData.rectStartPosition.y && h <= shapeData.rectStartPosition.y + shapeData.rectHeight)
            {
                addIn = result.ptr<uchar>(addImgRows);
                addImgRows++;
            }
        }
        addImgCols = 0;
        index = 0;
        resultIndex = 0;
        for (int w = 0; w < widht; w = w + channel) {
            uchar redValue = in[w + 2];
            uchar greenValue = in[w + 1];
            uchar blueValue = in[w + 0];
            int resultRedValue = redValue;
            int resultGreenValue = greenValue;
            int resultBlueValue = blueValue;
            if (shapeData.shapeType == mCircle) {
                //(x-a)²+(y-b)² = r²
                int currentIndexRadio = (pow((index - centerX), 2) + pow((h - centerY), 2));
                if ( currentIndexRadio <= pow(radio,2))
                {
                    addIn[resultIndex] = blueValue;
                    addIn[resultIndex + 1] = greenValue;
                    addIn[resultIndex + 2] = redValue;

                    //这里通过修改透明度来达到羽化的效果
                    float length = radio - sqrt(currentIndexRadio);
                    int aph = 255 * (length/ shapeData.eclosionValue);
                    if (aph > 255) {
                        aph = 255;
                    }
                    else if (aph < 0) {
                        aph = 0;
                    }

                    //cout << "透明度：" << aph << endl;
                    addIn[resultIndex + 3] = aph;
                    resultIndex = resultIndex + 4;
                }
                else {
                    if (abs(index - centerX) <= radio && abs(h - centerY) <= radio)
                    {
                        addIn[resultIndex] = 0;
                        addIn[resultIndex + 1] = 0;
                        addIn[resultIndex + 2] = 0;
                        addIn[resultIndex + 3] = 0;
                        resultIndex = resultIndex + 4;
                    }
                }
            }
            else if (shapeData.shapeType == mRect)
            {
                if (index >= shapeData.rectStartPosition.x &&
                    index < shapeData.rectStartPosition.x + shapeData.rectWidth &&
                    h >= shapeData.rectStartPosition.y &&
                    h < shapeData.rectStartPosition.y + shapeData.rectHeight) {
                    resultGreenValue = greenValue;
                    resultRedValue = redValue;
                    resultBlueValue = blueValue;
                    addIn[resultIndex] = resultBlueValue;
                    addIn[resultIndex + 1] = resultGreenValue;
                    addIn[resultIndex + 2] = resultRedValue;
                    addIn[resultIndex + 3] = 255;
                    resultIndex = resultIndex + 4;

                }
                else {
                    resultIndex = 0;
                }
            }

            index++;
        }
    }
    return result;
}

/**
 * 对外的图片融合接口
 * @param env
 * @param clazz
 * @param bitmap
 * @param addBitmap
 * @param addImgWeight
 * @param point
 * @param type
 * @return
 */
jobject twoBitmapToOne(JNIEnv *env, jclass clazz, jobject bitmap,jobject addBitmap, double addImgWeight, cv::Point point){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;

    CV_Assert(AndroidBitmap_getInfo(env, addBitmap, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, addBitmap, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    cv::Mat addImage(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);
    cv::Mat result = bitmapJoin(image,addImage);
    cvtColor(result,result,CV_RGBA2RGB);
    jobject resultBitmap = matToBitmap(env, result, true, addBitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, addBitmap);
    image.release();
    result.release();
    addImage.release();
    return resultBitmap;
}

jobject twoBitmapToOne(JNIEnv *env, jclass clazz, jobject bitmap,jobject addBitmap,jobject mask,double addImgWeight, cv::Point point){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat image(info.height, info.width, CV_8UC4, pixels);

    AndroidBitmapInfo infoTwo;
    void *pixelsTwo;

    CV_Assert(AndroidBitmap_getInfo(env, addBitmap, &infoTwo) >= 0);
    CV_Assert(infoTwo.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoTwo.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, addBitmap, &pixelsTwo) >= 0);
    CV_Assert(pixelsTwo);
    cv::Mat addImage(infoTwo.height, infoTwo.width, CV_8UC4, pixelsTwo);

    AndroidBitmapInfo infoMask;
    void *pixelsMask;
    CV_Assert(AndroidBitmap_getInfo(env, mask, &infoMask) >= 0);
    CV_Assert(infoMask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              infoMask.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, mask, &pixelsMask) >= 0);
    CV_Assert(pixelsMask);
    cv::Mat maskMt(infoMask.height, infoMask.width, CV_8UC4, pixelsMask);

    cv::Mat result = bitmapJoin(image,addImage,maskMt);
    jobject resultBitmap = matToBitmap(env, result, true, addBitmap);
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, addBitmap);
    AndroidBitmap_unlockPixels(env, mask);
    image.release();
    result.release();
    addImage.release();
    maskMt.release();
    return resultBitmap;
}

cv::Mat bitmapJoin(cv::Mat& image,cv::Mat addImg){
    int w = image.cols * image.channels();
    int h = image.rows;
    uchar channel = image.channels();
    cv::Mat result(image.size(),image.type());
    for (int row = 0; row < h; row++) {
        uchar *out = result.ptr<uchar>(row);
        const uchar *in = image.ptr<uchar>(row);
        uchar *add = addImg.ptr<uchar>(row);
        for (int col = 0; col < w; col = col + channel) {
            float weight = add[col + 3] / 255.0;
            int a = (1-weight) * in[col+0]+add[col+0];
            int b = (1-weight) * in[col+1]+add[col+1];
            int c = (1-weight) * in[col+2]+add[col+2];
            if (a > 255){
                a = 255;
            }
            if (b > 255){
                b = 255;
            }
            if (c > 255){
                c = 255;
            }
            out[col+0] = a;
            out[col+1] = b;
            out[col+2] = c;
            out[col+3] = 255;
        }
    }

    return result;
}

cv::Mat bitmapJoin(cv::Mat& image,cv::Mat addImg,cv::Mat mask){
    int w = image.cols * image.channels();
    int h = image.rows;
    uchar channel = image.channels();
    cv::Mat result(image.size(),image.type());
    for (int row = 0; row < h; row++) {
        uchar *out = result.ptr<uchar>(row);
        const uchar *in = image.ptr<uchar>(row);
        uchar *add = addImg.ptr<uchar>(row);
        uchar *maskIn = mask.ptr<uchar>(row);
        for (int col = 0; col < w; col = col + channel) {
            uchar maxMask = MAX(maskIn[col + 0], MAX(maskIn[col + 1], maskIn[col + 2]));
            if (maxMask == 0)
            {
                out[col + 2] = in[col+2];
                out[col + 1] = in[col+1];
                out[col] = in[col+0];
                out[col+3] = 255;
                continue;
            }
            float weight = add[col + 3] / 255.0;
            int a = (1-weight) * in[col+0]+add[col+0];
            int b = (1-weight) * in[col+1]+add[col+1];
            int c = (1-weight) * in[col+2]+add[col+2];
            if (a > 255){
                a = 255;
            }
            if (b > 255){
                b = 255;
            }
            if (c > 255){
                c = 255;
            }
            out[col + 2] = in[col+2] * (1 - maxMask / 255.0) + c * maxMask / 255.0;
            out[col + 1] = in[col+1] * (1 - maxMask / 255.0) + b * maxMask / 255.0;
            out[col] = in[col+0] * (1 - maxMask / 255.0) + a * maxMask / 255.0;
            /*out[col+0] = a;
            out[col+1] = b;
            out[col+2] = c;*/
            out[col+3] = 255;
        }
    }

    return result;
}

/**
 *
 * 图片融合
 * 原理：g(x) = (1-a)f(x)+af1(x) g(x)为融合后的图片 f(x)为背景图 f1(x)为需要融合的图片 a为融合图片的比重
 *
 * */
Mat makeBitmapJoin(cv::Mat& image,Mat addImg, double addImgWeight, cv::Point point,int type)
{
    //构造一个临时的图片
    cv::Mat tempImage;
    image.copyTo(tempImage);
    //默认载入的图片时三通道的，我们需要将图片转换成四通道的
    cv::cvtColor(addImg, addImg, COLOR_BGR2BGRA);
    cv::cvtColor(tempImage, tempImage, COLOR_BGR2BGRA);

    cv::Mat out(tempImage.size(), tempImage.type());
    //获取背景图的宽高信息
    int height = tempImage.rows;
    int width = tempImage.cols * tempImage.channels();
    //获取添加图片的宽高信息
    int addImgHeight = addImg.rows;
    int addImgWidth = addImg.cols * addImg.channels();

    int addImgRows = 0;
    int addImgCols = 0;

    for (int i = 0; i < height; i++)
    {
        //获取该行的指针
        const uchar* in = tempImage.ptr<uchar>(i);
        uchar* addIn = addImg.ptr<uchar>(0);
        if (i > point.y && i < point.y + addImgHeight)
        {
            addIn = addImg.ptr<uchar>(addImgRows);
            addImgRows++;
        }
        addImgCols = 0;
        uchar* outPiex = out.ptr<uchar>(i);

        if (/*addImg.cols == image.cols && addImg.rows == image.rows*/false){
            //addWeighted(image,0.5,addImg,0.5,0,out);
            add(image, addImg,out,noArray(),0);
        }else{
            //遍历该行中的每一列
            for (int j = 0; j < width; j++)
            {
                if (j * 4 > width)
                {
                    break;
                }
                if (i > point.y && i < point.y + addImgHeight)
                {
                    if (j * 4 >= point.x * 4 && j * 4 <= point.x * 4 + addImgWidth)
                    {
                        if (addIn[addImgCols * 4 + 3] == 0)
                        {
                            outPiex[j * 4] = in[j * 4];
                            outPiex[j * 4 + 1] = in[j * 4 + 1];
                            outPiex[j * 4 + 2] = in[j * 4 + 2];
                            outPiex[j * 4 + 3] = in[j * 4 + 3];
                        }
                        else
                        {
                            if (type == mCircle)
                            {
                                outPiex[j * 4] = (1 - ((int)addIn[addImgCols * 4+3])/255.0) * in[j * 4] + addIn[addImgCols * 4 + 3]/255.0 * addIn[addImgCols * 4];
                                outPiex[j * 4 + 1] = (1 - ((int)addIn[addImgCols * 4+3])/255.0) * in[j * 4 + 1] + addIn[addImgCols * 4 + 3]/255.0 * addIn[addImgCols * 4 + 1];
                                outPiex[j * 4 + 2] = (1 - ((int)addIn[addImgCols * 4+3])/255.0) * in[j * 4 + 2] + addIn[addImgCols * 4 + 3]/255.0 * addIn[addImgCols * 4 + 2];
                            }
                            else {
                                float weig = ((addIn[addImgCols * 4+3]/255.0));
                                if (weig >1){
                                    weig = 1;
                                }
                                outPiex[j * 4] =(1- weig) * in[j * 4] + weig * addIn[addImgCols * 4];
                                outPiex[j * 4 + 1] = (1- weig) * in[j * 4 + 1] + weig * addIn[addImgCols * 4 + 1];
                                outPiex[j * 4 + 2] = (1- weig) * in[j * 4 + 2] + weig * addIn[addImgCols * 4 + 2];

                            }
                            outPiex[j * 4 + 3] = 255;
                        }
                        addImgCols++;
                    }
                    else
                    {
                        outPiex[j * 4] = in[j * 4];
                        outPiex[j * 4 + 1] = in[j * 4 + 1];
                        outPiex[j * 4 + 2] = in[j * 4 + 2];
                        outPiex[j * 4 + 3] = in[j * 4 + 3];
                    }
                }
                else
                {
                    outPiex[j * 4] = in[j * 4];
                    outPiex[j * 4 + 1] = in[j * 4 + 1];
                    outPiex[j * 4 + 2] = in[j * 4 + 2];
                    outPiex[j * 4 + 3] = in[j * 4 + 3];
                }
            }
        }
    }

    return out;
}

