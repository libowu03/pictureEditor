package com.image.library.opencv.camera

import android.annotation.SuppressLint
import android.graphics.*
import android.graphics.Matrix.ScaleToFit
import android.util.LayoutDirection
import android.util.Size
import android.view.TextureView
import android.view.View
import androidx.annotation.RequiresApi
import androidx.annotation.VisibleForTesting
import androidx.camera.core.SurfaceRequest.TransformationInfo
import androidx.camera.view.PreviewView
import androidx.camera.view.TransformUtils
import androidx.core.util.Preconditions

@RequiresApi(21) // TODO(b/200306659): Remove and replace with annotation on package-info.java
class PreviewTransformation {
    // SurfaceRequest.getResolution().
    private var mResolution: Size? = null

    /**
     * Return the crop rect of the preview surface.
     */
    // This represents the area of the Surface that should be visible to end users. The area is
    // defined by the Viewport class.
    var surfaceCropRect: Rect? = null
        private set

    // TransformationInfo.getRotationDegrees().
    private var mPreviewRotationDegrees = 0

    // TransformationInfo.getTargetRotation.
    private var mTargetRotation = 0

    // Whether the preview is using front camera.
    private var mIsFrontCamera = false
    /**
     * Gets the [PreviewView.ScaleType].
     */
    /**
     * Sets the [PreviewView.ScaleType].
     */
    var scaleType = DEFAULT_SCALE_TYPE

    /**
     * Sets the inputs.
     *
     *
     *  All the values originally come from a [SurfaceRequest].
     */
    // TODO(b/185869869) Remove the UnsafeOptInUsageError once view's version matches core's.
    @SuppressLint("UnsafeOptInUsageError", "RestrictedApi")
    fun setTransformationInfo(
        transformationInfo: TransformationInfo,
        resolution: Size, isFrontCamera: Boolean
    ) {
        surfaceCropRect = transformationInfo.cropRect
        mPreviewRotationDegrees = transformationInfo.rotationDegrees
        mTargetRotation = transformationInfo.targetRotation
        mResolution = resolution
        mIsFrontCamera = isFrontCamera
    }

    /**
     * Creates a matrix that makes [TextureView]'s rotation matches the
     * [.mTargetRotation].
     *
     *
     *  The value should be applied by calling [TextureView.setTransform]. Usually
     * [.mTargetRotation] is the display rotation. In that case, this
     * matrix will just make a [TextureView] works like a [SurfaceView]. If not, then
     * it will further correct it to the desired rotation.
     *
     *
     *  This method is also needed in [.createTransformedBitmap] to correct the screenshot.
     */
    @get:VisibleForTesting
    val textureViewCorrectionMatrix: Matrix
        @SuppressLint("RestrictedApi")
        get() {
            Preconditions.checkState(isTransformationInfoReady)
            val surfaceRect = RectF(
                0f, 0f, mResolution!!.width.toFloat(),
                mResolution!!.height.toFloat()
            )
            val rotationDegrees = -TransformUtils.surfaceRotationToRotationDegrees(mTargetRotation)
            return TransformUtils.getRectToRect(surfaceRect, surfaceRect, rotationDegrees)
        }

    /**
     * Calculates the transformation and applies it to the inner view of [PreviewView].
     *
     *
     *  The inner view could be [SurfaceView] or a [TextureView].
     * [TextureView] needs a preliminary correction since it doesn't handle the
     * display rotation.
     */
    fun transformView(previewViewSize: Size, layoutDirection: Int, preview: View) {
        if (previewViewSize.height == 0 || previewViewSize.width == 0) {
            return
        }
        if (!isTransformationInfoReady) {
            return
        }
        if (preview is TextureView) {
            // For TextureView, correct the orientation to match the target rotation.
            preview.setTransform(textureViewCorrectionMatrix)
        } else {
            // Logs an error if non-display rotation is used with SurfaceView.
            val display = preview.display
            if (display != null && display.rotation != mTargetRotation) {
            }
        }
        val surfaceRectInPreviewView = getTransformedSurfaceRect(
            previewViewSize,
            layoutDirection
        )
        preview.pivotX = 0f
        preview.pivotY = 0f
        preview.scaleX = surfaceRectInPreviewView.width() / mResolution!!.width
        preview.scaleY = surfaceRectInPreviewView.height() / mResolution!!.height
        preview.translationX = surfaceRectInPreviewView.left - preview.left
        preview.translationY = surfaceRectInPreviewView.top - preview.top
    }

    /**
     * Gets the transformed [Surface] rect in PreviewView coordinates.
     *
     *
     *  Returns desired rect of the inner view that once applied, the only part visible to
     * end users is the crop rect.
     */
    @SuppressLint("RestrictedApi")
    private fun getTransformedSurfaceRect(previewViewSize: Size, layoutDirection: Int): RectF {
        Preconditions.checkState(isTransformationInfoReady)
        val surfaceToPreviewView = getSurfaceToPreviewViewMatrix(previewViewSize, layoutDirection)
        val rect = RectF(
            0f, 0f, mResolution!!.width.toFloat(),
            mResolution!!.height.toFloat()
        )
        surfaceToPreviewView.mapRect(rect)
        return rect
    }

