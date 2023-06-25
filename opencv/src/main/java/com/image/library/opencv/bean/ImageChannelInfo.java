package com.image.library.opencv.bean;

/**
 * 图像通道信息
 */
public class ImageChannelInfo {
    private int[] redChannel;
    private int[] greenChannel;
    private int[] blurChannel;
    private int[] rgbChannel;

    public ImageChannelInfo(int[] redChannel, int[] greenChannel, int[] blurChannel, int[] rgbChannel) {
        this.redChannel = redChannel;
        this.greenChannel = greenChannel;
        this.blurChannel = blurChannel;
        this.rgbChannel = rgbChannel;
    }

    public int[] getRedChannel() {
        return redChannel;
    }

    public void setRedChannel(int[] redChannel) {
        this.redChannel = redChannel;
    }

    public int[] getGreenChannel() {
        return greenChannel;
    }

    public void setGreenChannel(int[] greenChannel) {
        this.greenChannel = greenChannel;
    }

    public int[] getBlurChannel() {
        return blurChannel;
    }

    public void setBlurChannel(int[] blurChannel) {
        this.blurChannel = blurChannel;
    }

    public int[] getRgbChannel() {
        return rgbChannel;
    }

    public void setRgbChannel(int[] rgbChannel) {
        this.rgbChannel = rgbChannel;
    }
}
