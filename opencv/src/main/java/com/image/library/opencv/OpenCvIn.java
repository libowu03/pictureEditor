package com.image.library.opencv;

import static com.image.library.opencv.constants.Constants.MixType.MIX_COLOR;
import static com.image.library.opencv.constants.Constants.MixType.MIX_COLOR_CUT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_COLOR_DEEPER;
import static com.image.library.opencv.constants.Constants.MixType.MIX_DARK;
import static com.image.library.opencv.constants.Constants.MixType.MIX_DIFFERENCE_VALUE;
import static com.image.library.opencv.constants.Constants.MixType.MIX_DIVIDED_BY;
import static com.image.library.opencv.constants.Constants.MixType.MIX_EXCLUDE;
import static com.image.library.opencv.constants.Constants.MixType.MIX_FILTER_COLOR;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HARD_COLOR;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HARD_LIGHT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HIGHT_LIGHT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HSB_B;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HSB_H;
import static com.image.library.opencv.constants.Constants.MixType.MIX_HSB_S;
import static com.image.library.opencv.constants.Constants.MixType.MIX_LIGHT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_LIGHT_OVAL;
import static com.image.library.opencv.constants.Constants.MixType.MIX_LINE_CUT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_LINE_DEEPER;
import static com.image.library.opencv.constants.Constants.MixType.MIX_LINE_LIGHT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_MINUS;
import static com.image.library.opencv.constants.Constants.MixType.MIX_NORMAL;
import static com.image.library.opencv.constants.Constants.MixType.MIX_OVAL;
import static com.image.library.opencv.constants.Constants.MixType.MIX_POINT_LIGHT;
import static com.image.library.opencv.constants.Constants.MixType.MIX_SOFT_LIGHT;

import android.graphics.Bitmap;
import android.graphics.Point;
import android.graphics.Rect;

import com.image.library.opencv.bean.FaceBean;
import com.image.library.opencv.bean.ImageChannelInfo;
import com.image.library.opencv.bean.MixColorChannel;


/**
 * opencv方法调用入口
 */
public class OpenCvIn {

    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 证件照背景替换
     */
    public static native Bitmap setReplaceBackground(Bitmap bitmap);

    /**
     * 抠图
     *
     * @param bitmap
     * @param point    抠图区域的点
     * @return
     */
    public static native Bitmap imageChooseArea(Bitmap bitmap, int[][] point);

    /**
     * 圆形裁剪
     *
     * @param bitmap
     * @param centerPoint
     * @param radius
     * @return
     */
    public static native Bitmap imageCircleArea(Bitmap bitmap, Point centerPoint, int radius);

    /**
     * 亮度和对比度调整
     *
     * @param bitmap
     * @param light
     * @param contrast
     * @return
     */
    public static native Bitmap imageLightAndContrastChange(Bitmap bitmap, double light, double contrast);

    /**
     * 调整亮度和对比度
     *
     * @param bitmap
     * @param briness  亮度，范围是-255到255
     * @param contrast 对比度。范围是-255到255
     * @return
     */
    public static native Bitmap brinessAndContrastChange(Bitmap bitmap,Bitmap mask, int briness, int contrast);

    /**
     * 图片锐化
     *
     * @param bitmap
     * @param size
     * @return
     */
    public static native Bitmap imageSharpening(Bitmap bitmap, double size);

    /**
     * 图像大小重置
     *
     * @param bitmap       原图
     * @param resizeXValue x轴缩放比例
     * @param resizeYValue y轴缩放比例
     */
    public static native Bitmap imageResize(Bitmap bitmap, double resizeXValue, double resizeYValue);

    /**
     * @param bitmap
     * @param useRedChannel   是否保留红色通道
     * @param useGreenChannel 是否保留绿色通道
     * @param useBlueChannel  是否保留蓝色通道
     * @return
     */
    public static native Bitmap imageChooseColorChannel(Bitmap bitmap, boolean useRedChannel, boolean useGreenChannel, boolean useBlueChannel);


