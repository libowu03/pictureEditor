package com.tuji.pictureeditor

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Canvas
import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.ImageView
import com.image.library.opencv.OpenCvIn
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val image = findViewById<ImageView>(R.id.vImageCover)
        val bitmap = BitmapFactory.decodeResource(resources,R.mipmap.yu)
        image.setImageBitmap(bitmap)
        val bitmapMask = Bitmap.createBitmap(bitmap.width,bitmap.height,Bitmap.Config.ARGB_8888)
        val can = Canvas(bitmapMask)
        can.drawColor(Color.WHITE)
        findViewById<Button>(R.id.vBtnUse).setOnClickListener {
            CoroutineScope(Dispatchers.IO).launch {
                val result = OpenCvIn.makeBitmapToPixel(bitmap,bitmapMask,5)
                runOnUiThread{
                    image.setImageBitmap(result)
                }
            }
        }
    }
}