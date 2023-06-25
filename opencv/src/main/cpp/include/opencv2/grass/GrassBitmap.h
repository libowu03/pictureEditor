//
// Created by libowu on 2021/10/28.
//

#ifndef CHANGECOLORMASTER_GRASSBITMAP_H
#define CHANGECOLORMASTER_GRASSBITMAP_H
class Shape
{
public:
    //类型
    int shapeType = 0;
    //圆的半径
    int circleRadio = 0;
    //圆心
    cv::Point circleCenter;
    //模糊程度
    int grassWeight = 1;
    //羽化半径
    int eclosionValue = 20;
    //矩形的宽
    int rectWidth = 0;
    //矩形的高
    int rectHeight = 0;
    //矩形的起始位置
    cv::Point rectStartPosition;
};
extern jobject grassBitmap(JNIEnv *env, jclass clazz, jobject bitmap,Shape data);
extern jobject cutoutBitmap(JNIEnv *env, jclass clazz, jobject bitmap,Shape shapeData);
extern jobject twoBitmapToOne(JNIEnv *env, jclass clazz, jobject bitmap,jobject addBitmap, double addImgWeight, cv::Point point);
extern jobject twoBitmapToOne(JNIEnv *env, jclass clazz, jobject bitmap,jobject addBitmap,jobject mask,double addImgWeight, cv::Point point);
#endif //CHANGECOLORMASTER_GRASSBITMAP_H
