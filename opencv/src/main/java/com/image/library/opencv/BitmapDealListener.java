package com.image.library.opencv;

import android.graphics.Bitmap;

public interface BitmapDealListener {
    void onSuccess(Bitmap source, Bitmap result);
    void onError(Bitmap bitmap, String msg);
}
