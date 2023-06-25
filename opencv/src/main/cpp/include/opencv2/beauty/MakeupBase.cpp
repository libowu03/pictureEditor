#include"opencv2/utils/Commen.h"
#include"MakeupBase.h"
#include"AffineTransfrom.h"
#include<math.h>
#include <jni.h>
#include <opencv2/utils/utils.h>
#include"opencv2/skin/CommenMixLayer.h"
#include "FaceLandmark.h"

/*************************************************************************
*Function: Makeup base with Nearest Neighbour Interpolation
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*srcKeyPoints: 3 key points of source image.
*mskData: mask image buffer with format bgra32
*mWidth: width of mask image 
*mHeight: height of mask image
*mStride: Stride of mask image
*maskKeyPoints�� 3 key points of blush mask.
*ratio: intensity of effect, [0, 100]
*Return: 0-OK,other failed
**************************************************************************/
int f_MakeupBaseN(unsigned char* srcData, int width, int height, int stride, int srcKeyPoints[2 * 3], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], int ratio)
{
	int ret = 0; 
	float H[6] = {0};
	f_AffinetransformMetrixCompute((float)maskKeyPoints[0],(float)maskKeyPoints[1],(float)maskKeyPoints[2],(float)maskKeyPoints[3],(float)maskKeyPoints[4],(float)maskKeyPoints[5], (float)srcKeyPoints[0], (float)srcKeyPoints[1], (float)srcKeyPoints[2], (float)srcKeyPoints[3], (float)srcKeyPoints[4], (float)srcKeyPoints[5], H);
	unsigned char* pSrc = srcData;
	int alpha = 128 * ratio / 100;
	int nalpha = 128 - alpha;
	int AR, AG, AB, AA;
	int x0, y0, index_x0y0, index_x0y1, index_x1y0, index_x1y1;
	int disX, disY, tmp1, tmp2;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			int x0 = (H[0] * i + H[1] * j + H[2]);
	        int y0 = (H[3] * i + H[4] * j + H[5]);
			if(x0 > 0 && x0 < mWidth - 1 && y0 > 0 && y0 < mHeight - 1)
			{
			    int pos = (x0 << 2) + y0 * mStride;
			    int k = mskData[pos + 3];
			    int b = (k * mskData[pos]     + (255 - k) * pSrc[0]) / 255;
			    int g = (k * mskData[pos + 1] + (255 - k) * pSrc[1]) / 255;
			    int r = (k * mskData[pos + 2] + (255 - k) * pSrc[2]) / 255;
			    pSrc[0] = ((alpha * b + nalpha * pSrc[0]) >> 7);
			    pSrc[1] = ((alpha * g + nalpha * pSrc[1]) >> 7);
			    pSrc[2] = ((alpha * r + nalpha * pSrc[2]) >> 7);
			}
			pSrc += 4;
		}
	}
	return ret;
};


