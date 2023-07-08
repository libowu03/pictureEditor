package com.image.library.opencv.camera.render

import android.annotation.SuppressLint
import android.content.ContentValues
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.SurfaceTexture
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Environment
import android.provider.MediaStore
import android.util.Log
import android.util.Size
import android.view.OrientationEventListener
import android.view.Surface
import androidx.annotation.WorkerThread
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import com.image.library.opencv.OpenCvIn
import com.image.library.opencv.camera.CameraController
import com.image.library.opencv.camera.filter.Camera2Filter
import com.image.library.opencv.camera.filter.ColorFilter
import com.image.library.opencv.camera.filter.MatrixUtil
import com.image.library.opencv.utils.BitmapUtil
import com.image.library.opencv.utils.FileUtils
import com.image.library.opencv.camera.filter.CameraFilter
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.*
import java.nio.ByteBuffer
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.Executors
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10
import kotlin.math.abs
import kotlin.math.max
import kotlin.math.min

class CameraRender(var context:Context, private var onFrameAvailableCallback: () -> Unit?) : GLSurfaceView.Renderer, Preview.SurfaceProvider, SurfaceTexture.OnFrameAvailableListener {
    private var textures: IntArray = IntArray(1)
    private var surfaceTexture: SurfaceTexture? = null
    private var mFilter: CameraFilter?=null
    private var imageCapture:ImageCapture?=null
    private val executor = Executors.newSingleThreadExecutor()
    private var mSwitchCameraCallback: SwitchCameraCallback?=null
    private val RATIO_4_3_VALUE = 4.0 / 3.0
    private var orientationEventListener:OrientationEventListener?=null
    private val RATIO_16_9_VALUE = 16.0 / 9.0
    var cameraController:CameraControl?=null
    var cameraController2: CameraController?=null
    var cameraXCallback: CameraXCallback?=null
    var mCameraSize = Size(1080,1440)
    var useFront = false
    var matrix = FloatArray(16)

    var takingPhoto = false
    var recordingVideo = false


    var cameraFilter: Camera2Filter? = null
    var colorFilter: ColorFilter? = null
    var width = 0
    var height:Int = 0

