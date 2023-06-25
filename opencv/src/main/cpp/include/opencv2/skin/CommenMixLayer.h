#ifndef __COMMEN_MIX_LAYER__
#define __COMMEN_MIX_LAYER__

int ModeLinearLight(int basePixel, int mixPixel); //线性光

int ModeSuperposition(int basePixel, int mixPixel); //叠加//基色 < = 128：结果色 = 混合色 * 基色 / 128；基色 > 128：结果色 = 255 - （255 - 混合色）* (255 - 基色) / 128

int ModeSmoothLight(int basePixel,int mixPixel);//柔光
#endif