/*************************************************************************
*Function: Makeup base with bilinear interpolation
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*srcKeyPoints: 3 key points of source image.
*mskData: mask image buffer with format bgra32
*mWidth: width of mask image 
*mHeight: height of mask image
*mStride: Stride of mask image
*maskKeyPoints�� 3 key points of blush mask.
*ratio: intensity of effect, [0, 100]
*Return: 0-OK,other failed
**************************************************************************/
int f_MakeupBase(unsigned char* srcData, int width, int height, int stride, int srcKeyPoints[2 * 3], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], int ratio)
{
	int ret = 0; 
	float H[6] = {0};
	f_AffinetransformMetrixCompute((float)maskKeyPoints[0],(float)maskKeyPoints[1],(float)maskKeyPoints[2],(float)maskKeyPoints[3],(float)maskKeyPoints[4],(float)maskKeyPoints[5], (float)srcKeyPoints[0], (float)srcKeyPoints[1], (float)srcKeyPoints[2], (float)srcKeyPoints[3], (float)srcKeyPoints[4], (float)srcKeyPoints[5], H);
	unsigned char* pSrc = srcData;
	int alpha = 128 * ratio / 100;
	int nalpha = 128 - alpha;
	int AR, AG, AB, AA;
	int x0, y0, index_x0y0, index_x0y1, index_x1y0, index_x1y1;
	int disX, disY, tmp1, tmp2;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			float cx = (H[0] * i + H[1] * j + H[2]);
	        float cy = (H[3] * i + H[4] * j + H[5]);
			if(cx > 0 && cx < mWidth - 1 && cy > 0 && cy < mHeight - 1)
			{

				x0 = (int)CLIP3(floor(cx), 0, mWidth - 2);
				y0 = (int)CLIP3(floor(cy), 0, mHeight - 2);
				index_x0y0 = (x0 << 2) + y0 * mStride;
				index_x1y0 = index_x0y0 + 4;
				index_x0y1 = index_x0y0 + mStride;
				index_x1y1 = index_x0y1 + 4;
				disX = (int)((cx - x0) * 16384);
				disY = (int)((cy - y0) * 16384);	
				tmp1 = mskData[index_x0y0] + ((disX *(mskData[index_x1y0] - mskData[index_x0y0])) >> 14);
				tmp2 = mskData[index_x0y1] + ((disX *(mskData[index_x1y1] - mskData[index_x0y1])) >> 14);
				AB = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				tmp1 = mskData[index_x0y0 + 1] + ((disX *(mskData[index_x1y0 + 1] - mskData[index_x0y0 + 1])) >> 14);
				tmp2 = mskData[index_x0y1 + 1] + ((disX *(mskData[index_x1y1 + 1] - mskData[index_x0y1 + 1])) >> 14);
				AG = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				tmp1 = mskData[index_x0y0 + 2] + ((disX *(mskData[index_x1y0 + 2] - mskData[index_x0y0 + 2])) >> 14);
				tmp2 = mskData[index_x0y1 + 2] + ((disX *(mskData[index_x1y1 + 2] - mskData[index_x0y1 + 2])) >> 14);
				AR = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				tmp1 = mskData[index_x0y0 + 3] + ((disX *(mskData[index_x1y0 + 3] - mskData[index_x0y0 + 3])) >> 14);
				tmp2 = mskData[index_x0y1 + 3] + ((disX *(mskData[index_x1y1 + 3] - mskData[index_x0y1 + 3])) >> 14);
				AA = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);

				int pos = (x0 << 2) + y0 * mStride;
			    int k = AA;
			    int b = (k * AB     + (255 - k) * pSrc[0]) / 255;
			    int g = (k * AG + (255 - k) * pSrc[1]) / 255;
			    int r = (k * AR + (255 - k) * pSrc[2]) / 255;
			    pSrc[0] = ((alpha * b + nalpha * pSrc[0]) >> 7);
			    pSrc[1] = ((alpha * g + nalpha * pSrc[1]) >> 7);
			    pSrc[2] = ((alpha * r + nalpha * pSrc[2]) >> 7);
			}
			pSrc += 4;
		}
	}
	return ret;
}


inline bool f_IsBelowLine(int inputPoint[4], float x, float y)
{
    int x1 = inputPoint[0];
    int y1 = inputPoint[1];
    int x2 = inputPoint[2];
    int y2 = inputPoint[3];
    float val = (x2 - x1) * (y - y1) - (x - x1) * (y2 - y1);
    return (val > 0); 
}

