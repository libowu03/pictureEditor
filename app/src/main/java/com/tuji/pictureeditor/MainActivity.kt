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
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.core.widget.NestedScrollView
import com.image.library.opencv.OpenCvIn
import com.image.library.opencv.bean.ChangeColorDataBean
import com.image.library.opencv.bean.ColorBalance
import com.image.library.opencv.bean.CurveData
import com.image.library.opencv.bean.HslBean
import com.image.library.opencv.bean.MixColorChannel
import com.image.library.opencv.en.ColorChannelEnum
import com.image.library.opencv.ui.CameraActivity
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
    private var mChangeColorDataBean: ChangeColorDataBean = ChangeColorDataBean()
    private var mChannelType: ColorChannelEnum = ColorChannelEnum.COLOR_RED
    private var mIsSelectColorWidthModel = false
    private var mHhlData:HslBean = HslBean()
    private var mSaturationData: HslBean = HslBean()
    private var mLightData: HslBean = HslBean()
    private var mHueData: HslBean = HslBean()
    private var mColorPro: ChangeColorDataBean = ChangeColorDataBean()
    private var mColorMotely: ChangeColorDataBean = ChangeColorDataBean()
    private var mColorBalance = ColorBalance()
    private var usmOutlinePro = BaseSettingDataBean()
    private var usmDetailPro = BaseSettingDataBean()



    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val image = findViewById<ImageView>(R.id.vImageCover)
        val bitmap = BitmapFactory.decodeResource(resources,R.mipmap.test)
        image.setImageBitmap(bitmap)
        val bitmapMask = Bitmap.createBitmap(bitmap.width,bitmap.height,Bitmap.Config.ARGB_8888)
        val can = Canvas(bitmapMask)
        can.drawColor(Color.WHITE)
        findViewById<ScrollSelfView>(R.id.vScoreView).childV = findViewById(R.id.vCurve)
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
        findViewById<Button>(R.id.vBtn12).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //水平翻转
                val result = OpenCvIn.getRotationXY(bitmap,true)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn18).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //垂直翻转
                val result = OpenCvIn.getRotationXY(bitmap,false)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
        findViewById<Button>(R.id.vBtn19).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                //尺寸调整
                val result = OpenCvIn.imageResize(bitmap,0.4,0.4);
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
        findViewById<Button>(R.id.vBtn11).setOnClickListener {
            //自动白平衡
            CoroutineScope(Dispatchers.IO).launch {
                val bt =OpenCvIn.autoColorBalance(bitmap)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(bt)
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

        //==========颜色选择器==========
        findViewById<SeekBar>(R.id.vSeekBarCyan).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorSelectData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vSeekBarCarmine).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorSelectData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vSeekBarYellow).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorSelectData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<RadioGroup>(R.id.vRgSelectColorTab).setOnCheckedChangeListener { group, checkedId ->
            when(checkedId){
                R.id.vRbSelectRed -> {
                    mChannelType = ColorChannelEnum.COLOR_RED
                    setSeekbarData(
                        (mChangeColorDataBean.redRedValue * 100).toInt(),
                        (mChangeColorDataBean.redGreenValue * 100).toInt(),
                        (mChangeColorDataBean.redBlueValue * 100).toInt(),
                        (mChangeColorDataBean.redBlackValue * 100).toInt()
                    )
                }
                R.id.vRbSelectYellow -> {
                    mChannelType = ColorChannelEnum.COLOR_YELLOW
                    setSeekbarData(
                        (mChangeColorDataBean.yellowGreenValue * 100).toInt(),
                        (mChangeColorDataBean.yellowRedValue * 100).toInt(),
                        (mChangeColorDataBean.yellowBlueValue * 100).toInt(),
                        (mChangeColorDataBean.yellowBlackValue * 100).toInt()
                    )
                }
                R.id.vRbSelectGreen -> {
                    mChannelType = ColorChannelEnum.COLOR_GREEN
                    setSeekbarData(
                        (mChangeColorDataBean.greenRedValue * 100).toInt(),
                        (mChangeColorDataBean.greenGreenValue * 100).toInt(),
                        (mChangeColorDataBean.greenBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.greenBlackValue * 100 ).toInt()
                    )
                }
                R.id.vRbSelectCyan -> {
                    mChannelType = ColorChannelEnum.COLOR_CYAN
                    setSeekbarData(
                        (mChangeColorDataBean.cyanRedValue * 100 ).toInt(),
                        (mChangeColorDataBean.cyanGreenValue * 100 ).toInt(),
                        (mChangeColorDataBean.cyanBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.cyanBlackValue * 100 ).toInt()
                    )
                }
                R.id.vRbSelectBlue -> {
                    mChannelType = ColorChannelEnum.COLOR_BLUE
                    setSeekbarData(
                        (mChangeColorDataBean.blueRedValue * 100 ).toInt(),
                        (mChangeColorDataBean.blueGreenValue * 100 ).toInt(),
                        (mChangeColorDataBean.blueBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.blueBlackValue * 100 ).toInt()
                    )
                }
                R.id.vRbSelectCarmine -> {
                    mChannelType = ColorChannelEnum.COLOR_CARMINE
                    setSeekbarData(
                        (mChangeColorDataBean.carmineRedValue * 100 ).toInt(),
                        (mChangeColorDataBean.carmineGreenValue * 100 ).toInt(),
                        (mChangeColorDataBean.carmineBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.carmineBlackValue * 100 ).toInt()
                    )
                }
                R.id.vRbSelectWhite -> {
                    mChannelType = ColorChannelEnum.COLOR_WHITE
                    setSeekbarData(
                        (mChangeColorDataBean.whiteRedValue * 100 ).toInt(),
                        (mChangeColorDataBean.whiteGreenValue * 100 ).toInt(),
                        (mChangeColorDataBean.whiteBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.whiteBlackValue * 100 ).toInt()
                    )
                }
                R.id.vRbSelectBlack -> {
                    mChannelType = ColorChannelEnum.COLOR_BLACK
                    setSeekbarData(
                        (mChangeColorDataBean.blackRedValue * 100 ).toInt(),
                        (mChangeColorDataBean.blackGreenValue * 100 ).toInt(),
                        (mChangeColorDataBean.blackBlueValue * 100 ).toInt(),
                        (mChangeColorDataBean.blackBlackValue * 100 ).toInt()
                    )
                }
            }
        }
        findViewById<Button>(R.id.vBtnApplySelectColor).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val currentBitmap = OpenCvIn.changeColorByChannelPro(
                    bitmap,bitmapMask,
                    mChangeColorDataBean.redRedValue,
                    mChangeColorDataBean.redGreenValue,
                    mChangeColorDataBean.redBlueValue,
                    mChangeColorDataBean.redBlackValue,

                    mChangeColorDataBean.greenRedValue,
                    mChangeColorDataBean.greenGreenValue,
                    mChangeColorDataBean.blueBlueValue,
                    mChangeColorDataBean.blueBlackValue,

                    mChangeColorDataBean.blueRedValue,
                    mChangeColorDataBean.blueGreenValue,
                    mChangeColorDataBean.blueBlueValue,
                    mChangeColorDataBean.blueBlackValue,

                    mChangeColorDataBean.cyanRedValue,
                    mChangeColorDataBean.cyanGreenValue,
                    mChangeColorDataBean.cyanBlueValue,
                    mChangeColorDataBean.cyanBlackValue,

                    mChangeColorDataBean.yellowRedValue,
                    mChangeColorDataBean.yellowGreenValue,
                    mChangeColorDataBean.yellowBlueValue,
                    mChangeColorDataBean.yellowBlackValue,

                    mChangeColorDataBean.carmineRedValue,
                    mChangeColorDataBean.carmineGreenValue,
                    mChangeColorDataBean.carmineBlueValue,
                    mChangeColorDataBean.carmineBlackValue,

                    mChangeColorDataBean.whiteRedValue,
                    mChangeColorDataBean.whiteGreenValue,
                    mChangeColorDataBean.whiteBlueValue,
                    mChangeColorDataBean.whiteBlackValue,

                    mChangeColorDataBean.blackRedValue,
                    mChangeColorDataBean.blackGreenValue,
                    mChangeColorDataBean.blackBlueValue,
                    mChangeColorDataBean.blackBlackValue,

                    mIsSelectColorWidthModel
                )
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
                }
            }
        }
        findViewById<Button>(R.id.vBtnColorModel).setOnClickListener {
            findViewById<Button>(R.id.vBtnColorModel).text = if (!mIsSelectColorWidthModel) "绝对模式" else "相对模式"
            mIsSelectColorWidthModel = !mIsSelectColorWidthModel
        }

        //==========HSL==========
        findViewById<RadioGroup>(R.id.vRgColorHslTab).setOnCheckedChangeListener { group, checkedId ->
            chooseHslTab()
        }
        findViewById<SeekBar>(R.id.vColorSeekbarH).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setHslData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vColorSeekbarS).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setHslData()

            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vColorSeekbarL).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setHslData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyHslColor).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val mCurrentBitmap= OpenCvIn.hsl(bitmap,bitmapMask,
                    mHhlData.redSValue,mHhlData.redHValue,mHhlData.redLValue,
                    mHhlData.greenSValue,mHhlData.greenHValue,mHhlData.greenLValue,
                    mHhlData.blueSValue,mHhlData.blueHValue,mHhlData.blueLValue,
                    mHhlData.cyanSValue,mHhlData.cyanHValue,mHhlData.cyanLValue,
                    mHhlData.carmineSValue,mHhlData.carmineHValue,mHhlData.carmineLValue,
                    mHhlData.yellowSValue,mHhlData.yellowHValue,mHhlData.yellowLValue,
                    mHhlData.orangeSValue,mHhlData.orangeHValue,mHhlData.orangeLValue,
                    mHhlData.purpleSValue,mHhlData.purpleHValue,mHhlData.purpleLValue,)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
                }
            }
        }

        //=========饱和度========
        findViewById<RadioGroup>(R.id.vRgColorSaturationTab).setOnCheckedChangeListener { group, checkedId ->
            when(checkedId){
                R.id.vRbSaturationRed -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.redSValue*100).toInt()
                }
                R.id.vRbSaturationYellow -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.yellowSValue*100).toInt()

                }
                R.id.vRbSaturationGreen -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.greenSValue*100).toInt()

                }
                R.id.vRbSaturationCyan -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.cyanSValue*100).toInt()

                }
                R.id.vRbSaturationBlue -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.blueSValue*100).toInt()

                }
                R.id.vRbSaturationCarmine -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.carmineSValue*100).toInt()

                }
                R.id.vRbSaturationRGB -> {
                    findViewById<SeekBar>(R.id.vSeekSaturability).progress = (mSaturationData.rgbSValue*100).toInt()

                }
            }
        }
        findViewById<SeekBar>(R.id.vSeekSaturability).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setSaturationData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplySaturationColor).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val tmp = OpenCvIn.changeSaturationTwo(bitmap,bitmapMask,mSaturationData.rgbSValue)
                val mCurrentBitmap = OpenCvIn.startChangeHsbS(tmp,bitmapMask,mSaturationData.redSValue,mSaturationData.greenSValue,mSaturationData.blueSValue,mSaturationData.cyanSValue,mSaturationData.yellowSValue,mSaturationData.carmineSValue)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
                }
            }
        }

        //=========明度========
        findViewById<RadioGroup>(R.id.vRgColorLightTab).setOnCheckedChangeListener { group, checkedId ->
            when(checkedId){
                R.id.vRbLightRed -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.redSValue*100).toInt()
                }
                R.id.vRbLightYellow -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.yellowSValue*100).toInt()
                }
                R.id.vRbLightGreen -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.greenSValue*100).toInt()
                }
                R.id.vRbLightCyan -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.cyanSValue*100).toInt()
                }
                R.id.vRbLightBlue -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.blueSValue*100).toInt()
                }
                R.id.vRbLightCarmine -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.carmineSValue*100).toInt()
                }
                R.id.vRbLightRGB -> {
                    findViewById<SeekBar>(R.id.vSeekLight).progress = (mLightData.rgbSValue*100).toInt()

                }
            }
        }
        findViewById<SeekBar>(R.id.vSeekLight).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setLightData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyLightColor).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val tmp = OpenCvIn.changeLight(bitmap,bitmapMask,mLightData.rgbLValue)
                val mCurrentBitmap = OpenCvIn.changeLightByChannel(tmp,bitmapMask,mLightData.redLValue,mLightData.greenLValue,mLightData.blueLValue,mLightData.cyanLValue,mLightData.yellowLValue,mLightData.carmineLValue)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
                }
            }
        }

        //=========色相========
        findViewById<RadioGroup>(R.id.vRgColorHueTab).setOnCheckedChangeListener { group, checkedId ->
            when(checkedId){
                R.id.vRbHueRed -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.redHValue*100).toInt()
                }
                R.id.vRbHueYellow -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.yellowHValue*100).toInt()
                }
                R.id.vRbHueGreen -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.greenHValue*100).toInt()
                }
                R.id.vRbHueCyan -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.cyanHValue*100).toInt()
                }
                R.id.vRbHueBlue -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.blueHValue*100).toInt()
                }
                R.id.vRbHueCarmine -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.carmineHValue*100).toInt()
                }
                R.id.vRbHueRGB -> {
                    findViewById<SeekBar>(R.id.vSeekHue).progress = (mHueData.rgbHValue*100).toInt()

                }
            }
        }
        findViewById<SeekBar>(R.id.vSeekHue).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setHueData()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyHueColor).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val tmp = OpenCvIn.changeHue(bitmap,bitmapMask,mHueData.rgbHValue*180)
                val mCurrentBitmap = OpenCvIn.startChangeHsbH(tmp,bitmapMask,mHueData.redHValue,mHueData.greenHValue,mHueData.blueHValue,mHueData.cyanHValue,mHueData.yellowHValue,mHueData.carmineHValue)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
                }
            }
        }

        //========去除杂色=========
        findViewById<Button>(R.id.vBtnApplyRemoveColorMotely).setOnClickListener {
            mColorMotely.redRedValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.yellowRedValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.yellowGreenValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.greenGreenValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.cyanGreenValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.cyanBlueValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.blueBlueValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.carmineRedValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            mColorMotely.carmineBlueValue = -(findViewById<SeekBar>(R.id.vSeekColorMotely).progress/100.0f)
            val mCurrentBitmap = OpenCvIn.changeColorByChannelPro(
                bitmap,bitmapMask,
                mColorMotely.redRedValue,
                mColorMotely.redGreenValue,
                mColorMotely.redBlueValue,
                mColorMotely.redBlackValue,
                mColorMotely.greenRedValue,
                mColorMotely.greenGreenValue,
                mColorMotely.blueBlueValue,
                mColorMotely.blueBlackValue,
                mColorMotely.blueRedValue,
                mColorMotely.blueGreenValue,
                mColorMotely.blueBlueValue,
                mColorMotely.blueBlackValue,
                mColorMotely.cyanRedValue,
                mColorMotely.cyanGreenValue,
                mColorMotely.cyanBlueValue,
                mColorMotely.cyanBlackValue,
                mColorMotely.yellowRedValue,
                mColorMotely.yellowGreenValue,
                mColorMotely.yellowBlueValue,
                mColorMotely.yellowBlackValue,
                mColorMotely.carmineRedValue,
                mColorMotely.carmineGreenValue,
                mColorMotely.carmineBlueValue,
                mColorMotely.carmineBlackValue,
                mColorMotely.whiteRedValue,
                mColorMotely.whiteGreenValue,
                mColorMotely.whiteBlueValue,
                mColorMotely.whiteBlackValue,
                mColorMotely.blackRedValue,
                mColorMotely.blackGreenValue,
                mColorMotely.blackBlueValue,
                mColorMotely.blackBlackValue,
                mColorMotely.isRelative
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
            }
        }

        //=========颜色增强========
        findViewById<Button>(R.id.vBtnApplyColorPro).setOnClickListener {
            mColorPro.redGreenValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.redBlueValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.yellowBlueValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.greenRedValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.greenBlueValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.cyanRedValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.blueRedValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.blueGreenValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            mColorPro.carmineGreenValue = findViewById<SeekBar>(R.id.vSeekColorPro).progress/100f
            val mCurrentBitmap = OpenCvIn.changeColorByChannelPro(
                bitmap,bitmapMask,
                mColorPro.redRedValue,
                mColorPro.redGreenValue,
                mColorPro.redBlueValue,
                mColorPro.redBlackValue,
                mColorPro.greenRedValue,
                mColorPro.greenGreenValue,
                mColorPro.blueBlueValue,
                mColorPro.blueBlackValue,
                mColorPro.blueRedValue,
                mColorPro.blueGreenValue,
                mColorPro.blueBlueValue,
                mColorPro.blueBlackValue,
                mColorPro.cyanRedValue,
                mColorPro.cyanGreenValue,
                mColorPro.cyanBlueValue,
                mColorPro.cyanBlackValue,
                mColorPro.yellowRedValue,
                mColorPro.yellowGreenValue,
                mColorPro.yellowBlueValue,
                mColorPro.yellowBlackValue,
                mColorPro.carmineRedValue,
                mColorPro.carmineGreenValue,
                mColorPro.carmineBlueValue,
                mColorPro.carmineBlackValue,
                mColorPro.whiteRedValue,
                mColorPro.whiteGreenValue,
                mColorPro.whiteBlueValue,
                mColorPro.whiteBlackValue,
                mColorPro.blackRedValue,
                mColorPro.blackGreenValue,
                mColorPro.blackBlueValue,
                mColorPro.blackBlackValue,
                mColorPro.isRelative
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
            }
        }

        //=========色彩平衡========
        val vSeekBarHue = findViewById<SeekBar>(R.id.vSeekBarColorBalanceR)
        val vSeekBarSaturation = findViewById<SeekBar>(R.id.vSeekBarColorBalanceG)
        val vSeekBarBrightness = findViewById<SeekBar>(R.id.vSeekBarColorBalanceB)
        findViewById<RadioGroup>(R.id.vRgColorBalanceTab).setOnCheckedChangeListener { radioGroup, i ->
            when(i){
                R.id.vRbShadow -> {
                    mChannel = 0
                    //rgb
                    vSeekBarHue.progress = (mColorBalance.cyanAndRedLow )
                    vSeekBarSaturation.progress = (mColorBalance.carmineAndGreenLow)
                    vSeekBarBrightness.progress = (mColorBalance.yellowAndBlueLow)
                }
                R.id.vRbCenter -> {
                    mChannel = 1
                    //红色
                    vSeekBarHue.progress = (mColorBalance.cyanAndRedMiddle)
                    vSeekBarSaturation.progress = (mColorBalance.carmineAndGreenMiddle)
                    vSeekBarBrightness.progress = (mColorBalance.yellowAndBlueMiddle )
                }
                R.id.vRbHightLight -> {
                    mChannel = 2
                    vSeekBarHue.progress = (mColorBalance.cyanAndRedHight)
                    vSeekBarSaturation.progress = (mColorBalance.carmineAndGreenHight)
                    vSeekBarBrightness.progress = (mColorBalance.yellowAndBlueHight)
                }
            }
            setColorBalanceData((vSeekBarHue.progress).toInt(),(vSeekBarSaturation.progress ).toInt(),(vSeekBarBrightness.progress ).toInt())
        }
        findViewById<SeekBar>(R.id.vSeekBarColorBalanceR).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorBalanceData(vSeekBarHue.progress,vSeekBarSaturation.progress,vSeekBarBrightness.progress)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vSeekBarColorBalanceG).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorBalanceData(vSeekBarHue.progress,vSeekBarSaturation.progress,vSeekBarBrightness.progress)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<SeekBar>(R.id.vSeekBarColorBalanceB).setOnSeekBarChangeListener(object:OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                setColorBalanceData(vSeekBarHue.progress,vSeekBarSaturation.progress,vSeekBarBrightness.progress)
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyRemoveColorBalance).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val mCurrentBitmap = OpenCvIn.changeColorBalance(bitmap,bitmapMask,
                    mColorBalance.cyanAndRedLow,mColorBalance.carmineAndGreenLow,mColorBalance.yellowAndBlueLow,
                    mColorBalance.cyanAndRedMiddle,mColorBalance.carmineAndGreenMiddle,mColorBalance.yellowAndBlueMiddle,
                    mColorBalance.cyanAndRedHight,mColorBalance.carmineAndGreenHight,mColorBalance.yellowAndBlueHight)
                runOnUiThread {
                    findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
                }
            }
        }

        //=========轮廓增强========
        findViewById<SeekBar>(R.id.vSeekOutline).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                usmOutlinePro.dunkNumber = progress/100.0*50
                usmOutlinePro.dunkRadio = 50.0
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyOutline).setOnClickListener {
            val mCurrentBitmap = OpenCvIn.usmFilter(
                bitmap!!,bitmapMask,
                usmOutlinePro.dunkRadio.toInt(),
                usmOutlinePro.dunkNumber.toInt(),
                usmOutlinePro.dunkThreshold.toInt()
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
            }
        }

        //=========细节增强========
        findViewById<SeekBar>(R.id.vSeekUsmDetail).setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                usmDetailPro.dunkNumber = progress/100.0*50
                usmDetailPro.dunkRadio = 1.5
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {

            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {

            }
        })
        findViewById<Button>(R.id.vBtnApplyUsmDetail).setOnClickListener {
            val mCurrentBitmap = OpenCvIn.usmFilter(
                bitmap!!,bitmapMask,
                usmDetailPro.dunkRadio.toInt(),
                usmDetailPro.dunkNumber.toInt(),
                usmDetailPro.dunkThreshold.toInt()
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(mCurrentBitmap)
            }
        }

        //对比度
        findViewById<Button>(R.id.vBtnApplyContrast).setOnClickListener {
            val currentBitmap = OpenCvIn.brinessAndContrastChange(
                bitmap!!,bitmapMask,
                0,
                findViewById<SeekBar>(R.id.vSeekContrast).progress
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //亮度
        findViewById<Button>(R.id.vBtnApplyBriness).setOnClickListener {
            val currentBitmap = OpenCvIn.brinessAndContrastChange(
                bitmap!!,bitmapMask,
                findViewById<SeekBar>(R.id.vSeekBriness).progress,
                0
            )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //高光阴影
        findViewById<Button>(R.id.vBtnApplyHightLightAndShadow).setOnClickListener {
            val currentBitmap = OpenCvIn.HighAndDarkLightChange(bitmap,bitmapMask,findViewById<SeekBar>(R.id.vSeekShadow).progress/100f,findViewById<SeekBar>(R.id.vSeekHightLight).progress/100f)
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //色温
        findViewById<Button>(R.id.vBtnApplyColorTemperature).setOnClickListener {
            val currentBitmap = OpenCvIn.changeTemp(bitmap, bitmapMask,findViewById<SeekBar>(R.id.vSeekColorTemperature).progress)
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //色调
        findViewById<Button>(R.id.vBtnApplyTone).setOnClickListener {
            val currentBitmap = OpenCvIn.changeColorHue(bitmap, bitmapMask,findViewById<SeekBar>(R.id.vSeekTone).progress)
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //曝光
        findViewById<Button>(R.id.vBtnApplyExposure).setOnClickListener {
            var currentBitmap:Bitmap?=null
            var b = findViewById<SeekBar>(R.id.vSeekExposure).progress
            if (findViewById<SeekBar>(R.id.vSeekExposure).progress < 0){
                currentBitmap =OpenCvIn.curveToolsPro(bitmap,bitmapMask,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(-125*(b/100.0),255.0),doubleArrayOf(0.0,255.0 + 185*(b/100.0))
                )
            }else if (findViewById<SeekBar>(R.id.vSeekExposure).progress > 0){
                currentBitmap =OpenCvIn.curveToolsPro(bitmap,bitmapMask,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0 - 185*(b/100.0)),doubleArrayOf(185*(b/100.0),255.0)
                )
            }else{
                currentBitmap =OpenCvIn.curveToolsPro(bitmap,bitmapMask,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0) ,
                    doubleArrayOf(0.0,255.0),doubleArrayOf(0.0,255.0)
                )
            }
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        findViewById<Button>(R.id.vBtn20).setOnClickListener {
            CameraActivity.start(this)
        }

        //白色暗角
        findViewById<Button>(R.id.vBtnApplyWhiteDarkCorner).setOnClickListener {
            var maskBitmap = BitmapFactory.decodeResource(resources, com.image.library.main.R.drawable.bg_dark_corner)
            maskBitmap = OpenCvIn.imageResize(maskBitmap,bitmap.width/(maskBitmap!!.width*1.0) ,bitmap.height/(maskBitmap!!.height*1.0))
            val currentBitmap = OpenCvIn.darkCorner(bitmap,maskBitmap,false, (findViewById<SeekBar>(R.id.vSeekWhiteDarkCorner)).progress/100f )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

        //黑色暗角
        findViewById<Button>(R.id.vBtnApplyBlackDarkCorner).setOnClickListener {
            var maskBitmap = BitmapFactory.decodeResource(resources, com.image.library.main.R.drawable.bg_dark_corner)
            maskBitmap = OpenCvIn.imageResize(maskBitmap,bitmap.width/(maskBitmap!!.width*1.0) ,bitmap.height/(maskBitmap!!.height*1.0))
            val currentBitmap = OpenCvIn.darkCorner(bitmap,maskBitmap,true, (findViewById<SeekBar>(R.id.vSeekBlackDarkCorner)).progress/100f )
            runOnUiThread {
                findViewById<ImageView>(R.id.vImageCover).setImageBitmap(currentBitmap)
            }
        }

    }

    fun setColorBalanceData(h:Int,s:Int,b:Int){
        when(mChannel){
            0 -> {
                //rgb
                mColorBalance.cyanAndRedLow = h
                mColorBalance.carmineAndGreenLow = s
                mColorBalance.yellowAndBlueLow = b
            }
            1 -> {
                //红色
                mColorBalance.cyanAndRedMiddle = h
                mColorBalance.carmineAndGreenMiddle = s
                mColorBalance.yellowAndBlueMiddle = b
            }
            2 -> {
                //黄色
                mColorBalance.cyanAndRedHight = h
                mColorBalance.carmineAndGreenHight = s
                mColorBalance.yellowAndBlueHight = b
            }
        }
    }

    private fun setHueData() {
        when(findViewById<RadioGroup>(R.id.vRgColorHueTab).checkedRadioButtonId){
            R.id.vRbHueRed -> {
                mHueData.redHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueYellow -> {
                mHueData.yellowHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueGreen -> {
                mHueData.greenHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueCyan -> {
                mHueData.cyanHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueBlue -> {
                mHueData.blueHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueCarmine -> {
                mHueData.carmineHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
            R.id.vRbHueRGB -> {
                mHueData.rgbHValue = findViewById<SeekBar>(R.id.vSeekHue).progress/100f
            }
        }
    }

    private fun setLightData() {
        when(findViewById<RadioGroup>(R.id.vRgColorLightTab).checkedRadioButtonId){
            R.id.vRbLightRed -> {
                mLightData.redLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightYellow -> {
                mLightData.yellowLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightGreen -> {
                mLightData.greenLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightCyan -> {
                mLightData.cyanLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightBlue -> {
                mLightData.blueLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightCarmine -> {
                mLightData.carmineLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
            R.id.vRbLightRGB -> {
                mLightData.rgbLValue = findViewById<SeekBar>(R.id.vSeekLight).progress/100f
            }
        }
    }

    private fun setSaturationData() {
        when(findViewById<RadioGroup>(R.id.vRgColorSaturationTab).checkedRadioButtonId){
            R.id.vRbSaturationRed -> {
                mSaturationData.redSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationYellow -> {
                mSaturationData.yellowSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationGreen -> {
                mSaturationData.greenSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationCyan -> {
                mSaturationData.cyanSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationBlue -> {
                mSaturationData.blueSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationCarmine -> {
                mSaturationData.carmineSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
            R.id.vRbSaturationRGB -> {
                mSaturationData.rgbSValue = findViewById<SeekBar>(R.id.vSeekSaturability).progress/100f
            }
        }
    }

    private fun setHslData(){
        when(findViewById<RadioGroup>(R.id.vRgColorHslTab).checkedRadioButtonId){
            R.id.vRbHslRed -> {
                mHhlData.redHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.redSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.redLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslOrange -> {
                mHhlData.orangeHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.orangeSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.orangeLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslYellow -> {
                mHhlData.yellowHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.yellowSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.yellowLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslGreen -> {
                mHhlData.greenHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.greenSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.greenLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslCyan -> {
                mHhlData.cyanHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.cyanSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.cyanLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslBlue -> {
                mHhlData.blueHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.blueSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.blueLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslCarmine -> {
                mHhlData.carmineHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.carmineSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.carmineLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
            R.id.vRbHslPurple -> {
                mHhlData.purpleHValue = findViewById<SeekBar>(R.id.vColorSeekbarH).progress/100f
                mHhlData.purpleSValue = findViewById<SeekBar>(R.id.vColorSeekbarS).progress/100f
                mHhlData.purpleLValue = findViewById<SeekBar>(R.id.vColorSeekbarL).progress/100f
            }
        }
    }

    private fun chooseHslTab(){
        when(findViewById<RadioGroup>(R.id.vRgColorHslTab).checkedRadioButtonId){
            R.id.vRbHslRed -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.redHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.redSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.redLValue*100).toInt()
            }
            R.id.vRbHslOrange -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.orangeHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.orangeSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.orangeLValue*100).toInt()
            }
            R.id.vRbHslYellow -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.yellowHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.yellowSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.yellowLValue*100).toInt()
            }
            R.id.vRbHslGreen -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.greenHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.greenSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.greenLValue*100).toInt()
            }
            R.id.vRbHslCyan -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.cyanHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.cyanSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.cyanLValue*100).toInt()
            }
            R.id.vRbHslBlue -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.blueHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.blueSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.blueLValue*100).toInt()
            }
            R.id.vRbHslCarmine -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.carmineHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.carmineSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.carmineLValue*100).toInt()
            }
            R.id.vRbHslPurple -> {
                findViewById<SeekBar>(R.id.vColorSeekbarH).progress = (mHhlData.purpleHValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarS).progress = (mHhlData.purpleSValue*100).toInt()
                findViewById<SeekBar>(R.id.vColorSeekbarL).progress = (mHhlData.purpleLValue*100).toInt()
            }
        }
    }

    private fun setColorSelectData() {
        when(mChannelType){
            ColorChannelEnum.COLOR_RED -> {
                mChangeColorDataBean.redRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.redGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.redBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_YELLOW -> {
                mChangeColorDataBean.yellowRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.yellowGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.yellowBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_GREEN -> {
                mChangeColorDataBean.greenRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.greenGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.greenBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_CYAN -> {
                mChangeColorDataBean.cyanRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.cyanGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.cyanBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_BLUE -> {
                mChangeColorDataBean.blueRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.blueGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.blueBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_CARMINE -> {
                mChangeColorDataBean.carmineRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.carmineGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.carmineBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            ColorChannelEnum.COLOR_WHITE -> {
                mChangeColorDataBean.whiteRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.whiteGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.whiteBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
            else -> {
                mChangeColorDataBean.blackRedValue = findViewById<SeekBar>(R.id.vSeekBarCyan).progress/100f
                mChangeColorDataBean.blackGreenValue = findViewById<SeekBar>(R.id.vSeekBarCarmine).progress/100f
                mChangeColorDataBean.blackBlueValue = findViewById<SeekBar>(R.id.vSeekBarYellow).progress/100f
            }
        }
    }

    private fun setSeekbarData(valueOne:Int,valueTwo:Int,valueThree:Int,valueFour:Int){
        mChangeColorDataBean.channelType = mChannelType.getColorChannelValue()
        findViewById<SeekBar>(R.id.vSeekBarCyan).progress = ((valueOne)*100f).toInt()
        findViewById<SeekBar>(R.id.vSeekBarCarmine).progress = ((valueTwo)*100f).toInt()
        findViewById<SeekBar>(R.id.vSeekBarYellow).progress = ((valueThree)*100f).toInt()
        findViewById<SeekBar>(R.id.vSeekBarBlack).progress = ((valueFour)*100f).toInt()
    }

}