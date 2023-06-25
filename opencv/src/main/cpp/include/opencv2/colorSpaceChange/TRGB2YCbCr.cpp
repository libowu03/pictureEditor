#include "TRGB2YCbCr.h"

const float YCbCrYRF = 0.299F; // RGBתYCbCr��ϵ��(�������ͣ�
const float YCbCrYGF = 0.587F;
const float YCbCrYBF = 0.114F;
const float YCbCrCbRF = -0.168736F;
const float YCbCrCbGF = -0.331264F;
const float YCbCrCbBF = 0.500000F;
const float YCbCrCrRF = 0.500000F;
const float YCbCrCrGF = -0.418688F;
const float YCbCrCrBF = -0.081312F;

const float RGBRYF = 1.00000F; // YCbCrתRGB��ϵ��(�������ͣ�
const float RGBRCbF = 0.0000F;
const float RGBRCrF = 1.40200F;
const float RGBGYF = 1.00000F;
const float RGBGCbF = -0.34414F;
const float RGBGCrF = -0.71414F;
const float RGBBYF = 1.00000F;
const float RGBBCbF = 1.77200F;
const float RGBBCrF = 0.00000F;

const int Shift1 = 20;
const int HalfShiftValue = 1 << (Shift1 - 1);

const int YCbCrYRI = (int)(YCbCrYRF * (1 << Shift1) + 0.5); // RGBתYCbCr��ϵ��(�������ͣ�
const int YCbCrYGI = (int)(YCbCrYGF * (1 << Shift1) + 0.5);
const int YCbCrYBI = (int)(YCbCrYBF * (1 << Shift1) + 0.5);
const int YCbCrCbRI = (int)(YCbCrCbRF * (1 << Shift1) + 0.5);
const int YCbCrCbGI = (int)(YCbCrCbGF * (1 << Shift1) + 0.5);
const int YCbCrCbBI = (int)(YCbCrCbBF * (1 << Shift1) + 0.5);
const int YCbCrCrRI = (int)(YCbCrCrRF * (1 << Shift1) + 0.5);
const int YCbCrCrGI = (int)(YCbCrCrGF * (1 << Shift1) + 0.5);
const int YCbCrCrBI = (int)(YCbCrCrBF * (1 << Shift1) + 0.5);

const int RGBRYI = (int)(RGBRYF * (1 << Shift1) + 0.5); // YCbCrתRGB��ϵ��(�������ͣ�
const int RGBRCbI = (int)(RGBRCbF * (1 << Shift1) + 0.5);
const int RGBRCrI = (int)(RGBRCrF * (1 << Shift1) + 0.5);
const int RGBGYI = (int)(RGBGYF * (1 << Shift1) + 0.5);
const int RGBGCbI = (int)(RGBGCbF * (1 << Shift1) + 0.5);
const int RGBGCrI = (int)(RGBGCrF * (1 << Shift1) + 0.5);
const int RGBBYI = (int)(RGBBYF * (1 << Shift1) + 0.5);
const int RGBBCbI = (int)(RGBBCbF * (1 << Shift1) + 0.5);
const int RGBBCrI = (int)(RGBBCrF * (1 << Shift1) + 0.5);

void RGBToYCbCr2(int R, int G, int B, int *Y, int *Cb, int *Cr)
{
    *Y = ((YCbCrYRI * R + YCbCrYGI * G + YCbCrYBI * B + HalfShiftValue) >> Shift1);
    *Cb = (128 + ((YCbCrCbRI * R + YCbCrCbGI * G + YCbCrCbBI * B + HalfShiftValue) >> Shift1));
    *Cr = (128 + ((YCbCrCrRI * R + YCbCrCrGI * G + YCbCrCrBI * B + HalfShiftValue) >> Shift1));
}

void YCbCrToRGB2(int Y, int Cb, int Cr, int *R, int *G, int *B)
{
    Cb = Cb - 128;
    Cr = Cr - 128;
    *R = Y + ((RGBRCrI * Cr + HalfShiftValue) >> Shift1);
    *G = Y + ((RGBGCbI * Cb + RGBGCrI * Cr + HalfShiftValue) >> Shift1);
    *B = Y + ((RGBBCbI * Cb + HalfShiftValue) >> Shift1);
    if (*R > 255)
        *R = 255;
    else if (*R < 0)
        *R = 0;
    if (*G > 255)
        *G = 255;
    else if (*G < 0)
        *G = 0;
    if (*B > 255)
        *B = 255;
    else if (*B < 0)
        *B = 0;
}
