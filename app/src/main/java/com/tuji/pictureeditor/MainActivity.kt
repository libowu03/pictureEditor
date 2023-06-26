package com.tuji.pictureeditor

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Canvas
import android.graphics.Color
import android.media.MediaParser.InputReader
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.ImageView
import android.widget.RadioGroup
import android.widget.SeekBar
import androidx.core.widget.NestedScrollView
import com.image.library.opencv.OpenCvIn
import com.image.library.opencv.bean.CurveData
import com.image.library.opencv.bean.MixColorChannel
import com.image.library.opencv.view.CurveLineToolsView
import com.image.library.opencv.view.ImageHistogram
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.File
import java.io.FileWriter
import java.io.InputStreamReader

class MainActivity : AppCompatActivity() {
    private var mChannel: Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val image = findViewById<ImageView>(R.id.vImageCover)
        val bitmap = BitmapFactory.decodeResource(resources,R.mipmap.yu)
        image.setImageBitmap(bitmap)
        val bitmapMask = Bitmap.createBitmap(bitmap.width,bitmap.height,Bitmap.Config.ARGB_8888)
        val can = Canvas(bitmapMask)
        can.drawColor(Color.WHITE)
        findViewById<Button>(R.id.vBtn1).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //像素化
                val result = OpenCvIn.makeBitmapToPixel(bitmap,bitmapMask,5)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn2).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //反色
                val result = OpenCvIn.makeBitmapColorCounter(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn3).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //去雾
                val result = OpenCvIn.makeBitmapRemoveFog(bitmap,bitmapMask,0.5f)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn4).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //灰度
                val result = OpenCvIn.makeBitmapGray(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn5).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //高斯
                val result = OpenCvIn.makeBitmapBlur(bitmap,bitmapMask,10)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn6).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //颗粒
                val result = OpenCvIn.makeBitmapParticles(bitmap,bitmapMask,40)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn7).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //黑白
                val result = OpenCvIn.makeBitmapWhiteAndBlack(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn8).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //黑白
                val result = OpenCvIn.makeBitmapNostalgia(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn9).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //熔铸
                val result = OpenCvIn.makeBitmapFounding(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn10).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //冰冻滤镜
                val result = OpenCvIn.makeBitmapFrozen(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn13).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //描边滤镜
                val result = OpenCvIn.makeBitmapEdge(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn14).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //膨胀滤镜
                val result = OpenCvIn.makeBitmapDilate(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn15).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //连环画滤镜
                val result = OpenCvIn.makeBitmapComics(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn16).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //侵蚀滤镜
                val result = OpenCvIn.makeBitmapErode(bitmap)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn17).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val lutInput = resources.assets.open("蓝调街拍-益墨素材10.cube")
                val bu = BufferedReader(InputStreamReader(lutInput))
                val lutStr = StringBuffer()
                while (true){
                    val line = bu.readLine()
                    if (line == null || line.isEmpty()){
                        break
                    }
                    lutStr.append(line+"\n")
                }
                lutInput.close()
                bu.close()
                val fi = File(externalCacheDir!!.absolutePath+"/lut")
                if (!fi.exists()){
                    fi.mkdirs()
                }
                val lutFile = File(fi.absolutePath+"/lut.cube")
                val fileWrite = FileWriter(lutFile)
                val writer = BufferedWriter(fileWrite)
                writer.write(lutStr.toString())
                writer.flush()
                fileWrite.close()
                writer.close()

                //Lut滤镜
                val result = OpenCvIn.makeBitmapChangeByLut(bitmap,lutFile.absolutePath)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtnApplyUsm).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //usm锐化
                val result = OpenCvIn.usmFilter(bitmap,bitmapMask,findViewById<SeekBar>(R.id.vSeekbarStrength).progress,findViewById<SeekBar>(R.id.vSeekbarRatio).progress,findViewById<SeekBar>(R.id.vSeekbarTh).progress)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtnApplyMixChannel).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //通道混合器
                val mixChannel = MixColorChannel()
                mixChannel.redChannelRed = (findViewById<SeekBar>(R.id.vSeekbarMixChannelRedRed).progress - 200)/100f
                mixChannel.redChannelGreen = (findViewById<SeekBar>(R.id.vSeekbarMixChannelRedGreen).progress - 200)/100f
                mixChannel.redChannelBlue = (findViewById<SeekBar>(R.id.vSeekbarMixChannelRedBlue).progress - 200)/100f
                mixChannel.greenChannelRed = (findViewById<SeekBar>(R.id.vSeekbarMixChannelGreenRed).progress - 200)/100f
                mixChannel.greenChannelGreen = (findViewById<SeekBar>(R.id.vSeekbarMixChannelGreenGreen).progress - 200)/100f
                mixChannel.greenChannelBlue = (findViewById<SeekBar>(R.id.vSeekbarMixChannelGreenBlue).progress - 200)/100f
                mixChannel.blueChannelRed = (findViewById<SeekBar>(R.id.vSeekbarMixChannelBlueRed).progress - 200)/100f
                mixChannel.blueChannelGreen = (findViewById<SeekBar>(R.id.vSeekbarMixChannelBlueGreen).progress - 200)/100f
                mixChannel.blueChannelBlue = (findViewById<SeekBar>(R.id.vSeekbarMixChannelBlueBlue).progress - 200)/100f

                //通道混合器
                val result = OpenCvIn.mixColorChannel(bitmap,bitmapMask,mixChannel)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        CoroutineScope(Dispatchers.IO).launch {
            //曲线工具
            var mCurveMap = HashMap<Int, CurveData>()
            mCurveMap[0] = CurveData(doubleArrayOf(0.0, 255.0), doubleArrayOf(0.0, 255.0), 0)
            mCurveMap[1] = CurveData(doubleArrayOf(0.0, 255.0), doubleArrayOf(0.0, 255.0), 1)
            mCurveMap[2] = CurveData(doubleArrayOf(0.0, 255.0), doubleArrayOf(0.0, 255.0), 2)
            mCurveMap[3] = CurveData(doubleArrayOf(0.0, 255.0), doubleArrayOf(0.0, 255.0), 3)

            val bt = OpenCvIn.getImageChannelInfo(bitmap)
            runOnUiThread {
                findViewById<ImageHistogram>(R.id.vHistogram)?.setHistogramInfo(bt, true, true, true, true)
                val curveView = findViewById<CurveLineToolsView>(R.id.vCurve);
                findViewById<NestedScrollView>(R.id.vScoreView).isNestedScrollingEnabled = isFinishing
                curveView?.setCurveCallback { x, y, point, isStart, isFinish, isClick ->

                    if (mCurveMap[mChannel] != null) {
                        mCurveMap[mChannel]?.xArray = x.toDoubleArray()
                        mCurveMap[mChannel]?.yArray = y.toDoubleArray()
                    } else {
                        mCurveMap[mChannel] = CurveData(x.toDoubleArray(), y.toDoubleArray(), mChannel)
                    }
                    curveView.setEnableCreate(false)
                }
                curveView.chooseControllerPointListener = { pointIndex, point ->

                }
                findViewById<Button>(R.id.vBtnApplyCurve).setOnClickListener {
                    val result = OpenCvIn.curveToolsPro(
                        bitmap,
                        bitmapMask,
                        mCurveMap[1]!!.xArray,
                        mCurveMap[1]!!.yArray,
                        mCurveMap[2]!!.xArray,
                        mCurveMap[2]!!.yArray,
                        mCurveMap[3]!!.xArray,
                        mCurveMap[3]!!.yArray,
                        mCurveMap[0]!!.xArray,
                        mCurveMap[0]!!.yArray
                    )
                    image.setImageBitmap(result)
                }

                findViewById<RadioGroup>(R.id.vRgColorTab).setOnCheckedChangeListener { radioGroup, i ->
                    when (i) {
                        R.id.vRbRGB -> {
                            mChannel = 0
                        }
                        R.id.vRbRed -> {
                            mChannel = 1
                        }
                        R.id.vRbGreen -> {
                            mChannel = 2
                        }
                        R.id.vRbBlue -> {
                            mChannel = 3
                        }
                    }
                    mCurveMap[mChannel]?.let {
                        curveView.setPointList(it.xArray, it.yArray)
                    } ?: let {
                        curveView.reset()
                    }
                    curveView.setEnableCreate(true)
                }
            }
        }


    }
}