#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "PTC.h"

#include <assert.h>


#pragma warning(disable: 6385)

// Block order
// r[0]  r[1]  r[2]  r[3]
// r[4]  r[5]  r[6]  r[7]
// r[8]  r[9]  r[10] r[11]
// r[12] r[13] r[14] r[15]


// Peano + Spatial Frequency
// Ordered Scan (Fig 7)
const unsigned char kBlockOffsetAC[16] =
{
	0x00, // [0][0] 0
	0x04, // [0][1] 1
	0x44, // [1][1] 5
	0x40, // [1][0] 4
	0x80, // [2][0] 8
	0xC0, // [3][0] 12
	0xC4, // [3][1] 13
	0x84, // [2][1] 9
	0x88, // [2][2] 10
	0xC8, // [3][2] 14
	0xCC, // [3][3] 15
	0x8C, // [2][3] 11
	0x4C, // [1][3] 7
	0x48, // [1][2] 6
	0x08, // [0][2] 2
	0x0C  // [0][3] 3
};


// Spatial-Frequency Scan (Fig 8)
const unsigned char kBlockOffsetDC[16] =
{
	0x00, // [0][0] 0
	0x04, // [0][1] 1
	0x44, // [1][1] 5
	0x40, // [1][0] 4
	0x80, // [2][0] 8
	0x84, // [2][1] 9
	0x48, // [1][2] 6
	0x08, // [0][2] 2
	0x0C, // [0][3] 3
	0x4C, // [1][3] 7
	0x88, // [2][2] 10
	0xC4, // [3][1] 13
	0xC0, // [3][0] 12
	0xC8, // [3][2] 14
	0xCC, // [3][3] 15
	0x8C  // [2][3] 11
};


// Stream functions were refactored from FSX IStream
// This can be replaced with an actual stream in the future and/or
// made pluggable, but for now a simple C heap byte array will suffice


int streamSetPosition(PTCImage* image, int position)
{
	if (image == 0 || position < 0 || position > image->StreamLength)
		return -1;

	image->StreamPosition = position;
	
	return 0;
}


int streamRead(PTCImage* image, void* dest, int bytesToRead, int* pBytesRead)
{
	if (!image)
		return -1;

	memcpy(dest, (const char*)image->StreamBytes + image->StreamPosition, bytesToRead);
	image->StreamPosition += bytesToRead;
	*pBytesRead = bytesToRead;

	return 0;
}


int readArray(PTCImage* image, void* dest, int stride, int numEntries)
{
	 int bytesRead = 0;
	 streamRead(image, dest, numEntries * stride, &bytesRead);
	 return bytesRead / stride;
}


int readTile(PTCImage* image, int tile)
{
	if (tile < 0 || tile >= image->Frame.TileCount)
		return -18;

	if (streamSetPosition(image, image->TileOffsets[tile]))
		return -11;

	return readArray(image, image->TileData, image->TileSizes[tile], 1)
		== 1 ? 0 : -12;
}


int readTileLengths(PTCImage* image)
{
	int offset = image->Header.OffsetToFrame + image->Frame.OffsetToData + 
		(int)sizeof(int) * image->Header.NumFrames + 
		(int)sizeof(PTCFileHeader) + (int)sizeof(PTCFrameHeader);
	
	if (streamSetPosition(image, offset))
		return -11;

	const int size = (int)sizeof(int) * image->Frame.TileCount;
	
	if (readArray(image, image->TileSizes, size, 1) != 1)
		return -12;

	offset += size;
	for (int i = 0; i < image->Frame.TileCount; ++i)
	{
		image->TileOffsets[i] = offset;
		offset += image->TileSizes[i];
	}
	
	return 0;
}


int readPTCFrameLength(PTCImage* image, int* numBytes)
{
	if (streamSetPosition(image, image->Header.OffsetToFrame + (int)sizeof(PTCFileHeader)))
		return -11;

	return readArray(image, numBytes, (int)sizeof(int), 1) != 1 ? -12 : 0;
}


