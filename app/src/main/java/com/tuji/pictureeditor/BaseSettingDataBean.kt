package com.tuji.pictureeditor

/**
 * 图像基础调节的数据
 * @param light 亮度调节
 * @param contrast 对比度调节
 * @param darkArea 暗部调节
 * @param lightArea 亮部调节
 * @param dunk 锐化
 * @param dunkNumber 锐化（数量）
 * @param dunkRadio 锐化半径
 * @param dunkThreshold 锐化阈值
 * @param saturation 饱和度
 */
data class BaseSettingDataBean(
    var light: Double = 0.0,
    var contrast: Double = 0.0,
    var darkArea: Double = 0.0,
    var lightArea: Double = 0.0,
    var dunk: Double = 0.0,
    var dunkNumber: Double = 500.0,
    var dunkRadio: Double = 1.0,
    var dunkThreshold: Double = 0.0,
    var saturation:Double = 0.0
)