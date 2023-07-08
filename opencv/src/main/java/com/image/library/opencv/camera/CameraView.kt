package com.image.library.opencv.camera

import android.annotation.SuppressLint
import android.content.Context
import android.graphics.*
import android.opengl.GLSurfaceView
import android.os.Handler
import android.os.Looper
import android.util.AttributeSet
import android.util.Size
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.View
import android.view.View.OnLayoutChangeListener
import android.view.ViewConfiguration
import androidx.camera.core.CameraSelector
import androidx.camera.core.SurfaceRequest
import androidx.camera.core.impl.CameraInternal
import androidx.core.content.ContextCompat
import com.image.library.opencv.camera.render.CameraRender
import com.image.library.opencv.ext.dp2px
import java.io.File
import java.text.DecimalFormat

class CameraView : GLSurfaceView {
    private var mTouchUpEvent: MotionEvent? = null
    private var mScaleGestureDetector: ScaleGestureDetector? = null
    private val mPreviewTransform = PreviewTransformation()
    private var mPreviewViewMeteringPointFactory = PreviewViewMeteringPointFactory(mPreviewTransform)
    private var mFocusData = FocusData(false,false)
    private var mScaleData = ScaleData(false,1f)
    private var mHandle = Handler(Looper.getMainLooper())
    private var mRectFocus = RectF()
    private var mFocusPaint = Paint(Paint.ANTI_ALIAS_FLAG)
    private var mTextPaint = Paint(Paint.ANTI_ALIAS_FLAG)
    //九宫格
    private var mLinePaint = Paint(Paint.ANTI_ALIAS_FLAG)
    private var mFnum = DecimalFormat("##0.0")
    //是否开启参考线
    private var mIsOpenLine = false

    //对焦监听
    var focusCallback: (isSuccess: Boolean, errorMessage: String?) -> Unit =
        { isSuccess, errorMessage ->
            mHandle.removeMessages(0)
            mFocusData.isFocusSuccess = isSuccess
            invalidate()
            mHandle.postDelayed({
                mFocusData.isShowFocusRect = false
                invalidate()
            },400)
        }
    private var mRender = CameraRender(context) {
        requestRender()
    }
    private val mOnLayoutChangeListener =
        OnLayoutChangeListener { v: View?, left: Int, top: Int, right: Int, bottom: Int, oldLeft: Int, oldTop: Int, oldRight: Int, oldBottom: Int ->
            val isSizeChanged =
                right - left != oldRight - oldLeft || bottom - top != oldBottom - oldTop
            if (isSizeChanged) {
                redrawPreview()
            }
        }

    init {
        setEGLContextClientVersion(2)
        setRenderer(mRender)
        renderMode = RENDERMODE_WHEN_DIRTY
    }

    constructor(context: Context) : super(context)

    constructor(context: Context, attrs: AttributeSet) : super(context, attrs) {
        initPaint()
        //设置相机收拾缩放
        mScaleGestureDetector =
            ScaleGestureDetector(context, object : ScaleGestureDetector.OnScaleGestureListener {
                override fun onScale(detector: ScaleGestureDetector): Boolean {
                    getCameraController()?.onPinchToZoom(detector!!.scaleFactor)
                    mScaleData.scaleValue = getCameraController()?.getCameraScale()?:1f
                    mScaleData.isShowScale = true
                    invalidate()
                    return true
                }

                override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
                    return true
                }

                override fun onScaleEnd(detector: ScaleGestureDetector) {
                    mScaleData.scaleValue = getCameraController()?.getCameraScale()?:1f
                    mScaleData.isShowScale = false
                    invalidate()
                }
            })

