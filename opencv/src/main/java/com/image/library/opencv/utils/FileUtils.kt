package com.image.library.opencv.utils

import android.app.Activity
import android.content.ContentValues
import android.content.Context
import android.graphics.Bitmap
import android.net.Uri
import android.os.Environment
import android.provider.MediaStore
import android.util.Log
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.OutputStream
import java.text.SimpleDateFormat
import java.util.*


/**
 * @author:libowu
 * @Date:2021/9/13
 * @Description:
 */
object FileUtils {

    /**
     * 获取App私有缓存目录
     */
    fun getCacheFile(): File? {
        val cacheFile = Applications.context().externalCacheDir
        return cacheFile
    }

    /**
     * 获取滤镜文件缓存地址
     */
    fun getFilterCacheFile(): File {
        val cacheFile = getCacheFile()
        cacheFile?.let {
            val cacheFilterFile = File(cacheFile.path + "/cube")
            if (!cacheFilterFile.exists()) {
                val createSuccess = cacheFilterFile.mkdirs()
                if (createSuccess) {
                    return cacheFilterFile
                } else {
                    return File("")
                }
            } else {
                return cacheFilterFile
            }
        }
        return File("")
    }

    /**
     * 获取天空替换图片
     */
    fun getSkyChangeFile():File{
        val cacheFile = getCacheFile()
        cacheFile?.let {
            val cacheFilterFile = File(cacheFile.path + "/skyChange")
            if (!cacheFilterFile.exists()) {
                val createSuccess = cacheFilterFile.mkdirs()
                if (createSuccess) {
                    return cacheFilterFile
                } else {
                    return File("")
                }
            } else {
                return cacheFilterFile
            }
        }
        return File("")
    }


    /**
     * 获取字体文件缓存地址
     */
    fun getFontCacheFile(): File {
        val cacheFile = getCacheFile()
        cacheFile?.let {
            val cacheFontsFile = File(cacheFile.path + "/Fonts")
            if (!cacheFontsFile.exists()) {
                val createSuccess = cacheFontsFile.mkdirs()
                if (createSuccess) {
                    return cacheFontsFile
                } else {
                    return File("")
                }
            } else {
                return cacheFontsFile
            }
        }
        return File("")
    }

    /**
     * 读取贴纸文件列表
     */
    fun getStickerCacheFile():File{
        val cacheFile = getCacheFile()
        cacheFile?.let {
            val cacheFontsFile = File(cacheFile.path + "/Sticker")
            if (!cacheFontsFile.exists()) {
                val createSuccess = cacheFontsFile.mkdirs()
                if (createSuccess) {
                    return cacheFontsFile
                } else {
                    return File("")
                }
            } else {
                return cacheFontsFile
            }
        }
        return File("")
    }

    /**
     * 获取模型存放地点
     */
    fun getModelCacheFile():File{
        val cache = getCacheFile()?:File("")
        val tmpFile = File(cache.path+"/model")
        if (!tmpFile.exists()){
            tmpFile.mkdir()
        }
        return tmpFile;
    }

    /**
     * 获取配置存放位置
     */
    fun getConfigCacheFile():File{
        val cache = getCacheFile()?:File("")
        val tmpFile = File(cache.path+"/config")
        if (!tmpFile.exists()){
            tmpFile.mkdir()
        }
        return tmpFile;
    }

    /**
     * 获取配置存放位置
     */
    fun getEffectCacheFile():File{
        val cache = getCacheFile()?:File("")
        val tmpFile = File(cache.path+"/effect")
        if (!tmpFile.exists()){
            tmpFile.mkdir()
        }
        return tmpFile;
    }

    /**
     * 获取滤镜文件封面的地址
     */
    fun getFilterCoverFile(): File? {
        val cacheFile = getCacheFile()
        cacheFile?.let {
            val filterCover = File(cacheFile.path + "/filterCover")
            if (!filterCover.exists()) {
                val createSuccess = filterCover.mkdirs()
                if (createSuccess) {
                    return filterCover
                } else {
                    return null
                }
            } else {
                return filterCover
            }
        }
        return null
    }

    /**
     * 获取app公有目录，即在手机根目录下的App文件
     * 注意：需要读写权限，否则无法获取
     */
    fun getAppPublicFile(): File? {
        val file = Environment.getExternalStorageDirectory()
        val publicFile = File(file.path + "/MasterToner")
        if (publicFile.exists()) {
            return publicFile
        } else {
            val createSuccess = publicFile.mkdirs()
            if (createSuccess) {
                return publicFile
            } else {
                return null
            }
        }
    }

    fun getProjectBitmapCacheFile():File{
        return File((getCacheFile()?.path?:"")+"/project")
    }

    /**
     * 获取导入滤镜文件的地址
     */
    fun getAppFilterImportFile(): File? {
        val file = getAppPublicFile()
        file?.let {
            val filterImportFile = File(file.path + "/Filter")
            if (!filterImportFile.exists()) {
                val createSuccess = filterImportFile.mkdirs()
                if (createSuccess) {
                    return filterImportFile
                } else {
                    return null
                }
            } else {
                return filterImportFile
            }
        }
        return null
    }