    var exportFrame = IntArray(1)
    var exportTexture = IntArray(1)


    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        gl?.let {
            it.glGenTextures(textures.size, textures, 0)
            surfaceTexture = SurfaceTexture(textures[0])
            if(colorFilter == null){
                cameraFilter = Camera2Filter()
                colorFilter = ColorFilter()
            }

            cameraFilter!!.onSurfaceCreated()
            colorFilter!!.onSurfaceCreated()
            matrix = MatrixUtil.flip(colorFilter!!.getMatrix(), false, true)
            colorFilter!!.setMatrix(matrix)
        }
    }


    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        if (cameraController2 == null){
            cameraX()
        }
        if (this.width != width || this.height != height) {
            this.width = width
            this.height = height
            cameraFilter!!.onSurfaceChanged(width, height)
            colorFilter!!.onSurfaceChanged(width, height)
            delFrameBufferAndTexture()
            genFrameBufferAndTexture()
        }
    }

    fun delFrameBufferAndTexture() {
        GLES20.glDeleteFramebuffers(exportFrame.size, exportFrame, 0)
        GLES20.glDeleteTextures(exportTexture.size, exportTexture, 0)
    }

    fun genFrameBufferAndTexture() {
        GLES20.glGenFramebuffers(exportFrame.size, exportFrame, 0)
        GLES20.glGenTextures(exportTexture.size, exportTexture, 0)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, exportTexture[0])
        GLES20.glTexImage2D(
            GLES20.GL_TEXTURE_2D,
            0,
            GLES20.GL_RGBA,
            width,
            height,
            0,
            GLES20.GL_RGBA,
            GLES20.GL_UNSIGNED_BYTE,
            null
        )
        setTextureParameters()
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
    }

    fun setTextureParameters() {
        GLES20.glTexParameterf(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_MIN_FILTER,
            GLES20.GL_NEAREST.toFloat()
        )
        GLES20.glTexParameterf(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_MAG_FILTER,
            GLES20.GL_LINEAR.toFloat()
        )
        GLES20.glTexParameterf(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_WRAP_S,
            GLES20.GL_CLAMP_TO_EDGE.toFloat()
        )
        GLES20.glTexParameterf(
            GLES20.GL_TEXTURE_2D,
            GLES20.GL_TEXTURE_WRAP_T,
            GLES20.GL_CLAMP_TO_EDGE.toFloat()
        )
    }

    override fun onDrawFrame(gl: GL10?) {
        if (surfaceTexture != null) {
            surfaceTexture!!.updateTexImage()
        }
        cameraFilter!!.setTextureId(textures)
        cameraFilter!!.onDraw()
        colorFilter!!.setTextureId(cameraFilter!!.outputTextureId)
        if (isTakingPhoto()) {
            val exportBuffer = ByteBuffer.allocate(width * height * 4)
            bindFrameBufferAndTexture()
            colorFilter!!.setMatrix(MatrixUtil.flip(matrix, false, true))
            colorFilter!!.onDraw()
            GLES20.glReadPixels(
                0,
                0,
                width,
                height,
                GLES20.GL_RGBA,
                GLES20.GL_UNSIGNED_BYTE,
                exportBuffer
            )
            savePhoto(exportBuffer)
            unBindFrameBuffer()
            setTakingPhotoParam(false)
            colorFilter!!.setMatrix(MatrixUtil.flip(matrix, false, true))
        } else {
            colorFilter!!.onDraw()
        }

        if (mSwitchCameraCallback != null){
            mSwitchCameraCallback?.switchDone()
            mSwitchCameraCallback = null
        }
    }

    fun setTakingPhotoParam(takingPhoto: Boolean) {
        this.takingPhoto = takingPhoto
    }

    fun isTakingPhoto(): Boolean {
        return takingPhoto
    }


    fun bindFrameBufferAndTexture() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, exportFrame[0])
        GLES20.glFramebufferTexture2D(
            GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D,
            exportTexture[0], 0
        )
    }

    fun unBindFrameBuffer() {
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0)
    }

    override fun onSurfaceRequested(request: SurfaceRequest) {
        val resetTexture = resetPreviewTexture(request.resolution) ?: return
        val surface = Surface(resetTexture)
        cameraXCallback?.onSurfaceRequested(request)
        request.provideSurface(surface, executor) {
            setUseFrontCamera(useFront)
        }
    }

    @WorkerThread
    private fun resetPreviewTexture(size: Size): SurfaceTexture? {
        return this.surfaceTexture?.let { surfaceTexture ->
            surfaceTexture.setOnFrameAvailableListener(this)
            surfaceTexture.setDefaultBufferSize(size.width, size.height)
            surfaceTexture
        }
    }


    @SuppressLint("RestrictedApi")
    fun cameraX(){
        changeCamera()
    }

    fun changeCameraFontOrBack(useFontCamera:Boolean = false, callback: SwitchCameraCallback?=null){
        if (this.mSwitchCameraCallback != null){
            this.mSwitchCameraCallback = callback
        }
        this.useFront = useFontCamera
        matrix = MatrixUtil.flip(matrix, true, false)
        changeCamera()
    }

    fun changeCameraSize(size:Size = Size(1080,1440)){
        this.mCameraSize = size
        changeCamera()
    }

    private fun changeCamera(){
        val cameraProviderFuture = ProcessCameraProvider.getInstance(context)
        cameraProviderFuture.addListener({
            val cameraProvider = cameraProviderFuture.get()
            val preview: Preview = Preview.Builder()
                 .setTargetResolution(mCameraSize)
                .build()

            imageCapture = ImageCapture.Builder()
                .setJpegQuality(100)
                .setCaptureMode(ImageCapture.CAPTURE_MODE_MAXIMIZE_QUALITY)
                .setTargetAspectRatio(aspectRatio(mCameraSize.width, mCameraSize.height))
                .build()
            preview.setSurfaceProvider(this)


            cameraProvider.unbindAll()

            val camera =
                cameraProvider.bindToLifecycle(
                    context as LifecycleOwner,
                    if (this.useFront) CameraSelector.DEFAULT_FRONT_CAMERA else CameraSelector.DEFAULT_BACK_CAMERA,
                    imageCapture,
                    preview
                )
            cameraController2 = CameraController()
            cameraController2?.setCamera(camera)
            cameraController = camera.cameraControl
        }, ContextCompat.getMainExecutor(context))
    }

    private fun aspectRatio(width: Int, height: Int): Int {
        val previewRatio = max(width, height).toDouble() / min(width, height)
        if (abs(previewRatio - RATIO_4_3_VALUE) <= abs(previewRatio - RATIO_16_9_VALUE)) {
            return AspectRatio.RATIO_4_3
        }
        return AspectRatio.RATIO_16_9
    }

    fun savePhoto(buffer: ByteBuffer?) {
        Thread(Runnable {
            val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            bitmap!!.copyPixelsFromBuffer(buffer)
            val folderPath = Environment.getExternalStorageDirectory().toString() + "/DCIM/Camera/"
            val folder = File(folderPath)
            if (!folder.exists() && !folder.mkdirs()) {
                Log.e("demos", "图片目录异常")
                return@Runnable
            }
            val filePath = folderPath + System.currentTimeMillis() + ".jpg"
            var bos: BufferedOutputStream? = null
            try {
                val fos = FileOutputStream(filePath)
                bos = BufferedOutputStream(fos)
                bitmap.compress(Bitmap.CompressFormat.JPEG, 100, bos)
                FileUtils.saveToCameraFolder(context,bitmap,100)
            } catch (e: FileNotFoundException) {
                e.printStackTrace()
            } finally {
                if (bos != null) {
                    try {
                        bos.flush()
                        bos.close()
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }
                if (bitmap != null) {
                    bitmap.recycle()
                }
            }
        }).start()
    }

    fun updateLutStrength(strength:Int){
        colorFilter?.updateLutStrength(strength)
    }


    fun takePhoto(lutFile:File?,finishCallback:()->Unit = {}) {
        val name = SimpleDateFormat("yy-mm-dd", Locale.US)
            .format(System.currentTimeMillis())
        val contentValues = ContentValues().apply {
            put(MediaStore.MediaColumns.DISPLAY_NAME, name)
            put(MediaStore.MediaColumns.MIME_TYPE, "image/jpeg")
            if(Build.VERSION.SDK_INT > Build.VERSION_CODES.P) {
                put(MediaStore.Images.Media.RELATIVE_PATH, "Pictures/CameraX-Image")
            }
        }

        orientationEventListener = object : OrientationEventListener(context) {
            override fun onOrientationChanged(orientation : Int) {
                // Monitors orientation values to determine the target rotation value
                val rotation : Int = when (orientation) {
                    in 45..134 -> Surface.ROTATION_270
                    in 135..224 -> Surface.ROTATION_180
                    in 225..314 -> Surface.ROTATION_90
                    else -> Surface.ROTATION_0
                }
                imageCapture?.targetRotation = rotation
            }
        }
        orientationEventListener?.enable()

        val outputOptions = ImageCapture.OutputFileOptions
            .Builder(context.contentResolver, MediaStore.Images.Media.EXTERNAL_CONTENT_URI, contentValues)
            .build()
//        setTakingPhotoParam(true)
        imageCapture?.takePicture(ContextCompat.getMainExecutor(context),object : ImageCapture.OnImageCapturedCallback(){
            @SuppressLint("UnsafeOptInUsageError")
            override fun onCaptureSuccess(image: ImageProxy) {
                super.onCaptureSuccess(image)
                if (image.image == null || image.planes.isEmpty()){
                    image.close()
                    return
                }
                Log.i("测试日志","${image.format}")
                CoroutineScope(Dispatchers.IO).launch {
                    var bitmapS = BitmapUtil.jpegImageToJpegByteArray(image)
                    val bt = BitmapFactory.decodeByteArray(bitmapS,0,bitmapS.size)
                    var resultBitmap = OpenCvIn.changeBy3dLut(bt,lutFile?.path,(colorFilter?.lutStrength?:100f)/100f )
                    FileUtils.saveToCameraFolder(context,resultBitmap,90)
                    bt.recycle()
                    CoroutineScope(Dispatchers.Main).launch {
                        finishCallback.invoke()
                        image.close()
                    }
                }
            }

            override fun onError(exception: ImageCaptureException) {
                super.onError(exception)
            }
        })
    }

    /**
     * 设置闪光灯模式
     */
    fun setFlashModel(model:Int){
        imageCapture?.flashMode = model
    }

    fun destroyRotationListener(){
        orientationEventListener?.disable()
    }

    fun setUseFrontCamera(useFront: Boolean) {
        this.useFront = useFront
        cameraFilter!!.setUseCameraFront(useFront)
    }

    /**
     * 设置色温
     */
    fun setProgress(progress: Float) {
        mFilter?.temperature = progress
    }

    /**
     * 设置色调
     */
    fun setTint(progress: Float) {
        mFilter?.tint = progress
    }

    fun updateLutBitmap(lutBitmap: Bitmap?){
        colorFilter?.updateLutBitmap(lutBitmap)
    }

    override fun onFrameAvailable(surfaceTexture: SurfaceTexture?) {
        onFrameAvailableCallback.invoke()
    }

    interface CameraXCallback {
        fun onSurfaceRequested(request: SurfaceRequest)
    }

    interface SwitchCameraCallback{
        fun switchDone()
    }
}