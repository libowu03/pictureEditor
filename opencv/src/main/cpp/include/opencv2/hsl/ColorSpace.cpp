/*
 * ColorSpace.cpp
 *
 *  Created on: 2016��7��1��
 *      Author: Administrator
 */

#include "ColorSpace.hpp"

#include <iostream>
using namespace std;

#define MAX2(a, b) ((a) > (b)) ? (a) : (b)
#define MAX3(a, b, c) (((a) > (b)) ? MAX2(a, c) : MAX2(b, c))

#define MIN2(a, b) ((a) < (b)) ? (a) : (b)
#define MIN3(a, b, c) (((a) < (b)) ? MIN2(a, c) : MIN2(b, c))

#define SWAP(a, b, t) \
	do                \
	{                 \
		t = a;        \
		a = b;        \
		b = t;        \
	} while (0)
#define CLIP_RANGE(value, min, max) ((value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)))
#define COLOR_RANGE(value) CLIP_RANGE(value, 0, 255)

namespace cv
{

    /**
 * Convert BGR to Lab
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param lab [out] pointer to pixel data in L-a-b format
 *         L, value range [0, 1]
 *         a, value range [0, 1]
 *         b, value range [0, 1]
 */
    void BGR2Lab(const uchar *bgr, float *lab)
    //void rgb2lab( float R, float G, float B, float & l_s, float &a_s, float &b_s )
    {
        float var_R = bgr[2] / 255.0;
        float var_G = bgr[1] / 255.0;
        float var_B = bgr[0] / 255.0;

        if (var_R > 0.04045)
            var_R = pow(((var_R + 0.055) / 1.055), 2.4);
        else
            var_R = var_R / 12.92;

        if (var_G > 0.04045)
            var_G = pow(((var_G + 0.055) / 1.055), 2.4);
        else
            var_G = var_G / 12.92;

        if (var_B > 0.04045)
            var_B = pow(((var_B + 0.055) / 1.055), 2.4);
        else
            var_B = var_B / 12.92;

        var_R = var_R * 100.;
        var_G = var_G * 100.;
        var_B = var_B * 100.;

        //Observer. = 2��, Illuminant = D65
        float X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
        float Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
        float Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;

        float var_X = X / 95.047;  //ref_X =  95.047   Observer= 2��, Illuminant= D65
        float var_Y = Y / 100.000; //ref_Y = 100.000
        float var_Z = Z / 108.883; //ref_Z = 108.883

        if (var_X > 0.008856)
            var_X = pow(var_X, (1. / 3.));
        else
            var_X = (7.787 * var_X) + (16. / 116.);

        if (var_Y > 0.008856)
            var_Y = pow(var_Y, (1. / 3.));
        else
            var_Y = (7.787 * var_Y) + (16. / 116.);

        if (var_Z > 0.008856)
            var_Z = pow(var_Z, (1. / 3.));
        else
            var_Z = (7.787 * var_Z) + (16. / 116.);

        lab[0] = (116. * var_Y) - 16.;
        lab[1] = 500. * (var_X - var_Y);
        lab[2] = 200. * (var_Y - var_Z);
    }