    /**
     * 获取导入滤镜文件的地址
     */
    fun getAppFontImportFile(): File? {
        val file = getAppPublicFile()
        file?.let {
            val fontsImportFile = File(file.path + "/Fonts")
            if (!fontsImportFile.exists()) {
                val createSuccess = fontsImportFile.mkdirs()
                if (createSuccess) {
                    return fontsImportFile
                } else {
                    return null
                }
            } else {
                return fontsImportFile
            }
        }
        return null
    }

    /**
     * 获取导入贴纸文件的地址
     */
    fun getAppStickerImportFile(): File? {
        val file = getAppPublicFile()
        file?.let {
            val fontsImportFile = File(file.path + "/Sticker")
            if (!fontsImportFile.exists()) {
                val createSuccess = fontsImportFile.mkdirs()
                if (createSuccess) {
                    return fontsImportFile
                } else {
                    return null
                }
            } else {
                return fontsImportFile
            }
        }
        return null
    }


    fun getFileByUri(activity: Activity, uri: Uri): File? {
        var path: String? = null
        if ("file" == uri.scheme) {
            path = uri.encodedPath
            if (path != null) {
                path = Uri.decode(path)
                val cr = activity.contentResolver
                val buff = StringBuffer()
                buff.append("(").append(MediaStore.Images.ImageColumns.DATA).append("=").append(
                    "'$path'"
                ).append(")")
                val cur = cr.query(
                    MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
                    arrayOf(
                        MediaStore.Images.ImageColumns._ID,
                        MediaStore.Images.ImageColumns.DATA
                    ),
                    buff.toString(),
                    null,
                    null
                )
                var index = 0
                var dataIdx = 0
                cur!!.moveToFirst()
                while (!cur.isAfterLast) {
                    index = cur.getColumnIndex(MediaStore.Images.ImageColumns._ID)
                    index = cur.getInt(index)
                    dataIdx = cur.getColumnIndex(MediaStore.Images.ImageColumns.DATA)
                    path = cur.getString(dataIdx)
                    cur.moveToNext()
                }
                cur.close()
                if (index == 0) {
                } else {
                    val u = Uri.parse("content://media/external/images/media/$index")
                    println("temp uri is :$u")
                }
            }
            if (path != null) {
                return File(path)
            }
        } else if ("content" == uri.scheme) {
            // 4.2.2以后
            val proj = arrayOf(MediaStore.Images.Media.DATA)
            val cursor = activity.contentResolver.query(uri, proj, null, null, null)
            if (cursor!!.moveToFirst()) {
                val columnIndex = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA)
                path = cursor.getString(columnIndex)
            }
            cursor.close()
            return File(path)
        } else {
            //Log.i(TAG, "Uri Scheme:" + uri.getScheme());
        }
        return null
    }

    /**
     * 导出图片
     */
    fun saveToCameraFolder(context: Context, bitmap: Bitmap, quality: Int): Boolean {
        return try {
            val values = ContentValues()
            val name = SimpleDateFormat("yy-mm-dd", Locale.US).format(System.currentTimeMillis())
            values.put(MediaStore.Images.Media.DESCRIPTION, "This is an image")
            values.put(MediaStore.Images.Media.DISPLAY_NAME, "Image_${name}.jpg")
            /*if (bitmap.hasAlpha()){
                values.put(MediaStore.Images.Media.MIME_TYPE, "image/png")
                Log.i("测试日志","执行png")
            }else{
                values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg")
                Log.i("测试日志","执行jpeg")
            }*/
            values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg")
//            values.put(MediaStore.Images.Media.TITLE, System.currentTimeMillis().toString() + ".jpg")
            values.put(MediaStore.Images.Media.RELATIVE_PATH, "DCIM/Camera")

            val external = MediaStore.Images.Media.EXTERNAL_CONTENT_URI
            val resolver = context.contentResolver

            val insertUri = resolver.insert(external, values);
            var os: OutputStream? = null
            if (insertUri != null) {
                try {
                    os = resolver.openOutputStream(insertUri)
                    bitmap.compress(Bitmap.CompressFormat.JPEG, quality, os)
                } catch (e: IOException) {
                    e.printStackTrace()
                } finally {
                    try {
                        if (os != null) {
                            os.close()
                        }
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }
            }
            true
        } catch (e: Exception) {
            e.printStackTrace()
            Log.d("日志", "保存失败：${e.localizedMessage}")
            false
        }
    }


    fun cacheBitmap(bm: Bitmap): Uri? {
        val fileName = "${System.currentTimeMillis()}.jpg"
        return cache(bm, getCachePath(), fileName)
    }

    private fun cache(bm: Bitmap, dir: File?, name: String): Uri? {
        if (dir == null) {
            return null
        }
        return with(File(dir, name)) {
            bm.save(this)
            Uri.fromFile(this)
        }
    }

    fun Bitmap.save(file: File) {
        if (!file.exists()) {
            file.parentFile.mkdirs()
            file.createNewFile()
        }
        val out = FileOutputStream(file)
        this.compress(Bitmap.CompressFormat.JPEG, 100, out)
        out.flush()
        out.close()
    }

    private fun getCachePath(): File? {
        val pathCamera =
            "${Applications.context().externalCacheDir?.path ?: ""}${File.separator}Camera"
        var f: File = File(pathCamera)
        return f
    }

}