#include<stdlib.h>
#include<math.h>
#include<string.h>
#include"opencv2/utils/Commen.h"
#include"AffineTransfrom.h"

/*************************************************************************
*Function: Affinetransfrom matrix compute
*Params:
*x: x of input points
*y: y of input points
*tx: x of output points
*ty: y of output points
*hMatrix: affinetransfrom matrix,[a11, a12, b1, a21, a22, b2]
*Return: NULL
**************************************************************************/
void f_AffinetransformMetrixCompute(float x1, float y1, float x2, float y2, float x3, float y3, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, float hMatrix[6])
{
    //������ʽ|A|  
    float detA;  
    detA = tx1 * ty2 + tx2 * ty3 + tx3 * ty1 - tx3 * ty2 - tx1 * ty3 - tx2 * ty1;         
    // ��������A*  
    float A11, A12, A13, A21, A22, A23, A31, A32, A33;  
    A11 = ty2 - ty3;  
    A21 = -(ty1 - ty3);  
    A31 = ty1 - ty2;  
    A12 = -(tx2 - tx3);  
    A22 = tx1 - tx3;  
    A32 = -(tx1 - tx2);  
    A13 = tx2 * ty3 - tx3 * ty2;  
    A23 = -(tx1 * ty3 - tx3 * ty1);  
    A33 = tx1 * ty2 - tx2 * ty1;          
    //��任����H=A*/|A|  
    //float texMatrix[6]={0};  
    hMatrix[0] = (x1 * A11 + x2 * A21 + x3 * A31) / detA;  
    hMatrix[1] = (x1 * A12 + x2 * A22 + x3 * A32) / detA;  
    hMatrix[2] = (x1 * A13 + x2 * A23 + x3 * A33) / detA;  
    hMatrix[3] = (y1 * A11 + y2 * A21 + y3 * A31) / detA; 
    hMatrix[4] = (y1 * A12 + y2 * A22 + y3 * A32) / detA;       
    hMatrix[5] = (y1 * A13 + y2 * A23 + y3 * A33) / detA; 
};

/*************************************************************************
*Function: getWHFromHMatrix 
*Params:
*width: source image width
*height: source image height
*H: affinetransfrom matrix,[a11, a12, b1, a21, a22, b2]
*wh: the size of image outputs, wh[0]=dWidth, wh[1] = dHeight
*Return: NULL
**************************************************************************/
void getWHFromHMatrix(int width, int height, float H[6], int wh[2])
{
	int x0 = (H[0] * 0 + H[1] * 0 + H[2] + 0.5);
	int y0 = (H[3] * 0 + H[4] * 0 + H[5] + 0.5);
	int x1 = (H[0] * (float)(width - 1) + H[1] * (float)(height - 1) + H[2] + 0.5);
	int y1 = (H[3] * (float)(width - 1) + H[4] * (float)(height - 1) + H[5] + 0.5);
	int x2 = (H[0] * (float)(width - 1) + H[1] * 0 + H[2] + 0.5);
	int y2 = (H[3] * (float)(width - 1) + H[4] * 0 + H[5] + 0.5);
	int x3 = (H[0] * 0 + H[1] * (float)(height - 1) + H[2] + 0.5);
	int y3 = (H[3] * 0 + H[4] * (float)(height - 1) + H[5] + 0.5);
	wh[0] = MAX2(x0, MAX2(x1, MAX2(x2, x3))) - MIN2(x0, MIN2(x1, MIN2(x2, x3)));
	wh[1] = MAX2(y0, MAX2(y1, MAX2(y2, y3))) - MIN2(y0, MIN2(y1, MIN2(y2, y3)));
};
/*************************************************************************
*Function: Affinetransfrom
*Params:
*srcData:32BGRA image buffer
*width:  width of image
*height: height of image
*stride: Stride of image
*H: affinetransfrom matrix,[a11, a12, b1, a21, a22, b2]
*dstData: output image buffer with format bgra32
*dWidth: width of image outputs
*dHeight: height of image outputs
*dStride: Stride of image outputs
*Return: 0-OK,other failed
**************************************************************************/
int f_AffineTransform(unsigned char* srcData, int width, int height, int stride, float H[6], unsigned char* dstData, int dWidth, int dHeight, int dStride)
{
	int ret = 0;
	unsigned char* pSrc = dstData;
	int tx, ty, pos;
	int offset[2];
    offset[0] = ((dWidth/2.0) - (H[0]*(width/2.0) + H[1]*(height/2.0) + H[2]) + 0.5);
    offset[1] = ((dHeight/2.0) - (H[3]*(width/2.0) + H[4]*(height/2.0) + H[5]) + 0.5);
    H[2] += offset[0];
    H[5] += offset[1];
	pSrc = srcData;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
		    tx = CLIP3(((H[0] * (float)(i) + H[1] * (float)(j) + H[2])), 0, dWidth - 1);
            ty = CLIP3(((H[3] * (float)(i) + H[4] * (float)(j) + H[5])), 0, dHeight - 1);
		    pos = (tx << 2) + ty * dStride;
			dstData[pos]     = pSrc[0];
			dstData[pos + 1] = pSrc[1];
			dstData[pos + 2] = pSrc[2];
			dstData[pos + 3] = 255;
			pSrc += 4;
		}
	}
	return ret;
};