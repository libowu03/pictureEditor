#include"MeanFilter.h"
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdio.h>
#include"opencv2/utils/Commen.h"

//Fast mean filter based histagram computation
int f_FastMeanFilter(unsigned char* srcData, int width, int height ,int stride, int radius)
{
	int ret = 0;
	if(radius == 0)
		return ret;
	if(radius > MIN2(width,height) / 2)
		radius = (MIN2(width, height) / 2-0.5);
	unsigned char* dstData = (unsigned char*)malloc(sizeof(unsigned char) * height * stride);
	int unit = 4, t = 0, t1 = 0;
	int i,j,k,len = width * height * unit;
	int block = (radius << 1) + 1;
	int winSize = block * block;
	long sumB = 0, sumG = 0,sumR = 0;
	unsigned char* pSrc = srcData;
	int* temp = (int*)malloc(sizeof(int)* width * unit);
	memset(temp,0,sizeof(int) * width * unit);
	for(k = -radius; k <= radius; k++)
	{
		for(j = 0; j< width; j++)
		{
			t = j * unit;
			t1 = abs(k) * stride;
			temp[t] += pSrc[t + t1];
			temp[t + 1] += pSrc[t + 1 + t1];
			temp[t + 2] += pSrc[t + 2 + t1];
		}
	}
	for (i = 0; i < height; i++)
    {
        sumB = sumG = sumR = 0;
        for (j = -radius; j <= radius; j++)
        {
			 t = abs(j) * unit;
             sumB += temp[t];
			 sumG += temp[t + 1];
			 sumR += temp[t + 2];                                       
        }
        for (j = 0; j < width; j++)
        {
			 t = j * unit + i * stride;
             dstData[t] = (sumB / winSize);
			 dstData[t + 1] = (sumG / winSize);
			 dstData[t + 2] = (sumR / winSize);
			 if (j < width - 1)
			 {
				 t = abs(j - radius) * unit;
				 t1 = (j + radius + 1) % width * unit;
			     sumB = sumB - temp[t] + temp[t1];
			     sumG = sumG - temp[t + 1] + temp[t1 + 1];
			     sumR = sumR - temp[t + 2] + temp[t1 + 2];
			 }                                
        }
        if (i < height - 1)
        {
             for (k = 0; k < width; k++)
			 {
				 t = k * unit + abs(i - radius) * stride;
				 t1 = k * unit + (i + radius + 1) % height * stride;
			     temp[k * unit] = temp[k * unit] - pSrc[t] + pSrc[t1];
			     temp[k * unit + 1] = temp[k * unit + 1] - pSrc[t + 1] + pSrc[t1 + 1];
			     temp[k * unit + 2] = temp[k * unit + 2] - pSrc[t + 2] + pSrc[t1 + 2];
			 }               
        }
    }
	memcpy(srcData, dstData, sizeof(unsigned char) * height * stride);
	free(dstData);
	free(temp);
	return ret;
};
//std mean filter
int f_MeanFilter(unsigned char *srcData, int width, int height, int stride, int radius)
{
	int ret = 0;
	if(radius == 0)
		return ret;
	int offset = stride - width * 4;
    unsigned char* temp = (unsigned char*)malloc(sizeof(unsigned char) * height * stride);
	memcpy(temp, srcData, sizeof(unsigned char) * height * stride);
	int M = (radius * 2 + 1) * (radius * 2 + 1);
	int sumr = 0, sumg = 0, sumb = 0;
	unsigned char* pSrc = srcData;
    for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			sumr = sumg = sumb = 0;
			for(int n = -radius; n <=radius; n++)
			{
				for(int m = -radius; m <= radius; m++)
				{
					int ny = CLIP3(j + n, 0, height - 1);
					int nx = CLIP3(i + m, 0, width - 1);
					int pos = nx * 4 + ny * stride;
					sumb += temp[pos];
					sumg += temp[pos + 1];
					sumr += temp[pos + 2];
				}
			}
			pSrc[0] = sumb / M;
			pSrc[1] = sumg / M;
			pSrc[2] = sumr / M;
			pSrc += 4;
		}
		pSrc += offset;
	}
	free(temp);
	return ret;
};

