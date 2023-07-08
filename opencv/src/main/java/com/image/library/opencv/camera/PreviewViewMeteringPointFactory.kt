package com.image.library.opencv.camera

import android.annotation.SuppressLint
import android.graphics.Matrix
import android.graphics.PointF
import android.util.Size
import androidx.annotation.*
import androidx.camera.core.MeteringPointFactory
import androidx.camera.core.impl.utils.Threads


/**
 * [MeteringPointFactory] for [PreviewView].
 *
 * @hide
 */
@SuppressLint("RestrictedApi")
@RequiresApi(21)
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
class PreviewViewMeteringPointFactory(private val mPreviewTransformation: PreviewTransformation) : MeteringPointFactory() {
    @GuardedBy("this")
    private var mMatrix: Matrix? = null

    @AnyThread
    override fun convertPoint(x: Float, y: Float): PointF {
        val point = floatArrayOf(x, y)
        synchronized(this) {
            if (mMatrix == null) {
                return INVALID_POINT
            }
            mMatrix!!.mapPoints(point)
        }
        return PointF(point[0], point[1])
    }

    @UiThread
    fun recalculate(previewViewSize: Size, layoutDirection: Int) {
        Threads.checkMainThread()
        synchronized(this) {
            if (previewViewSize.width == 0 || previewViewSize.height == 0) {
                mMatrix = null
                return
            }
            mMatrix = mPreviewTransformation.getPreviewViewToNormalizedSurfaceMatrix(
                previewViewSize,
                layoutDirection
            )
        }
    }

    companion object {
        val INVALID_POINT = PointF(2f, 2f)
    }
}