
/*************************************************************************
Copyright:   HZ.
Author:		 Hu Yaowu
Date:		 2018-11-23
Mail:        dongtingyueh@163.com
Description: Affinetransfrom .
*************************************************************************/
#ifndef __T_AFFINE_TRANSFROM__
#define __T_AFFINE_TRANSFROM__


#ifdef _MSC_VER

#ifdef __cplusplus
#define EXPORT extern "C" _declspec(dllexport)
#else
#define EXPORT __declspec(dllexport)
#endif

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
EXPORT void f_AffinetransformMetrixCompute(float x1, float y1, float x2, float y2, float x3, float y3, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, float hMatrix[6]);
/*************************************************************************
*Function: getWHFromHMatrix 
*Params:
*width: source image width
*height: source image height
*H: affinetransfrom matrix,[a11, a12, b1, a21, a22, b2]
*wh: the size of image outputs, wh[0]=dWidth, wh[1] = dHeight
*Return: NULL
**************************************************************************/
EXPORT void getWHFromHMatrix(int width, int height, float H[6], int wh[2]);
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
EXPORT int f_AffineTransform(unsigned char* srcData, int width, int height, int stride, float H[6], unsigned char* dstData, int dWidth, int dHeight, int dStride);
#else

#ifdef __cplusplus
extern "C" {
#endif    

   int f_AffineTransform(unsigned char* srcData, int width, int height, int stride, float H[6], unsigned char* dstData, int dWidth, int dHeight, int dStride);
   void getWHFromHMatrix(int width, int height, float H[6], int wh[2]);
   void f_AffinetransformMetrixCompute(float x1, float y1, float x2, float y2, float x3, float y3, float tx1, float ty1, float tx2, float ty2, float tx3, float ty3, float hMatrix[6]);
#ifdef __cplusplus
}
#endif


#endif



#endif