int readPTCFrameHeader(PTCImage* image)
{
	if (streamSetPosition(image, image->Header.OffsetToFrame + (int)sizeof(PTCFileHeader) + 
		(int)sizeof(int) * image->Header.NumFrames))
		return -11;

	return readArray(image , &image->Frame.CompressedLength, 
		(int)sizeof(PTCFrameHeader), 1) != 1 ? -12 : 0;
}


void getChannelInfo(PTCImage* image, int channel, int* isAlpha, int* isOneBitAlpha, int* isScaledQuantized)
{
	*isAlpha = 0;
	if (channel == image->Frame.NumChannels - 1 && image->Frame.Flags & 0x1000)
		*isAlpha = 1;

	*isOneBitAlpha = 0;
	if (image->Frame.OneBitAlpha & 1 << channel)
		*isOneBitAlpha = 1;

	*isScaledQuantized = 0;
	const int colorSpace = ((unsigned char)image->Frame.Flags >> 4) & 0xF;
	if (image->Frame.NumChannels >= 3 &&
		(colorSpace >= 1 && colorSpace <= 3) &&
		(channel >= 1 && channel <= 3))
	{
		*isScaledQuantized = 1;
	}
}


void reorder(int* pSrc, int* pCoefficientsL0, int* pCoefficientsL1, int chunkWidth,
	 int stride, int genL0, int genL1)
{
	if (chunkWidth <= 0)
		return;

	// Fig 8, [0] is DC
	int* sg0 = &pSrc[1 * chunkWidth];
	int* sg1 = &pSrc[4 * chunkWidth];
	int* sg2 = &pSrc[8 * chunkWidth];
	int* sg3 = &pSrc[13 * chunkWidth];
	
	// L2 DC Coefficients (2 LBT)
	for (int i = 0, j = 0; j < chunkWidth; ++i, j += 16)
	{
		pCoefficientsL0[j] = pSrc[i];
		if (genL1)
		{
			pCoefficientsL1[j >> 2] = (pSrc[i] + 2) >> 2;
		}
	}

	for (int i = 0; i < chunkWidth; i += 16)
	{
		for (int k = 0; k < 16; ++k)
		{
			// k == 0 is L2 DC
			if (k != 0)
			{
				// L1 DC Coefficients (1 LBT) - Block
				const int dc = pSrc[(i >> 4) + k * 16];
				pCoefficientsL0[i +
					chunkWidth * (kBlockOffsetDC[k] >> 4) +
					(kBlockOffsetDC[k] & 0xF)] = dc;

				if (genL1)
				{
					pCoefficientsL1[
						((i + (kBlockOffsetDC[k] & 0xF)) >> 2) +
							(stride >> 2) * (kBlockOffsetDC[k] >> 6)] = (dc + 2) >> 2;
				}
			}

			if (!genL0)
				continue;

			// L1 AC Coefficients
			int* dest = &pCoefficientsL0[i +     // Block
				chunkWidth * (kBlockOffsetAC[k] >> 4) + // Row
				(kBlockOffsetAC[k] & 0xF)];           // Column

			// See Figure 8 for subgroups
			// [0] is DC
			dest[1] = sg0[0];
			dest[2] = sg1[0];
			dest[3] = sg2[0];

			dest += chunkWidth;
			dest[0] = sg0[1];				
			dest[1] = sg0[2];
			dest[2] = sg1[1];
			dest[3] = sg2[1];

			dest += chunkWidth;
			dest[0] = sg1[2];
			dest[1] = sg1[3];
			dest[2] = sg2[2];
			dest[3] = sg3[0];

			dest += chunkWidth;
			dest[0] = sg2[3];
			dest[1] = sg2[4];
			dest[2] = sg3[1];
			dest[3] = sg3[2];

			sg0 += 3;
			sg1 += 4;
			sg2 += 5;
			sg3 += 3;
		}
	}
}


