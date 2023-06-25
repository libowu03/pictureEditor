#include<stdlib.h>
#include<math.h>
#include<string.h>
#include <jni.h>
#include <opencv2/utils/utils.h>
#include"opencv2/utils/Commen.h"
#include"BeautyIris.h"
#include"opencv2/colorSpaceChange/TRGB2YUV.h"
#include"MeanFilter.h"
#include "FaceLandmark.h"
#define TAG "日志"
// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

bool f_PNPoly_OPT(int xPoint[],int yPoint[],int pointNum,int x, int y, int minX, int minY, int maxX, int maxY)
{
	if(x < minX || x > maxX || y < minY || y > maxY)
		return false;
	int i, j, c = 0;
	for(i = 0, j = pointNum - 1; i < pointNum; j = i++)
	{
		if((float)((yPoint[i] > y) != (yPoint[j] > y)) && (x < (float)(xPoint[j] - xPoint[i]) * (y - yPoint[i]) / (yPoint[j] - yPoint[i]) + xPoint[i]))
			c = !c;
	}
	return c;
}
/************************************************************
*Function:  f_PNPoly
*Description: Point in the poly or not
*Params:    
*xPoint:  x position or several points input.   
*yPoint:  y position or several points input.   
*pointNum :number of points input.
*(x,y): the current position.
*Return :true or false.    
************************************************************/
bool f_PNPoly(int xPoint[],int yPoint[],int pointNum,int x, int y)
{
	int i, j, c = 0;
	for(i = 0, j = pointNum - 1; i < pointNum; j = i++)
	{
		if((float)((yPoint[i] > y) != (yPoint[j] > y)) && (x < (float)(xPoint[j] - xPoint[i]) * (y - yPoint[i]) / (yPoint[j] - yPoint[i]) + xPoint[i]))
			c = !c;
	}
	return c;
}
/************************************************************
*Function:  Beauty Iris
*Description: Beauty Iris process
*Params:    
*srcData:  image bgra data     
*width  :image width
*height :image height
*stride :image stride
*srcPoints:  13 points of 101 face points.
*maskData:   iris mask data
*mWidth:  width of mask.
*mHeight: height of mask.
*mStride: stride of mask.
maskPoints: center point of mask.
*ratio:  intensity of beauty iris, [0, 100].
*Return :0-OK,or failed    
************************************************************/
int f_BeautyIris(unsigned char* srcData, int width, int height, int stride, int srcPoints[], unsigned char* maskData, int mWidth, int mHeight, int mStride, int ratio)
{
	int ret = 0;
	unsigned char* pSrc = srcData;
	int cenx = srcPoints[2 * 8];
	int ceny = srcPoints[2 * 8 + 1];
	//compute iris radius.
	int eyedistance = sqrt((float)(srcPoints[2 * 0] - srcPoints[2 * 4]) * (srcPoints[2 * 0] - srcPoints[2 * 4]) + (srcPoints[2 * 0 + 1] - srcPoints[2 * 4 + 1]) * (srcPoints[2 * 0 + 1] - srcPoints[2 * 4 + 1])) / 2;
	int irisRadius = eyedistance / 2;
	//compute the rectangle of eye area.
	int px = CLIP3(cenx - irisRadius, 0, width - 1);
	int py = CLIP3(ceny - irisRadius, 0, height - 1);
	int pmaxX = CLIP3(cenx + irisRadius, 0, width - 1);
	int pmaxY = CLIP3(ceny + irisRadius, 0, height - 1);
	int w = pmaxX - px;
	int h = pmaxY - py;
	const int pointNum = 8;
	int xPoints[pointNum];
	int yPoints[pointNum];
	int maxX = 0, minX = 100000, maxY = 0, minY = 100000;
	for(int i = 0; i < pointNum; i++)
	{
		xPoints[i] = srcPoints[2 * i];
		yPoints[i] = srcPoints[2 * i + 1];
		maxX = MAX2(srcPoints[2 * i], maxX);
		maxY = MAX2(srcPoints[2 * i + 1], maxY);
		minX = MIN2(srcPoints[2 * i], minX);
		minY = MIN2(srcPoints[2 * i + 1], minY);
	}
	//irisMask feather process
    unsigned char* irisMask = (unsigned char*)malloc(sizeof(unsigned char) * w * h * 4);
	unsigned char* pMask = irisMask;
	for(int j = 0; j < h; j++)
	{
		for(int i = 0; i < w; i++)
		{
			int dis = sqrt((float)(i - irisRadius) * (i - irisRadius) + (j - irisRadius) * (j - irisRadius));
			if(f_PNPoly_OPT(xPoints, yPoints, pointNum, i + px, j + py, minX, minY, maxX, maxY) && dis < irisRadius-3)
				pMask[0] = pMask[1] = pMask[2] = 255;
			else
				pMask[0] = pMask[1] = pMask[2] = 0;
			pMask += 4;
		}
	}
	f_FastMeanFilter(irisMask, w, h, w * 4, 3);
	pMask = irisMask;
	for(int j = 0; j < h; j++)
	{
		for(int i = 0; i < w; i++)
		{
			if(pMask[0] > 128)
				pMask[0] = pMask[1] = pMask[2] = 255;
			else
				pMask[0] = pMask[1] = pMask[2] = 0;
			pMask += 4;
		}
	}
	f_FastMeanFilter(irisMask, w, h, w * 4, 3);
	//beauty iris process
	pMask = irisMask;
	for(int j = 0; j < h; j++)
	{
		for(int i = 0; i < w; i++)
		{
			int a = pMask[0];
			int b = pMask[1];
			int c = pMask[2];
			if (a != 0 || b != 0 || c != 0){
				LOGI("执行非0");
			}
			if((pMask[0] + pMask[1] + pMask[2]) / 3 > 0)
			{
			    int pos_src = (px + i) * 4 + (py + j) * stride;
			    int mx = i * mWidth / w;
			    int my = j * mHeight / h;
			    int pos_mask = mx * 4 + my * mStride;
			    int r = srcData[pos_src + 2];
			    int g = srcData[pos_src + 1];
			    int b = srcData[pos_src];
			    int mr = maskData[pos_mask + 2];
			    int mg = maskData[pos_mask + 1];
			    int mb = maskData[pos_mask];
			    int Y, U, V, mY, mU, mV;
				//YUV
			    RGB2YUV(r, g, b,&Y, &U, &V);
			    RGB2YUV(mr, mg, mb, &mY, &mU, &mV);
			    unsigned char R, G, B;
			    YUV2RGB(Y, mU, mV, &R, &G, &B);
				//feather mask process
				int a = (pMask[0] + pMask[1] + pMask[2]) / 3;
				B = CLIP3((b * (255 - a) + B * a) >> 8, 0, 255);
				G = CLIP3((g * (255 - a) + G * a) >> 8, 0, 255);
				R = CLIP3((r * (255 - a) + R * a) >> 8, 0, 255);
				//YUV color correct
			    a = CLIP3((r + g + b) / 3 * 2, 0, 255);
				R = (R * a + r * (255 - a)) / 255;
				G = (G * a + g * (255 - a)) / 255;
				B = (B * a + b * (255 - a)) / 255;
				//ratio blending
				B = CLIP3((b * (100 - ratio) + B * ratio) / 100, 0, 255);
				G = CLIP3((g * (100 - ratio) + G * ratio) / 100, 0, 255);
				R = CLIP3((r * (100 - ratio) + R * ratio) / 100, 0, 255);

				srcData[pos_src]     = B;
				srcData[pos_src + 1] = G;
				srcData[pos_src + 2] = R;

				/*srcData[pos_src]     = 0;
				srcData[pos_src + 1] = 0;
				srcData[pos_src + 2] = 0;*/
			}
			pMask += 4;
		}
	}
	free(irisMask);
	return ret;
};

