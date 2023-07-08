package com.image.library.opencv.camera.filter

import android.content.Context
import android.opengl.GLES11Ext
import android.opengl.GLES20
import com.image.library.main.R
import com.image.library.opencv.camera.gl.OpenGLUtils
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer

class CameraFilter(context:Context) : FilterInter {
    //存放纹理坐标容器
    private val textureBuffer: FloatBuffer
    //存放顶点坐标容器
    private val vertexBuffer: FloatBuffer
    private val program: Int
    private val lineProgram: Int
    private val vPosition: Int
    private val vCoord: Int
    private val vTexture: Int
    private val vMatrix: Int
    private val vTemperature: Int
    private val vTint: Int
    private var mWidth: Int = 0
    private val vPositionLoc:Int
    private val uColorLoc:Int
    private var mHeight: Int = 0
    private var mtx: FloatArray = FloatArray(16)
    var tint: Float = 0.0f
    var temperature: Float = 5000f

    //顶点坐标(相当于画布的四个角)
    private val VERTEX = floatArrayOf(
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    )

    //纹理坐标(相当于画布上图片的四个角)
    private val TEXTURE = floatArrayOf(
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    )

    val colorBuffer:FloatBuffer

    val lineVertexBuffer:FloatBuffer



    init {
        vertexBuffer = ByteBuffer.allocateDirect(4 * 4 * 2)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
        vertexBuffer.clear()
        vertexBuffer.put(VERTEX)

        textureBuffer = ByteBuffer.allocateDirect(4 * 2 * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
        textureBuffer.clear()
        textureBuffer.put(TEXTURE)

        colorBuffer = ByteBuffer.allocateDirect(4 * 2 * 4)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
        colorBuffer.clear()
        colorBuffer.put(floatArrayOf(
            //r,g,b,a
            1F, 0F, 0F, 1F
        ))

        lineVertexBuffer = ByteBuffer.allocateDirect(4 * 4 * 2)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
        lineVertexBuffer.clear()
        lineVertexBuffer.put(floatArrayOf(
            -0.5F, 0.5F, 0F,
            0.5F, 0.5F, 0F,
            0.5F, -0.5F, 0F,
            -0.5F, -0.5F, 0F
        ))



        //形状shader，确定显示的形状
        val vertexShader = OpenGLUtils.readRawTextFile(context, R.raw.camera_vert)
        //着色shader，对前面已经确认好形状，完成光栅化后的图片进行上色处理
        val textureShader = OpenGLUtils.readRawTextFile(context, R.raw.white_blance_frag)
        program = OpenGLUtils.loadProgram(vertexShader, textureShader)
        vPosition = GLES20.glGetAttribLocation(program, "vPosition")
        vCoord = GLES20.glGetAttribLocation(program, "vCoord")
        vTexture = GLES20.glGetUniformLocation(program, "vTexture")
        vMatrix = GLES20.glGetUniformLocation(program, "vMatrix")
        vTemperature = GLES20.glGetUniformLocation(program, "temperature")
        vTint = GLES20.glGetUniformLocation(program, "tint")

        //==================绘制相机线条===========================
        //形状shader，确定显示的形状
        val lineVertexShader = OpenGLUtils.readRawTextFile(context, R.raw.line_vert)
        //着色shader，对前面已经确认好形状，完成光栅化后的图片进行上色处理
        val lineTextureShader = OpenGLUtils.readRawTextFile(context, R.raw.line_frag)
        lineProgram = OpenGLUtils.loadProgram(lineVertexShader, lineTextureShader)
        vPositionLoc = GLES20.glGetAttribLocation(lineProgram, "vPosition")
        uColorLoc = GLES20.glGetUniformLocation(lineProgram, "u_color")

    }

    override fun onDrawFrame(textureId: Int): Int {
        // 1.设置窗口大小
        GLES20.glViewport(0, 0, mWidth, mHeight)
        // 2.使用着色器程序
        GLES20.glUseProgram(program)
        // 3.给着色器程序中传值
        // 3.1 给顶点坐标数据传值
        vertexBuffer.position(0)
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 0, vertexBuffer)
        // 激活
        GLES20.glEnableVertexAttribArray(vPosition)
        // 3.2 给纹理坐标数据传值
        textureBuffer.position(0)
        GLES20.glVertexAttribPointer(vCoord, 2, GLES20.GL_FLOAT, false, 0, textureBuffer)
        GLES20.glEnableVertexAttribArray(vCoord)

        // 3.3 变化矩阵传值
        GLES20.glUniformMatrix4fv(vMatrix, 1, false, mtx, 0)
        //设置色温
        GLES20.glUniform1f(vTemperature, if (this.temperature < 5000) (0.0004 * (this.temperature - 5000.0)).toFloat() else (0.00006 * (this.temperature - 5000.0)).toFloat())
        //色调
        GLES20.glUniform1f(vTint, this.tint / 100.0f)
        // 3.4 给片元着色器中的 采样器绑定
        // 激活图层
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0)
        // 图像数据
        GLES20.glBindTexture(GLES11Ext.GL_SAMPLER_EXTERNAL_OES, textureId)
        // 传递参数
        GLES20.glUniform1i(vTexture, 0)
        //参数传递完毕,通知 opengl开始画画
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)
        // 解绑
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)

/*        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glUseProgram(lineProgram)
        lineVertexBuffer.position(0)
        GLES20.glEnableVertexAttribArray(vPositionLoc)
        GLES20.glVertexAttribPointer(vPositionLoc, 3, GLES20.GL_FLOAT, false, 0, lineVertexBuffer)
        GLES20.glUniform4fv(uColorLoc, 1, colorBuffer)
        GLES20.glDrawArrays(GLES20.GL_LINES, 0, 4)*/

        return textureId
    }

    override fun setTransformMatrix(mtx: FloatArray) {
        this.mtx = mtx
    }

    override fun onReady(width: Int, height: Int) {
        mWidth = width
        mHeight = height
    }
}