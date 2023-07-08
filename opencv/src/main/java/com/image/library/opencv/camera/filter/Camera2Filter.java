package com.image.library.opencv.camera.filter;

import static android.opengl.GLES20.glDrawArrays;
import static android.opengl.GLES20.glLineWidth;
import static android.opengl.GLES30.GL_CLAMP_TO_EDGE;
import static android.opengl.GLES30.GL_COLOR_ATTACHMENT0;
import static android.opengl.GLES30.GL_FRAMEBUFFER;
import static android.opengl.GLES30.GL_LINEAR;
import static android.opengl.GLES30.GL_NEAREST;
import static android.opengl.GLES30.GL_RGBA;
import static android.opengl.GLES30.GL_TEXTURE_2D;
import static android.opengl.GLES30.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES30.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES30.GL_TEXTURE_WRAP_S;
import static android.opengl.GLES30.GL_TEXTURE_WRAP_T;
import static android.opengl.GLES30.GL_UNSIGNED_BYTE;
import static android.opengl.GLES30.glBindFramebuffer;
import static android.opengl.GLES30.glBindTexture;
import static android.opengl.GLES30.glDeleteFramebuffers;
import static android.opengl.GLES30.glDeleteTextures;
import static android.opengl.GLES30.glFramebufferTexture2D;
import static android.opengl.GLES30.glGenFramebuffers;
import static android.opengl.GLES30.glGenTextures;
import static android.opengl.GLES30.glTexImage2D;
import static android.opengl.GLES30.glTexParameterf;

import javax.microedition.khronos.opengles.GL10;

/**
 * Created by wangyt on 2019/5/24
 */
public class Camera2Filter extends OesFilter {

    //后置相机，顺时针旋转90度
    public static final float[] textureCoordCameraBack = {
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
    };

    //前置相机，逆时针旋转90度
    public static final float[] textureCoordCameraFront = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };

    public static final float[] cameraLine = {
            -0.5f,1f,
            -0.5f,-1f,
    };

    public int[] frameBuffer = new int[1];
    public int[] frameTexture = new int[1];

    public boolean useFront = false;

    public boolean isUseFront() {
        return useFront;
    }

    public void setUseCameraFront(boolean useFront) {
        if (this.useFront != useFront) {
            this.useFront = useFront;
            textureCoordBuffer = useFront ? CommonUtil.getFloatBuffer(textureCoordCameraFront)
                    : CommonUtil.getFloatBuffer(textureCoordCameraBack);
        }
    }

    public Camera2Filter() {
        super();
    }

    @Override
    public void initBuffer() {
        vertexBuffer = CommonUtil.getFloatBuffer(vertex);
        textureCoordBuffer = useFront ? CommonUtil.getFloatBuffer(textureCoordCameraFront)
                : CommonUtil.getFloatBuffer(textureCoordCameraBack);
    }

    @Override
    public int[] getOutputTextureId() {
        return frameTexture;
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        if (this.width != width || this.height != height) {
            this.width = width;
            this.height = height;
            delFrameBufferAndTexture();
            genFrameBufferAndTexture();
        }
    }

    @Override
    public void onDraw() {
        bindFrameBufferAndTexture();
        super.onDraw();
        unBindFrameBuffer();
    }

    public void delFrameBufferAndTexture() {
        glDeleteFramebuffers(frameBuffer.length, frameBuffer, 0);
        glDeleteTextures(frameTexture.length, frameTexture, 0);
    }

    public void genFrameBufferAndTexture() {
        glGenFramebuffers(frameBuffer.length, frameBuffer, 0);

        glGenTextures(frameTexture.length, frameTexture, 0);
        glBindTexture(GL_TEXTURE_2D, frameTexture[0]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, null);
        setTextureParameters();
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    public void setTextureParameters() {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    public void bindFrameBufferAndTexture() {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer[0]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture[0], 0);

        //绘制相机线条
        glLineWidth(2f);
        glDrawArrays(GL10.GL_LINES,0,4);
    }

    public void unBindFrameBuffer() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