/*************************************************************************
*Function: f_MakeupBaseShadow
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*srcKeyPoints: 4 key points of source image.
*mskData: blush image buffer with format bgra32
*mWidth: width of mask image 
*mHeight: height of mask image
*mStride: Stride of mask image
*maskKeyPoints�� 4 key points of blush mask.
*ratio: intensity of effect, [0, 100]
*Return: 0-OK,other failed
**************************************************************************/
int f_MakeupBaseShadow(unsigned char* srcData, int width, int height, int stride, int srcKeyPoints[2 * 4], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 4], int mode, int ratio)
{
	int ret = 0;
	float HU[6] = {0}, HD[6] = {0};
	f_AffinetransformMetrixCompute((float)maskKeyPoints[0],(float)maskKeyPoints[1],(float)maskKeyPoints[2],(float)maskKeyPoints[3],(float)maskKeyPoints[4],(float)maskKeyPoints[5], (float)srcKeyPoints[0], (float)srcKeyPoints[1], (float)srcKeyPoints[2], (float)srcKeyPoints[3], (float)srcKeyPoints[4], (float)srcKeyPoints[5], HU);
	f_AffinetransformMetrixCompute((float)maskKeyPoints[0],(float)maskKeyPoints[1],(float)maskKeyPoints[6],(float)maskKeyPoints[7],(float)maskKeyPoints[4],(float)maskKeyPoints[5], (float)srcKeyPoints[0], (float)srcKeyPoints[1], (float)srcKeyPoints[6], (float)srcKeyPoints[7], (float)srcKeyPoints[4], (float)srcKeyPoints[5], HD);
	unsigned char* pSrc = srcData;
	int alpha = 128 * ratio / 100;
	int nalpha = 128 - alpha;
	int Line[4] = {srcKeyPoints[0], srcKeyPoints[1], srcKeyPoints[4], srcKeyPoints[5]};
	int AR, AG, AB, AA;
	int x0, y0, index_x0y0, index_x0y1, index_x1y0, index_x1y1;
	int disX, disY, tmp1, tmp2;
	if(mode == 0)
	{
	    for(int j = 0; j < height; j++)
	    {
	    	for(int i = 0; i < width; i++)
	    	{
	    		bool isBelowLine = f_IsBelowLine(Line, i, j);
				float cx = 0, cy = 0;
	    		if(!isBelowLine)
	    		{
	    		    cx = (HU[0] * i + HU[1] * j + HU[2]);
	                cy = (HU[3] * i + HU[4] * j + HU[5]);
				}
				else
				{
					cx = (HD[0] * i + HD[1] * j + HD[2]);
	                cy = (HD[3] * i + HD[4] * j + HD[5]);
				}
	    		if(cx > 0 && cy < mWidth - 1 && cx > 0 && cy < mHeight - 1)
	    		{
					x0 = (int)CLIP3(floor(cx), 0, mWidth - 2);
				    y0 = (int)CLIP3(floor(cy), 0, mHeight - 2);
				    index_x0y0 = (x0 << 2) + y0 * mStride;
				    index_x1y0 = index_x0y0 + 4;
				    index_x0y1 = index_x0y0 + mStride;
				    index_x1y1 = index_x0y1 + 4;
				    disX = (int)((cx - x0) * 16384);
				    disY = (int)((cy - y0) * 16384);	
				    tmp1 = mskData[index_x0y0] + ((disX *(mskData[index_x1y0] - mskData[index_x0y0])) >> 14);
				    tmp2 = mskData[index_x0y1] + ((disX *(mskData[index_x1y1] - mskData[index_x0y1])) >> 14);
				    AB = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 1] + ((disX *(mskData[index_x1y0 + 1] - mskData[index_x0y0 + 1])) >> 14);
				    tmp2 = mskData[index_x0y1 + 1] + ((disX *(mskData[index_x1y1 + 1] - mskData[index_x0y1 + 1])) >> 14);
				    AG = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 2] + ((disX *(mskData[index_x1y0 + 2] - mskData[index_x0y0 + 2])) >> 14);
				    tmp2 = mskData[index_x0y1 + 2] + ((disX *(mskData[index_x1y1 + 2] - mskData[index_x0y1 + 2])) >> 14);
				    AR = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 3] + ((disX *(mskData[index_x1y0 + 3] - mskData[index_x0y0 + 3])) >> 14);
				    tmp2 = mskData[index_x0y1 + 3] + ((disX *(mskData[index_x1y1 + 3] - mskData[index_x0y1 + 3])) >> 14);
				    AA = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
	    			if (AA != 0)
	    			{
	    				int mb = AB;
	    				int mg = AG;
	    				int mr = AR;
	    				mb = (mb * AA + pSrc[0] * (255 - AA)) / 255;
	    				mg = (mg * AA + pSrc[1] * (255 - AA)) / 255;
	    				mr = (mr * AA + pSrc[2] * (255 - AA)) / 255;
	    				pSrc[0] = (pSrc[0] * (100 - ratio) + mb * ratio) / 100;
	    				pSrc[1] = (pSrc[1] * (100 - ratio) + mg * ratio) / 100;
	    				pSrc[2] = (pSrc[2] * (100 - ratio) + mr * ratio) / 100;
	    			}
	    		}
	    		pSrc += 4;
	    	}
	    }
	}
	else
	{
		for(int j = 0; j < height; j++)
	    {
	    	for(int i = 0; i < width; i++)
	    	{
	    		bool isBelowLine = f_IsBelowLine(Line, i, j);
				float cx = 0, cy = 0;
	    		if(!isBelowLine)
	    		{
	    		    cx = (HU[0] * i + HU[1] * j + HU[2]);
	                cy = (HU[3] * i + HU[4] * j + HU[5]);
				}
				else
				{
					cx = (HD[0] * i + HD[1] * j + HD[2]);
	                cy = (HD[3] * i + HD[4] * j + HD[5]);
				}
	    		if(cx > 0 && cx < mWidth - 1 && cy > 0 && cy < mHeight - 1)
	    		{
					x0 = (int)CLIP3(floor(cx), 0, mWidth - 2);
				    y0 = (int)CLIP3(floor(cy), 0, mHeight - 2);
				    index_x0y0 = (x0 << 2) + y0 * mStride;
				    index_x1y0 = index_x0y0 + 4;
				    index_x0y1 = index_x0y0 + mStride;
				    index_x1y1 = index_x0y1 + 4;
				    disX = (int)((cx - x0) * 16384);
				    disY = (int)((cy - y0) * 16384);	
				    tmp1 = mskData[index_x0y0] + ((disX *(mskData[index_x1y0] - mskData[index_x0y0])) >> 14);
				    tmp2 = mskData[index_x0y1] + ((disX *(mskData[index_x1y1] - mskData[index_x0y1])) >> 14);
				    AB = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 1] + ((disX *(mskData[index_x1y0 + 1] - mskData[index_x0y0 + 1])) >> 14);
				    tmp2 = mskData[index_x0y1 + 1] + ((disX *(mskData[index_x1y1 + 1] - mskData[index_x0y1 + 1])) >> 14);
				    AG = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 2] + ((disX *(mskData[index_x1y0 + 2] - mskData[index_x0y0 + 2])) >> 14);
				    tmp2 = mskData[index_x0y1 + 2] + ((disX *(mskData[index_x1y1 + 2] - mskData[index_x0y1 + 2])) >> 14);
				    AR = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
				    tmp1 = mskData[index_x0y0 + 3] + ((disX *(mskData[index_x1y0 + 3] - mskData[index_x0y0 + 3])) >> 14);
				    tmp2 = mskData[index_x0y1 + 3] + ((disX *(mskData[index_x1y1 + 3] - mskData[index_x0y1 + 3])) >> 14);
				    AA = tmp1 + ((disY * (tmp2 - tmp1)) >> 14);
	    			if (AA != 0)
	    			{
	    				int mb = AB;
						int mg = AG;
						int mr = AR;
						int b = ModeSmoothLight(pSrc[0], mb);
						int g = ModeSmoothLight(pSrc[1], mg);
						int r = ModeSmoothLight(pSrc[2], mr);
						b = (b * AA + pSrc[0] * (255 - AA)) / 255;
						g = (g * AA + pSrc[1] * (255 - AA)) / 255;
						r = (r * AA + pSrc[2] * (255 - AA)) / 255;
						pSrc[0] = (pSrc[0] * (100 - ratio) + b * ratio) / 100;
						pSrc[1] = (pSrc[1] * (100 - ratio) + g * ratio) / 100;
						pSrc[2] = (pSrc[2] * (100 - ratio) + r * ratio) / 100;
	    			}
	    		}
	    		pSrc += 4;
	    	}
	    }
	}
	return ret;
}

