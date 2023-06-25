#include<stdlib.h>
#include<math.h>
#include<string.h>
#include <jni.h>
#include <opencv2/beauty/FaceLandmark.h>
#include <opencv2/gapi/imgproc.hpp>
#include <opencv2/utils/utils.h>
#include"opencv2/utils/Commen.h"
#include"opencv2/lut/Lut.h"
#include"SkinPdf.h"
#include"opencv2/filter/GaussFilter.h"
#include"SkinColor.h"
using namespace std;
using namespace cv;

/*************************************************************************
*Function: SKIN COLOR
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*skinMask: skin mask
*lutData: 32BGRA buffer of include.opencv2.lut image.
*ratio: Intensity of skin colored,range [0,100]
*Return: 0-OK,other failed
**************************************************************************/
int f_SkinColor(unsigned char* srcData, int width, int height, int stride, unsigned char* lutData, unsigned char* mask, int ratio)
{
	int ret = 0;
	int length = height * stride;
	unsigned char* tempData = (unsigned char*)malloc(sizeof(unsigned char) * length);
	memcpy(tempData, srcData, sizeof(unsigned char) * length);
	unsigned char* skinPDF = (unsigned char*)malloc(sizeof(unsigned char) * length);
	memcpy(skinPDF, srcData, sizeof(unsigned char) * length);
	ret = f_SkinPDF(skinPDF, width, height, stride);
	int maskSmoothRadius = 3;
//	ret = f_FastGaussFilter(skinPDF, width, height, stride, maskSmoothRadius);
	Mat tmpGauss(Size(width,height),CV_8UC4,skinPDF);
	cv::GaussianBlur(tmpGauss, tmpGauss, cv::Size(51, 51), 0, 0);
	ret = f_LUTFilter(tempData, width, height, stride, lutData);
	unsigned char* pSrc = srcData;
	unsigned char* pLut = tempData;
	unsigned char* pSkin = tmpGauss.data;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			uchar maxMask = MAX(mask[0], MAX(mask[1], mask[2]));
			if (maxMask == 0)
			{
				pSrc += 4;
				pLut += 4;
				pSkin += 4;
				mask +=4;
				continue;
			}
			int r, g, b, a;
			b = CLIP3((pSrc[0] * (100 - ratio) + pLut[0] * ratio) / 100, 0, 255);
			g = CLIP3((pSrc[1] * (100 - ratio) + pLut[1] * ratio) / 100, 0, 255);
			r = CLIP3((pSrc[2] * (100 - ratio) + pLut[2] * ratio) / 100, 0, 255);
			a = (pSkin[0] + pSkin[1] + pSkin[2]) / 3;
			int tb = CLIP3((b * a + pSrc[0] * (255 - a)) / 255, 0, 255);
			int tg = CLIP3((g * a + pSrc[1] * (255 - a)) / 255, 0, 255);
			int tr = CLIP3((r * a + pSrc[2] * (255 - a)) / 255, 0, 255);
			pSrc[0] = pSrc[0]*(1-maxMask/255.0)+tb*(maxMask/255.0);
			pSrc[1] = pSrc[1]*(1-maxMask/255.0)+tg*(maxMask/255.0);
			pSrc[2] = pSrc[2]*(1-maxMask/255.0)+tr*(maxMask/255.0);
			pSrc += 4;
			pLut += 4;
			pSkin += 4;
            mask +=4;
        }
	}
	free(tempData);
	free(skinPDF);
	tmpGauss.release();
	return ret;
}

/**
 * 修改肤色
 * @param env
 * @param clazz
 * @param bitmap
 * @param lutMap
 * @param strength
 * @return
 */
jobject changeSkinColor(JNIEnv *env, jclass clazz, jobject bitmap, jobject lutMap, jobject skinMask,jint strength) {
	AndroidBitmapInfo info;
	void *pixels;
	CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
	CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
	CV_Assert(pixels);
	cv::Mat image(info.height, info.width, CV_8UC4, pixels);
	cv::Mat result = image.clone();
    cvtColor(image,image,COLOR_RGBA2BGRA);
    cvtColor(result,result,COLOR_RGBA2BGRA);


	AndroidBitmapInfo map;
	void *pixelsMap;
	CV_Assert(AndroidBitmap_getInfo(env, lutMap, &map) >= 0);
	CV_Assert(map.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  map.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, lutMap, &pixelsMap) >= 0);
	CV_Assert(pixelsMap);
	cv::Mat mapMat(map.height, map.width, CV_8UC4, pixelsMap);
	cvtColor(mapMat,mapMat,COLOR_RGBA2BGRA);

	AndroidBitmapInfo mask;
	void *pixelsMask;
	CV_Assert(AndroidBitmap_getInfo(env, skinMask, &mask) >= 0);
	CV_Assert(mask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
					  mask.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, skinMask, &pixelsMask) >= 0);
	CV_Assert(pixelsMask);
	cv::Mat mapMask(mask.height, mask.width, CV_8UC4, pixelsMask);
	cvtColor(mapMask,mapMask,COLOR_RGBA2BGRA);


	f_SkinColor(result.data,result.cols,result.rows,result.step,mapMat.data,mapMask.data,strength);
	cvtColor(result,result,COLOR_BGRA2RGBA);
	cvtColor(image,image,COLOR_BGRA2RGBA);
	cvtColor(mapMat,mapMat,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, result, true, bitmap);
	image.release();
	result.release();
	mapMat.release();
	mapMask.release();
	AndroidBitmap_unlockPixels(env, bitmap);
	AndroidBitmap_unlockPixels(env, lutMap);
	AndroidBitmap_unlockPixels(env, skinMask);
	return resultBitmap;
}