int decodeTile(PTCImage* image, int tile, int width, int numChannels, int** ppCoefficientsL0, int** ppCoefficientsL1)
{
	int isAlpha = 0, isOneBitAlpha = 0, isScaledQuantized = 0;
	int chunkWidth = image->Frame.ChunkWidth;
	int genFull = 0;
	int genMip = 0;
	int totalBytesRead = 0;
	int channelOffsets[16];
	memset(channelOffsets, 0, 16 * sizeof(int));

	if (image->Params->MipGenerate[0])
	{
		genFull = 1;
	}

	for (int index = 2; index < 5; ++index)
	{
		if (image->Params->MipGenerate[index])
		{
			genMip = 1;
			break;
		}
	}

	const int numChunks = (image->Frame.ChunkWidth + width - 1) / image->Frame.ChunkWidth;
	for (int chunk = 0; chunk < numChunks; ++chunk)
	{
		if (chunk == numChunks - 1)
			chunkWidth = width - chunk * chunkWidth;

		if (readTile(image, chunk + image->Params->LeftChunk +
			image->Params->ChunksPerRow * (tile + image->Params->TopChunk)))
		{
			return totalBytesRead;
		}

		unsigned char* pSrc = image->TileData;
		int compressedLength = image->CurrentTileSize; // TODO - this is never updated

		for (int channel = 0; channel < image->Frame.NumChannels; ++channel)
		{
			int headerBytes = 0;
			int* pDest = &image->VectorEntropy[channel * 16 * chunkWidth];
			unsigned char* pSrcInit = pSrc;

			getChannelInfo(image, channel, &isAlpha, &isOneBitAlpha, &isScaledQuantized);

			int numCoefficients;
			if (isOneBitAlpha)
			{
				numCoefficients = (16 * chunkWidth) / 4;
			}
			else if (*pSrc & 1)
			{
				numCoefficients = 4 * (*(unsigned short*)pSrc & 0xFFFE);  // NOLINT(clang-diagnostic-cast-align)
				pSrc += 2;
				headerBytes = 2;
			}
			else
			{
				numCoefficients = 4 * *pSrc++;
				headerBytes = 1;
			}

			int qs;
			if (isAlpha)
				qs = image->Frame.QSAlpha;
			else
				qs = image->Frame.QSColor;

			int entropyBytes = 0;

			if (numCoefficients)
			{
				int totalBytes = image->CurrentTileSize - compressedLength;
				if (channel != image->Frame.NumChannels - 1)
				{
					totalBytes = *(unsigned short*)pSrc;  // NOLINT(clang-diagnostic-cast-align)
					pSrc += 2;
					headerBytes += 2;
				}

				int type = (isAlpha ? image->Frame.Flags >> 2 : image->Frame.Flags) & 3;
				if (isOneBitAlpha)
					type = 2;

				switch (type)
				{
				case 3:
					memcpy(pDest, pSrc, sizeof(int) * numCoefficients);
					entropyBytes = (int)sizeof(int) * numCoefficients;
					break;
				case 0:
					if (numCoefficients <= chunkWidth)
					{
						entropyBytes = entropyBPC(pSrc, compressedLength, 0, pDest, numCoefficients, 1);
					}
					else
					{
						entropyBytes = entropyBPC(pSrc, compressedLength, 0, pDest, chunkWidth, 1);
						if (genFull)
						{
							entropyBytes += entropyBPC(pSrc + entropyBytes, compressedLength - entropyBytes, 0, pDest + chunkWidth, numCoefficients - chunkWidth, 1);
						}
						else
						{
							headerBytes = 0;
							entropyBytes = totalBytes;
						}
					}
					break;
				case 1:
					if (numCoefficients <= chunkWidth)
					{
						int range = 81920000 >> (24 - image->Frame.BitDepth);
						if (qs)
							range = range / ((qs >> 3) + 1) + 1;
						entropyBytes = entropyRLGR(pSrc, compressedLength, pDest, numCoefficients, range);
					}
					else
					{
						int range = 81920000 >> (24 - image->Frame.BitDepth);
						if (qs)
							range = range / ((qs >> 3) + 1) + 1;
						entropyBytes = entropyRLGR(pSrc, compressedLength, pDest, chunkWidth, range);
						if (genFull)
						{
							range = 6225920 >> (24 - image->Frame.BitDepth);
							if (qs)
								range = range / ((qs >> 3) + 1) + 1;
							entropyBytes += entropyRLGR(pSrc + entropyBytes, compressedLength - entropyBytes, pDest + chunkWidth, numCoefficients - chunkWidth, range);
						}
						else
						{
							headerBytes = 0;
							entropyBytes = totalBytes;
						}
					}
					break;
				case 2:
					entropyBytes = entropyBLC(pSrc, compressedLength, (unsigned char*)pDest, (unsigned char*)image->VectorReorder, 16, chunkWidth);
					break;
				default:
					return totalBytesRead;
				}
			}
			const int offset = channelOffsets[channel] + channel * width;
			if (isOneBitAlpha)
			{
				unsigned char* pDestBye = (unsigned char*)pDest;
				for (int i = 0; i < 16; i++)
				{
					if (image->Frame.BitCount == 1)
					{
						for (int j = 0; j < chunkWidth; ++j)
							ppCoefficientsL0[16 + i][offset + j] = *pDestBye++;
					}
					else
					{
						const int bit = (1 << (image->Frame.BitCount - 1)) - 1;
						for (int j = 0; j < chunkWidth; ++j)
							ppCoefficientsL0[16 + i][offset + j] = *pDestBye++ ? bit : -1 - bit;
					}
				}
			} 
			else
			{
				int dcCount = chunkWidth / 16;
				if (numCoefficients < dcCount)
					dcCount = numCoefficients;

				for (int j = 1; j < dcCount; ++j) // LL3 differential
					pDest[j] += pDest[j - 1];

				if (qs) // dequantize
				{
					if (isScaledQuantized)
						qs *= 2;
					for (int k = 0; k < numCoefficients; ++k)
						pDest[k] *= qs;
				}
				
				if (genFull)
					memset(pDest + numCoefficients, 0, sizeof(int) * (size_t)(16 * chunkWidth - numCoefficients));

				const int mipOffset = numChannels * width * tile + 
					(channel * (width / 4) + chunk * (image->Frame.ChunkWidth / 4));
				
				reorder(
					pDest,
					image->VectorReorder,
					image->CoefficientsMip + mipOffset,
					chunkWidth,
					numChannels * width,
					genFull,
					genMip);

				for (int i = 0; i < 16; ++i)
				{
					memcpy(&ppCoefficientsL0[16 + i][offset],
						&image->VectorReorder[i * chunkWidth], 
						chunkWidth * sizeof(int));
				}
			}
			compressedLength -= entropyBytes;
			totalBytesRead += headerBytes + entropyBytes;
			pSrc = pSrcInit + headerBytes + entropyBytes;
			channelOffsets[channel] += chunkWidth;
		}
	}
	
	if (image->Params->MipGenerate[1])
	{
		for (int i = 0; i < 16; ++i)
		{
			memcpy(ppCoefficientsL1[16 + i], ppCoefficientsL0[16 + i], sizeof(int) * numChannels * width);
		}
	}
	return totalBytesRead;
}


