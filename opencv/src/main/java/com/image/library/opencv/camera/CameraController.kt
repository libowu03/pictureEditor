package com.image.library.opencv.camera

import android.annotation.SuppressLint
import android.util.Log
import android.util.Range
import androidx.camera.core.*
import androidx.camera.core.ZoomState
import androidx.camera.core.impl.utils.executor.CameraXExecutors
import androidx.camera.core.impl.utils.futures.FutureCallback
import androidx.camera.core.impl.utils.futures.Futures
import androidx.camera.view.CameraController
import androidx.lifecycle.MutableLiveData
import com.google.common.util.concurrent.ListenableFuture
import com.image.library.opencv.camera.ForwardingLiveData
import com.image.library.opencv.camera.PreviewViewMeteringPointFactory

class CameraController{
    private val mZoomState = ForwardingLiveData<ZoomState>()
    private val mTorchState = ForwardingLiveData<Int>()
    private var mClampedRatio = 1f
    /**
     * 用户没有点击对焦操作
     */
    val TAP_TO_FOCUS_NOT_STARTED = 0
    /**
     * 点击对焦动作已经出发，但是尚未完成
     * 如果此时有新的点击动作产生，上一个动作将结束
     */
    val TAP_TO_FOCUS_STARTED = 1
    /**
     * 成功完成点击对焦动作
     */
    val TAP_TO_FOCUS_FOCUSED = 2
    /**
     * 成功执行对焦动作，但是设备相机仍然无法对焦
     * 最好是让用户重新点击更远处或更亮的区域以完成对焦
     */
    val TAP_TO_FOCUS_NOT_FOCUSED = 3
    /**
     * 用户点击对焦动作执行失败
     */
    val TAP_TO_FOCUS_FAILED = 4
    // 自动对焦区域 1/6
    private val AF_SIZE = 1.0f / 6.0f
    private val AE_SIZE = AF_SIZE * 1.5f
    val mTapToFocusState = MutableLiveData(CameraController.TAP_TO_FOCUS_NOT_STARTED)
    var mainCamera: Camera? = null

    fun setCamera(cameraInfo: Camera){
        mainCamera = cameraInfo
        mZoomState.setSource(cameraInfo.cameraInfo.zoomState)
        mTorchState.setSource(cameraInfo.cameraInfo.torchState)
    }

    fun onPinchToZoom(pinchToZoomScale:Float){
        val zoomState: ZoomState = mZoomState.value ?: return
        var clampedRatio: Float = zoomState.zoomRatio * speedUpZoomBy2X(pinchToZoomScale)
        //缩放比例限制
        clampedRatio = Math.min(Math.max(clampedRatio, zoomState.minZoomRatio), zoomState.maxZoomRatio)
        setZoomRatio(clampedRatio)
        mClampedRatio = clampedRatio
    }

    /**
     * 获取相机缩放值
     */
    fun getCameraScale():Float{
        return mClampedRatio
    }

    private fun speedUpZoomBy2X(scaleFactor: Float): Float {
        return if (scaleFactor > 1f) {
            1.0f + (scaleFactor - 1.0f) * 2
        } else {
            1.0f - (1.0f - scaleFactor) * 2
        }
    }

    @SuppressLint("RestrictedApi")
    fun setZoomRatio(zoomRatio: Float): ListenableFuture<Void?>? {
        if (mainCamera == null) {
            return Futures.immediateFuture(null)
        }
        return mainCamera?.cameraControl?.setZoomRatio(zoomRatio)
    }

    @SuppressLint("RestrictedApi")
    fun onTapToFocus(meteringPointFactory: PreviewViewMeteringPointFactory, x: Float, y: Float, focusCallback:(isSuccess:Boolean, errorMessage:String?)->Unit = { isSuccess, errorMessage->}) {
        if (mainCamera == null){
            return
        }
        mTapToFocusState.postValue(CameraController.TAP_TO_FOCUS_STARTED)
        val afPoint: MeteringPoint = meteringPointFactory.createPoint(x, y, AF_SIZE)
        val aePoint: MeteringPoint = meteringPointFactory.createPoint(x, y, AE_SIZE)
        val focusMeteringAction = FocusMeteringAction.Builder(afPoint, FocusMeteringAction.FLAG_AF)
            .addPoint(aePoint, FocusMeteringAction.FLAG_AE)
            .build()
        Futures.addCallback(mainCamera!!.cameraControl.startFocusAndMetering(focusMeteringAction),
            object : FutureCallback<FocusMeteringResult?> {
                override fun onSuccess(result: FocusMeteringResult?) {
                    if (result == null) {
                        focusCallback.invoke(false,null)
                        return
                    }
                    mTapToFocusState.postValue(if (result.isFocusSuccessful) CameraController.TAP_TO_FOCUS_FOCUSED else CameraController.TAP_TO_FOCUS_NOT_FOCUSED)
                    focusCallback.invoke(true,null)
                }

                override fun onFailure(t: Throwable) {
                    focusCallback.invoke(false,t.message)
                    if (t is CameraControl.OperationCanceledException) {
                        return
                    }
                    Log.i("日志","错误：${t.message}")
                    mTapToFocusState.postValue(CameraController.TAP_TO_FOCUS_FAILED)
                }
            }, CameraXExecutors.directExecutor()
        )
    }

    /**
     * 设置曝光补偿(iso)
     */
    fun setExposureCompensationIndex(exposureIndex:Int){
        if (mainCamera == null){
            return
        }
        val range = mainCamera!!.cameraInfo.exposureState.exposureCompensationRange
        if (exposureIndex > range.upper || exposureIndex < range.lower){
            return
        }
        if (range.contains(exposureIndex + 1)) {
            mainCamera!!.cameraControl.setExposureCompensationIndex(exposureIndex)
            val ev = mainCamera!!.cameraInfo.exposureState.exposureCompensationStep.toFloat() * exposureIndex
        }
    }

    /**
     * 获取曝光区间
     */
    fun getExposureCompensationRange():Range<Int>{
        if (mainCamera == null){
            return Range(0,0)
        }
        return mainCamera!!.cameraInfo.exposureState.exposureCompensationRange
    }

    fun destory(){
        mainCamera = null
    }
}