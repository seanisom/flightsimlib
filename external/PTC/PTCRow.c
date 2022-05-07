// PTCRow.c
// Writes scanline pixel rows from decompressed image blocks

#include "PTC.h"


typedef void (*WritePixel)(int a, int r, int g, int b, void* pDest);


void writeRowRGB(int** ppSrc, int width, WritePixel writePixel, unsigned char* pDest, 
	int numBitsColor, int bitDepth, int destPixelBytes)
{
	const int shiftAmount = bitDepth - numBitsColor;
	const int offset = 1 << (numBitsColor - 1);

	for (int i = 0; i < width; ++i)
	{
		if (shiftAmount == 0)
			writePixel(
				255, 
				ppSrc[0][i] + offset,
				ppSrc[1][i] + offset,
				ppSrc[2][i] + offset,
				pDest);
		else if (shiftAmount < 0)
			writePixel(
				255,
				(ppSrc[0][i] + offset) >> -shiftAmount,
				(ppSrc[1][i] + offset) >> -shiftAmount,
				(ppSrc[2][i] + offset) >> -shiftAmount,
				pDest);
		else
			writePixel(
				255,
				(ppSrc[0][i] + offset) << shiftAmount,
				(ppSrc[1][i] + offset) << shiftAmount,
				(ppSrc[2][i] + offset) << shiftAmount,
				pDest);
		
		pDest += destPixelBytes;
	}
}


void writeRowARGB(int** ppSrc, int width, WritePixel writePixel, unsigned char* pDest, 
	int numBitsColor, int numBitsAlpha, int bitDepthColor, int bitDepthAlpha, int destPixelBytes)
{
	const int shiftColor = bitDepthColor - numBitsColor;
	const int shiftAlpha = bitDepthAlpha - numBitsAlpha;
	const int offsetColor = numBitsColor > 1 ? 1 << (numBitsColor - 1) : 0;
	const int offsetAlpha = numBitsAlpha > 1 ? 1 << (numBitsAlpha - 1) : 0;
	
	for (int i = 0; i < width; ++i)
	{
		if (shiftColor == 0 && shiftAlpha == 0)
			writePixel(
				ppSrc[3][i] + offsetAlpha,
				ppSrc[0][i] + offsetColor,
				ppSrc[1][i] + offsetColor,
				ppSrc[2][i] + offsetColor,
				pDest);
		else if (shiftColor > 0 && shiftAlpha > 0)
			writePixel(
				(ppSrc[3][i] + offsetAlpha) << shiftAlpha,
				(ppSrc[0][i] + offsetColor) << shiftColor,
				(ppSrc[1][i] + offsetColor) << shiftColor,
				(ppSrc[2][i] + offsetColor) << shiftColor,
				pDest);
		else if (shiftColor < 0 && shiftAlpha < 0)
			writePixel(
				(ppSrc[3][i] + offsetAlpha) >> -shiftAlpha,
				(ppSrc[0][i] + offsetColor) >> -shiftColor,
				(ppSrc[1][i] + offsetColor) >> -shiftColor,
				(ppSrc[2][i] + offsetColor) >> -shiftColor,
				pDest);
		else if (shiftColor < 0 && shiftAlpha > 0)
			writePixel(
				(ppSrc[3][i] + offsetAlpha) << shiftAlpha,
				(ppSrc[0][i] + offsetColor) >> -shiftColor,
				(ppSrc[1][i] + offsetColor) >> -shiftColor,
				(ppSrc[2][i] + offsetColor) >> -shiftColor,
				pDest);
		else if (shiftColor > 0 && shiftAlpha < 0)
			writePixel(
				(ppSrc[3][i] + offsetAlpha) >> -shiftAlpha,
				(ppSrc[0][i] + offsetColor) << shiftColor,
				(ppSrc[1][i] + offsetColor) << shiftColor,
				(ppSrc[2][i] + offsetColor) << shiftColor,
				pDest);
		
		pDest += destPixelBytes;
	}
}


void writeRowUniform(int** ppSrc, int width, unsigned char* pDest, int numBitsColor, int destPixelBytes)
{
	const int shift = 8 * destPixelBytes - numBitsColor;
	const int offset = 1 << (numBitsColor - 1);

	for (int i = 0; i < width; ++i)
	{
		int val;
		if (shift == 0)
			val = ppSrc[0][i] + offset;
		else if (shift < 0)
			val = (ppSrc[0][i] + offset) >> -shift;
		else
			val = (ppSrc[0][i] + offset) << shift;

		if (destPixelBytes == 2)
			*(unsigned short*)pDest = (unsigned short)val;  // NOLINT(clang-diagnostic-cast-align)
		else
			*pDest = (unsigned char)val;
		pDest += destPixelBytes;
	}
}