    /**
     * 像素化图片
     *
     * @param bitmap     需要处理的原始图片
     * @param pixelsSize 合并像素点的半径。比如值为1，表示将会以(1,1)为中心向四周合并 （ ((pixelsSize×2+1)×(pixelsSize×2+1))-1 ）个像素点
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapToPixel(Bitmap bitmap,Bitmap mask, int pixelsSize);

    /**
     * 图片颜色取反
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapColorCounter(Bitmap bitmap);

    /**
     * 获取灰度图片
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapGray(Bitmap bitmap);

    /**
     * 图片暗通道去雾
     *
     * @param bitmap   需要处理的原始图片
     * @param value 去雾程度
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapRemoveFog(Bitmap bitmap,Bitmap mask,float value);

    /**
     * 获取高斯模糊图片
     *
     * @param bitmap   需要处理的原始图片
     * @param blurSize 高斯程度
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapBlur(Bitmap bitmap,Bitmap mask, int blurSize);

    /**
     * 图片添加颗粒度
     * @param bitmap 原图
     * @param particlesSize 颗粒度
     * @param mask 蒙版
     * @return
     */
    public static native Bitmap makeBitmapParticles(Bitmap bitmap,Bitmap mask,int particlesSize);

    /**
     * 获取黑白图像
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapWhiteAndBlack(Bitmap bitmap);

    /**
     * 怀旧滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapNostalgia(Bitmap bitmap);

    /**
     * 熔铸滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapFounding(Bitmap bitmap);

    /**
     * 冰冻滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapFrozen(Bitmap bitmap);

    /**
     * 连环画滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapComics(Bitmap bitmap);

    /**
     * 图像侵蚀滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapErode(Bitmap bitmap);

    /**
     * 描边滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapEdge(Bitmap bitmap);

    /**
     * 膨胀滤镜
     *
     * @param bitmap   需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapDilate(Bitmap bitmap);

    /**
     * 通过3d lut方案改变图片风格
     *
     * @param bitmap 需要处理的原始图片
     * @return 处理好的图片
     */
    public static native Bitmap makeBitmapChangeByLut(Bitmap bitmap, String cubePath);

    /**
     * 获取图片通道直方图
     *
     * @param bitmap 目标图片
     * @return 图片直方图信息
     */
    public static native ImageChannelInfo getImageChannelInfo(Bitmap bitmap);


    /**
     * 颜色选择工具
     * @param bitmap 目标图片
     * @param redRed 红色目标红色通道 [-1.0f,1.0f]
     * @param redGreen 红色目标绿色通道 [-1.0f,1.0f]
     * @param redBlue 红色目标蓝色通道 [-1.0f,1.0f]
     * @param greenRed 红色目标红色通道 [-1.0f,1.0f]
     * @param greenGreen 红色目标红色通道 [-1.0f,1.0f]
     * @param greenBlue 红色目标红色通道 [-1.0f,1.0f]
     * @param blueRed 红色目标红色通道 [-1.0f,1.0f]
     * @param blueGreen 红色目标红色通道 [-1.0f,1.0f]
     * @param blueBlue 红色目标红色通道 [-1.0f,1.0f]
     * @param cyanRed 红色目标红色通道 [-1.0f,1.0f]
     * @param cyanGreen 红色目标红色通道 [-1.0f,1.0f]
     * @param cyanBlue 红色目标红色通道 [-1.0f,1.0f]
     * @param yellowRed 红色目标红色通道 [-1.0f,1.0f]
     * @param yellowGreen 红色目标红色通道 [-1.0f,1.0f]
     * @param yellowBlue 红色目标红色通道 [-1.0f,1.0f]
     * @param carmineRed 红色目标红色通道 [-1.0f,1.0f]
     * @param carmineGreen 红色目标红色通道 [-1.0f,1.0f]
     * @param carmineBlue 红色目标红色通道 [-1.0f,1.0f]
     * @param isRelative 绝对还是相对
     * @param mask 蒙版
     * @return
     */
    public static native Bitmap changeColorByChannelPro(Bitmap bitmap,Bitmap mask,
                                                        float redRed, float redGreen, float redBlue,float redBlack,
                                                        float greenRed, float greenGreen, float greenBlue,float greenBlack,
                                                        float blueRed, float blueGreen, float blueBlue,float blueBlack,
                                                        float cyanRed, float cyanGreen, float cyanBlue,float cyanBlack,
                                                        float yellowRed, float yellowGreen, float yellowBlue,float yellowBlack,
                                                        float carmineRed, float carmineGreen, float carmineBlue,float carmineBlack,
                                                        float whiteRed, float whiteGreen, float whiteBlue,float whiteBlack,
                                                        float blackRed, float blackGreen, float blackBlue,float blackBlack,
                                                        boolean isRelative);

