package com.image.library.opencv.bean;

import android.graphics.Bitmap;
import android.graphics.Point;

import java.util.List;

public class FaceBean {
    //人脸区域
    private Bitmap faceBitmap;
    //人脸特征点坐标
    private Point points[];

    public Bitmap getFaceBitmap() {
        return faceBitmap;
    }

    public void setFaceBitmap(Bitmap faceBitmap) {
        this.faceBitmap = faceBitmap;
    }

    public Point[] getPoints() {
        return points;
    }

    public void setPoints(Point[] points) {
        this.points = points;
    }
}