int f_MakeupBaseSteronose(unsigned char* srcData, int width, int height, int stride, int srcKeyPoints[2 * 3], unsigned char* mskData, int mWidth, int mHeight, int mStride, int maskKeyPoints[2 * 3], int ratio)
{
	int ret = 0; 
	float H[6] = {0};
	f_AffinetransformMetrixCompute((float)maskKeyPoints[0],(float)maskKeyPoints[1],(float)maskKeyPoints[2],(float)maskKeyPoints[3],(float)maskKeyPoints[4],(float)maskKeyPoints[5], (float)srcKeyPoints[0], (float)srcKeyPoints[1], (float)srcKeyPoints[2], (float)srcKeyPoints[3], (float)srcKeyPoints[4], (float)srcKeyPoints[5], H);
	unsigned char* pSrc = srcData;
	int alpha = 128 * ratio / 100;
	int nalpha = 128 - alpha;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			int x0 = (H[0] * i + H[1] * j + H[2]);
	        int y0 = (H[3] * i + H[4] * j + H[5]);
			if(x0 > 0 && x0 < mWidth - 1 && y0 > 0 && y0 < mHeight - 1)
			{
			    int pos = (x0 << 2) + y0 * mStride;
			    int k = mskData[pos + 3];
			    int sourceB = pSrc[0];
			    int sourceG = pSrc[1];
			    int sourceR = pSrc[2];
			    int b = ModeSuperposition(pSrc[0], mskData[pos]);
			    int g = ModeSuperposition(pSrc[1], mskData[pos + 1]);
			    int r = ModeSuperposition(pSrc[2], mskData[pos + 2]);
			    /*pSrc[0] = 0;
			    pSrc[1] = 0;
			    pSrc[2] = 0;*/
				pSrc[0] = ((alpha * b + nalpha * pSrc[0]) >> 7);
				pSrc[1] = ((alpha * g + nalpha * pSrc[1]) >> 7);
				pSrc[2] = ((alpha * r + nalpha * pSrc[2]) >> 7);
			}
			pSrc += 4;
		}
	}
	return ret;
}