    /**
     * 通道混合器
     *
     * @param bitmap
     * @return
     */
    private static native Bitmap mixColorChannel(Bitmap bitmap,Bitmap mask, float redChannelRed,
                                                 float redChannelGreen, float redChannelBlue, float greenChannelRed,
                                                 float greenChannelGreen, float greenChannelBlue, float blueChannelRed,
                                                 float blueChannelGreen, float blueChannelBlue);

    /**
     * 通道混合器
     *
     * @param bitmap
     * @param mixColorChannel
     * @return
     */
    public static Bitmap mixColorChannel(Bitmap bitmap,Bitmap mask, MixColorChannel mixColorChannel) {
        return mixColorChannel(bitmap,mask,
                mixColorChannel.getRedChannelRed(),
                mixColorChannel.getRedChannelGreen(),
                mixColorChannel.getRedChannelBlue(),
                mixColorChannel.getGreenChannelRed(),
                mixColorChannel.getGreenChannelGreen(),
                mixColorChannel.getGreenChannelBlue(),
                mixColorChannel.getBlueChannelRed(),
                mixColorChannel.getBlueChannelGreen(),
                mixColorChannel.getBlueChannelBlue());
    }

    /**
     * 三次样条插值，通过x坐标获取y坐标的值
     * 无论怎样，其实控制点和结束控制点必须添加
     *
     * @param x
     * @param xController 控制点的x坐标
     * @param yController 控制点的y坐标
     * @return
     */
    public static native double cubicSplineGetY(double x, double xController[], double yController[]);

    /**
     * 曲线选择器
     *
     * @return
     */
    public static native Bitmap curveTools(Bitmap bitmap,Bitmap mask, int channelType, double xController[], double yController[]);

    /**
     * 曲线选择器
     *
     * @param bitmap           输入图片
     * @param xRedController   红色通道曲线X坐标
     * @param yRedController   红色通道曲线Y坐标
     * @param xGreenController 绿色通道曲线X坐标
     * @param yGreenController 绿色通道曲线Y坐标
     * @param xBlueController  蓝色通道曲线X坐标
     * @param yBlueController  蓝色通道曲线Y坐标
     * @param xRgbController   rgb三通道曲线X坐标
     * @param yRgbController   rgb三通道曲线Y坐标
     * @return
     */
    public static native Bitmap curveToolsPro(Bitmap bitmap,Bitmap mask, double xRedController[], double yRedController[], double xGreenController[], double yGreenController[], double xBlueController[], double yBlueController[], double xRgbController[], double yRgbController[]);

    /**
     * 液化
     *
     * @param bitmap      输入图片
     * @param centerPoint 点击中心点
     * @param movePoint   液化终点
     * @param radio       液化半径
     * @param strength    液化强度
     * @return
     */
    public static native Bitmap liquefying(Bitmap bitmap, Point centerPoint, Point movePoint, int radio, int strength);

    /**
     * usm锐化
     *
     * @param bitmap    需要锐化的图片
     * @param radios    锐化半径
     * @param amuont    锐化数量
     * @param threshold 锐化阈值
     * @return 锐化后的图片
     */
    public static native Bitmap usmFilter(Bitmap bitmap,Bitmap mask, int radios, int amuont, int threshold);

    /**
     * 修改图形饱和度
     *
     * @param bitmap    需要修改饱和度的图片
     * @param hsvHValue 饱和度值
     * @return 修改后的图片
     */
    public static native Bitmap toChangeHSV(Bitmap bitmap, int hsvHValue);

    /**
     * @param bitmap       修改目标图像
     * @param hue          色相
     * @param saturation   饱和度
     * @param brightness   明度
     * @param colorChannel 颜色通道 0:RGB 1:红 2：黄 3：绿 4：青 5：蓝 6：洋红
     * @return 修改好的图像
     */
    public static native Bitmap toChangeHsl(Bitmap bitmap, int hue, int saturation, int brightness, int colorChannel);

    /**
     *
     * @param bitmap
     * @param  x 像素点的x坐标位置
     * @param  y 像素点的y坐标位置
     * @return 包含像素点颜色信息的数组，0分别为red，green，blue，alpha
     */
    public static native int[] getPixelColor(Bitmap bitmap,int x,int y);

    /**
     * 旋转图片
     * @param bitmap 目标图片
     * @param angle 旋转角度
     * @return 输出图片
     */
    public static native Bitmap getRotationBitmap(Bitmap bitmap,int angle);

