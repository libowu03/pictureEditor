package com.image.library.opencv.utils;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.BitmapRegionDecoder;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;

import androidx.annotation.IntRange;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.camera.core.ImageProxy;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Auther: Crazy.Mo
 * DateTime: 2017/8/25 17:34
 * Summary:
 */
public class BitmapUtil {

    /**
     * 压缩图片
     * @param bitmap
     * @return
     */
    public static Bitmap compressionBitmap(@NonNull Bitmap bitmap) {
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 100, baos);
        if (baos.toByteArray().length / 1024 > 1024*2) {//判断如果图片大于2M,进行压缩避免在生成图片（BitmapFactory.decodeStream）时溢出
            baos.reset();//重置baos即清空baos
            bitmap.compress(Bitmap.CompressFormat.JPEG, 60, baos);//这里压缩60%，把压缩后的数据存放到baos中
        }
        ByteArrayInputStream isBm = new ByteArrayInputStream(baos.toByteArray());
        BitmapFactory.Options newOpts = new BitmapFactory.Options();
        //开始读入图片，此时把options.inJustDecodeBounds 设回true了
        newOpts.inJustDecodeBounds = true;
        BitmapFactory.decodeStream(isBm, null, newOpts);
        newOpts.inJustDecodeBounds = false;
        int w = newOpts.outWidth;
        int h = newOpts.outHeight;
        float hh = 720f;
        float ww = 1280f;
        //缩放比。由于是固定比例缩放，只用高或者宽其中一个数据进行计算即可
        int be = 1;//be=1表示不缩放
        if (w > h && w > ww) {//如果宽度大的话根据宽度固定大小缩放
            be = (int) (newOpts.outWidth / ww);
        } else if (w < h && h > hh) {//如果高度高的话根据宽度固定大小缩放
            be = (int) (newOpts.outHeight / hh);
        }
        if (be <= 0) {
            be = 1;
        }
        newOpts.inSampleSize = be;//设置缩放比例
        newOpts.inPreferredConfig = Bitmap.Config.RGB_565;//降低图片从ARGB888到RGB565
        //重新读入图片，注意此时已经把options.inJustDecodeBounds 设回false了
        isBm = new ByteArrayInputStream(baos.toByteArray());
        bitmap = BitmapFactory.decodeStream(isBm, null, newOpts);
        return bitmap;//压缩好比例大小后再进行质量压缩
    }

    /**
     *  byte[]转换成Bitmap
     */
    public static Bitmap bytes2Bitmap(byte[] bytes) {
        if (bytes.length != 0) {
            return BitmapFactory.decodeByteArray(bytes, 0, bytes.length);
        }else {
            return null;
        }
    }

    /**
    *Bitmap转换成byte[]
     */
    public static byte[] bitmap2Bytes(Bitmap bitmap) {
        ByteArrayOutputStream byteArrOutStream = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 100, byteArrOutStream);
        return byteArrOutStream.toByteArray();
    }

    /**
     * 按采样率压缩
     * @param bitmap
     * @param sampleSize
     * @return
     */
    public static Bitmap compressBitmapBySampleSize(@NonNull Bitmap bitmap, int sampleSize) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = sampleSize;
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 100, outputStream);
        return BitmapFactory.decodeByteArray(outputStream.toByteArray(), 0, outputStream.toByteArray().length, options);
    }

    @NonNull
    byte[] imageToJpegByteArray(@NonNull ImageProxy image, @IntRange(from = 1,
            to = 100) int jpegQuality) throws Exception {
        boolean shouldCropImage = shouldCropImage(image);
        int imageFormat = image.getFormat();

        if (imageFormat == ImageFormat.JPEG) {
            if (!shouldCropImage) {
                // When cropping is unnecessary, the byte array doesn't need to be decoded and
                // re-encoded again. Therefore, jpegQuality is unnecessary in this case.
                return jpegImageToJpegByteArray(image);
            } else {
                return jpegImageToJpegByteArray(image, image.getCropRect(), jpegQuality);
            }
        } else if (imageFormat == ImageFormat.YUV_420_888) {
            return yuvImageToJpegByteArray(image, shouldCropImage ? image.getCropRect() : null, jpegQuality);
        }

        return null;
    }

    @NonNull
    public static byte[] yuvImageToJpegByteArray(@NonNull ImageProxy image,
                                                 @Nullable Rect cropRect, @IntRange(from = 1, to = 100) int jpegQuality)
            throws Exception {
        if (image.getFormat() != ImageFormat.YUV_420_888) {
            throw new IllegalArgumentException(
                    "Incorrect image format of the input image proxy: " + image.getFormat());
        }

        return nv21ToJpeg(
                yuv_420_888toNv21(image),
                image.getWidth(),
                image.getHeight(),
                cropRect,
                jpegQuality);
    }

    @NonNull
    public static byte[] yuv_420_888toNv21(@NonNull ImageProxy image) {
        ImageProxy.PlaneProxy yPlane = image.getPlanes()[0];
        ImageProxy.PlaneProxy uPlane = image.getPlanes()[1];
        ImageProxy.PlaneProxy vPlane = image.getPlanes()[2];

        ByteBuffer yBuffer = yPlane.getBuffer();
        ByteBuffer uBuffer = uPlane.getBuffer();
        ByteBuffer vBuffer = vPlane.getBuffer();
        yBuffer.rewind();
        uBuffer.rewind();
        vBuffer.rewind();

        int ySize = yBuffer.remaining();

        int position = 0;
        // TODO(b/115743986): Pull these bytes from a pool instead of allocating for every image.
        byte[] nv21 = new byte[ySize + (image.getWidth() * image.getHeight() / 2)];

        // Add the full y buffer to the array. If rowStride > 1, some padding may be skipped.
        for (int row = 0; row < image.getHeight(); row++) {
            yBuffer.get(nv21, position, image.getWidth());
            position += image.getWidth();
            yBuffer.position(
                    Math.min(ySize, yBuffer.position() - image.getWidth() + yPlane.getRowStride()));
        }

        int chromaHeight = image.getHeight() / 2;
        int chromaWidth = image.getWidth() / 2;
        int vRowStride = vPlane.getRowStride();
        int uRowStride = uPlane.getRowStride();
        int vPixelStride = vPlane.getPixelStride();
        int uPixelStride = uPlane.getPixelStride();

        // Interleave the u and v frames, filling up the rest of the buffer. Use two line buffers to
        // perform faster bulk gets from the byte buffers.
        byte[] vLineBuffer = new byte[vRowStride];
        byte[] uLineBuffer = new byte[uRowStride];
        for (int row = 0; row < chromaHeight; row++) {
            vBuffer.get(vLineBuffer, 0, Math.min(vRowStride, vBuffer.remaining()));
            uBuffer.get(uLineBuffer, 0, Math.min(uRowStride, uBuffer.remaining()));
            int vLineBufferPosition = 0;
            int uLineBufferPosition = 0;
            for (int col = 0; col < chromaWidth; col++) {
                nv21[position++] = vLineBuffer[vLineBufferPosition];
                nv21[position++] = uLineBuffer[uLineBufferPosition];
                vLineBufferPosition += vPixelStride;
                uLineBufferPosition += uPixelStride;
            }
        }

        return nv21;
    }

    private static byte[] nv21ToJpeg(@NonNull byte[] nv21, int width, int height, @Nullable Rect cropRect, @IntRange(from = 1, to = 100) int jpegQuality)
            throws Exception {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        YuvImage yuv = new YuvImage(nv21, ImageFormat.NV21, width, height, null);
        boolean success =
                yuv.compressToJpeg(cropRect == null ? new Rect(0, 0, width, height) : cropRect,
                        jpegQuality, out);
        if (!success) {
            throw new Exception("YuvImage failed to encode jpeg.");
        }
        return out.toByteArray();
    }

    @NonNull
    private static byte[] jpegImageToJpegByteArray(@NonNull ImageProxy image,
                                                  @NonNull Rect cropRect, @IntRange(from = 1, to = 100) int jpegQuality)
            throws Exception {
        if (image.getFormat() != ImageFormat.JPEG) {
            throw new IllegalArgumentException(
                    "Incorrect image format of the input image proxy: " + image.getFormat());
        }

        byte[] data = jpegImageToJpegByteArray(image);
        data = cropJpegByteArray(data, cropRect, jpegQuality);


        return data;
    }

    @NonNull
    @SuppressWarnings("deprecation")
    private static byte[] cropJpegByteArray(@NonNull byte[] data, @NonNull Rect cropRect,
                                            @IntRange(from = 1, to = 100) int jpegQuality) throws Exception {
        Bitmap bitmap;
        try {
            BitmapRegionDecoder decoder = BitmapRegionDecoder.newInstance(data, 0, data.length,
                    false);
            bitmap = decoder.decodeRegion(cropRect, new BitmapFactory.Options());
            decoder.recycle();
        } catch (IllegalArgumentException e) {
            throw new Exception("Decode byte array failed with illegal argument." + e);
        } catch (IOException e) {
            throw new Exception("Decode byte array failed.");
        }

        if (bitmap == null) {
            throw new Exception("Decode byte array failed.");
        }

        ByteArrayOutputStream out = new ByteArrayOutputStream();
        boolean success = bitmap.compress(Bitmap.CompressFormat.JPEG, jpegQuality, out);
        if (!success) {
            throw new Exception("Encode bitmap failed.");
        }
        bitmap.recycle();

        return out.toByteArray();
    }


    private static boolean shouldCropImage(@NonNull ImageProxy image) {
        return shouldCropImage(image.getWidth(), image.getHeight(), image.getCropRect().width(),
                image.getCropRect().height());
    }

    public static boolean shouldCropImage(int sourceWidth, int sourceHeight, int cropRectWidth,
                                          int cropRectHeight) {
        return sourceWidth != cropRectWidth || sourceHeight != cropRectHeight;
    }

    @NonNull
    public static byte[] jpegImageToJpegByteArray(@NonNull ImageProxy image) {
        if (image.getFormat() != ImageFormat.JPEG) {
            throw new IllegalArgumentException(
                    "Incorrect image format of the input image proxy: " + image.getFormat());
        }

        ImageProxy.PlaneProxy[] planes = image.getPlanes();
        ByteBuffer buffer = planes[0].getBuffer();
        byte[] data = new byte[buffer.capacity()];
        buffer.rewind();
        buffer.get(data);

        return data;
    }
}