    /**
 * Convert Lab to BGR
 *
 * @param lab [in] pointer to pixel data in L-a-b format
 *         L, value range [0, 1]
 *         a, value range [0, 1]
 *         b, value range [0, 1]
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void Lab2BGR(const float *lab, uchar *bgr)
    //http://www.easyrgb.com/index.php?X=MATH&H=08#text8
    //void lab2rgb( float l_s, float a_s, float b_s, float& R, float& G, float& B )
    {
        float var_Y = (lab[0] + 16.) / 116.;
        float var_X = lab[1] / 500. + var_Y;
        float var_Z = var_Y - lab[2] / 200.;

        if (pow(var_Y, 3) > 0.008856)
            var_Y = pow(var_Y, 3);
        else
            var_Y = (var_Y - 16. / 116.) / 7.787;

        if (pow(var_X, 3) > 0.008856)
            var_X = pow(var_X, 3);
        else
            var_X = (var_X - 16. / 116.) / 7.787;

        if (pow(var_Z, 3) > 0.008856)
            var_Z = pow(var_Z, 3);
        else
            var_Z = (var_Z - 16. / 116.) / 7.787;

        float X = 95.047 * var_X;  //ref_X =  95.047     Observer= 2��, Illuminant= D65
        float Y = 100.000 * var_Y; //ref_Y = 100.000
        float Z = 108.883 * var_Z; //ref_Z = 108.883

        var_X = X / 100.; //X from 0 to  95.047      (Observer = 2��, Illuminant = D65)
        var_Y = Y / 100.; //Y from 0 to 100.000
        var_Z = Z / 100.; //Z from 0 to 108.883

        float var_R = var_X * 3.2406 + var_Y * -1.5372 + var_Z * -0.4986;
        float var_G = var_X * -0.9689 + var_Y * 1.8758 + var_Z * 0.0415;
        float var_B = var_X * 0.0557 + var_Y * -0.2040 + var_Z * 1.0570;

        if (var_R > 0.0031308)
            var_R = 1.055 * pow(var_R, (1 / 2.4)) - 0.055;
        else
            var_R = 12.92 * var_R;

        if (var_G > 0.0031308)
            var_G = 1.055 * pow(var_G, (1 / 2.4)) - 0.055;
        else
            var_G = 12.92 * var_G;

        if (var_B > 0.0031308)
            var_B = 1.055 * pow(var_B, (1 / 2.4)) - 0.055;
        else
            var_B = 12.92 * var_B;

        bgr[2] = var_R * 255.;
        bgr[1] = var_G * 255.;
        bgr[0] = var_B * 255.;
    }

    /**
 * Convert RGB to Lab
 *
 * @param R [in] red,   value range [0, 255]
 * @param G [in] green, value range [0, 255]
 * @param B [in] blue,  value range [0, 255]
 * @param L [out] L,    value range [0, 1]
 * @param a [out] a, value range [0, 1]
 * @param b [out] b,  value range [0, 1]
 */
    void RGB2Lab(uchar R, uchar G, uchar B, float *L, float *a, float *b)
    {
        float lab[3];
        uchar bgr[3];
        bgr[0] = B;
        bgr[1] = G;
        bgr[2] = R;
        BGR2Lab(bgr, lab);
        *L = lab[0];
        *a = lab[1];
        *b = lab[2];
    }

    /**
 * Convert Lab to RGB
 *
 * @param L [in] L,  value range [0, 1]
 * @param a [in] a,  value range [0, 1]
 * @param b [in] b,  value range [0, 1]
 * @param R [out] red,   value range [0, 255]
 * @param G [out] green, value range [0, 255]
 * @param B [out] blue,  value range [0, 255]
 */
    void Lab2RGB(float L, float a, float b, uchar *R, uchar *G, uchar *B)
    {
        float lab[3];
        uchar bgr[3];
        lab[0] = L;
        lab[1] = a;
        lab[2] = b;
        Lab2BGR(lab, bgr);
        *R = bgr[2];
        *G = bgr[1];
        *B = bgr[0];
    }

    /**
 * Convert RGB to CMYK
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param cmyk [out] pointer to pixel data in Cyan-Magenta-Yellow-Black format
 *         cyan,    value range [0, 1]
 *         magenta, value range [0, 1]
 *         yellow,  value range [0, 1]
 *         black,   value range [0, 1]
 */
    void BGR2CMYK(const uchar *bgr, float *cmyk)
    {
        float r1 = bgr[2] / 255.0;
        float g1 = bgr[1] / 255.0;
        float b1 = bgr[0] / 255.0;

        cmyk[3] = 1 - MAX3(r1, g1, b1);
        cmyk[0] = (1 - r1 - cmyk[3]) / (1 - cmyk[3]);
        cmyk[1] = (1 - g1 - cmyk[3]) / (1 - cmyk[3]);
        cmyk[2] = (1 - b1 - cmyk[3]) / (1 - cmyk[3]);
    }