    /**
     * 旋转图片
     * @param bitmap 目标图片
     * @param angle 旋转角度
     * @return 输出图片
     */
    public static native Bitmap getRotationBitmapResize(Bitmap bitmap,int angle);

    /**
     * 水平翻转图片或垂直翻转图片
     * @param bitmap 目标图片
     * @param isY 是否以Y轴进行翻转
     * @return 处理完成图片
     */
    public static native Bitmap getRotationXY(Bitmap bitmap,boolean isY);

    /**
     * 改变色相，饱和度，明度，算法与photoshop基本一致
     * @param bitmap
     * @param rgbH
     * @param rgbS
     * @param rgbB
     * @param rH
     * @param rS
     * @param rB
     * @param gH
     * @param gS
     * @param gB
     * @param bH
     * @param bS
     * @param bB
     * @param cH
     * @param cS
     * @param cB
     * @param mH
     * @param mS
     * @param mB
     * @param yH
     * @param yS
     * @param yB
     * @return
     */
    public static native Bitmap changeHSB(Bitmap bitmap,Bitmap mask,
                                          float rgbH,float rgbS,float rgbB,
                                          float rH,float rS,float rB,
                                          float gH,float gS,float gB,
                                          float bH,float bS,float bB,
                                          float cH,float cS,float cB,
                                          float mH,float mS,float mB,
                                          float yH,float yS,float yB,
                                          int leftBorder,int rightBorder,int left,int right);

    /**
     *
     * @param bitmap
     * @param cyanAndRedLow 阴影 “青色-红色”
     * @param carmineAndGreenLow 阴影 “洋红-绿色”
     * @param yellowAndBlueLow 阴影 “黄色-蓝色”
     * @param cyanAndRedMiddle 中间调 “青色-红色”
     * @param carmineAndGreenMiddle 中间调 “洋红-绿色”
     * @param yellowAndBlueMiddle 中间调 “黄色-蓝色”
     * @param cyanAndRedHight 高光 “青色-红色”
     * @param carmineAndGreenHight 高光 “洋红-绿色”
     * @param yellowAndBlueHight 高光 “黄色-蓝色”
     * @return
     */
    public static native Bitmap changeColorBalance(Bitmap bitmap,Bitmap mask,
                                                   int cyanAndRedLow, int carmineAndGreenLow, int yellowAndBlueLow,
                                                   int cyanAndRedMiddle, int carmineAndGreenMiddle, int yellowAndBlueMiddle,
                                                   int cyanAndRedHight, int carmineAndGreenHight, int yellowAndBlueHight);

    /**
     * 自动白平衡
     * @param bitmap
     * @return
     */
    public static native Bitmap autoColorBalance(Bitmap bitmap);

    /**
     * 圆形模糊
     * @param centerX 圆心x
     * @param centerY 圆心y
     * @param radius 圆的半径
     * @param liquidation 边缘液化半径
     * @return
     */
    public static native Bitmap grassCircle(Bitmap bitmap,int centerX,int centerY,int radius,int liquidation);

    /**
     * 图片裁剪
     * @param  bitmap 需要裁剪的图片
     * @param  imageHeight 需要生成图片的宽度
     * @param  imageWidth 需要生成裁剪图片的宽度
     * @param  startPositionX 裁剪起始x坐标
     * @param  startPositionY 裁剪结束y坐标
     * @return 裁剪完成的图片
     */
    public static native Bitmap cutoutBitmap(Bitmap bitmap,int imageWidth,int imageHeight,int startPositionX,int startPositionY);

    /**
     * 透视裁剪
     * @return
     */
    public static native Bitmap shapeChangeCutout(Bitmap bitmap, int[][] point);

    /**
     * 自动瘦脸
     * @param bitmap 输入图片
     * @param strength 瘦脸强度
     * @return 处理好的图片
     */
    public static native Bitmap autoThinFace(Bitmap bitmap, int strength);

    /**
     * 高光阴影调节
     * @param bitmap
     * @param darkLight 高光进度[-1,1]
     * @param highLight 阴影进度[-1,1]
     * @return
     */
    public static native Bitmap HighAndDarkLightChange(Bitmap bitmap,Bitmap mask,float darkLight,float highLight);


