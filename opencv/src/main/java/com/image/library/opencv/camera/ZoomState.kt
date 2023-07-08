package com.image.library.opencv.camera

import androidx.annotation.RequiresApi


@RequiresApi(21) // TODO(b/200306659): Remove and replace with annotation on package-info.java
interface ZoomState {
    /**
     * 返回缩放比例，默认比例是1
     */
    val zoomRatio: Float

    /** 最大缩放比例  */
    val maxZoomRatio: Float

    /**
     * 返回最小缩放比例
     * 缩放比例无法小于1，除非相机设备支持zoom-out(只在abdroid11或更新的系统上)
     */
    val minZoomRatio: Float

    /**
     * Returns the linearZoom which is in range [0..1].
     * 通过范围从0f到1.0f的线性缩放值设置当前缩放，返回当前缩放值
     */
    val linearZoom: Float
}
