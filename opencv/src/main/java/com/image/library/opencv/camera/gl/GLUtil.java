package com.image.library.opencv.camera.gl;

import static android.opengl.GLES30.GL_COMPILE_STATUS;
import static android.opengl.GLES30.GL_FRAGMENT_SHADER;
import static android.opengl.GLES30.GL_LINK_STATUS;
import static android.opengl.GLES30.GL_NEAREST;
import static android.opengl.GLES30.GL_TEXTURE_2D;
import static android.opengl.GLES30.GL_TEXTURE_MAG_FILTER;
import static android.opengl.GLES30.GL_TEXTURE_MIN_FILTER;
import static android.opengl.GLES30.GL_VERTEX_SHADER;
import static android.opengl.GLES30.glAttachShader;
import static android.opengl.GLES30.glBindTexture;
import static android.opengl.GLES30.glCompileShader;
import static android.opengl.GLES30.glCreateProgram;
import static android.opengl.GLES30.glCreateShader;
import static android.opengl.GLES30.glDeleteProgram;
import static android.opengl.GLES30.glDeleteShader;
import static android.opengl.GLES30.glDeleteTextures;
import static android.opengl.GLES30.glGenTextures;
import static android.opengl.GLES30.glGetProgramiv;
import static android.opengl.GLES30.glGetShaderInfoLog;
import static android.opengl.GLES30.glGetShaderiv;
import static android.opengl.GLES30.glLinkProgram;
import static android.opengl.GLES30.glShaderSource;
import static android.opengl.GLES30.glTexParameterf;
import static android.opengl.GLUtils.texImage2D;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by wangyt on 2019/5/9
 */
public class GLUtil {
    private static final String TAG = "opengl-demos";

    private static Context context;

    public static void init(Context ctx){
        context = ctx;
    }

    /*********************** 纹理 ************************/
    public static int loadTextureFromRes(int resId){
        //创建纹理对象
        int[] textureId = new int[1];
        //生成纹理：纹理数量、保存纹理的数组，数组偏移量
        glGenTextures(1, textureId,0);
        if(textureId[0] == 0){
            Log.e(TAG, "创建纹理对象失败");
        }
        //原尺寸加载位图资源（禁止缩放）
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resId, options);
        if (bitmap == null){
            //删除纹理对象
            glDeleteTextures(1, textureId, 0);
            Log.e(TAG, "加载位图失败");
        }
        //绑定纹理到opengl
        glBindTexture(GL_TEXTURE_2D, textureId[0]);
        //设置放大、缩小时的纹理过滤方式，必须设定，否则纹理全黑
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //将位图加载到opengl中，并复制到当前绑定的纹理对象上
        texImage2D(GL_TEXTURE_2D, 0, bitmap, 0);
        //释放bitmap资源（上面已经把bitmap的数据复制到纹理上了）
        bitmap.recycle();
        //解绑当前纹理，防止其他地方以外改变该纹理
        glBindTexture(GL_TEXTURE_2D, 0);
        //返回纹理对象
        return textureId[0];
    }

    public static int loadTextureFromBitmap(Bitmap lutBitmap){
        //创建纹理对象
        int[] textureId = new int[1];
        //生成纹理：纹理数量、保存纹理的数组，数组偏移量
        glGenTextures(1, textureId,0);
        if(textureId[0] == 0){
            Log.e(TAG, "创建纹理对象失败");
        }
        //原尺寸加载位图资源（禁止缩放）
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        if (lutBitmap == null){
            //删除纹理对象
            glDeleteTextures(1, textureId, 0);
            Log.e(TAG, "加载位图失败");
        }
        //绑定纹理到opengl
        glBindTexture(GL_TEXTURE_2D, textureId[0]);
        //设置放大、缩小时的纹理过滤方式，必须设定，否则纹理全黑
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //将位图加载到opengl中，并复制到当前绑定的纹理对象上
        texImage2D(GL_TEXTURE_2D, 0, lutBitmap, 0);
        //释放bitmap资源（上面已经把bitmap的数据复制到纹理上了）
        lutBitmap.recycle();
        //解绑当前纹理，防止其他地方以外改变该纹理
        glBindTexture(GL_TEXTURE_2D, 0);
        //返回纹理对象
        return textureId[0];
    }


    /*********************** 着色器、程序 ************************/
    public static String loadShaderSource(int resId){
        StringBuilder res = new StringBuilder();

        InputStream is = context.getResources().openRawResource(resId);
        InputStreamReader isr = new InputStreamReader(is);
        BufferedReader br = new BufferedReader(isr);

        String nextLine;
            try {
                while ((nextLine = br.readLine()) != null) {
                    res.append(nextLine);
                    res.append('\n');
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        return res.toString();
    }

    /**
     * 加载着色器源，并编译
     *
     * @param type         顶点着色器（GL_VERTEX_SHADER）/片段着色器（GL_FRAGMENT_SHADER）
     * @param shaderSource 着色器源
     * @return 着色器
     */
    public static int loadShader(int type, String shaderSource){
        //创建着色器对象
        int shader = glCreateShader(type);
        if (shader == 0) return 0;//创建失败
        //加载着色器源
        glShaderSource(shader, shaderSource);
        //编译着色器
        glCompileShader(shader);
        //检查编译状态
        int[] compiled = new int[1];
        glGetShaderiv(shader, GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            Log.e(TAG, glGetShaderInfoLog(shader));
            glDeleteShader(shader);
            return 0;//编译失败
        }

        return shader;
    }

    public static int createAndLinkProgram(int vertextShaderResId, int fragmentShaderResId){
        //获取顶点着色器
        int vertexShader = GLUtil.loadShader(GL_VERTEX_SHADER, loadShaderSource(vertextShaderResId));
        if (0 == vertexShader){
            Log.e(TAG, "failed to load vertexShader");
            return 0;
        }
        //获取片段着色器
        int fragmentShader = GLUtil.loadShader(GL_FRAGMENT_SHADER, loadShaderSource(fragmentShaderResId));
        if (0 == fragmentShader){
            Log.e(TAG, "failed to load fragmentShader");
            return 0;
        }
        int program = glCreateProgram();
        if (program == 0){
            Log.e(TAG, "failed to create program");
        }
        //绑定着色器到程序
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        //连接程序
        glLinkProgram(program);
        //检查连接状态
        int[] linked = new int[1];
        glGetProgramiv(program,GL_LINK_STATUS, linked, 0);
        if (linked[0] == 0){
            glDeleteProgram(program);
            Log.e(TAG, "failed to link program");
            return 0;
        }
        return program;
    }
}
