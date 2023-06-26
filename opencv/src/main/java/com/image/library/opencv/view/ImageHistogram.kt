package com.image.library.opencv.view

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.util.AttributeSet
import android.view.View
import com.image.library.opencv.bean.ImageChannelInfo
import kotlin.math.max

/**
 * 通道直方图
 */
class ImageHistogram : View {
    private val mPaintLine = Paint()
    private val mLinePath = Path()
    private var mHistogram: ImageChannelInfo ?= null
    private var mOpenRed:Boolean = true
    private var mOpenGreen:Boolean = false
    private var mOpenBlur:Boolean = false
    //通道最大值，即在256个颜色中，那个颜色的色值数量最多
    private var mRChannelMax = 0
    private var mGChannelMax = 0
    private var mBChannelMax = 0
    private var mRgbChannelMax = 0
    private var mEnableDifferentColor = false

    constructor(context: Context) : super(context, null)

    constructor(context: Context, attr: AttributeSet?) : super(context, attr, 0)

    constructor(context: Context, attr: AttributeSet?, defStyleAttr: Int) : super(
        context,
        attr,
        defStyleAttr,
        0
    )

    constructor(context: Context, attr: AttributeSet?, defStyleAttr: Int, defStyleRes: Int) : super(
        context,
        attr,
        defStyleAttr,
        defStyleRes
    ) {
        initView()
    }

    private fun initView() {
        mPaintLine.strokeCap = Paint.Cap.ROUND
        mPaintLine.isAntiAlias = true
    }

    override fun onDraw(canvas: Canvas?) {
        mHistogram?.let {
            var channelMax = 0
            if (mOpenRed && !mOpenGreen && !mOpenBlur){
                channelMax = mRChannelMax
            }
            if (mOpenRed && mOpenGreen && !mOpenBlur){
                channelMax = max(mRChannelMax,mGChannelMax)
            }
            if (mOpenRed && mOpenGreen && mOpenBlur){
                channelMax = max(max(mRChannelMax,mGChannelMax),mBChannelMax)
            }
            if (!mOpenRed && mOpenGreen && !mOpenBlur){
                channelMax = mGChannelMax
            }
            if (!mOpenRed && mOpenGreen && mOpenBlur){
                channelMax = max(mGChannelMax,mBChannelMax)
            }
            if (!mOpenRed && !mOpenGreen && mOpenBlur){
                channelMax = mBChannelMax
            }
            if (mOpenRed && !mOpenGreen && mOpenBlur){
                channelMax = max(mBChannelMax,mRChannelMax)
            }

            if (mOpenRed && mOpenGreen && mOpenBlur && !mEnableDifferentColor){
                channelMax = mRgbChannelMax
                mLinePath.reset()
                mPaintLine.color = Color.parseColor("#333333")
                mLinePath.moveTo(0f,height.toFloat())
                for (item in it.rgbChannel.withIndex()){
                    val ratio = item.value/channelMax.toFloat()
                    val y = height - height*ratio
                    val x = width/256.0f*item.index
                    mLinePath.lineTo(x.toFloat(),y.toFloat())
                }
                mLinePath.lineTo(width.toFloat(),height.toFloat())
                mLinePath.close()
                canvas?.drawPath(mLinePath,mPaintLine)
            }else{
                if (mOpenRed){
                    mLinePath.reset()
                    mPaintLine.color = Color.parseColor("#E3725D")
                    mLinePath.moveTo(0f,height.toFloat())
                    for (item in it.redChannel.withIndex()){
                        val ratio = item.value/channelMax.toFloat()
                        val y = height - height*ratio
                        val x = width/256.0f*item.index
                        mLinePath.lineTo(x.toFloat(),y.toFloat())
                    }
                    mLinePath.lineTo(width.toFloat(),height.toFloat())
                    mLinePath.close()
                    canvas?.drawPath(mLinePath,mPaintLine)
                }

                if (mOpenGreen){
                    mLinePath.reset()
                    mPaintLine.color = Color.parseColor("#A3CA55")
                    mLinePath.moveTo(0f,height.toFloat())
                    for (item in it.greenChannel.withIndex()){
                        val ratio = item.value/channelMax.toFloat()
                        val y = height - height*ratio
                        val x = width/256.0f*item.index
                        mLinePath.lineTo(x.toFloat(),y.toFloat())
                    }
                    mLinePath.lineTo(width.toFloat(),height.toFloat())
                    mLinePath.close()
                    canvas?.drawPath(mLinePath,mPaintLine)
                }

                if (mOpenBlur){
                    mLinePath.reset()
                    mPaintLine.color = Color.parseColor("#65C1C2")
                    mLinePath.moveTo(0f,height.toFloat())
                    for (item in it.blurChannel.withIndex()){
                        val ratio = item.value/channelMax.toFloat()
                        val y = height - height*ratio
                        val x = width/256.0f*item.index
                        mLinePath.lineTo(x.toFloat(),y.toFloat())
                    }
                    mLinePath.lineTo(width.toFloat(),height.toFloat())
                    mLinePath.close()
                    canvas?.drawPath(mLinePath,mPaintLine)
                }else{}
            }
        }
    }

    /**
     *
     * 设置直方图数据
     * @param channelInfo 通道数量信息
     * @param openR 允许绘制红色通道
     * @param openR 允许绘制蓝色通道
     * @param openB 允许绘制绿色通道
     */
    fun setHistogramInfo(channelInfo:ImageChannelInfo?,openR:Boolean = false,openG:Boolean=false,openB:Boolean = true,enableDifferentColor:Boolean = false){
        this.mHistogram = channelInfo
        this.mOpenBlur = openB
        this.mOpenGreen = openG
        this.mOpenRed = openR
        channelInfo?.let {
            for (item in channelInfo.redChannel){
                if (this.mRChannelMax < item){
                    this.mRChannelMax = item
                }
            }
            for (item in channelInfo.greenChannel){
                if (this.mGChannelMax < item){
                    this.mGChannelMax = item
                }
            }
            for (item in channelInfo.blurChannel){
                if (this.mBChannelMax < item){
                    this.mBChannelMax = item
                }
            }
            for (item in channelInfo.rgbChannel){
                if (this.mRgbChannelMax < item){
                    this.mRgbChannelMax = item
                }
            }
        }
        this.mEnableDifferentColor = enableDifferentColor
        invalidate()
    }
}