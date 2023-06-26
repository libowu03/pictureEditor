package com.image.library.opencv.view

import android.content.Context
import android.graphics.*
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import com.image.library.opencv.OpenCvIn
import kotlin.math.abs

/**
 * 曲线工具
 */
class CurveLineToolsView : View {
    //绘制横线的画笔
    private val mLinePaing = Paint()
    //点击后是否允许创建控制点
    private var mEnableCreate:Boolean = true
    //控制点画笔
    private val mControllerPaint = Paint()
    private var mChooseControllerIndex = -1
    private var mChooseIndex = 0
    private var mChangeBitmapTime = 0L
    private var mBorderWidth:Float = 5f

    private var mChangeCurveLineCallback: (x: MutableList<Double>, y: MutableList<Double>, point: MutableList<PointF>,isStart:Boolean,isFinish:Boolean,isClick:Boolean) -> Unit =
        { x, y, point,isStart,isFinish,isClick -> }
    var chooseControllerPointListener:(pointIndex:Int,point:PointF)->Unit = {pointIndex, point ->  }
    var currentControllerPoint:PointF?=null
    var viewWidth = 0
    var viewHeight = 0
    var mClickDate = 0L
    //绘制曲线的路径
    private val mPath = Path()
    private val mControllerPoint = mutableListOf<PointF>()
    public var clickCallback:(isStartClickL:Boolean) -> Unit = {}
    constructor(context: Context) : this(context, null)

    constructor(context: Context, attr: AttributeSet?) : this(context, attr, 0)

    constructor(context: Context, attr: AttributeSet?, defStyleAttr: Int) : this(
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
        initPaint()
    }

