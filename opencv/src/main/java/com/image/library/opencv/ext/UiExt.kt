package com.image.library.opencv.ext

import android.app.Activity
import android.content.Context
import android.content.res.Resources
import android.graphics.Color
import android.graphics.Outline
import android.os.Build
import android.provider.Settings
import android.view.*
import java.lang.reflect.Method
import android.util.DisplayMetrics
import android.view.WindowManager


fun setStatusBarHidden(activity: Activity, isHidden: Boolean, isDark: Boolean = true) {
    if (isHidden) {
        try {
            val window = activity.window
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
                val option =
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                    var vis =
                        window.decorView.systemUiVisibility and View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                    if (isDark) {
                        vis =
                            window.decorView.systemUiVisibility and View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                        window.decorView.systemUiVisibility = /*option or vis*/View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
                    } else {
                        window.decorView.systemUiVisibility = option
                    }
                } else {
                    window.decorView.systemUiVisibility = option
                }
                window.statusBarColor = Color.TRANSPARENT
            } else {
                window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            }
            val parent = activity.findViewById<View>(android.R.id.content) as ViewGroup
            val fakeStatusBarView = parent.findViewWithTag<View>("TAG_STATUS_BAR")
            if (fakeStatusBarView != null) {
                if (fakeStatusBarView.visibility == View.GONE) {
                    fakeStatusBarView.visibility = View.VISIBLE
                }
                fakeStatusBarView.setBackgroundColor(Color.TRANSPARENT)
            } else {
                val statusBarView = View(activity)
                statusBarView.layoutParams = ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, getStatusBarHeightExt()
                )
                statusBarView.setBackgroundColor(Color.TRANSPARENT)
                statusBarView.tag = "TAG_STATUS_BAR"
                parent.addView(statusBarView)
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}

fun setStatusBarColor(activity: Activity, color: Int) {
    if (color == -1) {
        return
    }
    val parent = activity.findViewById<View>(android.R.id.content) as ViewGroup
    val statusBarView = parent.findViewWithTag<View>("TAG_STATUS_BAR")
    if (statusBarView != null) {
        statusBarView.visibility = View.VISIBLE
        statusBarView.setBackgroundColor(color)
    } else {
        val statusBarView = View(activity)
        statusBarView.layoutParams = ViewGroup.LayoutParams(
            ViewGroup.LayoutParams.MATCH_PARENT, getStatusBarHeightExt()
        )
        statusBarView.setBackgroundColor(color)
        statusBarView.tag = "TAG_STATUS_BAR"
        parent.addView(statusBarView)
    }
}

/** 状态栏高度 */
fun getStatusBarHeightExt(): Int {
    val resources = Resources.getSystem()
    val resourceId = resources.getIdentifier("status_bar_height", "dimen", "android")
    return resources.getDimensionPixelSize(resourceId)
}


//获取是否存在NavigationBar
fun Context.checkDeviceHasNavigationBar(): Boolean {
    var hasNavigationBar = false;
    val rs = resources;
    val id = rs.getIdentifier("config_showNavigationBar", "bool", "android");
    if (id > 0) {
        hasNavigationBar = rs.getBoolean(id);
    }
    try {
        val systemPropertiesClass = Class.forName("android.os.SystemProperties");
        val m = systemPropertiesClass.getMethod("get", String::class.java) as Method
        val navBarOverride = m.invoke(systemPropertiesClass, "qemu.hw.mainkeys") as String
        //判断是否隐藏了底部虚拟导航
        var navigationBarIsMin = 0
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.LOLLIPOP) {
            navigationBarIsMin = Settings.System.getInt(
                contentResolver,
                "navigationbar_is_min", 0
            )
        } else {
            navigationBarIsMin = Settings.Global.getInt(
                contentResolver,
                "navigationbar_is_min", 0
            )
        }
        if ("1" == navBarOverride || 1 == navigationBarIsMin) {
            hasNavigationBar = false
        } else if ("0" == navBarOverride) {
            hasNavigationBar = true
        }
    } catch (e: java.lang.Exception) {
    }
    return hasNavigationBar
}

/**
 * 获取虚拟功能键高度
 * @param context
 * @return
 */
fun Context.getVirtualBarHeigh(): Int {
    return if (checkDeviceHasNavigationBar()) {
        var totalHeight = getDpi()
        var contentHeight = getScreenHeight();
        totalHeight - contentHeight - getStatusBarHeightExt()
    } else {
        0
    }
}

//获取屏幕原始尺寸高度，包括虚拟功能键高度
fun Context.getDpi():Int{
    if (this !is Activity){
        return -1
    }
    var dpi = 0
    val windowManager = windowManager
    getSystemService(Context.WINDOW_SERVICE);
    val display = windowManager.getDefaultDisplay();
    val displayMetrics = DisplayMetrics();
    @SuppressWarnings("rawtypes")
    var c:Class<*>?=null
    try {
        c = Class.forName("android.view.Display");
        @SuppressWarnings("unchecked")
        val method = c.getMethod("getRealMetrics", DisplayMetrics::class.java)
        method.invoke(display, displayMetrics)
        dpi = displayMetrics.heightPixels
    } catch (e:java.lang.Exception) {
        e.printStackTrace()
    }
    return dpi
}

//获取屏幕高度 不包含虚拟按键=
fun Context.getScreenHeight():Int{
    val dm = resources.displayMetrics;
    return dm.heightPixels
}


/**
 * 设置圆角
 */
fun View.setRoundExt(radius: Float) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
        this?.outlineProvider = object : ViewOutlineProvider() {
            override fun getOutline(view: View?, outline: Outline?) {
                view?.let {
                    outline?.setRoundRect(
                        0, 0, it.width, it.height,
                        dp2px(radius).toFloat()
                    )
                }
            }
        }
        this?.clipToOutline = true
    }
}


//region 像素转换
fun dp2px(dpValue: Float): Int {
    return (0.5f + dpValue * Resources.getSystem().displayMetrics.density).toInt()
}

fun px2dp(px: Float): Int {
    val scale = kotlin.math.max(Resources.getSystem().displayMetrics.density, 1f)
    val dp = 0.5f + px / scale
    return dp.toInt()
}