int decompress(PTCImage* image)
{
	int isAlpha = 0;
	int isOneBitAlpha = 0;
	int isScaledQuantized = 0;
	
	int* ppCoefficientsL0[32];
	int* ppCoefficientsL1[32];
	memset(&ppCoefficientsL0, 0, 32 * sizeof(int*));
	memset(&ppCoefficientsL1, 0, 32 * sizeof(int*));

	int useOverlapTransform = 1;
	if (image->Frame.Flags & 0x800)
		useOverlapTransform = 0;

	int height = image->TotalHeight;
	int width = image->TotalWidth;
	if ((image->Frame.Flags & 0xF0) == 64)
	{
		height /= 2;
		width /= 2;
		useOverlapTransform = 0;
		image->Frame.NumChannels = 4;
	}

	if (readTileLengths(image) < 0)
		return -61;

	int bytesRead = image->Frame.OffsetToData + 4 * image->Frame.TileCount + (int)sizeof(PTCFrameHeader);

	int tile = 0;
	int rowInMacroblock = 16;
	int rowInBlock = 2;

	for (int row = 0; row < height; ++row, ++rowInBlock, ++rowInMacroblock)
	{
		if (row == 0)
		{
			for (int i = 0; i < 32; i++)
			{
				ppCoefficientsL0[i] = image->CoefficientsL0 + i * width * image->Frame.NumChannels;
				if (image->Params->MipGenerate[1])
					ppCoefficientsL1[i] = image->CoefficientsL1 + i * width * image->Frame.NumChannels;
			}

			bytesRead += decodeTile(image, tile++, width, image->Frame.NumChannels, ppCoefficientsL0, ppCoefficientsL1);

			for (int channel = 0; channel < image->Frame.NumChannels; ++channel)
			{
				getChannelInfo(image, channel, &isAlpha, &isOneBitAlpha, &isScaledQuantized);
				if (!isOneBitAlpha)
				{
					if (image->Params->MipGenerate[0])
						invTransformInitial(ppCoefficientsL0, width, channel * width, useOverlapTransform, 0);
					if (image->Params->MipGenerate[1])
						invTransformInitial(ppCoefficientsL1, width, channel * width, useOverlapTransform, 1);
				}
			}
		}
		if (rowInMacroblock == 16)
		{
			rowInMacroblock = 0;
			
			// alternate block order
			int blockRow = tile % 2 ? 16 : 0;
			for (int i = 0; i < 32; i++)
			{
				ppCoefficientsL0[i] = image->CoefficientsL0 + width * image->Frame.NumChannels * blockRow;
				if (image->Params->MipGenerate[1])
					ppCoefficientsL1[i] = image->CoefficientsL1 + width * image->Frame.NumChannels * blockRow;
				if (++blockRow == 32)
					blockRow = 0;
			}
			
			if (row != height - 16)
			{
				bytesRead += decodeTile(image, tile++, width, image->Frame.NumChannels, ppCoefficientsL0, ppCoefficientsL1);
				
				for (int channel = 0; channel < image->Frame.NumChannels; ++channel)
				{
					getChannelInfo(image, channel, &isAlpha, &isOneBitAlpha, &isScaledQuantized);
					if (!isOneBitAlpha)
					{
						if (image->Params->MipGenerate[0])
							invTransformMacroblock(ppCoefficientsL0, width, channel * width, useOverlapTransform, row == height - 32);
						if (image->Params->MipGenerate[1])
							invTransformMacroblock(ppCoefficientsL1, width, channel * width, useOverlapTransform, row == height - 32);
					}
				}
			}
		}
		if (rowInBlock == 4 && row != height - 2)
		{
			rowInBlock = 0;
			
			for (int channel = 0; channel < image->Frame.NumChannels; ++channel)
			{
				getChannelInfo(image, channel, &isAlpha, &isOneBitAlpha, &isScaledQuantized);
				if (!isOneBitAlpha)
				{
					if (image->Params->MipGenerate[0])
						invTransformBlock(ppCoefficientsL0, width, channel * width, useOverlapTransform, 0, row == height - 6, rowInMacroblock);
					if (image->Params->MipGenerate[1])
						invTransformBlock(ppCoefficientsL1, width, channel * width, useOverlapTransform, 1, row == height - 6, rowInMacroblock);
				}
			}
		}
		if (row >= image->Params->TopOffset)
		{
			if (image->Params->MipGenerate[0])
				decodeRowNoMip(
					image,
					image->Params->RowParams[0],
					image->Params->LeftOffset,
					row - image->Params->TopOffset,
					image->TotalWidth,
					ppCoefficientsL0[rowInMacroblock],
					image->ChannelVectors,
					0);
		}
	}

	// Mip logic removed

	return bytesRead;
}