    private fun initPaint() {
        mLinePaing.isAntiAlias = true
        mLinePaing.strokeWidth = 4f
        mLinePaing.style = Paint.Style.STROKE
        mLinePaing.strokeCap = Paint.Cap.ROUND
    }

    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)
        viewHeight = height
        viewWidth = width
        canvas?.let {
            //如果第一次初始化，先设置三个初始控制点，即起始点，中点，终点
            if (mControllerPoint.isEmpty()) {
                mControllerPoint.add(PointF(0.0f, 0.0f))
                mControllerPoint.add(PointF(255f, 255f))
            }

            //绘制横线
            mLinePaing.setColor(Color.parseColor("#99333333"))
            /*for (item in 1..3) {
                canvas.drawLine(
                    0f,
                    canvas.height / 4.0f * item,
                    canvas.width.toFloat(),
                    canvas.height / 4.0f * item,
                    mLinePaing
                )
            }*/

            //将point集合转换成普通的x，y集合，并绘制控制点
            val xController = mutableListOf<Double>()
            val yController = mutableListOf<Double>()
            for (item in mControllerPoint.withIndex()) {
                xController.add(item.value.x.toDouble())
                yController.add(item.value.y.toDouble())
            }

            //绘制三次样条曲线
            mPath.reset()
            mPath.moveTo(mControllerPoint[0].x/255*width, height - mControllerPoint[0].y/255*height)
            mLinePaing.setColor(Color.BLACK)
            for (item in 0..255) {
                //获取x坐标对应的y坐标
                val pointY = OpenCvIn.cubicSplineGetY(
                    item.toDouble(),
                    xController.toDoubleArray(),
                    yController.toDoubleArray()
                )
                var lineY = height - pointY/255*height
                if (lineY > height) {
                    lineY = height.toDouble() - 1
                } else if (lineY < 0f) {
                    lineY = 1.0
                }
                mPath.lineTo(item.toFloat()/255*width, lineY.toFloat())
            }
            mLinePaing.strokeWidth = mBorderWidth*2
            mLinePaing.color = Color.WHITE
            canvas.drawPath(mPath, mLinePaing)
            mLinePaing.strokeWidth = mBorderWidth
            mLinePaing.color = Color.BLACK
            canvas.drawPath(mPath, mLinePaing)

            mLinePaing.color = Color.WHITE
            mLinePaing.strokeWidth = mBorderWidth*2
            canvas?.drawRect(mBorderWidth,mBorderWidth, (canvas?.width?:0).toFloat()-mBorderWidth, (canvas?.height?:0).toFloat()-mBorderWidth,mLinePaing)
            mLinePaing.color = Color.BLACK
            mLinePaing.strokeWidth = mBorderWidth
            canvas?.drawRect(mBorderWidth,mBorderWidth, (canvas?.width?:0).toFloat()-mBorderWidth, (canvas?.height?:0).toFloat()-mBorderWidth,mLinePaing)

            for (item in mControllerPoint.withIndex()) {
                mControllerPaint.color = Color.WHITE
                if (item.value.x <= 7 && item.value.y >= 248){
                    canvas.drawCircle(item.value.x/255*width+(14-item.value.x), height - item.value.y/255*height+(14-(255-item.value.y)), 14f, mControllerPaint)
                }else if (item.value.x <= 7 && item.value.y <= 7){
                    canvas.drawCircle(item.value.x/255*width+(14-item.value.x), height - item.value.y/255*height-(14-item.value.y), 14f, mControllerPaint)
                }else if (item.value.x >= 248 && item.value.y <= 7){
                    canvas.drawCircle(item.value.x/255*width-(14-(255-item.value.x)), height - item.value.y/255*height+(14-item.value.y), 14f, mControllerPaint)
                }else if (item.value.x >= 248 && item.value.y >=248){
                    canvas.drawCircle(item.value.x/255*width-(14-(255-item.value.x)), height - item.value.y/255*height+(14-(255-item.value.y)), 14f, mControllerPaint)
                }else if (item.value.x <= 7){
                    canvas.drawCircle(item.value.x/255*width+(14-item.value.x), height - item.value.y/255*height, 14f, mControllerPaint)
                }else if (item.value.y > 248){
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height+(14-(255-item.value.y)), 14f, mControllerPaint)
                }else if (item.value.x > 248){
                    canvas.drawCircle(item.value.x/255*width-(14-(255-item.value.x)), height - item.value.y/255*height, 14f, mControllerPaint)
                }else if (item.value.y < 7){
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height-(14-item.value.y), 14f, mControllerPaint)
                }else{
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height, 14f, mControllerPaint)
                }
                if (item.index == mChooseIndex){
                    mControllerPaint.color = Color.RED
                }else{
                    mControllerPaint.color = Color.BLACK
                }

                if (item.value.x <= 6 && item.value.y >= 249){
                    canvas.drawCircle(item.value.x/255*width+(12-item.value.x), height - item.value.y/255*height+(12-(255-item.value.y)), 12f, mControllerPaint)
                }else if (item.value.x <= 6 && item.value.y <= 6){
                    canvas.drawCircle(item.value.x/255*width+(12-item.value.x), height - item.value.y/255*height-(12-item.value.y), 12f, mControllerPaint)
                }else if (item.value.x >= 249 && item.value.y <= 6){
                    canvas.drawCircle(item.value.x/255*width-(12-(255-item.value.x)), height - item.value.y/255*height-(12-item.value.y), 12f, mControllerPaint)
                }else if (item.value.x >= 249 && item.value.y >=249){
                    canvas.drawCircle(item.value.x/255*width-(12-(255-item.value.x)), height - item.value.y/255*height+(12-(255-item.value.y)), 12f, mControllerPaint)
                }else if (item.value.x <= 6){
                    canvas.drawCircle(item.value.x/255*width+(12-item.value.x), height - item.value.y/255*height, 12f, mControllerPaint)
                }else if (item.value.y > 249){
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height+(12-(255-item.value.y)), 12f, mControllerPaint)
                }else if (item.value.x > 249){
                    canvas.drawCircle(item.value.x/255*width-(12-(255-item.value.x)), height - item.value.y/255*height, 12f, mControllerPaint)
                }else if (item.value.y < 6){
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height-(12-item.value.y), 12f, mControllerPaint)
                }else{
                    canvas.drawCircle(item.value.x/255*width, height - item.value.y/255*height, 14f, mControllerPaint)
                }
            }
        }
    }

    /**
     * 是否是创建模式
     */
    fun setEnableCreate(isEnable:Boolean){
        this.mEnableCreate = isEnable
    }

    /**
     * 获取当前控制点的位置
     */
    fun getCurrentPointIndex():Int{
        return mChooseIndex
    }

    /**
     * 设置控制点的值
     */
    fun setPointValue(pointIndex:Int,x:Float,y:Float){
        if (pointIndex >= mControllerPoint.size){
            Log.i("日志","不存在控制点")
            return
        }
        mChooseControllerIndex = pointIndex
        mChooseIndex = pointIndex
        mControllerPoint[pointIndex].x = x
        mControllerPoint[pointIndex].y = y
        invalidate()

        mChangeCurveLineCallback.invoke(
            getControllerArray().xController ?: mutableListOf(),
            getControllerArray().yController ?: mutableListOf(),
            getControllerArray().point ?: mutableListOf<PointF>(),false,false,false)
    }

    /**
     * 选中某个控制点
     */
    fun selectController(index:Int){
        mChooseIndex = index
        mChooseControllerIndex = index
        invalidate()
    }

    /**
     * 设置控制点
     */
    fun setPointList(xArray:DoubleArray,yArray: DoubleArray){
        if (xArray.size != yArray.size){
            return
        }
        mControllerPoint.clear()
        for (item in xArray.withIndex()){
            mControllerPoint.add(PointF(item.value.toFloat(),yArray[item.index].toFloat()))
        }
        invalidate()
    }

    /**
     * 重置控制点
     */
    fun reset() {
        mControllerPoint.clear()
        mControllerPoint.add(PointF(0.0f, 0.0f))
        mControllerPoint.add(PointF(255f, 255f))
        invalidate()
    }

    /**
     * 获取控制的列表
     */
    fun getControllerPointList():MutableList<PointF>{
        if (mControllerPoint.isEmpty()) {
            mControllerPoint.add(PointF(0.0f, 0.0f))
            mControllerPoint.add(PointF(255f, 255f))
        }
        return mControllerPoint
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        when (event?.action) {
            MotionEvent.ACTION_DOWN -> {
                clickCallback.invoke(true)
                //如果点击的点不存在，则创建一个,需要创建点插入的地方
                var mInsertIndex = -1
                var needToInsert = true
                for (item in mControllerPoint.withIndex()) {
                    if (abs((event.x - item.value.x/255*width)) < 100 && abs((height - event.y) - item.value.y/255*height) < 100) {
                        mChooseControllerIndex = item.index
                        mChooseIndex = mChooseControllerIndex
                        needToInsert = false
                        break
                    }
                }

                if (needToInsert){
                    for (item in mControllerPoint.withIndex()) {
                        //这里y坐标计算需要反转一下
                        if (item.value.x/255*width < event.x && item.index+1 < mControllerPoint.size && mControllerPoint.get(item.index+1).x/255*width > event.x ){
                            mInsertIndex = item.index + 1
                            break
                        }
                    }
                }


                if (mInsertIndex != -1){
                    mControllerPoint.add( mInsertIndex,PointF(event.x/width*255,event.y/height*255))
                    mChooseControllerIndex = mInsertIndex
                    mChooseIndex = mChooseControllerIndex
                }
                mClickDate = System.currentTimeMillis()

                mChangeCurveLineCallback.invoke(
                    getControllerArray().xController ?: mutableListOf(),
                    getControllerArray().yController ?: mutableListOf(),
                    getControllerArray().point ?: mutableListOf<PointF>(),true
                    ,false,true)
            }
            MotionEvent.ACTION_MOVE -> {
                var xw = 0f
                var yh = 0f
                if (mChooseControllerIndex != -1) {
                    if (event.x > width){
                        xw = 255f
                    }else if (event.x < 0 ){
                        xw = 0f
                    }else{
                        xw = event.x/width*255
                    }
                    if (event.y < 0f){
                        yh = 255f
                    }else if (event.y > height){
                        yh = 0f
                    }else{
                        yh = (height - event.y)/height*255
                    }
                    if (mChooseControllerIndex+1 <= mControllerPoint.size-1 && event.x >= mControllerPoint[mChooseControllerIndex+1].x/255*width){
                        return true
                    }
                    if (mChooseControllerIndex-1 >= 0 && event.x <= mControllerPoint[mChooseControllerIndex-1].x/255*width){
                        return true
                    }
                    mControllerPoint[mChooseControllerIndex].x = xw
                    mControllerPoint[mChooseControllerIndex].y = yh
                    val curveData = getControllerArray()
                    mChangeCurveLineCallback.invoke(
                        curveData.xController ?: mutableListOf(),
                        curveData.yController ?: mutableListOf(),
                        curveData.point ?: mutableListOf<PointF>(),false
                        ,false,false)
                    chooseControllerPointListener.invoke(mChooseControllerIndex,mControllerPoint[mChooseControllerIndex])
                    invalidate()
                }
            }
            MotionEvent.ACTION_UP -> {
                clickCallback.invoke(false)
                if (mChooseControllerIndex == -1){
                    return true
                }
                if (mChooseControllerIndex != -1) {
                    val curveData = getControllerArray()
                    mChangeCurveLineCallback.invoke(
                        curveData.xController ?: mutableListOf(),
                        curveData.yController ?: mutableListOf(),
                        curveData.point ?: mutableListOf<PointF>(),false
                    ,true   ,false)
                }
                chooseControllerPointListener.invoke(mChooseControllerIndex,mControllerPoint[mChooseControllerIndex])
                currentControllerPoint = mControllerPoint[mChooseControllerIndex]
                mChooseIndex = mChooseControllerIndex
                return true
            }
        }
        return true
    }

    /**
     * 设置三次曲线改变的监听事件
     */
    fun setCurveCallback(mChangeCurveLineCallback: (x: MutableList<Double>, y: MutableList<Double>, point: MutableList<PointF>,isStart:Boolean,isFinish:Boolean,isClick:Boolean) -> Unit){
        this.mChangeCurveLineCallback = mChangeCurveLineCallback
    }

    fun getControllerArray(): CurveLineData {
        var curveLineData = CurveLineData()
        for (item in mControllerPoint) {
            curveLineData.xController?.add(item.x.toDouble())
            curveLineData.yController?.add(item.y.toDouble())
            curveLineData.point?.add(item)
        }
        return curveLineData
    }

    class CurveLineData(
        var xController: MutableList<Double>? = mutableListOf(),
        var yController: MutableList<Double>? = mutableListOf(),
        var point: MutableList<PointF>? = mutableListOf()
    )


}