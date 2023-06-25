//
// Created by Administrator on 2022/6/17.
//

#include "Test.h"
#include "net.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/utils/utils.h>
#include <opencv2/dnn.hpp>
#include "opencv2/lite/lite.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;
using namespace cv::dnn;


jobject test(JNIEnv *env, jobject image,jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath){
    AndroidBitmapInfo img_size;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, image, &img_size) >= 0);
    CV_Assert(img_size.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              img_size.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, image, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(img_size.height, img_size.width, CV_8UC4, pixels);
    cvtColor(img,img,CV_RGBA2BGR);
    Mat tmp = img.clone();
    /*lite::ncnn::cv::face::align::FaceLandmark1000 *face_landmarks_1000 =new lite::ncnn::cv::face::align::FaceLandmark1000(jstring2str(env,paramPathForBox), jstring2str(env,binPathForBox));
    lite::types::Landmarks landmarks;
    face_landmarks_1000->detect(img, landmarks);
    lite::utils::draw_landmarks_inplace(img, landmarks);
    delete face_landmarks_1000;*/
   /* auto *yolov5face = new lite::ncnn::cv::face::detect::YOLO5Face(jstring2str(env,paramPathForBox), jstring2str(env,binPathForBox), 1, 640, 640);
    std::vector<lite::types::BoxfWithLandmarks> detected_boxes;
    yolov5face->detect(img, detected_boxes);
    lite::utils::draw_boxes_with_landmarks_inplace(img, detected_boxes);
    delete yolov5face;*/
    lite::ncnn::cv::face::detect::FaceBoxesV2 *faceboxesv2 =
            new lite::ncnn::cv::face::detect::FaceBoxesV2(jstring2str(env,paramPathForBox), jstring2str(env,binPathForBox));
    std::vector<lite::types::Boxf> detected_boxes;
    faceboxesv2->detect(img, detected_boxes);
//    lite::utils::draw_boxes_inplace(img, detected_boxes);
    delete faceboxesv2;

    int x = detected_boxes[0].rect().x - detected_boxes[0].rect().width*0.1;
    int y = detected_boxes[0].rect().y - detected_boxes[0].rect().height*0.1;
    if (x < 0){
        x = 0;
    }
    if (y < 0){
        y = 0;
    }
    int width = detected_boxes[0].rect().width + detected_boxes[0].rect().width*0.1*2;
    if (x + width > img.cols){
        width = img.cols - x;
    }
    int height = detected_boxes[0].rect().height + detected_boxes[0].rect().height*0.1*2;
    if (y + height > img.rows){
        height = img.rows - y;
    }

    cv::Mat roi = tmp(cv::Rect(x,y,width,height)).clone();


    lite::ncnn::cv::face::align::FaceLandmark1000 *face_landmarks_1000 =new lite::ncnn::cv::face::align::FaceLandmark1000(jstring2str(env,paramPath), jstring2str(env,binPath));
    lite::types::Landmarks landmarks;
    face_landmarks_1000->detect(roi, landmarks);
    delete face_landmarks_1000;

    for (int i = 0; i < landmarks.points.size(); ++i) {
        landmarks.points[i].x = x + landmarks.points[i].x;
        landmarks.points[i].y = y + landmarks.points[i].y;
    }
    //lite::utils::draw_landmarks_inplace(img, landmarks);
    cvtColor(img,img,CV_BGR2RGB);
    jobject resultBitmap = matToBitmap(env, img, false, image);
    AndroidBitmap_unlockPixels(env, image);
    return resultBitmap;
}

jobjectArray getFacePoint(JNIEnv *env, jobject image,jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath){
    AndroidBitmapInfo img_size;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, image, &img_size) >= 0);
    CV_Assert(img_size.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              img_size.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, image, &pixels) >= 0);
    CV_Assert(pixels);
    cv::Mat img(img_size.height, img_size.width, CV_8UC4, pixels);
    cvtColor(img,img,CV_RGBA2BGR);

    lite::ncnn::cv::face::detect::FaceBoxesV2 *faceboxesv2 =
            new lite::ncnn::cv::face::detect::FaceBoxesV2(jstring2str(env,paramPathForBox), jstring2str(env,binPathForBox));
    std::vector<lite::types::Boxf> detected_boxes;
    faceboxesv2->detect(img, detected_boxes);
    delete faceboxesv2;

    jclass clazz = env->FindClass("android/graphics/Point");
    //不存在头像，放回空数组
    if (detected_boxes.size() == 0){
        jobjectArray colorArray = env->NewObjectArray(0,clazz,NULL);;
        return colorArray;
    }
    //稍微扩大一点裁剪头像区域
    int x = detected_boxes[0].rect().x - detected_boxes[0].rect().width*0.1;
    int y = detected_boxes[0].rect().y - detected_boxes[0].rect().height*0.1;
    if (x < 0){
        x = 0;
    }
    if (y < 0){
        y = 0;
    }
    int width = detected_boxes[0].rect().width + detected_boxes[0].rect().width*0.1*2;
    if (x + width > img.cols){
        width = img.cols - x;
    }
    int height = detected_boxes[0].rect().height + detected_boxes[0].rect().height*0.1*2;
    if (y + height > img.rows){
        height = img.rows - y;
    }
    //获取头部区域
    cv::Mat roi = img(cv::Rect(x,y,width,height)).clone();

    lite::ncnn::cv::face::align::FaceLandmark1000 *face_landmarks_1000 =new lite::ncnn::cv::face::align::FaceLandmark1000(jstring2str(env,paramPath), jstring2str(env,binPath));
    lite::types::Landmarks landmarks;
    face_landmarks_1000->detect(roi, landmarks);
    delete face_landmarks_1000;

    jobjectArray colorArray = env->NewObjectArray(landmarks.points.size(),clazz,NULL);
    LOGI("总长度为%d",landmarks.points.size());
    for (int i = 0; i < landmarks.points.size(); ++i) {
        landmarks.points[i].x = x + landmarks.points[i].x;
        landmarks.points[i].y = y + landmarks.points[i].y;
        int x = landmarks.points[i].x;
        int y = landmarks.points[i].y;
        jmethodID constructor = env->GetMethodID(clazz,"<init>","(II)V");
        jobject p = env->NewObject(clazz,constructor,x,y);
        env->SetObjectArrayElement(colorArray,i,p);
    }
    img.release();
    AndroidBitmap_unlockPixels(env, image);
    return colorArray;
}