    /**
 * Convert CYMK to BGR
 *
 * @param cmyk [in] pointer to pixel data in Cyan-Magenta-Yellow-Black format
 *         cyan,    value range [0, 1]
 *         magenta, value range [0, 1]
 *         yellow,  value range [0, 1]
 *         black,   value range [0, 1]
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void CMYK2BGR(const float *cmyk, uchar *bgr)
    {
        bgr[2] = cvRound(255.0 * (1 - cmyk[0]) * (1 - cmyk[3]));
        bgr[1] = cvRound(255.0 * (1 - cmyk[1]) * (1 - cmyk[3]));
        bgr[0] = cvRound(255.0 * (1 - cmyk[2]) * (1 - cmyk[3]));
    }

    /**
 * Convert RGB to CMYK
 *
 * @param R [in] red,   value range [0, 255]
 * @param G [in] green, value range [0, 255]
 * @param B [in] blue,  value range [0, 255]
 * @param C [out] cyan,    value range [0, 1]
 * @param M [out] magenta, value range [0, 1]
 * @param Y [out] yellow,  value range [0, 1]
 * @param K [out] black,   value range [0, 1]
 */
    void RGB2CMYK(uchar R, uchar G, uchar B, float *C, float *M, float *Y, float *K)
    {
        float cmyk[4];
        uchar bgr[3];
        bgr[0] = B;
        bgr[1] = G;
        bgr[2] = R;
        BGR2CMYK(bgr, cmyk);
        *C = cmyk[0];
        *M = cmyk[1];
        *Y = cmyk[2];
        *K = cmyk[3];
    }

    /**
 * Convert CMYK to RGB
 *
 * @param C [in] cyan,    value range [0, 1]
 * @param M [in] magenta, value range [0, 1]
 * @param Y [in] yellow,  value range [0, 1]
 * @param K [in] black,   value range [0, 1]
 * @param R [out] red,   value range [0, 255]
 * @param G [out] green, value range [0, 255]
 * @param B [out] blue,  value range [0, 255]
 */
    void CMYK2RGB(float C, float M, float Y, float K, uchar *R, uchar *G, uchar *B)
    {
        float cmyk[4];
        uchar bgr[3];
        cmyk[0] = C;
        cmyk[1] = M;
        cmyk[2] = Y;
        cmyk[3] = K;
        CMYK2BGR(cmyk, bgr);
        *B = bgr[0];
        *G = bgr[1];
        *R = bgr[2];
    }

    /**
 * Convert RGB to HSL
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param hsl [out] pointer to pixel data in Hue-Saturation-Lightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         lightness,   value range [0, 1]
 */
    void BGR2HSL(const uchar *bgr, float *hsl)
    {
        float r1, g1, b1, cmax, cmin, delta;
        r1 = bgr[2] / 255.0;
        g1 = bgr[1] / 255.0;
        b1 = bgr[0] / 255.0;
        cmax = MAX3(r1, g1, b1);
        cmin = MIN3(r1, g1, b1);
        delta = cmax - cmin;
        hsl[2] = (cmax + cmin) / 2;
        if (delta == 0)
        {
            hsl[1] = 0;
            hsl[0] = 0;
        }
        else
        {
            if (hsl[2] <= 0.5)
                hsl[1] = delta / (cmax + cmin);
            else
                hsl[1] = delta / (2 - (cmax + cmin));

            if (cmax == r1)
            {
                if (g1 >= b1)
                    hsl[0] = int(60 * (g1 - b1) / delta + 0.5);
                else
                    hsl[0] = int(60 * (g1 - b1) / delta + 360 + 0.5);
            }
            else if (cmax == g1)
            {
                hsl[0] = int(60 * (b1 - r1) / delta + 120 + 0.5);
            }
            else
            { // cmax == b1
                hsl[0] = int(60 * (r1 - g1) / delta + 240 + 0.5);
            }
        }
    }