    /**
     * 将两张图片合成成一张图片
     * @param sourceBitmap 原始图片
     * @param addBitmap 需要添加的图片
     * @param addWeight 添加图片的显示权重，权重越大，显示越明显，最大值为1,最小值为0
     * @param startX 添加图片在原始图片中的相对X坐标位置
     * @param startY 添加图片在原始图片中的相对Y坐标位置
     */
    public static native Bitmap twoBitmapToOne(Bitmap sourceBitmap,Bitmap addBitmap,float addWeight,float startX,float startY);

    /**
     * 将两张图片合成成一张图片
     * @param sourceBitmap 原始图片
     * @param addBitmap 需要添加的图片
     * @param addWeight 添加图片的显示权重，权重越大，显示越明显，最大值为1,最小值为0
     * @param startX 添加图片在原始图片中的相对X坐标位置
     * @param startY 添加图片在原始图片中的相对Y坐标位置
     */
    public static native Bitmap twoBitmapToOneUseMask(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask, float addWeight,float startX,float startY);

    /**
     * 通过3dlut文件修改图片
     * @param bitmap
     * @param lutPath
     * @return
     */
    public static native Bitmap changeBy3dLut(Bitmap bitmap,String lutPath,float strength);

    /**
     * 将cube文件转成png图片
     * @param lutFileName
     * @param bitmap
     * @return
     */
    private static native Bitmap cubeToPng(String lutFileName,Bitmap bitmap);

    /**
     * 外部暴露的cube转png方法转换方法
     * @param lutFilePath
     * @return
     */
    public static Bitmap cubeFileToPng(String lutFilePath){
        Bitmap tmp = Bitmap.createBitmap(1,1,Bitmap.Config.ARGB_8888);
        Bitmap result = cubeToPng(lutFilePath,tmp);
        tmp.recycle();
        return result;
    }


    /**
     * 读取图片
     * @param imagePath
     * @return
     */
    public static native Bitmap readImageByPath(Bitmap bitmap,String imagePath,int maxWidth);

    /**
     * 自动对比度
     * @param bitmap
     * @return
     */
    public static native Bitmap autoContrast(Bitmap bitmap);

    /**
     * 改变色温
     * @param bitmap
     * @param tempValue
     * @return
     */
    public static native Bitmap changeTemp(Bitmap bitmap,Bitmap mask,int tempValue);

    /**
     * 改变色温
     * @param bitmap
     * @param tempValue
     * @return
     */
    public static native Bitmap changeColorHue(Bitmap bitmap,Bitmap mask,int tempValue);

    /**
     * 图片降噪
     * @param bitmap
     * @param strength
     * @return
     */
    public static native Bitmap denoise(Bitmap bitmap,float strength);

    /**
     * 自然饱和度
     * @param bitmap
     * @param stregth
     * @return
     */
    public static native Bitmap vibranceAlgorithm(Bitmap bitmap,int stregth);

    /**
     * 修改明度
     * @param bitmap
     * @param lisghtStrength
     * @return
     */
    public static native Bitmap changeLight(Bitmap bitmap,Bitmap mask,float lisghtStrength);

    /**
     * 分开修改各通道的明度（变化区间是【-1，1】）
     * @param bitmap
     * @param redValue 红色通道
     * @param yellowValue 黄色色通道
     * @return
     */
    public static native Bitmap changeLightByChannel(Bitmap bitmap,Bitmap mask,float redValue,float yellowValue,float greenValue,float cyanValue,float blueValue,float carmineValue);

    /**
     * 修改饱和度
     * @param bitmap
     * @param saturationStrength
     * @return
     */
    public static native Bitmap changeSaturation(Bitmap bitmap,float saturationStrength);

    /**
     * 修改色相
     * @param bitmap
     * @param saturationStrength
     * @return
     */
    public static native Bitmap changeHue(Bitmap bitmap,Bitmap mask,float saturationStrength);

    /**
     * 修改饱和度
     * @param bitmap
     * @param saturationStrength
     * @return
     */
    public static native Bitmap changeSaturationTwo(Bitmap bitmap,Bitmap mask,float saturationStrength);

    /**
     * 初始化面部特征获取
     */
    public static native void faceMarkInit(String faceMark1000ParamPathIn,String faceMark1000BinPathIn,String faceBoxParamPathIn,String faceBoxBinPathIn);

    /**
     * 情况面部识别点
     */
    public static native void clearFaceKeyPoint();

    /**
     * 释放面部识别
     */
//    public static native void destoryFaceModel();

    /**
     * 获取图片中人脸的图片及其特征点位置坐标
     * @param bitmap
     * @return
     */
    public static native FaceBean[] getFaceList(Bitmap bitmap);

