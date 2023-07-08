package com.image.library.opencv.camera.filter;

import android.opengl.Matrix;

/**
 * Created by wangyt on 2019/6/4
 */
public class MatrixUtil {

    /**
     * x/y 方向翻转矩阵
     * @param m 矩阵
     * @param x true：x方向翻转；false：x方向不翻转
     * @param y true：y方向翻转；false：y方向不翻转
     * @return
     */
    public static float[] flip(float[] m,boolean x,boolean y){
        if(x||y){
            Matrix.scaleM(m,0,x?-1:1,y?-1:1,1);
        }
        return m;
    }
}