void writePixel888(int a, int r, int g, int b, void* pDest)
{
	unsigned char* rgb = pDest;
	rgb[0] = (unsigned char)b;
	rgb[1] = (unsigned char)g;
	rgb[2] = (unsigned char)r;
}


void writePixel565(int a, int r, int g, int b, void* pDest)
{
	// Note this is a 1666 source, r and b are scaled / truncated.
	*(unsigned short*)pDest = (unsigned short)(((b >> 1) & 0x1F) | 32 * (32 * r ^ ((g ^ (32 * r)) & 0x3F)));
}


void writePixel555(int a, int r, int g, int b, void* pDest)
{
	*(unsigned short*)pDest = (unsigned short)((b & 0x1F) | 32 * ((g & 0x1F) | 32 * (r & 0x1F)));
}


void writePixel8888(int a, int r, int g, int b, void* pDest)
{
	*(unsigned int*)pDest = b | ((g | ((r | (a << 8)) << 8)) << 8);
}


void writePixel1555(int a, int r, int g, int b, void* pDest)
{
	*(unsigned short*)pDest = (unsigned short)((b & 0x1F) | 32 * ((g & 0x1F) | 32 * (32 * a | (r & 0x1F))));
}


void writePixel4444(int a, int r, int g, int b, void* pDest)
{
	*(unsigned short*)pDest = (unsigned short)((b & 0xF) | 16 * ((g & 0xF) | 16 * ((r & 0xF) | 16 * a)));
}


int decodePixels(WriteRowParams* pParams, int** ppSrc, int row)
{
	unsigned char* pDest = &pParams->Dest[row * pParams->StrideBytes];

	switch (pParams->Type)
	{
		case PT888:
			writeRowRGB(ppSrc, pParams->RowWidth, writePixel888, pDest, pParams->PixelParams.NumBitsColor, 8, 3);
			break;
		case PT565:
			writeRowRGB(ppSrc, pParams->RowWidth, writePixel565, pDest, pParams->PixelParams.NumBitsColor, 6, 2);
			break;
		case PT555:
			writeRowRGB(ppSrc, pParams->RowWidth, writePixel555, pDest, pParams->PixelParams.NumBitsColor, 5, 2);
			break;
		case PT8888:
			if (pParams->PixelParams.NumBitsAlpha == 0)
				writeRowRGB(ppSrc, pParams->RowWidth, writePixel8888, pDest, pParams->PixelParams.NumBitsColor, 8, 4);
			else
				writeRowARGB(ppSrc, pParams->RowWidth, writePixel8888, pDest, pParams->PixelParams.NumBitsColor, pParams->PixelParams.NumBitsAlpha, 8, 8, 4);
			break;
		case PT1555:
			if (pParams->PixelParams.NumBitsAlpha == 0)
				writeRowRGB(ppSrc, pParams->RowWidth, writePixel1555, pDest, pParams->PixelParams.NumBitsColor, 5, 2);
			else
				writeRowARGB(ppSrc, pParams->RowWidth, writePixel1555, pDest, pParams->PixelParams.NumBitsColor, pParams->PixelParams.NumBitsAlpha, 5, 1, 2);
			break;
		case PT4444:
			if (pParams->PixelParams.NumBitsAlpha == 0)
				writeRowRGB(ppSrc, pParams->RowWidth, writePixel4444, pDest, pParams->PixelParams.NumBitsColor, 4, 2);
			else
				writeRowARGB(ppSrc, pParams->RowWidth, writePixel4444, pDest, pParams->PixelParams.NumBitsColor, pParams->PixelParams.NumBitsAlpha, 4, 4, 2);
			break;
		case PT8:
			writeRowUniform(ppSrc, pParams->RowWidth, pDest, pParams->PixelParams.NumBitsColor, 1);
			break;
		case PT16:
			writeRowUniform(ppSrc, pParams->RowWidth, pDest, pParams->PixelParams.NumBitsColor, 2);
			break;
		case PT32:
		{
			float* dest = (float*)pDest;  // NOLINT(clang-diagnostic-cast-align)
			for (int i = 0; i < pParams->RowWidth; ++i)
			{
				dest[i] = (float)ppSrc[0][i] * pParams->PixelParams.Scale + pParams->PixelParams.Bias;
			}
			break;
		}
		case 0:
		default:  // NOLINT(clang-diagnostic-covered-switch-default)
			return -36;
	}
	return 0;
}