    /**
     * Calculates the transformation from [Surface] coordinates to [PreviewView]
     * coordinates.
     *
     *
     *  The calculation is based on making the crop rect to fill or fit the [PreviewView].
     */
    @SuppressLint("RestrictedApi")
    fun getSurfaceToPreviewViewMatrix(previewViewSize: Size, layoutDirection: Int): Matrix {
        Preconditions.checkState(isTransformationInfoReady)

        // Get the target of the mapping, the coordinates of the crop rect in PreviewView.
        val previewViewCropRect: RectF
        previewViewCropRect = if (isViewportAspectRatioMatchPreviewView(previewViewSize)) {
            // If crop rect has the same aspect ratio as PreviewView, scale the crop rect to fill
            // the entire PreviewView. This happens if the scale type is FILL_* AND a
            // PreviewView-based viewport is used.
            RectF(
                0f, 0f, previewViewSize.width.toFloat(),
                previewViewSize.height.toFloat()
            )
        } else {
            // If the aspect ratios don't match, it could be 1) scale type is FIT_*, 2) the
            // Viewport is not based on the PreviewView or 3) both.
            getPreviewViewViewportRectForMismatchedAspectRatios(
                previewViewSize, layoutDirection
            )
        }
        val matrix = TransformUtils.getRectToRect(
            RectF(surfaceCropRect), previewViewCropRect,
            mPreviewRotationDegrees
        )
        if (mIsFrontCamera) {
            // SurfaceView/TextureView automatically mirrors the Surface for front camera, which
            // needs to be compensated by mirroring the Surface around the upright direction of the
            // output image.
            if (TransformUtils.is90or270(mPreviewRotationDegrees)) {
                // If the rotation is 90/270, the Surface should be flipped vertically.
                //   +---+     90 +---+  270 +---+
                //   | ^ | -->    | < |      | > |
                //   +---+        +---+      +---+
                matrix.preScale(
                    1f,
                    -1f,
                    surfaceCropRect!!.centerX().toFloat(),
                    surfaceCropRect!!.centerY().toFloat()
                )
            } else {
                // If the rotation is 0/180, the Surface should be flipped horizontally.
                //   +---+      0 +---+  180 +---+
                //   | ^ | -->    | ^ |      | v |
                //   +---+        +---+      +---+
                matrix.preScale(
                    -1f,
                    1f,
                    surfaceCropRect!!.centerX().toFloat(),
                    surfaceCropRect!!.centerY().toFloat()
                )
            }
        }
        return matrix
    }

    /**
     * Gets the viewport rect in [PreviewView] coordinates for the case where viewport's
     * aspect ratio doesn't match [PreviewView]'s aspect ratio.
     *
     *
     *  When aspect ratios don't match, additional calculation is needed to figure out how to
     * fit crop rect into the[PreviewView].
     */
    fun getPreviewViewViewportRectForMismatchedAspectRatios(
        previewViewSize: Size,
        layoutDirection: Int
    ): RectF {
        val previewViewRect = RectF(
            0f, 0f, previewViewSize.width.toFloat(),
            previewViewSize.height.toFloat()
        )
        val rotatedViewportSize = rotatedViewportSize
        val rotatedViewportRect = RectF(
            0f, 0f, rotatedViewportSize.width.toFloat(),
            rotatedViewportSize.height.toFloat()
        )
        val matrix = Matrix()
        setMatrixRectToRect(
            matrix, rotatedViewportRect, previewViewRect,
            scaleType
        )
        matrix.mapRect(rotatedViewportRect)
        return if (layoutDirection == LayoutDirection.RTL) {
            flipHorizontally(
                rotatedViewportRect, previewViewSize.width
                    .toFloat() / 2
            )
        } else rotatedViewportRect
    }

    /**
     * Returns viewport size with target rotation applied.
     */
    private val rotatedViewportSize: Size
        @SuppressLint("RestrictedApi")
        private get() = if (TransformUtils.is90or270(mPreviewRotationDegrees)) {
            Size(surfaceCropRect!!.height(), surfaceCropRect!!.width())
        } else Size(surfaceCropRect!!.width(), surfaceCropRect!!.height())

    /**
     * Checks if the viewport's aspect ratio matches that of the [PreviewView].
     *
     *
     *  The mismatch could happen if the [ViewPort] is not based on the
     * [PreviewView], or the [PreviewView.getScaleType] is FIT_*. In this case, we
     * need to calculate how the crop rect should be fitted.
     */
    @SuppressLint("RestrictedApi")
    @VisibleForTesting
    fun isViewportAspectRatioMatchPreviewView(previewViewSize: Size?): Boolean {
        // Using viewport rect to check if the viewport is based on the PreviewView.
        val rotatedViewportSize = rotatedViewportSize
        return TransformUtils.isAspectRatioMatchingWithRoundingError(
            previewViewSize!!,  /* isAccurate1= */true,
            rotatedViewportSize,  /* isAccurate2= */false
        )
    }

