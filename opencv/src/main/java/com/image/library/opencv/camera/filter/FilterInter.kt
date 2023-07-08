package com.image.library.opencv.camera.filter

interface FilterInter {
    fun onDrawFrame(textureId: Int): Int
    fun setTransformMatrix(mtx: FloatArray)
    fun onReady(width: Int, height: Int)
}