    /**
 * Convert RGB to HSL
 *
 * @param R [in] red,   value range [0, 255]
 * @param G [in] green, value range [0, 255]
 * @param B [in] blue,  value range [0, 255]
 * @param H [out] hue,  value range [0, 360]
 * @param S [out] saturation,  value range [0, 1]
 * @param L [out] lightness,   value range [0, 1]
 */
    void RGB2HSL(uchar R, uchar G, uchar B, float *H, float *S, float *L)
    {
        float hsl[3];
        uchar bgr[3];
        bgr[0] = B;
        bgr[1] = G;
        bgr[2] = R;
        BGR2HSL(bgr, hsl);
        *H = hsl[0];
        *S = hsl[1];
        *L = hsl[2];
    }

    /** Helper method that converts hue to rgb */
    static float hueToRgb(float p, float q, float t)
    {
        if (t < 0.0)
            t += 1.0;
        if (t > 1.0)
            t -= 1.0;
        if (t < 1.0 / 6.0)
            return p + (q - p) * 6.0 * t;
        if (t < 1.0 / 2.0)
            return q;
        if (t < 2.0 / 3.0)
            return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
        return p;
    }

    /**
 * Convert HSL to RGB
 *
 * @param hsl [in] pointer to pixel data in Hue-Saturation-Lightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         lightness,   value range [0, 1]
 *
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void HSL2BGR(const float *hsl, uchar *bgr)
    {
        float r1, g1, b1;
        float h = hsl[0] / 360.0;

        if (hsl[1] == 0)
        {
            r1 = g1 = b1 = hsl[2]; // gray
        }
        else
        {
            float q = hsl[2] < 0.5 ? hsl[2] * (1 + hsl[1]) : hsl[2] + hsl[1] - hsl[2] * hsl[1];
            float p = 2 * hsl[2] - q;
            r1 = hueToRgb(p, q, h + 1.0 / 3.0);
            g1 = hueToRgb(p, q, h);
            b1 = hueToRgb(p, q, h - 1.0 / 3.0);
        }

        bgr[0] = int(b1 * 255);
        bgr[1] = int(g1 * 255);
        bgr[2] = int(r1 * 255);
    }

    /**
 * Convert HSL to RGB
 *
 * @param H [in] hue,  value range [0, 360]
 * @param S [in] saturation,  value range [0, 1]
 * @param L [in] lightness,   value range [0, 1]
 * @param R [out] red,   value range [0, 255]
 * @param G [out] green, value range [0, 255]
 * @param B [out] blue,  value range [0, 255]
 */
    void HSL2RGB(float H, float S, float L, uchar *R, uchar *G, uchar *B)
    {
        float hsl[3];
        uchar bgr[3];
        hsl[0] = H;
        hsl[1] = S;
        hsl[2] = L;
        HSL2BGR(hsl, bgr);
        *B = bgr[0];
        *G = bgr[1];
        *R = bgr[2];
    }

