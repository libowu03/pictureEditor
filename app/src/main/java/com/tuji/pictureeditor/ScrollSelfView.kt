package com.tuji.pictureeditor

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import androidx.core.widget.NestedScrollView

class ScrollSelfView : NestedScrollView {
    var childV:View?=null
    var isAllowChildMove = false

    constructor(context:Context):this(context,null)

    constructor(context: Context,attrs: AttributeSet?):this(context,attrs,0)

    constructor(context: Context,attrs: AttributeSet?,def:Int):super(context,attrs,def){

    }


    override fun onInterceptTouchEvent(event: MotionEvent): Boolean {
        when(event?.action){
            MotionEvent.ACTION_DOWN -> {
                Log.i("测试日志","x,y:"+childV!!.x+","+(childV!!.y-childV!!.height)+",宽高为："+childV!!.width+","+childV!!.height+","+event.rawY)
                isAllowChildMove = event.rawY > childV!!.y-childV!!.height  && event.rawY < childV!!.y + childV!!.height && event.rawX < childV!!.x && event.rawX < childV!!.width+childV!!.x
            }
            MotionEvent.ACTION_MOVE -> {
                Log.i("测试日志","内容："+isAllowChildMove)
               if (isAllowChildMove){
                   return false
               }
            }
            MotionEvent.ACTION_UP -> {
                isAllowChildMove = false
            }
        }
        return super.onInterceptTouchEvent(event)
    }

}