void cleanup(PTCImage* image)
{
	if (image->Params->HasMipmaps)
	{
		if (image->Params->CoefficientsL2 != 0)
			free(image->Params->CoefficientsL2);
		if (image->Params->CoefficientsL3 != 0)
			free(image->Params->CoefficientsL3);
		if (image->Params->CoefficientsL4 != 0)
			free(image->Params->CoefficientsL4);
	}
	free(image->TileData);
	free(image->VectorEntropy);
	free(image->VectorReorder);
	for (int i = 0; i < image->Frame.NumChannels; ++i)
	{
		if (image->ChannelVectors[i] != 0)
			free(image->ChannelVectors[i]);
	}
	free(image->TileOffsets);
	free(image->TileSizes);
	if (image->Params->MipGenerate[1])
		free(image->CoefficientsL1);
	free(image->CoefficientsL0);
	
}


int processHeader(PTCImage* image)
{
	if (strncmp(image->Header.Magic, "PTC+MSHM", 8u) != 0 || image->Header.VersionMajor > 0x1AE)
		return -16;
	
	image->TotalWidth = 32 * ((image->Frame.Width + 31) >> 5);
	image->TotalHeight = 32 * ((image->Frame.Height + 31) >> 5);
	image->Params->LeftOffset = 0;
	image->Params->TopOffset = 0;
	image->Params->TopChunk = 0;
	image->Params->LeftChunk = 0;
	image->Params->ChunksPerRow = (image->TotalWidth + image->Frame.ChunkWidth - 1) / image->Frame.ChunkWidth;
	
	if (image->Params->HasSubregion)
	{
		if (image->Params->Left < 0
			|| image->Params->Width < 0
			|| image->Params->Left + image->Params->Width > image->Frame.Width
			|| image->Params->Top < 0
			|| image->Frame.Height < 0
			|| image->Frame.Height + image->Params->Top > image->Frame.Height)
			return -67;

		image->Params->TopChunk = (image->Params->Top - 10) / 16;
		if (image->Params->TopChunk < 0)
			image->Params->TopChunk = 0;

		int bottomChunk = (image->Frame.Height + image->Params->Top + 9) / 16;
		if (bottomChunk >= (image->TotalHeight + 15) / 16)
			bottomChunk = (image->TotalHeight + 15) / 16 - 1;

		image->Params->LeftChunk = (image->Params->Left - 10) / image->Frame.ChunkWidth;
		if (image->Params->LeftChunk < 0)
			image->Params->LeftChunk = 0;

		int rightChunk = (image->Params->Left + image->Params->Width + 10) / image->Frame.ChunkWidth;
		if (rightChunk >= image->Params->ChunksPerRow)
			rightChunk = image->Params->ChunksPerRow - 1;

		image->TotalHeight = 32 * ((16 * (bottomChunk - image->Params->TopChunk) + 47) >> 5);

		const int leftChunkWidth = image->Frame.ChunkWidth * image->Params->LeftChunk;
		int totalWidth = image->Frame.ChunkWidth * (rightChunk - image->Params->LeftChunk + 1);
		if (leftChunkWidth + totalWidth > image->TotalWidth)
			totalWidth = image->TotalWidth - leftChunkWidth;

		image->TotalWidth = totalWidth;
		image->Frame.Height = image->Params->Height;
		image->Frame.Width = image->Params->Width;
		image->Params->LeftOffset = image->Params->Left - leftChunkWidth;
		image->Params->TopOffset = image->Params->Top - 16 * image->Params->TopChunk;
	}
	
	if (image->Params->HasMipmaps)
	{
		if (image->Frame.BitCount == 1)
			return -71;
		if (image->Frame.Flags & 0x800)
			return -72;
		if ((image->Frame.Flags & 0xF0) == 64)
			return -70;
	}
	
	int allocRow = image->TotalWidth * image->Frame.NumChannels;
	int allocChunk = image->Frame.ChunkWidth * image->Frame.NumChannels;
	if ((image->Frame.Flags & 0xF0) == 64)
	{
		if (image->Frame.NumChannels != 1)
			return -66;
		allocRow = image->TotalWidth * 2;
		allocChunk = 4 * image->Frame.ChunkWidth;
	}
	
	image->CoefficientsL0 = malloc((size_t)allocRow * 32 * sizeof(int));
	if (image->Params->MipGenerate[1])
		image->CoefficientsL1 = malloc((size_t)allocRow * 32 * sizeof(int));
	image->TileSizes = malloc(sizeof(int) * image->Frame.TileCount);
	image->TileOffsets = malloc(sizeof(int) * image->Frame.TileCount);

	if (image->CoefficientsL0 == 0
		|| (image->Params->MipGenerate[1] && image->CoefficientsL1 == 0)
		|| image->TileSizes == 0 || image->TileOffsets == 0)
		return -65;
	
	for (int i = 0; i < image->Frame.NumChannels; ++i)
	{
		image->ChannelVectors[i] = calloc(image->TotalWidth, 4);
		if (image->ChannelVectors[i] == 0)
			return -65;
	}

	image->CurrentTileSize = image->Frame.TileLength;
	image->VectorReorder = malloc((size_t)image->Frame.ChunkWidth * 16 * sizeof(uintptr_t));
	image->VectorEntropy = malloc((size_t)allocChunk * 16 * sizeof(uintptr_t));
	image->TileData = malloc((size_t)image->Frame.TileLength + 1);
	if (image->VectorReorder == 0 || image->VectorEntropy == 0 || image->TileData == 0)
		return -65;

	image->Params->CoefficientsL2 = 0;
	image->Params->CoefficientsL3 = 0;
	image->Params->CoefficientsL4 = 0;
	if (image->Params->HasMipmaps)
	{
		const size_t allocMip = (size_t)(image->TotalHeight / 4) * (image->TotalWidth / 4) * image->Frame.NumChannels;
		if (image->Params->MipGenerate[2])
			image->Params->CoefficientsL2 = calloc(allocMip, 4);

		if (image->Params->MipGenerate[3])
			image->Params->CoefficientsL3 = calloc(allocMip, 4);

		if (image->Params->MipGenerate[4])
			image->Params->CoefficientsL4 = calloc(allocMip, 4);

		image->CoefficientsMip = image->Params->CoefficientsL2;
	}  
	return 0;
}


int PTCDecompress(DecodeParams* dp, const void* pSrc, int length)
{
	int numBytesToRead = 0;

	PTCImage image = {0};
	image.Params = dp;
	image.StreamLength = length;
	image.StreamBytes = pSrc;

	if (readArray(&image, &image, sizeof(PTCFileHeader), 1) != 1 || 
		image.Header.NumFrames != 1)
		return -1;
	int totalBytesRead = sizeof(PTCFileHeader);

	if (readPTCFrameLength(&image, &numBytesToRead))
		return -1;
	totalBytesRead += sizeof(int);

	if(readPTCFrameHeader(&image) || numBytesToRead != image.Frame.CompressedLength)
		return -1;
	//totalBytesRead += sizeof(PTCFrameHeader); // This is incorporated into decompress()

	if (processHeader(&image))
	{
		cleanup(&image);
		return -1;
	}

	const int bytesRead = decompress(&image);
	assert(bytesRead == numBytesToRead);
	totalBytesRead += bytesRead;

	cleanup(&image);

	assert(totalBytesRead == length);
	return totalBytesRead;
}