    /**
 * Convert RGB to HSB
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param hsb [out] pointer to pixel data in Hue-Saturation-Brightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         brightness,   value range [0, 1]
 */
    void BGR2HSB(const uchar *bgr, float *hsb)
    {
        uchar sorted[3], temp;
        memcpy(sorted, bgr, 3);
        if (sorted[1] > sorted[0])
            SWAP(sorted[0], sorted[1], temp);
        if (sorted[2] > sorted[1])
            SWAP(sorted[1], sorted[2], temp);
        if (sorted[1] > sorted[0])
            SWAP(sorted[0], sorted[1], temp);

        uchar max = sorted[0];
        uchar min = sorted[2];

        hsb[2] = max / 255.0;
        hsb[1] = max == 0 ? 0 : (max - min) / (float)max;

        hsb[0] = 0;
        if (max == bgr[2] && bgr[1] >= bgr[0])
        {
            if (max - min == 0){
                hsb[0] = 0;
            }else{
                hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 0;
            }
        }
        else if (max == bgr[2] && bgr[1] < bgr[0])
        {
            if (max - min == 0){
                hsb[0] = 0;
            }else{
                hsb[0] = (bgr[1] - bgr[0]) * 60.0 / (max - min) + 360;
            }

        }
        else if (max == bgr[1])
        {
            if (max - min == 0){
                hsb[0] = 0;
            }else{
                hsb[0] = (bgr[0] - bgr[2]) * 60.0 / (max - min) + 120;
            }

        }
        else if (max == bgr[0])
        {
            if (max - min == 0){
                hsb[0] = 0;
            }else{
                hsb[0] = (bgr[2] - bgr[1]) * 60.0 / (max - min) + 240;
            }

        }
    }

    /**
 * Convert RGB to HSB
 *
 * @param R [in] red,   value range [0, 255]
 * @param G [in] green, value range [0, 255]
 * @param B [in] blue,  value range [0, 255]
 * @param h [out] hue,  value range [0, 360]
 * @param s [out] saturation,  value range [0, 1]
 * @param b [out] brightness,   value range [0, 1]
 */
    void RGB2HSB(uchar R, uchar G, uchar B, float *h, float *s, float *b)
    {
        float hsb[3];
        uchar bgr[3];
        bgr[0] = B;
        bgr[1] = G;
        bgr[2] = R;
        BGR2HSB(bgr, hsb);
        *h = hsb[0];
        *s = hsb[1];
        *b = hsb[2];
    }

    /**
 *  Convert HSB to BGR
 *
 * @param hsb [in] pointer to pixel data in Hue-Saturation-Brightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         brightness,   value range [0, 1]
 *
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void HSB2BGR(const float *hsb, uchar *bgr)
    {
        if (hsb[2] > 1 || hsb[2] < 0){
            int a = 0;
        }
        float r = 0, g = 0, b = 0;
        int i = (int)(int(hsb[0] / 60) % 6);
        float f = (hsb[0] / 60.0) - i;
        float p = hsb[2] * (1 - hsb[1]);
        float q = hsb[2] * (1 - f * hsb[1]);
        float t = hsb[2] * (1 - (1 - f) * hsb[1]);
        switch (i)
        {
            case 0:
                r = hsb[2];
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = hsb[2];
                b = p;
                break;
            case 2:
                r = p;
                g = hsb[2];
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = hsb[2];
                break;
            case 4:
                r = t;
                g = p;
                b = hsb[2];
                break;
            case 5:
                r = hsb[2];
                g = p;
                b = q;
                break;
            default:
                r = bgr[2] / 255.0;
                g = bgr[1] / 255.0;
                b = bgr[0] / 255.0;
                break;
        }
        bgr[0] = int(b * 255);
        bgr[1] = int(g * 255);
        bgr[2] = int(r * 255);
    }

    /**
 * Convert HSB to RGB
 *
 * @param h [in] hue,  value range [0, 360]
 * @param s [in] saturation,  value range [0, 1]
 * @param b [in] brightness,   value range [0, 1]
 * @param R [out] red,   value range [0, 255]
 * @param G [out] green, value range [0, 255]
 * @param B [out] blue,  value range [0, 255]
 */
    void HSB2RGB(float h, float s, float b, uchar *R, uchar *G, uchar *B)
    {
        uchar bgr[3];
        float hsb[3];
        hsb[0] = h;
        hsb[1] = s;
        hsb[2] = b;
        HSB2BGR(hsb, bgr);
        *R = bgr[2];
        *G = bgr[1];
        *B = bgr[0];
    }

} /* namespace cv */
