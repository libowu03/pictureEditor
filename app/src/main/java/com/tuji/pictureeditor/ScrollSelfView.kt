package com.tuji.pictureeditor

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import androidx.core.widget.NestedScrollView
import kotlin.math.log

class ScrollSelfView : NestedScrollView {
    var childV:View?=null
    var isAllowChildMove = false
    private var array = intArrayOf(0,0)

    constructor(context:Context):this(context,null)

    constructor(context: Context,attrs: AttributeSet?):this(context,attrs,0)

    constructor(context: Context,attrs: AttributeSet?,def:Int):super(context,attrs,def){

    }


    override fun onInterceptTouchEvent(event: MotionEvent): Boolean {
        if (array[0] == 0 && array[1] == 0){
            childV!!.getLocationOnScreen(array)
        }
        when(event?.action){
            MotionEvent.ACTION_DOWN -> {
                isAllowChildMove = event.rawY + scrollY > array[1]  && event.rawY+scrollY < array[1] + childV!!.height && event.rawX < array[0]+childV!!.width && event.rawX < childV!!.width+array[0]
                if (isAllowChildMove){
                    return false
                }
            }
            MotionEvent.ACTION_MOVE -> {
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