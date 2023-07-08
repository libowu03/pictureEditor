package com.image.library.opencv.utils;


import android.annotation.SuppressLint;
import android.app.Application;

import androidx.annotation.NonNull;

import java.lang.reflect.Method;


public class Applications {

    @NonNull
    public static Application context() {
        return CURRENT;
    }

    @SuppressLint("StaticFieldLeak")
    private static final Application CURRENT;

    static {
        try {
            Object activityThread = getActivityThread();
            Object app = activityThread.getClass().getMethod("getApplication")
                    .invoke(activityThread);
            CURRENT = (Application) app;
        } catch (Throwable e) {
            throw new IllegalStateException("获取application失败", e);
        }
    }

    private static Object getActivityThread() {
        Object activityThread = null;
        try {
            @SuppressLint("PrivateApi") Method method = Class.forName("android.app.ActivityThread")
                    .getMethod("currentActivityThread");
            method.setAccessible(true);
            activityThread = method.invoke(null);
        } catch (final Exception e) {
            //Log.w(TAG, e);
        }
        return activityThread;
    }
}