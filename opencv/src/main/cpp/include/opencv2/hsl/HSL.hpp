/*
 * Hue Saturation Lightness Adjust
 *
 * Author: JoStudio
 */

#ifndef OPENCV2_PS_HSL_HPP_
#define OPENCV2_PS_HSL_HPP_

#include "opencv2/core.hpp"
using namespace cv;

namespace cv {

enum HSL_COLOR
{
	HSL_ALL,
	HSL_RED,
	HSL_YELLOW,
	HSL_GREEN,
	HSL_CYAN,
	HSL_BLUE,
	HSL_MAGENTA,
};

/**
 * Class of HSL parameters for one channel
 */
class HSLChannel {
public:
	int hue;          //ɫ�ȵ���ֵ��     ���ݷ�Χ:  [-180, 180]
	int saturation;   //���Ͷȵ���ֵ�����ݷ�Χ:  [-100, 100]
	int brightness;   //���ȵ���ֵ��    ���ݷ�Χ:  [-100, 100]

	int   colorIndex;  //color index: 0 = RED, 1 = YELLOW, 2 = GREEN
	float left_left;  //hue range left-left
	float left;       //hue range left
	float right;	  //hue range right
	float right_right;//hue range right-right
	bool defined;     //

	HSLChannel();
	virtual ~HSLChannel();

	void calcDefined();
	void setColorIndex(int index);
	bool match(float hue);
	void adjust(int h, float *delta_hsb);
};

/**
 * Class of HSL
 */
class HSL {
public:
	HSL();
	virtual ~HSL();

	HSLChannel channels[7];

	int adjust(InputArray src, OutputArray dst);
};

} /* namespace cv */

#endif /* OPENCV2_PS_HSL_HPP_ */
