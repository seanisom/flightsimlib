#include "PTC.h"


// TODO Validate mip logic
void recolorY(PTCImage* image, int rowWidth, const int* pSrc, int leftOffset, int** ppChannels,
	 int bLosslessColor, int bLosslessAlpha, int bitDepth, int isOddMipLevel)
{
	const int dynamicRange = 32768 >> (16 - bitDepth);
	const int minValue = -dynamicRange;
	const int maxValue = dynamicRange - 1;

	int width = image->Frame.Width;
	int offset = 1;
	if (isOddMipLevel)
	{
		width = image->Frame.Width >> 1;
		offset = 2;
	}

	for (int channel = 0; channel < image->Frame.NumChannels; ++channel)
	{
		int isAlpha = 0, isOneBitAlpha = 0, setting3 = 0;
		getChannelInfo(image, channel, &isAlpha, &isOneBitAlpha, &setting3);

		int lossLess = bLosslessAlpha;
		if (!isAlpha)
			lossLess = bLosslessColor;
		if (isOneBitAlpha)
			lossLess = 1;

		for (int i = 0; i < width; ++i)
		{
			int v = pSrc[leftOffset + rowWidth * channel + offset * i];

			if (!lossLess)
			{
				v = (v + 4) >> 3;
				if (v < minValue)
					v = minValue;
				else if (v > maxValue)
					v = maxValue;
			}

			ppChannels[channel][i] = v;
		}
	}
}


void recolorYCoCg(int width, int numChannels, int rowWidth, const int* pSrc, int leftOffset, int** ppChannels,
	 int bLosslessColor, int bLosslessAlpha, int colorSpace, int bHasAlpha, int bitDepth, int isOddMipLevel)
{
	const int dynamicRange = 32768 >> (16 - bitDepth);
	const int minValue = -dynamicRange;
	const int maxValue = dynamicRange - 1;
	 
	int offset = 1;
	if (isOddMipLevel)
	{
		width >>= 1;
		offset = 2;
	}

	if (colorSpace == 3)
	{
		for (int i = 0; i < width; ++i)
		{
			const int y0 = pSrc[leftOffset + 0 * rowWidth + offset * i];
			const int co = pSrc[leftOffset + 1 * rowWidth + offset * i];
			const int cg = pSrc[leftOffset + 2 * rowWidth + offset * i];
			const int k0 = pSrc[leftOffset + 3 * rowWidth + offset * i];

			const int s = y0 - (k0 >> 1);
			const int t = s - (cg >> 1);
			int k = s + k0;
			int m = t + cg;
			int y = t - (co >> 1);
			int c = y + co;

			if (!bLosslessColor)
			{
				c = (c + 4) >> 3;
				m = (m + 4) >> 3;
				y = (y + 4) >> 3;
				k = (k + 4) >> 3;

				if (c < minValue)
					c = minValue;
				else if (c > maxValue)
					c = maxValue;

				if (m < minValue)
					m = minValue;
				else if (m > maxValue)
					m = maxValue;

				if (y < minValue)
					y = minValue;
				else if (y > maxValue)
					y = maxValue;

				if (k < minValue)
					k = minValue;
				else if (k > maxValue)
					k = maxValue;
			}

			ppChannels[0][i] = c;
			ppChannels[1][i] = m;
			ppChannels[2][i] = y;
			ppChannels[3][i] = k;
		}
	}
	else
	{
		for (int i = 0; i < width; ++i)
		{
			const int y  = pSrc[leftOffset + 0 * rowWidth + offset * i];
			const int co = pSrc[leftOffset + 1 * rowWidth + offset * i];
			const int cg = pSrc[leftOffset + 2 * rowWidth + offset * i];

			const int t = y - (cg >> 1);
			int g = t + cg;
			int b = t - (co >> 1);
			int r = b + co;

			if (!bLosslessColor)
			{
				r = (r + 4) >> 3;
				g = (g + 4) >> 3;
				b = (b + 4) >> 3;

				if (r < minValue)
					r = minValue;
				else if (r > maxValue)
					r = maxValue;

				if (g < minValue)
					g = minValue;
				else if (g > maxValue)
					g = maxValue;

				if (b < minValue)
					b = minValue;
				else if (b > maxValue)
					b = maxValue;
			}

			ppChannels[0][i] = r;
			ppChannels[1][i] = g;
			ppChannels[2][i] = b;
		}
	}
	if (bHasAlpha)
	{
		for (int i = 0; i < width; ++i)
		{
			int v = pSrc[leftOffset + (numChannels - 1) * rowWidth + offset * i];
			if (!bLosslessAlpha)
			{
				v = (v + 4) >> 3;

				if (v < minValue)
					v = minValue;
				else if (v > maxValue)
					v = maxValue;
			}

			ppChannels[numChannels - 1][i] = v;
		} 
	}
}