/**
 * 去除蒙版中皮肤部分颜色
 * @return
 */
jobject RemoveHairMaskSkinColorAre(JNIEnv *env, jclass clazz, jobject hairMask, jobject headMask,jobject skinMask){
	AndroidBitmapInfo info;
	void *pixels;
	CV_Assert(AndroidBitmap_getInfo(env, hairMask, &info) >= 0);
	CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, hairMask, &pixels) >= 0);
	CV_Assert(pixels);
	cv::Mat hairMaskMap(info.height, info.width, CV_8UC4, pixels);
	cvtColor(hairMaskMap,hairMaskMap,COLOR_RGBA2BGR);

	AndroidBitmapInfo mask;
	void *pixelsMask;
	CV_Assert(AndroidBitmap_getInfo(env, headMask, &mask) >= 0);
	CV_Assert(mask.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  mask.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, headMask, &pixelsMask) >= 0);
	CV_Assert(pixelsMask);
	cv::Mat headMaskMap(mask.height, mask.width, CV_8UC4, pixelsMask);
	cvtColor(headMaskMap,headMaskMap,COLOR_RGBA2BGR);

	AndroidBitmapInfo maskSkin;
	void *pixelsMaskSkin;
	CV_Assert(AndroidBitmap_getInfo(env, skinMask, &maskSkin) >= 0);
	CV_Assert(maskSkin.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
					  maskSkin.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, skinMask, &pixelsMaskSkin) >= 0);
	CV_Assert(pixelsMaskSkin);
	cv::Mat maskMatSkin(maskSkin.height, maskSkin.width, CV_8UC4, pixelsMaskSkin);
	cvtColor(maskMatSkin,maskMatSkin,COLOR_RGBA2BGR);

	cv::Mat result = hairMaskMap.clone();

	int width = hairMaskMap.cols * hairMaskMap.channels();
	int height = hairMaskMap.rows;
	uchar channel = hairMaskMap.channels();
	for (int i = 0; i < height; i++) {
		const uchar *hairIn = hairMaskMap.ptr<uchar>(i);
		const uchar *headIn = headMaskMap.ptr<uchar>(i);
		const uchar *skinIn = maskMatSkin.ptr<uchar>(i);
		uchar *out = result.ptr<uchar>(i);
		for (int j = 0; j < width; j += channel) {
			uchar maxHair = MAX(hairIn[j + 0], MAX(hairIn[j + 1], hairIn[j + 2]));
			uchar maxHead = MAX(headIn[j + 0], MAX(headIn[j + 1], headIn[j + 2]));
			if (maxHair == 0) {
				out[j + 2] = 0;
				out[j + 1] = 0;
				out[j] = 0;
				continue;
			}
			if(maxHead != 0){
				out[j + 2] = skinIn[j+2];
				out[j + 1] = skinIn[j+1];
				out[j] = skinIn[j];
			}else{
				out[j + 2] = hairIn[j+2];
				out[j + 1] = hairIn[j+1];
				out[j] = hairIn[j];
			}
		}
	}
	cvtColor(hairMaskMap,hairMaskMap,COLOR_BGRA2RGBA);
	cvtColor(headMaskMap,headMaskMap,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, result, true, hairMask);
	headMaskMap.release();
	hairMaskMap.release();
	maskMatSkin.release();
	result.release();
	AndroidBitmap_unlockPixels(env, hairMask);
	AndroidBitmap_unlockPixels(env, headMask);
	AndroidBitmap_unlockPixels(env, skinMask);
	return resultBitmap;
}

/**
 * 获取皮肤遮罩
 * @return
 */
jobject GetSkinMask(JNIEnv *env, jclass clazz, jobject bitmap){
	AndroidBitmapInfo info;
	void *pixels;
	CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
	CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
	CV_Assert(pixels);
	cv::Mat image(info.height, info.width, CV_8UC4, pixels);
	cvtColor(image,image,COLOR_RGBA2BGRA);
	cv::Mat result = image.clone();
	f_SkinPDF(result.data, result.cols,result.rows,result.step);
	cvtColor(image,image,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, result, true, bitmap);
	image.release();
	result.release();
	AndroidBitmap_unlockPixels(env, bitmap);
	return resultBitmap;

}

