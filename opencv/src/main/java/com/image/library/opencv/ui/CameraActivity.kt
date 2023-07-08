package com.image.library.opencv.ui

import android.animation.ObjectAnimator
import android.content.Context
import android.content.Intent
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.PersistableBundle
import android.util.Log
import android.util.Size
import android.view.View
import android.view.animation.LinearInterpolator
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.TextView
import androidx.camera.core.ImageCapture
import androidx.fragment.app.FragmentActivity
import com.image.library.main.R
import com.image.library.opencv.OpenCvIn
import com.image.library.opencv.camera.CameraView
import com.image.library.opencv.camera.gl.GLUtil
import com.image.library.opencv.utils.Applications
import com.image.library.opencv.utils.FileUtils
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.File
import java.io.FileWriter
import java.io.InputStreamReader
import java.lang.Exception

//https://zhuanlan.zhihu.com/p/115248588
class CameraActivity : FragmentActivity(){
    private var imageCapture: ImageCapture?=null
    private var mAnimationForCover:ObjectAnimator?=null
    private var mLutFile:File?=null
    private var mAniamtion: ObjectAnimator?=null
    private var animationIsRunning = false
    private var clickConfirmNumber = 0
    private var mCameraSize = Size(1080,1440)
    private lateinit var vCamera:CameraView
    private lateinit var vLlAnimationBox:LinearLayout
    private lateinit var vBtnTake:View
    private lateinit var vVSwitchCamera:View
    private lateinit var vImageLoading:ImageView
    private lateinit var vVFilter:View

    companion object {
        fun start(context: Context) {
            val intent = Intent(context,CameraActivity::class.java)
            context.startActivity(intent)
        }
    }



    private fun startApplyEffectAnim(){
        if (mAniamtion != null){
            try{
                mAniamtion?.cancel()
            }catch (e: Exception){
                e.printStackTrace()
            }
        }
        vLlAnimationBox.visibility = View.VISIBLE
        mAniamtion = ObjectAnimator.ofFloat(vImageLoading,"rotation",0f,360f)
        mAniamtion?.duration = 500
        mAniamtion?.interpolator = LinearInterpolator()
        mAniamtion?.repeatCount = -1
        mAniamtion?.start()
        animationIsRunning = true
    }

    private fun stopApplyEffectAnim(){
        vLlAnimationBox.visibility = View.INVISIBLE
        try{
            mAniamtion?.cancel()
            animationIsRunning = false
        }catch (e: Exception){
            e.printStackTrace()
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try{
            vCamera.destroyRotationListener()
        }catch (e:Exception){
            e.printStackTrace()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_camera)
        GLUtil.init(Applications.context())
        vCamera = findViewById(R.id.vCamera)
        vLlAnimationBox = findViewById(R.id.vLlAnimationBox)
        vBtnTake = findViewById(R.id.vBtnTake)
        vVSwitchCamera = findViewById(R.id.vVSwitchCamera)
        vVFilter = findViewById(R.id.vVFilter)
        vImageLoading = findViewById(R.id.vImageLoading)
        initListener()
    }

    private fun initListener() {
        vBtnTake.setOnClickListener {
            clickConfirmNumber++
            startApplyEffectAnim()
            vCamera.takePhoto(mLutFile){
                clickConfirmNumber -= 1
                if (clickConfirmNumber == 0){
                    stopApplyEffectAnim()
                }
            }
        }

        vVSwitchCamera.setOnClickListener {
            vCamera.switchCamera(null)
            mAnimationForCover?.cancel()
            mAnimationForCover = ObjectAnimator.ofFloat(vCamera,"scaleX",1f,0f,1f)
            mAnimationForCover?.duration = 1000
            mAnimationForCover?.start()
        }


        vVFilter.setOnClickListener {
            /*val dialog = CameraFilterDialog( { bitmap, isFinish,filterPath ->
                mLutFile = File(filterPath)
                if (bitmap == null){
                    if (mCurrentFilter.isNullOrBlank()){
                        val bt = BitmapFactory.decodeResource(resources,R.drawable.neutral_lut)
                        vCamera.updateLutBitmap(bt)
                    }else{
                        val bt = OpenCvIn.cubeFileToPng(mCurrentFilter)
                        vCamera.updateLutBitmap(null)
                    }
                }else{e
                    vCamera.updateLutBitmap(bitmap)
                    if (isFinish){
                        mCurrentFilter = filterPath
                    }
                }
            },{
                vCamera.setLutStrength(it)
            })
            dialog.show(supportFragmentManager,"")*/
            val inputStream = resources.assets.open("赛博朋克科幻[缇米设计]-5 强度5.cube")
            val file = File(FileUtils.getFilterCacheFile().path+"/test.cube")
            mLutFile = file
            if (!file.exists()){
                CoroutineScope(Dispatchers.IO).launch {
                    val write = BufferedWriter(FileWriter(file))
                    val streanReader = InputStreamReader(inputStream)
                    val reader = BufferedReader(streanReader)
                    while (true){
                        val line = reader.readLine() ?: break
                        write.write(line+"\n")
                    }
                    write.flush()
                    reader.close()
                    streanReader.close()
                    write.close()
                    Log.i("测试日志","地址为："+file.path)
                    //将lut文件转成png
                    val bt = OpenCvIn.cubeFileToPng(file.path)
                    CoroutineScope(Dispatchers.Main).launch {
                        vCamera.updateLutBitmap(bt)
                        vCamera.setLutStrength(100)
                    }
                }
            }else{
                Log.i("测试日志","地址为："+file.path)
                //将lut文件转成png
                val bt = OpenCvIn.cubeFileToPng(file.path)
                vCamera.updateLutBitmap(bt)
                vCamera.setLutStrength(100)
            }
        }
    }

}