    /**
     * Creates a transformed screenshot of [PreviewView].
     *
     *
     *  Creates the transformed [Bitmap] by applying the same transformation applied to
     * the inner view. T
     *
     * @param original a snapshot of the untransformed inner view.
     */
    fun createTransformedBitmap(
        original: Bitmap, previewViewSize: Size,
        layoutDirection: Int
    ): Bitmap {
        if (!isTransformationInfoReady) {
            return original
        }
        val textureViewCorrection = textureViewCorrectionMatrix
        val surfaceRectInPreviewView = getTransformedSurfaceRect(
            previewViewSize,
            layoutDirection
        )
        val transformed = Bitmap.createBitmap(
            previewViewSize.width, previewViewSize.height, original.config
        )
        val canvas = Canvas(transformed)
        val canvasTransform = Matrix()
        canvasTransform.postConcat(textureViewCorrection)
        canvasTransform.postScale(
            surfaceRectInPreviewView.width() / mResolution!!.width,
            surfaceRectInPreviewView.height() / mResolution!!.height
        )
        canvasTransform.postTranslate(surfaceRectInPreviewView.left, surfaceRectInPreviewView.top)
        canvas.drawBitmap(
            original, canvasTransform,
            Paint(Paint.ANTI_ALIAS_FLAG or Paint.FILTER_BITMAP_FLAG or Paint.DITHER_FLAG)
        )
        return transformed
    }

    /**
     * Calculates the mapping from a UI touch point (0, 0) - (width, height) to normalized
     * space (-1, -1) - (1, 1).
     *
     *
     *  This is used by [PreviewViewMeteringPointFactory].
     *
     * @return null if transformation info is not set.
     */
    fun getPreviewViewToNormalizedSurfaceMatrix(
        previewViewSize: Size,
        layoutDirection: Int
    ): Matrix? {
        if (!isTransformationInfoReady) {
            return null
        }
        val matrix = Matrix()

        // Map PreviewView coordinates to Surface coordinates.
        getSurfaceToPreviewViewMatrix(previewViewSize, layoutDirection).invert(matrix)

        // Map Surface coordinates to normalized coordinates (-1, -1) - (1, 1).
        val normalization = Matrix()
        normalization.setRectToRect(
            RectF(0f, 0f, mResolution!!.width.toFloat(), mResolution!!.height.toFloat()),
            RectF(0f, 0f, 1f, 1f), ScaleToFit.FILL
        )
        matrix.postConcat(normalization)
        return matrix
    }

    private val isTransformationInfoReady: Boolean
        private get() = surfaceCropRect != null && mResolution != null

    companion object {
        private const val TAG = "PreviewTransform"
        private val DEFAULT_SCALE_TYPE = PreviewView.ScaleType.FILL_CENTER

        /**
         * Set the matrix that maps the source rectangle to the destination rectangle.
         *
         *
         *  This static method is an extension of [Matrix.setRectToRect] with an additional
         * support for FILL_* types.
         */
        private fun setMatrixRectToRect(
            matrix: Matrix, source: RectF, destination: RectF,
            scaleType: PreviewView.ScaleType
        ) {
            val matrixScaleType: ScaleToFit
            matrixScaleType =
                when (scaleType) {
                    PreviewView.ScaleType.FIT_CENTER, PreviewView.ScaleType.FILL_CENTER -> ScaleToFit.CENTER
                    PreviewView.ScaleType.FIT_END, PreviewView.ScaleType.FILL_END -> ScaleToFit.END
                    PreviewView.ScaleType.FIT_START, PreviewView.ScaleType.FILL_START -> ScaleToFit.START
                    else -> {
                        ScaleToFit.FILL
                    }
                }
            val isFitTypes =
                scaleType == PreviewView.ScaleType.FIT_CENTER || scaleType == PreviewView.ScaleType.FIT_START || scaleType == PreviewView.ScaleType.FIT_END
            if (isFitTypes) {
                matrix.setRectToRect(source, destination, matrixScaleType)
            } else {
                // android.graphics.Matrix doesn't support fill scale types. The workaround is
                // mapping inversely from destination to source, then invert the matrix.
                matrix.setRectToRect(destination, source, matrixScaleType)
                matrix.invert(matrix)
            }
        }

        /**
         * Flips the given rect along a vertical line for RTL layout direction.
         */
        private fun flipHorizontally(original: RectF, flipLineX: Float): RectF {
            return RectF(
                flipLineX + flipLineX - original.right,
                original.top,
                flipLineX + flipLineX - original.left,
                original.bottom
            )
        }
    }
}