    /**
     * 获取一纯色的图片
     * @param bitmap
     * @param r
     * @param g
     * @param b
     * @return
     */
    public static native Bitmap getSigleColorPhoto(Bitmap bitmap,int r,int g,int b);

    /**
     * 修改饱和度新算法
     * @param bitmap
     * @param redAdjust
     * @param greenAdjust
     * @param blueAdjust
     * @param cyanAdjust
     * @param yellowAdjust
     * @param carmineAdjust
     * @return
     */
    public static native Bitmap startChangeHsbS(Bitmap bitmap,Bitmap mask, float redAdjust, float greenAdjust, float blueAdjust, float cyanAdjust, float yellowAdjust, float carmineAdjust);

    /**
     * 色相修改新算法
     * @param bitmap
     * @param redAdjust
     * @param greenAdjust
     * @param blueAdjust
     * @param cyanAdjust
     * @param yellowAdjust
     * @param carmineAdjust
     * @return
     */
    public static native Bitmap startChangeHsbH(Bitmap bitmap,Bitmap mask, float redAdjust, float greenAdjust, float blueAdjust, float cyanAdjust, float yellowAdjust, float carmineAdjust);

    /**
     *
     * @param skin 需要磨皮的图片
     * @param lutMap 需要用的滤镜图片，位置为assets/Lut/SoftSkinFilter.png
     * @param strength 磨皮强度
     * @return
     */
    public static native Bitmap SoftSkin(Bitmap skin,Bitmap lutMap,Bitmap mask,int strength);

    /**
     * 通过lut文件使用滤镜
     * @param inputBitmap 输入将要使用滤镜的图片
     * @param filterBitmap 滤镜图片
     * @return 目标图片
     */
    public static native Bitmap applyLutByPng(Bitmap inputBitmap,Bitmap filterBitmap);

    /**
     * 修改嘴唇颜色
     * @param bitmap 待修改图片
     * @param strength 目标图片
     * @return
     */
    public static native Bitmap lipColorChange(Bitmap bitmap,Bitmap filterBitamp,int strength);


    /**
     * 使鼻子变得更加立体
     * @param bitmap 待修改图片
     * @param mskBitmap 模型图片
     * @param strength 强度
     * @return
     */
    public static native Bitmap noseStrength(Bitmap bitmap,Bitmap mskBitmap,int strength);

    /**
     * 修改皮肤颜色
     * @param bitmap
     * @param skinLut
     * @param strength
     * @return
     */
    public static native Bitmap skinColor(Bitmap bitmap,Bitmap skinLut,Bitmap skinMask,int strength);

    /**
     * 腮红
     * @param bitmap 待修改图片
     * @param leftModel 左边的模型
     * @param rightModel 右边模型
     * @param strength 强度
     * @return
     */
    public static native Bitmap faceBlush(Bitmap bitmap,Bitmap leftModel,Bitmap rightModel,int strength);

    /**
     * 美瞳
     * @param bitmap 待修改图片
     * @param maskBitmap 遮罩图片
     * @param strength 强度
     * @return 修改完的图片
     */
    public static native Bitmap beautyIris(Bitmap bitmap,Bitmap maskBitmap,int strength);

    /**
     * 修改眉毛
     * @param bitmap 待修改图片
     * @param eyeBrowLeft 左眼眉毛
     * @param eyeBitmapRight 右眼眉毛
     * @param strength 强度
     * @return 目标图片
     */
    public static native Bitmap beautyEyeBrow(Bitmap bitmap,Bitmap eyeBrowLeft,Bitmap eyeBitmapRight,int strength);

    /**
     * 修改眼线
     * @param bitmap 待修改图片
     * @param eyeBrowLeft 左眼眉毛
     * @param eyeBitmapRight 右眼眉毛
     * @param eyeLineDownStrength 下眼线强度
     * @param eyeLineUpStrength 上眼线强度
     * @return 目标图片
     */
    public static native Bitmap beautyEyeLine(Bitmap bitmap,Bitmap eyeBrowLeft,Bitmap eyeBitmapRight,int eyeLineUpStrength,int eyeLineDownStrength);

    /**
     * 超分辨率
     * @param bitmap 原始图片
     * @param size 放大倍数
     * @return
     */
    public static native Bitmap superres(Bitmap bitmap,int size,String modelPath,String modelName);

