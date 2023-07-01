package com.image.library.opencv.en

enum class ColorChannelEnum(type:Int):ColorChannelValue {
    //红色
    COLOR_RED(1) {
        override fun getColorChannelValue(): Int {
            return 1
        }
    },
    //绿色
    COLOR_GREEN(2) {
        override fun getColorChannelValue(): Int {
            return 2
        }
    },
    //蓝色
    COLOR_BLUE(3) {
        override fun getColorChannelValue(): Int {
            return 3
        }
    },
    //黄色
    COLOR_YELLOW(-1) {
        override fun getColorChannelValue(): Int {
            return -1
        }
    },
    //青色
    COLOR_CYAN(-2) {
        override fun getColorChannelValue(): Int {
            return -2
        }
    },
    //洋红色
    COLOR_CARMINE(-3) {
        override fun getColorChannelValue(): Int {
            return -3
        }
    },
    //白色
    COLOR_WHITE(5) {
        override fun getColorChannelValue(): Int {
            return 5
        }
    },
    //白色
    COLOR_BLACK(6) {
        override fun getColorChannelValue(): Int {
            return 6
        }
    }
}

interface ColorChannelValue{
    fun getColorChannelValue():Int
}