#include "opencv2/utils/Commen.h"
#include <math.h>

int ModeLinearLight(int basePixel, int mixPixel)
{
    int res = 0;
    res = 2 * mixPixel + basePixel - 256;
    return CLIP3(res, 0, 255);
}

int ModeSuperposition(int basePixel, int mixPixel) //基色 < = 128：结果色 = 混合色 * 基色 / 128；基色 > 128：结果色 = 255 - （255 - 混合色）* (255 - 基色) / 128
{
    int res = 0;
    res = ((basePixel <= 128) ? (mixPixel * basePixel / 128) : (255 - (255 - mixPixel) * (255 - basePixel) / 128));
    return CLIP3(res, 0, 255);
}

int ModeSmoothLight(int basePixel,int mixPixel)
{
    int res = 0;
    res = mixPixel > 128 ? ((int)((float)basePixel+((float)mixPixel+(float)mixPixel-255.0f)*((sqrt((float)basePixel/255.0f))*255.0f-(float)basePixel)/255.0f)):
          ((int)((float)basePixel+((float)mixPixel+(float)mixPixel-255.0f)*((float)basePixel-(float)basePixel*(float)basePixel/255.0f)/255.0f));
    return CLIP3(res, 0, 255);
}