package com.image.library.opencv.bean

/**
 * @author:libowu
 * @Date:2021/10/22
 * @Description:
 */
data class ColorBalance (var cyanAndRedLow:Int = 0,var carmineAndGreenLow:Int = 0, var yellowAndBlueLow:Int = 0,
                         var cyanAndRedMiddle:Int = 0, var carmineAndGreenMiddle:Int = 0, var yellowAndBlueMiddle:Int = 0,
                         var cyanAndRedHight:Int = 0, var carmineAndGreenHight:Int = 0, var yellowAndBlueHight:Int = 0)