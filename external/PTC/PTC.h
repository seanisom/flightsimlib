#pragma once

#include "PTCLib.h"


typedef struct 
{
	char Magic[8];
	short VersionMajor; // 430
	short VersionMinor; // Unused
	short Reserved1; // Unused
	short Reserved2; // Unused
	int DataLength; // Size of file - this header and metadata
	int NumFrames; // 1 always
	int Reserved3; // Unused
	int Reserved4; // Unused
	int Reserved5; // Unused
	int OffsetToFrame; // Length of global metadata (can be 0)
} PTCFileHeader;


typedef struct
{
	int CompressedLength; // 0x571B
	int TileLength;  // 0x503 - largest compressed tile length
	int TileCount; // 16
	int Reserved1; // 0 - Impossible to set
	int Reserved2; // 0 - Impossible to set
	unsigned short OneBitAlpha; // 0 - Channel mask
	// 0-1 EC | 2-3 ECA | 4-7 CS | 8-10 Bayer | 11 NPP | 12 Alpha | 13 HDR 
	unsigned short Flags; // 5
	int QSColor; // 1E
	int QSAlpha; // 4
	int Height; // 256
	int Width; // 256
	int Reserved3; // 0 - 4.30, 11811 - 4.10
	int Reserved4; // 0 - 4.30, 11811 - 4.10
	short BitCount; // 24 - Bits Per Pixel from source bmp +14
	short BitDepth; // 16 - bd (bits to preserve)
	short NumChannels; // 1
	short ChunkWidth; // 256
	int Reserved5; // 0x898 - Unused - Raw 1000 else 2200
	int Reserved6; // 0 - Impossible to set (4.10 stack garbage)
	float Scale; // -1.0f
	int FloatMode; // 0
	int OffsetToData; // 5 offset past frame metadata
} PTCFrameHeader;


typedef struct
{
	PTCFileHeader Header;
	PTCFrameHeader Frame;
	unsigned char* TileData;
	int* TileSizes;
	int* TileOffsets;
	int CurrentTileSize;
	int* ChannelVectors[16];
	int* CoefficientsMip;
	int* CoefficientsL0;
	int* CoefficientsL1;
	int* VectorReorder;
	int* VectorEntropy;
	int TotalHeight;
	int TotalWidth;
	int Reserved1; // 224
	int Reserved2; // 228
	DecodeParams* Params;
	const void* StreamBytes;
	int StreamPosition;
	int StreamLength;
} PTCImage;


// PTCRow 
extern int decodePixels(WriteRowParams* pParams, int** ppSrc, int row);

// PTCAdaptiveDecoder
extern int entropyBPC(const unsigned char* pCompressed, int length, int planeCount, int* pDest, int destCount, int kInit);
extern int entropyRLGR(const unsigned char* pCompressed, int length, int* pDest, int destCount, int range);
extern int entropyBLC(const unsigned char* pCompressed, int length, unsigned char* pDest, unsigned char* pLines, int blockCount, int width);

// PTCTransform
extern void invTransformInitial(int** coefficients, int tileWidth, int channel, int bOverlap, int bDownscale);
extern void invTransformMacroblock(int** coefficients, int tileWidth, int channel, int bOverlap, int bLast);
extern void invTransformBlock(int** coefficients, int tileWidth, int channel, int bOverlap, int bDownscale, int bLast, int rowInMacroblock);

// PTCColorMapper
extern int decodeRowNoMip(PTCImage* image, WriteRowParams* pParams, int leftOffset, int row, int rowWidth,
	const int* pSrc, int** ppChannels, int mipLevel);

// PTC
extern void getChannelInfo(PTCImage* image, int channel, int* isAlpha, int* isOneBitAlpha, int* isScaledQuantized);
