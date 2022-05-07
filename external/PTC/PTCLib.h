#pragma once

typedef enum
{
	PTNone = 0,   // Unknown
	PT888 = 1,   // 24 bit | RGB 888
	PT8888 = 2,   // 32 bit | RGBA 8888
	PT565 = 3,   // 16 bit | RGB 565
	PT1555 = 4,   // 16 bit | ARGB 1555
	PT555 = 5,   // 16 bit | RGB 555
	PT4444 = 6,   // 16 bit | ARGB 4444
	PT8 = 7,   //  8 bit | Uniform
	PT16 = 8,   // 16 bit | Uniform
	PT32 = 9    // 32 bit | Uniform (FP)
}PixelType;


#pragma pack(push)
#pragma pack(4)
typedef struct
{
	unsigned char* Dest;
	PixelType Type;
	int RowWidth;
	int StrideBytes;
	union
	{
		struct
		{
			float Scale;
			float Bias;
		};
		struct
		{
			int NumBitsColor;
			int NumBitsAlpha;
		};
	}PixelParams;
} WriteRowParams;


typedef struct
{
	int HasSubregion; // 0
	int Top; // 4
	int Left; // 8
	int Height; // 12
	int Width; // 16
	int ChunksPerRow; // Full blocks
	int TopChunk;
	int LeftChunk;
	int TopOffset;
	int LeftOffset; // 36
	int LP2; // 40
	int LP4; // 44
	int Reserved1;
	int HasMipmaps; // 52
	int MipGenerate[5]; // 56
	WriteRowParams* RowParams[5]; // 76
	int* CoefficientsL2;
	int* CoefficientsL3;
	int* CoefficientsL4;
} DecodeParams;

#pragma pack( pop )


#ifdef __cplusplus
extern "C"
#endif
int PTCDecompress(DecodeParams* dp, const void* pSrc, int length);