    /**
     * hsb快速蒙版
     * @param bitmap 目标图片
     * @param targetR 目标r
     * @param targetG 目标g
     * @param targetB 目标b
     * @param allowExtentB 明度（B）的容差值
     * @param allowExtentH 色相（H）的容差值
     * @param allowExtentS 饱和度（S）的容差值
     * @param isShowModel 是否是显示模式，如果是显示模式，非选中部分将会渲染成黑色
     * @param isAddModel true：添加模式 false：减去模式
     * @return 蒙版图片
     */
    public static native Bitmap HSBMask(Bitmap bitmap,Bitmap mask,int targetR,int targetG,int targetB,float allowExtentH,float allowExtentS,float allowExtentB,boolean isShowModel,boolean isAddModel);

    /**
     *
     * @param bitmap 原始图片
     * @param mask 蒙版图片
     * @param threshold 阈值
     * @return
     */
    public static native Bitmap ThresholdMask(Bitmap bitmap,Bitmap mask,int threshold,boolean isShowModel,boolean isAddModel);

    /**
     * 读取华为的应用ClientId
     * @return
     */
    public static native String getApplicationClientId();

    /**
     * 读取华为的应用ClientSecret
     * @return
     */
    public static native String getApplicationClientSecret();

    public static Bitmap cutoutMask(int type,float addWeight,Bitmap sourceBitmap,Bitmap maskBitmap,Bitmap cutoutBitmap,int left,int right){
        if (type == MIX_NORMAL){
            return normalMix(sourceBitmap,maskBitmap,cutoutBitmap,left,right,addWeight);
        } else if (type == MIX_SOFT_LIGHT){
            return toSoftLightMix(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_HARD_LIGHT){
            return toHardLightMix(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_OVAL){
            return toHardLightMix(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_HIGHT_LIGHT ){
            return toOverlayMix(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_LINE_LIGHT){
            return toLinearLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_POINT_LIGHT){
            return toPointLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_HARD_COLOR){
            return toHardMix(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_DARK){
            return toDark(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_LIGHT_OVAL){
            return toMultiply(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_COLOR_DEEPER){
            return toColorDark(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_LINE_DEEPER){
            return toLineColorLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_LIGHT){
            return toLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_FILTER_COLOR){
            return toFilterColor(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_COLOR_CUT){
            return toColorLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_LINE_CUT){
            return toLineColorLight(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_DIFFERENCE_VALUE){
            return toDifference(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_EXCLUDE){
            return toExclusion(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_MINUS){
            return toSubtract(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_DIVIDED_BY){
            return toDivide(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_HSB_H){
            return toHue(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_HSB_S){
            return toSaturation(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        } else if (type == MIX_COLOR){
            return toColor(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        }  else if (type == MIX_HSB_B){
            return toLuminosity(sourceBitmap,cutoutBitmap,maskBitmap,left,right,addWeight);
        }  else {
            return null;
        }
    }

    /**
     * 剪贴蒙版
     * @param sourceBitmap
     * @param maskBitmap
     * @param cutoutBitmap
     * @param left
     * @param right
     * @return
     */
    public static native Bitmap normalMix(Bitmap sourceBitmap,Bitmap maskBitmap,Bitmap cutoutBitmap,int left,int right,float addWeight);

    /**
     * 设置暗角
     * @param sourceBitmap
     * @param maskBitmap
     * @return
     */
    public static native Bitmap darkCorner(Bitmap sourceBitmap,Bitmap maskBitmap,boolean isBlac,float strength);

    /**
     * 图片修复
     * @param bitmap
     * @return
     */
    public static native Bitmap fixImage(Bitmap bitmap,Bitmap mask);

    /**
     * 将图片装换成蒙版
     * @param bitmap
     * @return
     */
    public static native Bitmap imageToMask(Bitmap bitmap);

    /**
     * 将图片装换成蒙版
     * @param bitmap
     * @param pro 增大非选中区域权重比值，最小值为1，不设最大值
     * @return
     */
    public static native Bitmap imageToMask(Bitmap bitmap,float pro);

    /**
     * 柔光叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toSoftLightMix(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 强光叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toHardLightMix(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 亮光叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toVividLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 线性光叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toLinearLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 点光叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toPointLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 实色叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toHardMix(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 差值叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toDifference(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 差值叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toExclusion(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 差值叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toSubtract(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 划分叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toDivide(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 色相叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toHue(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toSaturation(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toLuminosity(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);


    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toColor(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toFilterColor(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toColorDark(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toLineColorLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toLineColorDark(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toColorLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toMultiply(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);


    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toLight(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);


    /**
     * 饱和度叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toDark(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 将蒙版装换成透明度蒙版
     * @param mask
     * @return
     */
    public static native Bitmap maskToAlphaMask(Bitmap mask);


    /**
     * 叠加
     * @param sourceBitmap 底部原始图片
     * @param addBitmap 需要叠加的图片
     * @param mask 蒙版图片
     * @param left 叠加图片左边距（允许负值）
     * @param top 叠加图片上边距（允许负值）
     * @param addWeight 叠加权重
     * @return
     */
    public static native Bitmap toOverlayMix(Bitmap sourceBitmap,Bitmap addBitmap,Bitmap mask,int left,int top,float addWeight);

    /**
     * 将rgb不同通道的图片转成蒙版
     * @param sourceBitmap 原始图片
     * @param channelType 需要指定的颜色通道，0：R，1：G，2：B
     * @param needToOverValue 需要超过该值才被转换
     * @param isUseDefaultIfNotOver 没超过部分如何填充
     * @return
     */
    public static native Bitmap rgbBitmapToMask(Bitmap sourceBitmap,int channelType,int needToOverValue,boolean isUseDefaultIfNotOver);

    /**
     *
     * @param bitmap
     * @param borderCount 绿边，紫边数量
     * @param startH 起始色相
     * @param endH 终止色相
     * @return
     */
    public static native Bitmap removeBorder(Bitmap bitmap,int borderCount,int startH,int endH);

    public static native Bitmap test(Bitmap image,String paramPathForBox,String binPathForBox,String paramPath,String binPath);

    public static native Bitmap hsl(Bitmap bitmap,Bitmap mask,float r_s,float r_h,float r_b
            ,float g_s,float g_h,float g_b
            ,float b_s,float b_h,float b_b
            ,float c_s,float c_h,float c_b
            ,float m_s,float m_h,float m_b
            ,float y_s,float y_h,float y_b
            ,float o_s,float o_h,float o_b
            ,float p_s,float p_h,float p_b);

    public static native Point[] getFacePoint(Bitmap image,String paramPathForBox,String binPathForBox,String paramPath,String binPath);

    /**
     * 获取选中区域,飞选中区域的范围
     * @param mask
     * @return 选中区域范围
     */
    public static native Rect getMaskRect(Bitmap mask);

    /**
     * 替换图片中的色相和饱和度
     * @param targetH
     * @param targetS
     * @param bitmap
     * @param mask
     * @param strength
     * @return
     */
    public static native Bitmap replaceHAndS(int targetH,int targetS,int targetB,Bitmap bitmap,Bitmap mask,float strength,float proS,float proB);


    /**
     * 去除发型遮罩中皮肤部分
     * @param hairMask 头发遮罩
     * @param faceMask 面部遮罩
     * @param  skinMask 皮肤遮罩
     * @return
     */
    public static native Bitmap removeHairSkinArea(Bitmap hairMask,Bitmap faceMask,Bitmap skinMask);

    /**
     * 获取图片平均亮度
     * @param bitmap
     * @param mask
     * @return
     */
    public static native int getAverageLight(Bitmap bitmap,Bitmap mask);

    /**
     * 读取皮肤遮罩
     * @param bitmap
     * @return
     */
    public static native Bitmap getSkinMask(Bitmap bitmap);

    /**
     * 合并两张蒙层
     * @param maskOne
     * @param maskTwo
     * @return
     */
    public static native Bitmap maskAdd(Bitmap maskOne,Bitmap maskTwo);

    /**
     * 蒙层相减
     * @param sourceBitamp 被减蒙层
     * @param cutBitmap 减蒙层
     * @return
     */
    public static native Bitmap maskCut(Bitmap sourceBitamp,Bitmap cutBitmap);

    /**
     * 只保留相交部分选中区域的蒙版
     * @param sourceBitamp
     * @param cutBitmap
     * @return
     */
    public static native Bitmap keepIntersectMask(Bitmap sourceBitamp,Bitmap cutBitmap);

    /**
     * 获取瞳孔蒙层
     * @param sourceBitmap 原始图片
     * @param eyeMask 两只眼睛的蒙层
     * @return
     */
    public static native Bitmap getPupilMaskByEyeMask(Bitmap sourceBitmap,Bitmap eyeMask);
}
