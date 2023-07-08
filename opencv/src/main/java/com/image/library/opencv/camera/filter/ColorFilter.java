package com.image.library.opencv.camera.filter;

import static android.opengl.GLES20.glUniform1f;
import static android.opengl.GLES30.GL_TEXTURE0;
import static android.opengl.GLES30.GL_TEXTURE_2D;
import static android.opengl.GLES30.glActiveTexture;
import static android.opengl.GLES30.glBindTexture;
import static android.opengl.GLES30.glGetUniformLocation;
import static android.opengl.GLES30.glUniform1i;
import android.graphics.Bitmap;

import com.image.library.main.R;
import com.image.library.opencv.camera.gl.GLUtil;

/**
 * Created by wangyt on 2019/5/27
 */
public class ColorFilter extends BaseFilter {

    public static final String UNIFORM_COLOR_FLAG = "colorFlag";
    public static final String UNIFORM_LUT_STRENGTH = "lutStrength";
    public static final String UNIFORM_TEXTURE_LUT = "textureLUT";

    public static int COLOR_FLAG = -1;
    public static int COLOR_FLAG_USE_LUT = 6;
    private boolean needToUpdateLut = false;
    private Bitmap lutBitmap;
    public int hColorFlag;
    public int hLutStrength;
    public int hTextureLUT;
    private int LUTTextureId;
    public static float lutStrength = 0;

    @Override
    public void onSurfaceCreated() {
        super.onSurfaceCreated();
        LUTTextureId = GLUtil.loadTextureFromRes(R.drawable.neutral_lut);
    }

    public void updateLutBitmap(Bitmap bitmap){
        if (bitmap == null){
            COLOR_FLAG = -1;
        }else {
            COLOR_FLAG = 6;
            glUniform1i(hColorFlag, COLOR_FLAG);
            this.lutBitmap = bitmap;
            needToUpdateLut = true;
        }
    }

    /**
     * 更新lut强度
     * @param strength
     */
    public void updateLutStrength(int strength){
        lutStrength = strength;
        glUniform1f(hLutStrength,lutStrength);
    }
    public float getLutStrength(){
        return lutStrength;
    }

    @Override
    public int initProgram() {
        return GLUtil.createAndLinkProgram(R.raw.texture_vertex_shader, R.raw.texture_color_fragtment_shader);
    }

    @Override
    public void initAttribLocations() {
        super.initAttribLocations();

        hColorFlag = glGetUniformLocation(program, UNIFORM_COLOR_FLAG);
        hTextureLUT = glGetUniformLocation(program, UNIFORM_TEXTURE_LUT);
        hLutStrength = glGetUniformLocation(program,UNIFORM_LUT_STRENGTH);
    }

    @Override
    public void setExtend() {
        super.setExtend();
        glUniform1i(hColorFlag, COLOR_FLAG);
        glUniform1f(hLutStrength,lutStrength);
    }

    @Override
    public void bindTexture() {
        super.bindTexture();
        if (needToUpdateLut){
            LUTTextureId = GLUtil.loadTextureFromBitmap(lutBitmap);
            lutBitmap = null;
            needToUpdateLut = false;
        }
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, LUTTextureId);
        glUniform1i(hTextureLUT, 1);
    }
}
