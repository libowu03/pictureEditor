#include "Lut.h"
#include "opencv2/utils/Commen.h"
static int f_Filter512(unsigned char *srcData, int width, int height, int stride, unsigned char *Map)
{
    int i, j, r, g, b, offset, pos, nx, ny, k;
    unsigned char *pSrc = srcData;
    offset = stride - (width * 4);
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            b = pSrc[0];
            g = pSrc[1];
            r = pSrc[2];
            k = (b >> 2);
            nx = (int)(r >> 2) + ((k - ((k >> 3) << 3)) << 6);
            ny = (int)(((b >> 5) << 6) + (g >> 2));
            pos = (nx * 4) + (ny * 512 * 4);
            pSrc[0] = Map[pos];
            pSrc[1] = Map[pos + 1];
            pSrc[2] = Map[pos + 2];
            pSrc += 4;
        }
        pSrc += offset;
    }
    return 0;
};

int f_LUTFilter(unsigned char *srcData, int width, int height, int stride, unsigned char *Map)
{
    return f_Filter512(srcData, width, height, stride, Map);
};