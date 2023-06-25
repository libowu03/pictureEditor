//
// Created by libowu on 20-8-29.
//
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#ifndef OPENCV_UTILS_H
#define OPENCV_UTILS_H
extern jobject matToBitmap(JNIEnv * env, Mat & src, bool needPremultiplyAlpha, jobject sourceBitmap);
extern void callListener(jobject listener,JNIEnv *env,jobject sourceBitmap,jobject resultBitmap, bool isSuccess, const char* msg);
extern Mat addOrCut(Mat source, double addOrCut, double scale);
extern Mat mul(Mat source, Mat target, double scale);
extern vector<string> split(const string& str, const string& delim);
extern std::string jstring2str(JNIEnv* env, jstring jstr);
extern jobject matToBitmapNew(JNIEnv *env, Mat &src, bool needPremultiplyAlpha, jobject sourceBitmap);
extern bool BitmapToMatrix(JNIEnv *env, jobject bitmap, cv::Mat &matrix);
extern bool matToBitmapPro(JNIEnv * env, cv::Mat & matrix, jobject obj_bitmap);
#endif //OPENCV_UTILS_H