        //设置camerax的回调监听
        mRender.cameraXCallback = object : CameraRender.CameraXCallback {
            @SuppressLint("RestrictedApi")
            override fun onSurfaceRequested(request: SurfaceRequest) {
                val camera: CameraInternal = request.camera
                //是否是前置相机
                val isFrontCamera =
                    (camera.cameraInfoInternal.lensFacing == CameraSelector.LENS_FACING_FRONT)
                //设置翻转监听
                request.setTransformationInfoListener(
                    ContextCompat.getMainExecutor(getContext()),
                    { transformationInfo ->
                        mPreviewTransform.setTransformationInfo(
                            transformationInfo,
                            request.resolution,
                            isFrontCamera
                        )
                        redrawPreview()
                    })
            }
        }
    }

    private fun initPaint() {
        mFocusPaint.color = Color.RED
        mFocusPaint.style = Paint.Style.STROKE
        mFocusPaint.textSize = dp2px(6f).toFloat()
        mFocusPaint.strokeWidth = dp2px(2f).toFloat()

        mTextPaint.color = Color.WHITE
        mTextPaint.style = Paint.Style.FILL_AND_STROKE
        mTextPaint.textSize = dp2px(15f).toFloat()
        mTextPaint.strokeWidth = dp2px(1f).toFloat()

        mLinePaint.color = Color.WHITE
        mLinePaint.style = Paint.Style.FILL_AND_STROKE
        mLinePaint.strokeWidth = dp2px(0.5f).toFloat()
    }

    fun setProgress(progress: Float) {
        mRender.setProgress(progress)
    }

    fun destroyRotationListener(){
        mRender.destroyRotationListener()
    }

    /**
     * 设置闪光灯模式
     * ImageCapture.FLASH_MODE_AUTO,FLASH_MODE_OFF,FLASH_MODE_ON
     */
    fun setCameraFlashModel(model:Int){
        mRender.setFlashModel(model)
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        //是否是单指点击
        val isSingleTouch = event!!.pointerCount == 1
        //是否是抬起事件
        val isUpEvent = event.action == MotionEvent.ACTION_UP
        //是否是长按击
        val notALongPress =
            event.eventTime - event.downTime < ViewConfiguration.getLongPressTimeout();
        if (isSingleTouch && isUpEvent && notALongPress) {
            mTouchUpEvent = event
            mTouchUpEvent?.let {
                mRectFocus.left = it.x - dp2px(20f)
                mRectFocus.right = it.x + dp2px(20f)
                mRectFocus.top = it.y - dp2px(20f)
                mRectFocus.bottom = it.y + dp2px(20f)
            }
            performClick()
            return true
        }
        return (mScaleGestureDetector?.onTouchEvent(event) ?: false) || super.onTouchEvent(event);
    }

    fun setTint(progress: Float) {
        mRender.setTint(progress)
    }

    private fun getCameraController(): CameraController? {
        return mRender.cameraController2
    }

    fun redrawPreview() {
        mPreviewViewMeteringPointFactory.recalculate(
            Size(width, height),
            layoutDirection
        )
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        addOnLayoutChangeListener(mOnLayoutChangeListener)
    }

    override fun onDetachedFromWindow() {
        super.onDetachedFromWindow()
        removeOnLayoutChangeListener(mOnLayoutChangeListener)
        getCameraController()?.destory()
    }


    override fun performClick(): Boolean {
        if (mTouchUpEvent == null) {
            return super.performClick()
        }
        val x = if (mTouchUpEvent != null) mTouchUpEvent!!.x else width / 2f
        val y = if (mTouchUpEvent != null) mTouchUpEvent!!.y else height / 2f
        getCameraController()?.onTapToFocus(mPreviewViewMeteringPointFactory, x, y, focusCallback)
        mTouchUpEvent = null

        mFocusData.isShowFocusRect = true
        mFocusData.isFocusSuccess = false
        invalidate()
        return super.performClick()
    }


    override fun dispatchDraw(canvas: Canvas?) {
        super.dispatchDraw(canvas)
        //绘制对焦框
        if (mFocusData.isShowFocusRect){
            mFocusPaint.color = if (mFocusData.isFocusSuccess) Color.GREEN else Color.RED
            canvas?.drawRoundRect(mRectFocus, dp2px(2f).toFloat(), dp2px(2f).toFloat(),mFocusPaint)
        }
        //绘制缩放大小文案
        val str = "${mFnum.format(mScaleData.scaleValue)}"
        val wi = mTextPaint.measureText(str)
        canvas?.drawText(str,width/2f - wi/2f,height.toFloat() - dp2px(20f),mTextPaint)
        if (mIsOpenLine){
            //绘制九宫格
            val lineW = width/3f
            val lineH = height/3f
            for (item in 0 until 2){
                mLinePaint.color = Color.parseColor("#5FFFFFFF")
                mLinePaint.strokeWidth = 2f
                canvas?.drawLine(lineW*(item+1),0f,lineW*(item+1),height.toFloat(),mLinePaint)
            }
            for (item in 0 until 2){
                mLinePaint.color = Color.parseColor("#5FFFFFFF")
                mLinePaint.strokeWidth = 2f
                canvas?.drawLine(0f,lineH*(item+1),width.toFloat(),lineH*(item+1),mLinePaint)
            }
        }
    }

    fun openCameraLine(isOpen:Boolean){
        this.mIsOpenLine = isOpen
        invalidate()
    }


    fun updateLutBitmap(lutBitmap: Bitmap?) {
        mRender?.updateLutBitmap(lutBitmap)
    }

    fun isUseFront(): Boolean {
        return mRender.useFront
    }

    fun switchCamera(switchCallback: CameraRender.SwitchCameraCallback? = null) {
        mRender?.changeCameraFontOrBack(!(mRender?.useFront ?: false), switchCallback)
    }

    fun changeCameraSize(size: Size = Size(1080, 1440)) {
        mRender?.changeCameraSize(size)
    }

    fun setCameraRatio() {

    }

    fun takePhoto(lutFile: File?, finishCallback: () -> Unit = {}) {
        mRender?.takePhoto(lutFile, finishCallback)
    }

    fun setLutStrength(strength: Int) {
        mRender?.updateLutStrength(strength)
    }

    data class FocusData(var isShowFocusRect:Boolean,var isFocusSuccess:Boolean)
    data class ScaleData(var isShowScale:Boolean,var scaleValue:Float)
}