// This function has been modified due to undefined behavior present in the binary
// It is only used for HDR and thus was not hooked up, so was minimally modified to work
void recolorYCrCxDc(int rowWidth, const int* pSrc, int width, int leftOffset, 
	 int** ppChannels, int bayerPattern, int bLossless, int bitDepth)
{
	if (bayerPattern)
		return;

	const int dynamicRange = 32768 >> (16 - bitDepth);
	const int minValue = -dynamicRange;
	const int maxValue = dynamicRange - 1;

	for (int i = 0; i < width >> 1; ++i)
	{
		const int Y  = pSrc[leftOffset + 0 * rowWidth + i];
		const int Cr = pSrc[leftOffset + 1 * rowWidth + i];
		const int Cx = pSrc[leftOffset + 2 * rowWidth + i];
		const int Dc = pSrc[leftOffset + 3 * rowWidth + i];

		const int t = Y - (Dc >> 1);
		int m = t + Dc - (Cr >> 1);
		int k = t - (Cx >> 1);
		int c = k + Cx;
		int y = m + Cr;

		if (!bLossless)
		{
			c = (c + 4) >> 3;
			m = (m + 4) >> 3;
			y = (y + 4) >> 3;
			k = (k + 4) >> 3;

			if (c < minValue)
				c = minValue;
			else if (c > maxValue)
				c = maxValue;

			if (m < minValue)
				m = minValue;
			else if (m > maxValue)
				m = maxValue;

			if (y < minValue)
				y = minValue;
			else if (y > maxValue)
				y = maxValue;

			if (k < minValue)
				k = minValue;
			else if (k > maxValue)
				k = maxValue;
		}
		
		ppChannels[0][0 + 2 * i] = c;
		ppChannels[0][1 + 2 * i] = m;
		ppChannels[0][0 + 2 * i + width] = y;
		ppChannels[0][1 + 2 * i + width] = k;
	}
}


int decodeRowNoMip(PTCImage* image, WriteRowParams* pParams, int leftOffset, int row, int rowWidth, 
	const int* pSrc, int** ppChannels, int mipLevel)
{
	int numChannels = image->Frame.NumChannels;
	int height = image->Frame.Height >> mipLevel;
	int width = image->Frame.Width;
	int isOddMipLevel = 0;

	switch (mipLevel)
	{
		case 1:
			isOddMipLevel = 1;
			break;
		case 2:
			width >>= 2;
			leftOffset >>= 2;
			break;
		case 3:
			width >>= 2;
			leftOffset >>= 2;
			isOddMipLevel = 1;
			break;
		case 4:
			width >>= 4;
			leftOffset >>= 4;
			break;
		default:
			break;
	}
	
	 if ((image->Frame.Flags & 0xF0) == 64)
	 {
		  height >>= 1;
		  rowWidth >>= 1;
		  numChannels = 4;
	 }
	 if (row < 0 || row > height)
	 {
		 return -1;
	 }

	const int bLosslessColor = image->Frame.QSColor == 0;
	int bLosslessAlpha = image->Frame.QSAlpha == 0;
	if (image->Frame.Flags & 0x1000 && (1 << (image->Frame.NumChannels - 1)) & image->Frame.OneBitAlpha)
		bLosslessAlpha = 1;

	int colorSpace = (image->Frame.Flags >> 4) & 0xF;
	if (numChannels < 3 && colorSpace != 4)
		colorSpace = 0;

	switch(colorSpace)
	{
	case 0:
		recolorY(
			image,
			rowWidth,
			pSrc,
			leftOffset,
			ppChannels,
			bLosslessColor,
			bLosslessAlpha,
			image->Frame.BitDepth,
			isOddMipLevel
		);
		break;
	case 1:
	case 2:
	case 3:
		recolorYCoCg(
			width,
			numChannels,
			rowWidth,
			pSrc,
			leftOffset,
			ppChannels,
			bLosslessColor,
			bLosslessAlpha,
			colorSpace,
			(image->Frame.Flags >> 12) & 1, // hasAlpha
			image->Frame.BitDepth,
			isOddMipLevel
		);
		break;
	case 4:
		// Note: this was >> 6 in ptcdecode, but this is actually wrong
		recolorYCrCxDc(
			rowWidth,
			pSrc,
			width,
			leftOffset,
			ppChannels,
			(image->Frame.Flags >> 8) & 0xF,
			bLosslessColor,
			image->Frame.BitDepth
		);
		break;
	default:
		return -1;
	}

	decodePixels(pParams, ppChannels, row);
	return 0;
}
