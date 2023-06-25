//
// Created by WZTENG on 2020/08/29 029.
//

#include <opencv2/lite/lite.h>
#include <opencv2/imgproc/types_c.h>
#include "include/opencv2/beauty/FaceLandmark.h"
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
bool FaceLandmark::hasGPU = true;
bool FaceLandmark::toUseGPU = true;
FaceLandmark *FaceLandmark::detector = nullptr;
string faceMark1000ParamPath = "";
string faceMark1000BinPath = "";
string faceBoxParamPath = "";
string faceBoxBinPath = "";
std::vector<FaceKeyPoint> keyPointList;

FaceLandmark::FaceLandmark(string faceMark1000ParamPathIn,string faceMark1000BinPathIn,string faceBoxParamPathIn,string faceBoxBinPathIn) {
    faceMark1000ParamPath = faceMark1000ParamPathIn;
    faceMark1000BinPath = faceMark1000BinPathIn;
    faceBoxParamPath = faceBoxParamPathIn;
    faceBoxBinPath = faceBoxBinPathIn;
}

void FaceLandmark::ClearFaceKeyPoint(){
    keyPointList.clear();
}

FaceLandmark::~FaceLandmark() {
    FaceNet->clear();
    LandmarkNet->clear();
    keyPointList.clear();
    delete FaceNet;
    delete LandmarkNet;
}

std::vector<FaceKeyPoint> FaceLandmark::detect(JNIEnv *env, jobject image) {
    if (keyPointList.size() != 0){
        return keyPointList;
    }
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
            new lite::ncnn::cv::face::detect::FaceBoxesV2(faceBoxParamPath, faceBoxBinPath);
    std::vector<lite::types::Boxf> detected_boxes;
    faceboxesv2->detect(img, detected_boxes);
    delete faceboxesv2;
    //不存在头像，放回空数组
    if (detected_boxes.size() == 0){
        return keyPointList;
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
    lite::ncnn::cv::face::align::FaceLandmark1000 *face_landmarks_1000 =new lite::ncnn::cv::face::align::FaceLandmark1000(faceMark1000ParamPath, faceMark1000BinPath);
    lite::types::Landmarks landmarks;
    face_landmarks_1000->detect(roi, landmarks);
    delete face_landmarks_1000;
    for (int i = 0; i < landmarks.points.size(); ++i) {
        FaceKeyPoint keypoint;
        keypoint.p = cv::Point2f(x+landmarks.points[i].x, y+landmarks.points[i].y);
        keyPointList.push_back(keypoint);
    }
    img.release();
    AndroidBitmap_unlockPixels(env, image);
    return keyPointList;
}
