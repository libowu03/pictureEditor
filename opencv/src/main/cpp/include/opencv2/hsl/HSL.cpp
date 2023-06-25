/*
 * Hue Saturation Lightness Adjust
 *
 * Author: JoStudio
 */

#include "HSL.hpp"
#include "ColorSpace.hpp"
#include <math.h>

#define CLIP_RANGE(value, min, max)  ( (value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)) )
#define COLOR_RANGE(value)  CLIP_RANGE(value, 0, 255)

namespace cv {

HSLChannel::HSLChannel()
{
	hue = 0;
	saturation = 0;
	brightness = 0;
	defined = false;

	setColorIndex(0);
}

HSLChannel::~HSLChannel()
{

}

void HSLChannel::setColorIndex(int index)
{
	int data[][4] = {
			{  0,   0, 360, 360},
			{315, 345,  15,  45},
			{ 15,  45,  75, 105},
			{ 75, 105, 135, 165},
			{135, 165, 195, 225},
			{195, 225, 255, 285},
			{255, 285, 315, 345}
	};

	if (index < 0 ) index = 0;
	if (index > 6 ) index = 6;

	colorIndex = index;
	left_left = data[index][0];
	left      = data[index][1];
	right     = data[index][2];
	right_right = data[index][3];
}

void HSLChannel::calcDefined()
{
	if ( hue != 0 || saturation != 0 || brightness != 0 ) {
		defined = true;
		return;
	}
	defined = false;
}


bool  HSLChannel::match(float hue)
{
	if ( left < right ) {
		if ( hue >= left_left && hue <= right_right )
			return true;
	} else {
		if ( hue >=left_left && hue <= 360 )
			return true;
		if ( hue >=0 && hue <= right_right )
			return true;
	}
	return false;
}

void HSLChannel::adjust(int h, float *delta_hsb)
{
	if (colorIndex == 0 ) {
		delta_hsb[0] += hue;
		delta_hsb[1] += saturation;
		delta_hsb[2] += brightness;
		return;
	}

	if ( left < right ) {
		if ( h >= left_left && h <= right_right ) {
			if ( h >=left && h <= right) {
				delta_hsb[0] += hue;
				delta_hsb[1] += saturation;
				delta_hsb[2] += brightness;
				return;
			}

			if ( h >=left_left && h <= left && left > left_left) {
				delta_hsb[0] += hue * (h - left_left) / (left - left_left);
				delta_hsb[1] += saturation * (h - left_left) / (left - left_left);
				delta_hsb[2] += brightness * (h - left_left) / (left - left_left);
				return;
			}

			if ( h >=right && h <= right_right && right_right > right) {
				delta_hsb[0] += hue * (right_right - h) / (right_right - right);
				delta_hsb[1] += saturation * (right_right - h) / (right_right - right);
				delta_hsb[2] += brightness * (right_right - h) / (right_right - right);
				return;
			}
		}

	} else {
		if ( h >=left && h <= 360 ) {
			delta_hsb[0] += hue;
			delta_hsb[1] += saturation;
			delta_hsb[2] += brightness;
			return;
		}

		if ( h >=0 && h <= right ) {
			delta_hsb[0] += hue;
			delta_hsb[1] += saturation;
			delta_hsb[2] += brightness;
			return;
		}

		if ( h >=left_left && h <= left && left > left_left) {
			delta_hsb[0] += hue * (h - left_left) / (left - left_left);
			delta_hsb[1] += saturation * (h - left_left) / (left - left_left);
			delta_hsb[2] += brightness * (h - left_left) / (left - left_left);
			return;
		}

		if ( h >=right && h <= right_right && right_right > right) {
			delta_hsb[0] += hue * (right_right - h) / (right_right - right);
			delta_hsb[1] += saturation * (right_right - h) / (right_right - right);
			delta_hsb[2] += brightness * (right_right - h) / (right_right - right);
			return;
		}
	}
}


//----------------------------------------------------------
//HSL class

HSL::HSL()
{
	for (int i = 0; i < 7; i++)
		channels[i].setColorIndex(i);
}

HSL::~HSL()
{
}

int HSL::adjust(InputArray src, OutputArray dst)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	dst.create(src.size(), src.type());
	Mat output = dst.getMat();

	const uchar *in;
	uchar *out;
	int width = input.cols;
	int height = input.rows;
	int channel_count = input.channels();

	float hsb[3];
	float delta_hsb[3];

	//calculate defined
	for (int i = 0; i < 7; i++)
		channels[i].calcDefined();

	//scan pixels of image
	for (int y = 0; y < height; y ++) {
		in = input.ptr<uchar>(y);
		out = output.ptr<uchar>(y);

		for (int x = 0; x < width; x ++) {
			//RGB to HSL conversion
			BGR2HSB(in, hsb);

			//adjust each channel
			delta_hsb[0] = delta_hsb[1] = delta_hsb[2] = 0;
			for (int i = 0; i < 7; i++) {
				if ( channels[i].defined ) {
					 channels[i].adjust(hsb[0], delta_hsb);
				}
			}

			//adjust hue
			hsb[0] = int(hsb[0] + delta_hsb[0]) % 360;
			if ( hsb[0] <  0 ) hsb[0] += 360;

			//adjust saturation
			delta_hsb[1] = CLIP_RANGE(delta_hsb[1], -100, 100);
			if ( delta_hsb[1] < 0) {
				hsb[1] = hsb[1] * (1 + delta_hsb[1] / 100.0);
			} else {
				hsb[1] = hsb[1] + ( 1 - hsb[1] ) * delta_hsb[1] / 100.0; //saturation increase
				hsb[2] = hsb[2] + ( 1 - hsb[2] ) * delta_hsb[1] / 100.0; //brightness increase
			}

			//adjust brightness
			delta_hsb[2] = CLIP_RANGE(delta_hsb[2], -100, 100);
			if ( delta_hsb[2] < 0) {
				hsb[2] = hsb[2] * (1 + delta_hsb[2] / 100.0);
			} else {
				hsb[2] = hsb[2] + ( 1 - hsb[2] ) * delta_hsb[2] / 100.0; //brightness increase
				hsb[1] = hsb[1] - hsb[1]  * delta_hsb[2] / 100.0; //saturation decrease
			}

			//save to output
			HSB2BGR(hsb, out);

			//move to next pixel
			in += 3;
			out += 3;
			for (int c = 0; c < channel_count - 3; c++) {
				*out++ = *in++;
			}
		}
	}

	return 0;
}


} /* namespace cv */
