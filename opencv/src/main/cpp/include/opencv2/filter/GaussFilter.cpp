#include "GaussFilter.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "opencv2/utils/Commen.h"
/*************************************************************
 *Function: Gauss mask compute
 *Params:
 *      r-radius of gauss filter
 *      sigma-sigma of gauss filter
 *      gaussMask-gauss weight to compute with size of (2r+1)*(2r+1)
 *Return NULL
 ************************************************************/
void GaussMask(int r, double sigma, double gaussMask[])
{
    double PI = 3.1415926;
    double sum = 0;
    int stride = 2 * r + 1;
    for (int y = -r, h = 0; y <= r; y++, h++)
    {
        for (int x = -r, w = 0; x <= r; x++, w++)
        {
            gaussMask[w + h * stride] = (1.0 / (2.0 * PI * sigma * sigma)) * (exp(-((double)x * (double)x + (double)y * (double)y) / (2.0 * sigma * sigma)));
            sum += gaussMask[w + h * stride];
        }
    }
    for (int i = 0; i < stride * stride; i++)
    {
        gaussMask[i] = gaussMask[i] / sum;
    }
};
int f_FastGaussFilter(unsigned char *srcData, int width, int height, int stride, float r)
{
    int ret = 0;
    int radius = (int)r;
    if (r == 0)
        return ret;
    unsigned char *dstData = (unsigned char *)malloc(sizeof(unsigned char) * height * stride);
    unsigned char *tempData = (unsigned char *)malloc(sizeof(unsigned char) * height * stride);
    int totalWei = 0;
    int i, j, k;
    float sigma = r;
    unsigned char *kernel = (unsigned char *)malloc(2 * radius + 1);
    for (i = -radius; i <= radius; i++)
    {
        kernel[i + radius] = (unsigned char)(exp(-(float)i * i / (2 * sigma * sigma)) * 128);
        totalWei += kernel[i + radius];
    }
    int tempR = 0, tempG = 0, tempB = 0;
    int v = 0;
    int K = 0;
    int rem = 0;
    int t = 0;
    int offset = stride - width * 4;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            tempR = 0;
            tempG = 0;
            tempB = 0;
            for (k = -radius; k <= radius; k++)
            {
                rem = (abs(i + k) % width);
                t = rem * 4 + j * stride;
                K = kernel[k + radius];
                tempB += srcData[t] * K;
                tempG += srcData[t + 1] * K;
                tempR += srcData[t + 2] * K;
            }
            v = i * 4 + j * stride;
            tempData[v] = tempB / totalWei;
            tempData[v + 1] = tempG / totalWei;
            tempData[v + 2] = tempR / totalWei;
        }
    }
    for (i = 0; i < width; i++)
    {
        for (j = 0; j < height; j++)
        {
            tempR = 0;
            tempG = 0;
            tempB = 0;
            for (k = -radius; k <= radius; k++)
            {
                rem = (abs(j + k) % height);
                t = rem * stride + i * 4;
                K = kernel[k + radius];
                tempB += tempData[t] * K;
                tempG += tempData[t + 1] * K;
                tempR += tempData[t + 2] * K;
            }
            v = i * 4 + j * stride;
            dstData[v] = tempB / totalWei;
            dstData[v + 1] = tempG / totalWei;
            dstData[v + 2] = tempR / totalWei;
        }
    }
    memcpy(srcData, dstData, sizeof(unsigned char) * height * stride);
    free(dstData);
    free(tempData);
    return ret;
};