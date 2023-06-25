/*
 * ColorSpace
 *
 * Author: JoStudio
 */

#ifndef OPENCV2_PS_COLORSPACE_HPP_
#define OPENCV2_PS_COLORSPACE_HPP_

#include "opencv2/core.hpp"

namespace cv {

/**
 * Convert BGR to Lab
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param lab [out] pointer to pixel data in L-a-b format
 *         L,  value range [0, 100]
 *         a,  value range [-128, 127]
 *         b,  value range [-128, 127]
 */
    void BGR2Lab(const uchar *bgr, float *lab);


/**
 * Convert Lab to BGR
 *
 * @param lab [in] pointer to pixel data in L-a-b format
 *         L,  value range [0, 100]
 *         a,  value range [-128, 127]
 *         b,  value range [-128, 127]
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void Lab2BGR(const  float *lab, uchar *bgr);


/**
 * Convert BGR to HSL
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param hsl [out] pointer to pixel data in Hue-Saturation-Lightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         lightness,   value range [0, 1]
 */
    void BGR2HSL(const uchar *bgr, float *hsl);

/**
 * Convert HSL to BGR
 *
 * @param hsl [in] pointer to pixel data in Hue-Saturation-Lightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         lightness,   value range [0, 1]
 *
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void HSL2BGR(const float *hsl, uchar *bgr);

/**
 * Convert BGR to HSB
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param hsb [out] pointer to pixel data in Hue-Saturation-Brightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         brightness,   value range [0, 1]
 */
    void BGR2HSB(const uchar *bgr, float *hsb);

/**
 * Convert HSB to BGR
 *
 * @param hsb [in] pointer to pixel data in Hue-Saturation-Brightness format
 *         hue,  value range [0, 360]
 *         saturation,  value range [0, 1]
 *         brightness,   value range [0, 1]
 *
 * @param bgr [out] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 */
    void HSB2BGR(const float *hsb, uchar *bgr);


/**
 * Convert BGR to CMYK
 *
 * @param bgr [in] pointer to pixel data in Blue-Green-Red format, each channel's value range [0, 255]
 * @param cmyk [out] pointer to pixel data in Cyan-Magenta-Yellow-Black format
 *         cyan,    value range [0, 1]
 *         magenta, value range [0, 1]
 *         yellow,  value range [0, 1]
 *         black,   value range [0, 1]
 */
    void BGR2CMYK(const uchar *bgr, float *cmyk);

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
    void CMYK2BGR(const float *cmyk, uchar *bgr);


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
    void RGB2HSL(uchar R, uchar G, uchar B, float *H, float *S, float *L);


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
    void HSL2RGB(float H, float S, float L, uchar *R, uchar *G, uchar *B);


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
    void RGB2HSB(uchar R, uchar G, uchar B, float *h, float *s, float *b);


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
    void HSB2RGB(float h, float s, float b, uchar *R, uchar *G, uchar *B);


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
    void RGB2CMYK(uchar R, uchar G, uchar B, float *C, float *M, float *Y, float *K);


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
    void CMYK2RGB(float C, float M, float Y, float K, uchar *R, uchar *G, uchar *B);


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
    void RGB2Lab(uchar R, uchar G, uchar B, float *L, float *a, float *b);

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
    void Lab2RGB(float L, float a, float b, uchar *R, uchar *G, uchar *B);

} /* namespace cv */

#endif /* OPENCV2_PS_COLORSPACE_HPP_ */
