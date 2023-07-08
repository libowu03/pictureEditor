package com.image.library.opencv.camera.filter;

import static android.opengl.GLES30.GL_TEXTURE0;
import static android.opengl.GLES30.glActiveTexture;
import static android.opengl.GLES30.glBindTexture;
import static android.opengl.GLES30.glUniform1i;

import android.opengl.GLES11Ext;

import com.image.library.main.R;
import com.image.library.opencv.camera.gl.GLUtil;

/**
 * Created by wangyt on 2019/5/24
 */
public class OesFilter extends BaseFilter{

    public OesFilter() {
        super();
    }

    @Override
    public int initProgram() {
        return GLUtil.createAndLinkProgram(R.raw.texture_vertex_shader, R.raw.texture_oes_fragtment_shader);
    }

    @Override
    public void bindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, getTextureId()[0]);
        glUniform1i(hTexture, 0);
    }
}
