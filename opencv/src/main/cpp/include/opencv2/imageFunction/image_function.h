//
// Created by libowu on 20-8-30.
//
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#ifndef OPENCV_IMAGEFUNCTION_H
#endif //OPENCV_IMAGEFUNCTION_H

extern jobject imageIdPhotoReplaceBg(JNIEnv *env, jclass clazz, jobject bitmap);

extern jobject
imageChooseArea(JNIEnv *env, jclass clazz, jobject bitmap, jobjectArray point);

extern jobject
imageCircleArea(JNIEnv *env, jclass clazz, jobject bitmap, jobject point, jint radius);

extern jobject
imageResize(JNIEnv *env, jclass clazz, jobject bitmap, jdouble resizeXValue, jdouble resizeYValue);

extern jobject
imageStayColorChannel(JNIEnv *env, jclass clazz, jobject bitmap, jboolean useRedChannel,
                      jboolean useGreenChannel, jboolean useBlureChannel);

extern jobject
imageMakeBitmapLight(JNIEnv *env, jclass clazz, jobject bitmap, jdouble alpha, jdouble beta);

extern jobject imageSharpening(JNIEnv *env, jclass clazz, jobject bitmap, jdouble sharpeningSize);

extern jobject makeBitmapLutChange(JNIEnv *env, jclass clazz, jobject bitmap, jstring cubePath);

extern jobject getHistogram(JNIEnv *env, jclass clazz, jobject bitmap);

extern jobject readImage(JNIEnv *env, jclass clazz, char *path, jobject bitmap,jint maxWidth);

extern jobject
chooseColor(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
            jfloat redRed, jfloat redGreen, jfloat redBlue,jfloat redBlack,
            jfloat greenRed, jfloat greenGreen, jfloat greenBlue,jfloat greenBlack,
            jfloat blueRed, jfloat blueGreen, jfloat blueBlue,jfloat blueBlack,
            jfloat cyanRed, jfloat cyanGreen, jfloat cyanBlue,jfloat cyanBlack,
            jfloat yellowRed, jfloat yellowGreen, jfloat yellowBlue,jfloat yellowBlack,
            jfloat carmineRed, jfloat carmineGreen, jfloat carmineBlue,jfloat carmineBlack,
            jfloat whiteRed, jfloat whiteGreen, jfloat whiteBlue,jfloat whiteBlack,
            jfloat blackRed, jfloat blackGreen, jfloat blackBlue,jfloat blackBlack,
            jboolean is_relative);



extern jobject mixColorChannel(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, jfloat redChannelRed,
                               jfloat redChannelGreen, jfloat redChannelBlue,
                               jfloat greenChannelRed,
                               jfloat greenChannelGreen, jfloat greenChannelBlue,
                               jfloat blueChannelRed,
                               jfloat blueChannelGreen, jfloat blueChannelBlue);

extern jobject
curve(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, double *xArray, double *yArray, int pointSize,
      int RGB);

extern jobject
deformation(JNIEnv *env, jclass clazz, jobject bitmap, Point centerPoint, Point movePoint,
            int radio, int str);

extern jobject
usmFilter(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,int Radius, int Amount, int Threshold);

extern jobject toChangeHSV_H(JNIEnv *env, jclass clazz, jobject bitmap, int saturation);

extern jobject
changeBrightnessContrast(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask, int brightness, int contrast);

extern jobject
toChangeHsl(JNIEnv *env, jclass clazz, jobject bitmap, int hue, int saturation, int brightness,
            int colorChannel);

extern jobject curve(JNIEnv
*env,
jclass clazz, jobject
bitmap,
jobject mask,
double *xRedArray,
double *yRedArray,
int pointRedSize,
double *xGreenArray,
double *yGreenArray,
int pointGreenSize,
double *xBlueArray,
double *yBlueArray,
int pointBlueSize,
double *xRgbArray,
double *yRgbArray,
int pointRgbSize
);

extern jintArray getPixelColor(JNIEnv *env, jclass clazz, jobject bitmap,int x,int y);
extern jobject getRotationBitmap(JNIEnv *env, jclass clazz, jobject bitmap,int angle);
extern jobject makeBitmapRollingXY(JNIEnv *env, jclass clazz, jobject bitmap,jboolean isY);
extern jobject autoContrast(JNIEnv *env, jclass clazz, jobject bitmap);
extern jobject ChangeTemp(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint percent);
extern jobject ChangeColorHue(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,jint percent);
extern jobject denoise(JNIEnv *env, jclass clazz, jobject bitmap,jfloat strength);
extern jobject VibranceAlgorithm(JNIEnv *env, jclass clazz, jobject bitmap,jint strength);
extern jobject getSigleColorPhoto(JNIEnv *env, jclass clazz, jobject bitmap, jint r,jint g,jint b);
extern jobject fixImage(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask);
extern jobject getRotationBitmapResize(JNIEnv *env, jclass clazz, jobject bitmap,int degree);
extern jobject removeBorder(JNIEnv *env, jclass clazz, jobject bitmap,jint borderCount,jint startH,jint endH);