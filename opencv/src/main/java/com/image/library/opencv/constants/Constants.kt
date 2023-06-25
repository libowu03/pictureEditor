package com.image.library.opencv.constants

object Constants {

    object MixType{
        //正常叠加
        const val MIX_NORMAL = 0
        //叠加
        const val MIX_OVAL = 1
        //柔光叠加
        const val MIX_SOFT_LIGHT = 2
        //强光叠加
        const val MIX_HARD_LIGHT = 3
        //亮光叠加
        const val MIX_HIGHT_LIGHT = 4
        //线性光叠加
        const val MIX_LINE_LIGHT = 5
        //点光
        const val MIX_POINT_LIGHT = 6
        //实色混合
        const val MIX_HARD_COLOR = 7

        //变暗
        const val MIX_DARK = 8
        //正片叠底
        const val MIX_LIGHT_OVAL = 9
        //颜色加深
        const val MIX_COLOR_DEEPER = 10
        //线性加深
        const val MIX_LINE_DEEPER = 11
        //深色
        //const val MIX_DEEPER = 12

        //变亮
        const val MIX_LIGHT = 12
        //滤色
        const val MIX_FILTER_COLOR = 13
        //颜色减淡
        const val MIX_COLOR_CUT = 14
        //线性减淡
        const val MIX_LINE_CUT = 15

        //差值
        const val MIX_DIFFERENCE_VALUE = 16
        //排除
        const val MIX_EXCLUDE = 17
        //减去
        const val MIX_MINUS = 18
        //划分
        const val MIX_DIVIDED_BY = 19

        //色相
        const val MIX_HSB_H = 20
        //饱和度
        const val MIX_HSB_S = 21
        //颜色
        const val MIX_COLOR = 22
        //明度
        const val MIX_HSB_B = 23
    }

}