jobject beautyIris(JNIEnv *env, jclass clazz, jobject bitmap,jobject maskBitmap,jint strength){
	std::vector<FaceKeyPoint>  result = FaceLandmark::detector->detect(env, bitmap);
	if (result.size() == 0){
		return bitmap;
	}
	AndroidBitmapInfo info;
	void *pixels;
	CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
	CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
	CV_Assert(pixels);
	cv::Mat img_src(info.height, info.width, CV_8UC4, pixels);
	cv::Mat bgr = img_src.clone();
	cvtColor(bgr,bgr,COLOR_RGBA2BGRA);


	AndroidBitmapInfo mskLeft;
	void *pixelsMskLeft;
	CV_Assert(AndroidBitmap_getInfo(env, maskBitmap, &mskLeft) >= 0);
	CV_Assert(mskLeft.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
			  mskLeft.format == ANDROID_BITMAP_FORMAT_RGB_565);
	CV_Assert(AndroidBitmap_lockPixels(env, maskBitmap, &pixelsMskLeft) >= 0);
	CV_Assert(pixelsMskLeft);
	cv::Mat mskMatLeft(mskLeft.height, mskLeft.width, CV_8UC4, pixelsMskLeft);
	cvtColor(mskMatLeft,mskMatLeft,COLOR_RGBA2BGRA);


	int point[973 * 2] = {0};
	int irisPoint[9*2] = {0};
	int irisRightPoint[9*2] = {0};
	//有可能识别到多张脸，只选取第一张脸
	for (int i = 0; i < 973; i++) {
		point[i * 2] = result[i].p.x;
		point[i * 2 + 1] = result[i].p.y;
	}

	irisPoint[0] = point[104];
	irisPoint[1] = point[105];

	irisPoint[2] = point[106];
	irisPoint[3] = point[107];

	irisPoint[4] = point[144];
	irisPoint[5] = point[145];

	irisPoint[6] = point[108];
	irisPoint[7] = point[109];

	irisPoint[8] = point[110];
	irisPoint[9] = point[111];

	irisPoint[10] = point[112];
	irisPoint[11] = point[113];

	irisPoint[12] = point[146];
	irisPoint[13] = point[147];

	irisPoint[14] = point[114];
	irisPoint[15] = point[115];

	irisPoint[16] = point[208];
	irisPoint[17] = point[209];

	irisRightPoint[0] = point[116];
	irisRightPoint[1] = point[117];

	irisRightPoint[2] = point[118];
	irisRightPoint[3] = point[119];

	irisRightPoint[4] = point[150];
	irisRightPoint[5] = point[151];

	irisRightPoint[6] = point[120];
	irisRightPoint[7] = point[121];

	irisRightPoint[8] = point[122];
	irisRightPoint[9] = point[123];

	irisRightPoint[10] = point[124];
	irisRightPoint[11] = point[125];

	irisRightPoint[12] = point[152];
	irisRightPoint[13] = point[153];

	irisRightPoint[14] = point[126];
	irisRightPoint[15] = point[127];

	irisRightPoint[16] = point[210];
	irisRightPoint[17] = point[211];

	//circle(bgr,Point(irisRightPoint[0],irisRightPoint[1]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[2],irisRightPoint[3]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[4],irisRightPoint[5]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[6],irisRightPoint[7]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[8],irisRightPoint[9]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[10],irisRightPoint[11]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[12],irisRightPoint[13]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[14],irisRightPoint[15]),5,Scalar(255,20,0,255));
	//circle(bgr,Point(irisRightPoint[16],irisRightPoint[17]),5,Scalar(255,20,0,255));

	f_BeautyIris(bgr.data,bgr.cols,bgr.rows,bgr.step,irisPoint,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,strength);
	f_BeautyIris(bgr.data,bgr.cols,bgr.rows,bgr.step,irisRightPoint,mskMatLeft.data,mskMatLeft.cols,mskMatLeft.rows,mskMatLeft.step,strength);
	cvtColor(bgr,bgr,COLOR_BGRA2RGBA);
	jobject resultBitmap = matToBitmap(env, bgr, true, bitmap);
	img_src.release();
	bgr.release();
	mskMatLeft.release();
	AndroidBitmap_unlockPixels(env, bitmap);
	AndroidBitmap_unlockPixels(env, maskBitmap);
	return resultBitmap;
}