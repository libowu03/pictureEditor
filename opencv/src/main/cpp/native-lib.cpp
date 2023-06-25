#include <jni.h>
#include "opencv2/filter/filter.h"
#include "opencv2/imageFunction/image_function.h"
#include "opencv2/color/ColorBalance.h"
#include "opencv2/match/SplineInterpolation.h"
#include <jni.h>
#include <android/asset_manager_jni.h>
#include <opencv2/utils/utils.h>
#include "opencv2/grass/GrassBitmap.h"
#include "opencv2/shapeChange/ShapeChange.h"
#include "opencv2/beauty/BeautyPeople.h"
#include "net.h"
#include "opencv2/base/HightLightAndDarkLight.h"
#include "opencv2/beauty/FaceLandmark.h"
#include "opencv2/lut/lut3d.h"
#include "opencv2/skin/SoftSkin.h"
#include "lips/LipsColorChange.h"
#include "HsbNew.h"
#include "nose/SteroNose.h"
#include "opencv2/skin/SkinColor.h"
#include "opencv2/beauty/FaceBlush.h"
#include "opencv2/beauty/BeautyIris.h"
#include "opencv2/beauty/EyeBrow.h"
#include "opencv2/beauty/EyeLine.h"
#include "opencv2/superres.hpp"
#include "Mask.h"
#include "MixImageLayer.h"
#include "Test.h"
#include "Superres.h"
#include "effects.h"

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_setReplaceBackground(JNIEnv *env, jclass clazz,
                                                            jobject bitmap) {
    return imageIdPhotoReplaceBg(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapColorCounter(JNIEnv *env, jclass clazz,
                                                              jobject bitmap) {
    return makeBitmapColorCounter(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapGray(JNIEnv *env, jclass clazz, jobject bitmap) {
    return makeBitmapGray(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapBlur(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                      jint blurSize) {
    return makeBitmapBlur(env, clazz, bitmap,mask, blurSize);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapWhiteAndBlack(JNIEnv *env, jclass clazz,
                                                               jobject bitmap) {
    return makeBitmapWhiteAndBlack(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapNostalgia(JNIEnv *env, jclass clazz,
                                                           jobject bitmap) {
    return makeBitmapNostalgia(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapFounding(JNIEnv *env, jclass clazz, jobject bitmap,
                                                          jobject listener) {
    return makeBitmapFounding(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapFrozen(JNIEnv *env, jclass clazz, jobject bitmap,
                                                        jobject listener) {
    return makeBitmapFrozen(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapComics(JNIEnv *env, jclass clazz, jobject bitmap,
                                                        jobject listener) {
    return makeBitmapComics(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapErode(JNIEnv *env, jclass clazz, jobject bitmap,
                                                       jobject listener) {
    return makeBitmapErode(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapEdge(JNIEnv *env, jclass clazz, jobject bitmap) {
    return makeBitmapEdge(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapDilate(JNIEnv *env, jclass clazz, jobject bitmap) {
    return makeBitmapDilate(env, clazz, bitmap);
}

/**
 * 抠图
 */
extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageChooseArea(JNIEnv *env, jclass clazz, jobject bitmap,
                                                       jobjectArray point) {
    return imageChooseArea(env, clazz, bitmap, point);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapToPixel(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                         jint pixels_size) {
    return makeBitmapToPixels(env, clazz, bitmap,mask, pixels_size);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageCircleArea(JNIEnv *env, jclass clazz, jobject bitmap,
                                                       jobject center_point, jint radius) {
    return imageCircleArea(env, clazz, bitmap, center_point, radius);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageResize(JNIEnv *env, jclass clazz, jobject bitmap,
                                                   jdouble resize_xvalue, jdouble resize_yvalue) {
    return imageResize(env, clazz, bitmap, resize_xvalue, resize_yvalue);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageChooseColorChannel(JNIEnv *env, jclass clazz,
                                                               jobject bitmap,
                                                               jboolean use_red_channel,
                                                               jboolean use_green_channel,
                                                               jboolean use_blue_channel) {
    return imageStayColorChannel(env, clazz, bitmap, use_red_channel, use_green_channel,
                                 use_blue_channel);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageLightAndContrastChange(JNIEnv *env, jclass clazz,
                                                                   jobject bitmap, jdouble light,
                                                                   jdouble contrast) {
    return imageMakeBitmapLight(env, clazz, bitmap, light, contrast);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageSharpening(JNIEnv *env, jclass clazz, jobject bitmap,
                                                       jdouble size) {
    return imageSharpening(env, clazz, bitmap, size);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapRemoveFog(JNIEnv *env, jclass clazz,
                                                           jobject bitmap,jobject mask,float value) {
    return makeBitmapRemoveHaze(env, clazz, bitmap, mask,value);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapChangeByLut(JNIEnv *env, jclass clazz,
                                                             jobject bitmap, jstring cube_path) {
    return makeBitmapLutChange(env, clazz, bitmap, cube_path);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getImageChannelInfo(JNIEnv *env, jclass clazz,
                                                           jobject bitmap) {
    return getHistogram(env, clazz, bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeColorByChannelPro(JNIEnv *env, jclass clazz,
                                                               jobject bitmap,jobject mask,
                                                               jfloat red_red,jfloat red_green, jfloat red_blue,jfloat  red_black,
                                                               jfloat green_red, jfloat green_green,jfloat green_blue, jfloat green_black,
                                                               jfloat blue_red,jfloat blue_green, jfloat blue_blue,jfloat blue_black,
                                                               jfloat cyan_red, jfloat cyan_green,jfloat cyan_blue,jfloat cyan_black,
                                                               jfloat yellow_red,jfloat yellow_green,jfloat yellow_blue,jfloat yellow_black,
                                                               jfloat carmine_red,jfloat carmine_green,jfloat carmine_blue,jfloat carmine_black,
                                                               jfloat white_red,jfloat white_green,jfloat white_blue,jfloat white_black,
                                                               jfloat black_red,jfloat black_green,jfloat black_blue,jfloat black_black,
                                                               jboolean is_relative) {
    return chooseColor(env,clazz,bitmap,mask,
                       red_red,red_green,red_blue,red_black,
                       green_red,green_green,green_blue,green_black,
                       blue_red,blue_green,blue_blue,blue_black,
                       cyan_red,cyan_green,cyan_blue,cyan_black,
                       yellow_red,yellow_green,yellow_blue,yellow_black,
                       carmine_red,carmine_green,carmine_blue,carmine_black,
                       white_red,white_green,white_blue,white_black,
                       black_red,black_green,black_blue,black_black,is_relative
                       );
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_mixColorChannel(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                       jfloat redChannelRed,
                                                       jfloat redChannelGreen,
                                                       jfloat redChannelBlue,
                                                       jfloat greenChannelRed,
                                                       jfloat greenChannelGreen,
                                                       jfloat greenChannelBlue,
                                                       jfloat blueChannelRed,
                                                       jfloat blueChannelGreen,
                                                       jfloat blueChannelBlue) {
    return mixColorChannel(env, clazz, bitmap,mask, redChannelRed,
                           redChannelGreen, redChannelBlue, greenChannelRed,
                           greenChannelGreen, greenChannelBlue, blueChannelRed,
                           blueChannelGreen, blueChannelBlue);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_image_library_opencv_OpenCvIn_cubicSplineGetY(JNIEnv *env, jclass clazz, jdouble x,
                                                       jdoubleArray x_controller,
                                                       jdoubleArray y_controller) {
    jdouble *xControllerArray;
    jdouble *yControllerArray;
    xControllerArray = (*env).GetDoubleArrayElements(x_controller, NULL);
    yControllerArray = (*env).GetDoubleArrayElements(y_controller, NULL);
    jsize length = (*env).GetArrayLength(x_controller);
    CCubicSplineInterpolation cul(xControllerArray, yControllerArray, length);
    jdouble result = 0.0;
    bool j = cul.GetYByX(x, result);
    return result;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_curveTools(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                  jint channelType, jdoubleArray x_controller,
                                                  jdoubleArray y_controller) {
    jdouble *xControllerArray;
    jdouble *yControllerArray;
    xControllerArray = (*env).GetDoubleArrayElements(x_controller, NULL);
    yControllerArray = (*env).GetDoubleArrayElements(y_controller, NULL);
    jsize length = (*env).GetArrayLength(x_controller);
    return curve(env, clazz, bitmap, mask,xControllerArray, yControllerArray, length, channelType);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_curveToolsPro(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                     jdoubleArray x_red_controller,
                                                     jdoubleArray y_red_controller,
                                                     jdoubleArray x_green_controller,
                                                     jdoubleArray y_green_controller,
                                                     jdoubleArray x_blue_controller,
                                                     jdoubleArray y_blue_controller,
                                                     jdoubleArray x_rgb_controller,
                                                     jdoubleArray y_rgb_controller) {

    //红色通道
    jdouble *xRedControllerArray;
    jdouble *yRedControllerArray;
    xRedControllerArray = (*env).GetDoubleArrayElements(x_red_controller, NULL);
    yRedControllerArray = (*env).GetDoubleArrayElements(y_red_controller, NULL);
    jsize lengthRed = (*env).GetArrayLength(x_red_controller);

    //绿色通道
    jdouble *xGreenControllerArray;
    jdouble *yGreenControllerArray;
    xGreenControllerArray = (*env).GetDoubleArrayElements(x_green_controller, NULL);
    yGreenControllerArray = (*env).GetDoubleArrayElements(y_green_controller, NULL);
    jsize lengthGreen = (*env).GetArrayLength(x_green_controller);

    //蓝色通道
    jdouble *xBlueControllerArray;
    jdouble *yBlueControllerArray;
    xBlueControllerArray = (*env).GetDoubleArrayElements(x_blue_controller, NULL);
    yBlueControllerArray = (*env).GetDoubleArrayElements(y_blue_controller, NULL);
    jsize lengthBlue = (*env).GetArrayLength(x_blue_controller);

    //rgb三通道
    jdouble *xRgbControllerArray;
    jdouble *yRgbControllerArray;
    xRgbControllerArray = (*env).GetDoubleArrayElements(x_rgb_controller, NULL);
    yRgbControllerArray = (*env).GetDoubleArrayElements(y_rgb_controller, NULL);
    jsize lengthRgb = (*env).GetArrayLength(x_rgb_controller);
    return curve(env, clazz, bitmap,mask, xRedControllerArray, yRedControllerArray, lengthRed,
                 xGreenControllerArray, yGreenControllerArray, lengthGreen, xBlueControllerArray,
                 yBlueControllerArray, lengthBlue, xRgbControllerArray, yRgbControllerArray,
                 lengthRgb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_liquefying(JNIEnv *env, jclass clazz, jobject bitmap,
                                                  jobject center_point, jobject move_point,
                                                  jint radio,
                                                  jint strength) {

    jclass pointClazz = env->FindClass("android/graphics/Point");
    jfieldID javaPointX = env->GetFieldID(pointClazz, "x", "I");
    jfieldID javaPointY = env->GetFieldID(pointClazz, "y", "I");
    jint pointX = env->GetIntField(center_point, javaPointX);
    jint pointY = env->GetIntField(center_point, javaPointY);

    jclass pointClazzCp = env->FindClass("android/graphics/Point");
    jfieldID javaPointXCp = env->GetFieldID(pointClazzCp, "x", "I");
    jfieldID javaPointYCp = env->GetFieldID(pointClazzCp, "y", "I");
    jint pointXCp = env->GetIntField(move_point, javaPointXCp);
    jint pointYCp = env->GetIntField(move_point, javaPointYCp);

    Point center(pointX, pointY);
    Point move(pointXCp, pointYCp);
    return deformation(env, clazz, bitmap, center, move, radio, strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_usmFilter(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
                                                 jint radios, jint amuont, jint threshold) {
    return usmFilter(env, clazz, bitmap,mask ,radios, amuont, threshold);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_brinessAndContrastChange(JNIEnv *env, jclass clazz,
                                                                jobject bitmap,jobject mask, jint brightness,
                                                                jint contrast) {
    return changeBrightnessContrast(env, clazz, bitmap,mask, brightness, contrast);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toChangeHSV(JNIEnv *env, jclass clazz, jobject bitmap,
                                                   jint hsvHValue) {
    return toChangeHSV_H(env, clazz, bitmap, hsvHValue);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toChangeHsl(JNIEnv *env, jclass clazz, jobject bitmap,
                                                   jint hue, jint saturation, jint brightness,
                                                   jint colorChannel) {
    return toChangeHsl(env, clazz, bitmap, hue, saturation, brightness, colorChannel);
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_image_library_opencv_OpenCvIn_getPixelColor(JNIEnv *env, jclass clazz, jobject bitmap,
                                                     jint x, jint y) {
    return getPixelColor(env,clazz,bitmap,x,y);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getRotationBitmap(JNIEnv *env, jclass clazz, jobject bitmap,jint angle) {
    return getRotationBitmap(env,clazz,bitmap,angle);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getRotationXY(JNIEnv *env, jclass clazz, jobject bitmap,
                                                     jboolean is_y) {
    return makeBitmapRollingXY(env,clazz,bitmap,is_y);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeHSB(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
                                                 jfloat rgb_h, jfloat rgb_s, jfloat rgb_b, jfloat r_h,
                                                 jfloat r_s, jfloat r_b, jfloat g_h, jfloat g_s,
                                                 jfloat g_b, jfloat b_h, jfloat b_s, jfloat b_b,
                                                 jfloat c_h, jfloat c_s, jfloat c_b, jfloat m_h,
                                                 jfloat m_s, jfloat m_b, jfloat y_h, jfloat y_s,
                                                 jfloat y_b,
                                                 jint leftBorder,jint rightBorder,jint left,jint right
                                                 ) {
    HueChange hsb;
    hsb.rgbS = rgb_s;
    hsb.rgbH = rgb_h;
    hsb.rgbB = rgb_b;

    hsb.rS = r_s;
    hsb.rH = r_h;
    hsb.rB = r_b;

    hsb.gS = g_s;
    hsb.gH = g_h;
    hsb.gB = g_b;

    hsb.bS = b_s;
    hsb.bH = b_h;
    hsb.bB = b_b;

    hsb.cS = c_s;
    hsb.cH = c_h;
    hsb.cB = c_b;

    hsb.mS = m_s;
    hsb.mH = m_h;
    hsb.mB = m_b;

    hsb.yS = y_s;
    hsb.yH = y_h;
    hsb.yB = y_b;

    hsb.minChoose = leftBorder;
    hsb.maxChoose = rightBorder;
    hsb.leftChoose = left;
    hsb.rightChoose = right;
    //return changeHsb(env,clazz,bitmap,hsb);
    return startChangeHsb(env,clazz,bitmap,mask,hsb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeColorBalance(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                          jint cyan_and_red_low,
                                                          jint carmine_and_green_low,
                                                          jint yellow_and_blue_low,
                                                          jint cyan_and_red_middle,
                                                          jint carmine_and_green_middle,
                                                          jint yellow_and_blue_middle,
                                                          jint cyan_and_red_hight,
                                                          jint carmine_and_green_hight,
                                                          jint yellow_and_blue_hight) {
    ColorBalance balance;
    balance.cyanAndRedLow = cyan_and_red_low;
    balance.carmineAndGreenLow = carmine_and_green_low;
    balance.yellowAndBlueLow = yellow_and_blue_low;

    balance.cyanAndRedMiddle = cyan_and_red_middle;
    balance.carmineAndGreenMiddle = carmine_and_green_middle;
    balance.yellowAndBlueMiddle = yellow_and_blue_middle;

    balance.cyanAndRedHight = cyan_and_red_hight;
    balance.carmineAndGreenHight = carmine_and_green_hight;
    balance.yellowAndBlueHight = yellow_and_blue_hight;
    return colorBalance(env,clazz,bitmap,mask,balance);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_grassCircle(JNIEnv *env, jclass clazz, jobject bitmap,
                                                   jint center_x, jint center_y, jint radius,
                                                   jint liquidation) {
    Shape shape;
    cv::Point circleCenter(center_x,center_y);
    shape.circleRadio = radius;
    shape.shapeType = 0;
    shape.circleCenter = circleCenter;
    shape.eclosionValue = liquidation;
    return grassBitmap(env,clazz,bitmap,shape);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_cutoutBitmap(JNIEnv *env, jclass clazz, jobject bitmap,
                                                    jint image_width, jint image_height,
                                                    jint start_position_x, jint start_position_y) {
    Shape shape;
    cv::Point rectStartPosition(start_position_x,start_position_y);
    shape.shapeType = 1;
    shape.rectStartPosition = rectStartPosition;
    shape.rectHeight = image_height;
    shape.rectWidth = image_width;
    return cutoutBitmap(env,clazz,bitmap,shape);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_shapeChangeCutout(JNIEnv *env, jclass clazz, jobject bitmap,
                                                         jobjectArray point) {
    return transformImage(env,clazz,bitmap,point);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_autoColorBalance(JNIEnv *env, jclass clazz, jobject bitmap) {
    return autoColorBalance(env,clazz,bitmap);
}



extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_autoThinFace(JNIEnv *env, jclass clazz, jobject bitmap,jint strength) {
    return thinFace(env,clazz,bitmap,strength);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_HighAndDarkLightChange(JNIEnv *env, jclass clazz,
                                                              jobject bitmap,jobject mask, jfloat dark_light,
                                                              jfloat high_light) {
    return changeHighLigAndDarkLight(env,clazz,bitmap,mask,dark_light,high_light);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_twoBitmapToOne(JNIEnv *env, jclass clazz,
                                                      jobject source_bitmap, jobject add_bitmap,
                                                      jfloat add_weight, jfloat start_x,
                                                      jfloat start_y) {
    if (add_weight > 1){
        add_weight = 1;
    } else if (add_weight < 0){
        add_weight = 0;
    }
    cv::Point startPosition(start_x,start_y);
    return twoBitmapToOne(env,clazz,source_bitmap,add_bitmap,add_weight,startPosition);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_twoBitmapToOneUseMask(JNIEnv *env, jclass clazz,
                                                      jobject source_bitmap,jobject add_bitmap,jobject mask,
                                                      jfloat add_weight, jfloat start_x,
                                                      jfloat start_y) {
    if (add_weight > 1){
        add_weight = 1;
    } else if (add_weight < 0){
        add_weight = 0;
    }
    cv::Point startPosition(start_x,start_y);
    return twoBitmapToOne(env,clazz,source_bitmap,add_bitmap,mask,add_weight,startPosition);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeBy3dLut(JNIEnv *env, jclass clazz, jobject bitmap,jstring lutPath,jfloat strength) {
    try {
        char *a = const_cast<char *>(env->GetStringUTFChars(lutPath, JNI_FALSE));
        jobject result= changeByLut(env,clazz,bitmap,a,strength);
        //env->ReleaseStringUTFChars(lutPath, a);
        return result;
    }catch (Exception e){
        return bitmap;
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_cubeToPng(JNIEnv *env, jclass clazz, jstring lut_file_name,jobject bitmap) {
    char *a = const_cast<char *>(env->GetStringUTFChars(lut_file_name, JNI_FALSE));
    return cubeToPng(env,clazz,a,bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_readImageByPath(JNIEnv *env, jclass clazz,jobject bitmap,jstring image_path,jint maxWidth) {
    char *a = const_cast<char *>(env->GetStringUTFChars(image_path, JNI_FALSE));
    jobject result = readImage(env,clazz,a,bitmap,maxWidth);
    env->ReleaseStringUTFChars(image_path, a);
    return result;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_autoContrast(JNIEnv *env, jclass clazz, jobject bitmap) {
    return autoContrast(env,clazz,bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeTemp(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                  jint temp_value) {
    return ChangeTemp(env,clazz,bitmap,mask,temp_value);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeColorHue(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                      jint temp_value) {
    return ChangeColorHue(env,clazz,bitmap,mask,temp_value);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_denoise(JNIEnv *env, jclass clazz, jobject bitmap,
                                               jfloat strength) {
    return denoise(env,clazz,bitmap,strength);
}

//自然饱和度：https://zhuanlan.zhihu.com/p/127294285
extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_vibranceAlgorithm(JNIEnv *env, jclass clazz, jobject bitmap,jint stregth) {
    return VibranceAlgorithm(env,clazz,bitmap,stregth);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeLight(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
                                                   jfloat lisght_strength) {
    HueChange hsb;
    hsb.rgbS = lisght_strength;
    hsb.rgbH = lisght_strength;
    hsb.rgbB = lisght_strength;
    return ChangeHsbB(env,clazz,bitmap,mask,hsb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeSaturation(JNIEnv *env, jclass clazz, jobject bitmap,
                                                        jfloat saturation_strength) {
    HueChange hsb;
    hsb.rgbS = saturation_strength;
    hsb.rgbH = saturation_strength;
    hsb.rgbB = saturation_strength;
    return ChangeHsbS(env,clazz,bitmap,hsb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeHue(JNIEnv *env, jclass clazz, jobject bitmap,jobject mask,
                                                        jfloat saturation_strength) {
    HueChange hsb;
    hsb.rgbS = saturation_strength;
    hsb.rgbH = saturation_strength;
    hsb.rgbB = saturation_strength;
    return ChangeHsbH(env,clazz,bitmap,mask,hsb);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_image_library_opencv_OpenCvIn_faceMarkInit(JNIEnv *env, jclass clazz,jstring faceMark1000ParamPathIn,jstring faceMark1000BinPathIn,jstring faceBoxParamPathIn,jstring faceBoxBinPathIn) {
    if (FaceLandmark::detector != nullptr) {
//        delete FaceLandmark::detector;
        FaceLandmark::detector = nullptr;
    }
    if (FaceLandmark::detector == nullptr) {
        FaceLandmark::detector = new FaceLandmark(jstring2str(env,faceMark1000ParamPathIn),jstring2str(env,faceMark1000BinPathIn),jstring2str(env,faceBoxParamPathIn),jstring2str(env,faceBoxBinPathIn));
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeSaturationTwo(JNIEnv *env, jclass clazz,
                                                           jobject bitmap,jobject mask,
                                                           jfloat saturation_strength) {
    return changeSaturationTwo(env,clazz,bitmap,mask,saturation_strength);
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_image_library_opencv_OpenCvIn_getFaceList(JNIEnv *env, jclass clazz, jobject bitmap) {

}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getSigleColorPhoto(JNIEnv *env, jclass clazz, jobject bitmap,
                                                          jint r, jint g, jint b) {
    return getSigleColorPhoto(env,clazz,bitmap,r,g,b);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_startChangeHsbS(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                       jfloat red_adjust, jfloat green_adjust,
                                                       jfloat blue_adjust, jfloat cyan_adjust,
                                                       jfloat yellow_adjust,
                                                       jfloat carmine_adjust) {
    return startChangeS(env,clazz,bitmap,mask,red_adjust,green_adjust,blue_adjust,cyan_adjust,yellow_adjust,carmine_adjust);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_changeLightByChannel(JNIEnv *env, jclass clazz,
                                                            jobject bitmap,jobject mask, jfloat red_value,
                                                            jfloat yellow_value, jfloat green_value,
                                                            jfloat cyan_value, jfloat blue_value,
                                                            jfloat carmine_value) {
    HueChange hsb;
    hsb.rB = red_value;
    hsb.yB = yellow_value;
    hsb.gB = green_value;
    hsb.bB = blue_value;
    hsb.cB = cyan_value;
    hsb.mB = carmine_value;
    return ChangeHsbB(env,clazz,bitmap,mask,hsb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_startChangeHsbH(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                                       jfloat red_adjust, jfloat green_adjust,
                                                       jfloat blue_adjust, jfloat cyan_adjust,
                                                       jfloat yellow_adjust,
                                                       jfloat carmine_adjust) {
    return startChangeH(env,clazz,bitmap,mask,red_adjust,green_adjust,blue_adjust,cyan_adjust,yellow_adjust,carmine_adjust);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_SoftSkin(JNIEnv *env, jclass clazz, jobject skin,
                                                jobject lut_map,jobject mask, jint strength) {
    return SoftSkin(env,clazz,skin,lut_map,mask,strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_applyLutByPng(JNIEnv *env, jclass clazz,
                                                     jobject input_bitmap, jobject filter_bitmap) {
    return changeLutPyPng(env,clazz,input_bitmap,filter_bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_lipColorChange(JNIEnv *env, jclass clazz, jobject bitmap, jobject filterBitmap,
                                                      jint strength) {
    return changeLipsColor(env,clazz,bitmap,filterBitmap,strength);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_noseStrength(JNIEnv *env, jclass clazz, jobject bitmap,
                                                    jobject msk_bitmap, jint strength) {
    return StrengthNose(env,clazz,bitmap,msk_bitmap,strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_skinColor(JNIEnv *env, jclass clazz, jobject bitmap,
                                                 jobject skin_lut,jobject skin_mask, jint strength) {
    return changeSkinColor(env,clazz,bitmap,skin_lut,skin_mask,strength);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_faceBlush(JNIEnv *env, jclass clazz, jobject bitmap,
                                                 jobject left_model, jobject right_model,
                                                 jint strength) {
    return addFaceBlush(env,clazz,bitmap,left_model,right_model,strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_beautyIris(JNIEnv *env, jclass clazz, jobject bitmap,
                                                  jobject mask_bitmap, jint strength) {
    return beautyIris(env,clazz,bitmap,mask_bitmap,strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_beautyEyeBrow(JNIEnv *env, jclass clazz, jobject bitmap,
                                                     jobject eye_brow_left,
                                                     jobject eye_bitmap_right, jint strength) {
    return eyeBrow(env,clazz,bitmap,eye_brow_left,eye_bitmap_right,strength);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_beautyEyeLine(JNIEnv *env, jclass clazz, jobject bitmap,
                                                     jobject eye_brow_left,
                                                     jobject eye_bitmap_right ,jint eyeLineUpStrength,jint eyeLineDownStrength) {
    return eyeLine(env,clazz,bitmap,eye_brow_left,eye_bitmap_right,eyeLineUpStrength,eyeLineDownStrength);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_superres(JNIEnv *env, jclass clazz, jobject bitmap,
                                                jint size,jstring modelPath,jstring modelName) {
    return startSuperres(env,clazz,bitmap,size,modelPath,modelName);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_HSBMask(JNIEnv *env, jclass clazz, jobject bitmap, jobject mask,
                                               jint target_r, jint target_g, jint target_b,
                                               jfloat allow_extent_h, jfloat allow_extent_s,
                                               jfloat allow_extent_b, jboolean is_show_model,jboolean is_add_model) {
    return makeHsbMask(env,clazz,bitmap,mask,target_r,target_g,target_b,allow_extent_h,allow_extent_s,allow_extent_b,is_show_model,is_add_model);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_ThresholdMask(JNIEnv *env, jclass clazz, jobject bitmap,
                                                     jobject mask, jint threshold,jboolean isShowModel,jboolean isAddModel) {
    return makeThresholdMask(env,clazz,bitmap,mask,threshold,isShowModel,isAddModel);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_image_library_opencv_OpenCvIn_getApplicationClientId(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("105627857");
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_image_library_opencv_OpenCvIn_getApplicationClientSecret(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("6f5b306b61b441de9cc268382e4e6eedd6920eb11670f3bdb5de74c499836058");
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_normalMix(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                  jobject mask_bitmap, jobject cutout_bitmap, jint left, jint right,jfloat add_weight) {
    return normal(env,clazz,source_bitmap,mask_bitmap,cutout_bitmap,left,right,add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_darkCorner(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                  jobject mask_bitmap, jboolean is_black,jfloat strength) {
    return darkCorner(env,clazz,source_bitmap,mask_bitmap,is_black,strength);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_test(JNIEnv *env, jclass clazz, jobject bitmap,
                                                jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath) {
    return test(env,bitmap,paramPathForBox,binPathForBox,paramPath,binPath);
}


extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_image_library_opencv_OpenCvIn_getFacePoint(JNIEnv *env, jclass clazz,jobject bitmap,
                                                    jstring paramPathForBox,jstring binPathForBox,jstring paramPath,jstring binPath) {
    return getFacePoint(env,bitmap,paramPathForBox,binPathForBox,paramPath,binPath);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageToMask(JNIEnv *env, jclass clazz, jobject bitmap) {
    return imageToMask(env,bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_imageToMaskPro(JNIEnv *env, jclass clazz, jobject bitmap,jfloat pro) {
    return imageToMaskPro(env,bitmap,pro);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_fixImage(JNIEnv *env, jclass clazz, jobject bitmap,
                                                jobject mask) {
    return fixImage(env,clazz,bitmap,mask);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toSoftLightMix(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                   jobject add_bitmap, jobject mask,jint left, jint top,
                                                   jfloat add_weight) {
    return toSoftLight(env,clazz,add_bitmap,source_bitmap,mask,left,top,add_weight);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toHardLightMix(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                   jobject add_bitmap, jobject mask, jint left,
                                                   jint top, jfloat add_weight) {
    return toHardLight(env,clazz,source_bitmap,mask,add_bitmap,left,top,add_weight);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toOverlayMix(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                 jobject add_bitmap, jobject mask, jint left,
                                                 jint top, jfloat add_weight) {
    return toOverlay(env,clazz,source_bitmap,mask,add_bitmap,left,top,add_weight);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toVividLight(JNIEnv *env, jclass clazz,
                                                    jobject source_bitmap, jobject add_bitmap,
                                                    jobject mask, jint left, jint top,
                                                    jfloat add_weight) {
    return toVividLight(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toLinearLight(JNIEnv *env, jclass clazz,
                                                     jobject source_bitmap, jobject add_bitmap,
                                                     jobject mask, jint left, jint top,
                                                     jfloat add_weight) {
    return toLinearLight(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toPointLight(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                   jobject add_bitmap, jobject mask, jint left,
                                                   jint top, jfloat add_weight) {
    return toPinLight(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);

}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toHardMix(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                 jobject add_bitmap, jobject mask, jint left,
                                                 jint top, jfloat add_weight) {
    return toHardMix(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toDifference(JNIEnv *env, jclass clazz,
                                                    jobject source_bitmap, jobject add_bitmap,
                                                    jobject mask, jint left, jint top,
                                                    jfloat add_weight) {
    return toDifference(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toExclusion(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                   jobject add_bitmap, jobject mask, jint left,
                                                   jint top, jfloat add_weight) {
    return toExclusion(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toSubtract(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                  jobject add_bitmap, jobject mask, jint left,
                                                  jint top, jfloat add_weight) {
    return toSubtract(env,clazz, add_bitmap, source_bitmap, mask,left,top,add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toDivide(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                jobject add_bitmap, jobject mask, jint left,
                                                jint top, jfloat add_weight) {
    return toDivide(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toHue(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                             jobject add_bitmap, jobject mask, jint left, jint top,
                                             jfloat add_weight) {
    return toHue(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toSaturation(JNIEnv *env, jclass clazz,
                                                    jobject source_bitmap, jobject add_bitmap,
                                                    jobject mask, jint left, jint top,
                                                    jfloat add_weight) {
    return toSaturation(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toLuminosity(JNIEnv *env, jclass clazz,
                                                    jobject source_bitmap, jobject add_bitmap,
                                                    jobject mask, jint left, jint top,
                                                    jfloat add_weight) {
    return toLuminosity(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toColor(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                               jobject add_bitmap, jobject mask, jint left,
                                               jint top, jfloat add_weight) {
    return toColor(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);

}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toFilterColor(JNIEnv *env, jclass clazz,
                                                     jobject source_bitmap, jobject add_bitmap,
                                                     jobject mask, jint left, jint top,
                                                     jfloat add_weight) {
    return toFilterColor(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toColorDark(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                   jobject add_bitmap, jobject mask, jint left,
                                                   jint top, jfloat add_weight) {
    return toColorDark(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toLineColorLight(JNIEnv *env, jclass clazz,
                                                        jobject source_bitmap, jobject add_bitmap,
                                                        jobject mask, jint left, jint top,
                                                        jfloat add_weight) {
    return toLineColorLight(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toLineColorDark(JNIEnv *env, jclass clazz,
                                                       jobject source_bitmap, jobject add_bitmap,
                                                       jobject mask, jint left, jint top,
                                                       jfloat add_weight) {
    return toLineColorDark(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toColorLight(JNIEnv *env, jclass clazz,
                                                    jobject source_bitmap, jobject add_bitmap,
                                                    jobject mask, jint left, jint top,
                                                    jfloat add_weight) {
    return toColorLight(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toMultiply(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                                  jobject add_bitmap, jobject mask, jint left,
                                                  jint top, jfloat add_weight) {
    return toMultiply(env, clazz,  add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toLight(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                               jobject add_bitmap, jobject mask, jint left,
                                               jint top, jfloat add_weight) {
    return toLight(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_toDark(JNIEnv *env, jclass clazz, jobject source_bitmap,
                                              jobject add_bitmap, jobject mask, jint left, jint top,
                                              jfloat add_weight) {
    return toDark(env, clazz, add_bitmap, source_bitmap, mask, left, top, add_weight);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getRotationBitmapResize(JNIEnv *env, jclass clazz,
                                                               jobject bitmap, jint angle) {
    return getRotationBitmapResize(env,clazz,bitmap,angle);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_maskToAlphaMask(JNIEnv *env, jclass clazz, jobject mask) {
    return maskToAlphaMask(env,clazz,mask);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_rgbBitmapToMask(JNIEnv *env, jclass clazz, jobject bitmap,jint channelType,jint needToOverValue,jboolean isUseDefaultIfNotOver) {
    return getRGBMask(env,bitmap,channelType,needToOverValue,isUseDefaultIfNotOver);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_removeBorder(JNIEnv *env, jclass clazz, jobject bitmap,
                                                    jint border_count, jint start_h,jint end_h) {
    return removeBorder(env,clazz,bitmap,border_count,start_h,end_h);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_image_library_opencv_OpenCvIn_clearFaceKeyPoint(JNIEnv *env, jclass clazz) {
    if (FaceLandmark::detector != nullptr) {
        FaceLandmark::detector->ClearFaceKeyPoint();
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getMaskRect(JNIEnv *env, jclass clazz, jobject mask) {
    jclass myClass = env->FindClass("android/graphics/Rect");
    // 获取类的构造函数，记住这里是调用无参的构造函数
    jmethodID id = env->GetMethodID(myClass, "<init>", "()V");
    // 创建一个新的对象
    jobject rectResult = env->NewObject(myClass, id);
    jfieldID left = env->GetFieldID(myClass, "left", "I");
    jfieldID right = env->GetFieldID(myClass, "right", "I");
    jfieldID top = env->GetFieldID(myClass, "top", "I");
    jfieldID bottom = env->GetFieldID(myClass, "bottom", "I");
    vector<int> rect = getMaskRect(env,mask);
    env->SetIntField(rectResult, left, rect[0]);
    env->SetIntField(rectResult, right, rect[1]);
    env->SetIntField(rectResult, top, rect[2]);
    env->SetIntField(rectResult, bottom, rect[3]);
    return  rectResult;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_hsl(
        JNIEnv *env, jclass clazz,
        jobject bitmap,jobject mask,
        jfloat r_s,jfloat r_h,jfloat r_b
        ,jfloat g_s,jfloat g_h,jfloat g_b
        ,jfloat b_s,jfloat b_h,jfloat b_b
        ,jfloat c_s,jfloat c_h,jfloat c_b
        ,jfloat m_s,jfloat m_h,jfloat m_b
        ,jfloat y_s,jfloat y_h,jfloat y_b
        ,jfloat o_s,jfloat o_h,jfloat o_b
        ,jfloat p_s,jfloat p_h,jfloat p_b
) {
    HueChange hsb;
    hsb.rS = r_s;
    hsb.rH = r_h;
    hsb.rB = r_b;

    hsb.gS = g_s;
    hsb.gH = g_h;
    hsb.gB = g_b;

    hsb.bS = b_s;
    hsb.bH = b_h;
    hsb.bB = b_b;

    hsb.cS = c_s;
    hsb.cH = c_h;
    hsb.cB = c_b;

    hsb.mS = m_s;
    hsb.mH = m_h;
    hsb.mB = m_b;

    hsb.yS = y_s;
    hsb.yH = y_h;
    hsb.yB = y_b;

    hsb.oS = o_s;
    hsb.oH = o_h;
    hsb.oB = o_b;

    hsb.pS = p_s;
    hsb.pH = p_h;
    hsb.pB = p_b;
    return testHsb(env,clazz,bitmap,mask,hsb);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_replaceHAndS(JNIEnv *env, jclass clazz, jint target_h,
                                                    jint target_s,jint target_b, jobject bitmap, jobject mask,
                                                    jfloat strength,jfloat pro_s,jfloat pro_b) {
    return ReplaceHAndS(target_h,target_s,target_b,env,clazz,bitmap,mask,strength,pro_s,pro_b);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_removeHairSkinArea(JNIEnv *env, jclass clazz,jobject bitmap,jobject faceMask,jobject skinMask) {
    return RemoveHairMaskSkinColorAre(env,clazz,bitmap,faceMask,skinMask);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_image_library_opencv_OpenCvIn_getAverageLight(JNIEnv *env, jclass clazz, jobject bitmap,
                                                       jobject mask) {
    return GetHsbAverageValue(env,clazz,bitmap,mask);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getSkinMask(JNIEnv *env, jclass clazz, jobject bitmap) {
    return GetSkinMask(env,clazz,bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_maskAdd(JNIEnv *env, jclass clazz, jobject mask_one,
                                               jobject mask_two) {
    return getMaskAdd(env,mask_one,mask_two);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_maskCut(JNIEnv *env, jclass clazz, jobject source_bitamp,
                                               jobject cut_bitmap) {
    return getMaskCut(env,source_bitamp,cut_bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_getPupilMaskByEyeMask(JNIEnv *env, jclass clazz,
                                                             jobject source_bitmap,
                                                             jobject eye_mask) {
    return getPupilMaskByEyeMask(env,source_bitmap,eye_mask);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_keepIntersectMask(JNIEnv *env, jclass clazz,
                                                         jobject source_bitamp,
                                                         jobject cut_bitmap) {
    return KeepIntersectMask(env,source_bitamp,cut_bitmap);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_image_library_opencv_OpenCvIn_makeBitmapParticles(JNIEnv *env, jclass clazz,jobject bitmap,jobject mask, jint particles_size) {
    return imageAddParticles(env,clazz,bitmap,mask,particles_size);
}