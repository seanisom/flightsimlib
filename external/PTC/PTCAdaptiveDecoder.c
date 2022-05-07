#include <string.h>


const int kMask[8] = { 0, 1, 3, 7, 0xF, 0x1F, 0x3F, 0x7F };
const int kBitsPerByte = 8; 


typedef struct 
{
	const unsigned char* pBuffer;
	int length;
	int acck;
	int accb;
	int bytes;
} BitIO;


inline int readByte(BitIO* bio)
{
	if (bio->bytes >= bio->length)
		return -1;

	const int k = *bio->pBuffer++;
	const int val = (bio->accb << (kBitsPerByte - bio->acck)) | (k >> bio->acck);
	bio->accb = k & kMask[bio->acck];
	++bio->bytes;
	return val;
}


inline int readBit(BitIO* bio)
{
	if (!bio->acck) 
	{
		if (bio->bytes >= bio->length)
			return -1;
		
		bio->acck = kBitsPerByte;
		bio->accb = *bio->pBuffer++;
		++bio->bytes;
	}

	--bio->acck;
	const int val = bio->accb >> bio->acck;
	bio->accb &= kMask[bio->acck];
	return val;
}


inline int readBits(BitIO* bio, int nb)
{
	int val = 0;
	
	while (nb >= kBitsPerByte) 
	{
		nb -= kBitsPerByte;
		val = (val << kBitsPerByte) | readByte(bio);
	}

	if (!nb)
		return val;

	if (bio->acck < nb) 
	{
		if (bio->bytes >= bio->length)
			return -1;

		bio->accb = (bio->accb << kBitsPerByte) | (*bio->pBuffer++);
		bio->acck += kBitsPerByte;
		++bio->bytes;
	}
	
	bio->acck -= nb; 
	val = (val << nb) | (bio->accb >> bio->acck);
	bio->accb &= kMask[bio->acck];
	
	return val;
}


inline int flushBitIO(BitIO* bio)
{
	bio->acck = 0;
	bio->accb = 0;
	return bio->bytes;
}


int entropyBPC(const unsigned char* pCompressed, int length, int planeCount, int* pDest, int destCount, int kInit)
{
	if (length <= 0 || destCount <= 0)
		return 0;

	BitIO bio =
	{
		.pBuffer = pCompressed,
		.length = length,
		.acck = 0,
		.accb = 0,
		.bytes = 0,
	};

// Intellisense false positive (fine on cl)
#pragma warning(push)
#pragma warning(disable: 6385)
	
	unsigned int* dest = (unsigned int*)pDest;
	memset(dest, 0, 4 * (size_t)destCount);

	const int planes = readBits(&bio, 6);
	const int unknownCount = readBits(&bio, 2);
	if (unknownCount)
	{
		const int unknownLength = readBits(&bio, 4);

		for (int i = 0; i < unknownCount + 1; ++i)
		{
			readBits(&bio, unknownLength);
		}
	}
	flushBitIO(&bio);

	for (int plane = planes - 1; plane > planeCount - planes; --plane)
	{
		const int localMask = 1 << plane;
		
		if (plane != planes - 1)
		{
			for (int i = 0; i < destCount; ++i)
			{
				if (dest[i] & 0x40000000u)
				{
					if (readBit(&bio))
					{
						dest[i] |= localMask;
					}
				}
			}
		}

		int kp = kInit << 3;

		for (int i = 0; i < destCount; ++i)
		{
			if (dest[i] & 0x40000000u)
				continue;

			const int k = kp >> 3;
			if (k == 0)
			{
				if (!readBit(&bio))
				{
					kp += 4;
					if (kp > 96)
						kp = 96;
					continue;
				}

				dest[i] |= 0x40000000u;
				if (readBit(&bio))
					dest[i] |= 0x80000000u;
				dest[i] |= localMask;

				// if k is 0, is this is redundant?
				kp -= 3;
				if (kp < 0)
					kp = 0;
			}
			else if (!readBit(&bio))
			{
				int run = 1 << k;
				
				while (run > 0 && i < destCount)
				{
					if (!(dest[i++] & 0x40000000u))
					{
						--run;
					}
				}
				--i;
				kp += 5;
				if (kp > 96)
					kp = 96;
			}
			else
			{
				const int sign = readBit(&bio);
				int run = readBits(&bio, k) + 1;

				while (run > 0 && i < destCount)
				{
					if (!(dest[i++] & 0x40000000u))
					{
						--run;
					}
				}

				if (i >= destCount)
				{
					continue;
				}

				dest[i] |= 0x40000000u;
				if (sign)
					dest[i] |= 0x80000000u;
				dest[i] |= localMask;
				kp -= 6;
				if (kp < 0)
					kp = 0;
			}
		}

		flushBitIO(&bio);
	}

	for (int i = 0; i < destCount; ++i)
	{
		const int val = pDest[i];
		if (val < 0)
			pDest[i] = -(val & 0x3FFFFFFF);
		else
			pDest[i] = val & 0x3FFFFFFF;
	}
#pragma warning(pop)
	
	return bio.bytes;
}


typedef	struct {
	int	run;
	int	k;
	int	kp;
	int kr;
	int krp;
} RLGRState;


inline int runLength(RLGRState* bpk, int* pDest, int count, int destCount, int bPartial)
{
	if (bpk->run > 0)
	{
		do
		{
			pDest[count++] = 0;
		} while (count < destCount && --bpk->run > 0);
	}

	if (bPartial)
	{
		bpk->kp -= 6;
		if (bpk->kp < 0)
			bpk->kp = 0;
	}
	else
	{
		--count;
		bpk->kp += 4;
		if (bpk->kp > 120)
			bpk->kp = 120;
	}
	
	return count;
}


inline int codeGR(BitIO* bio, RLGRState* bpk)
{
	bpk->kr = bpk->krp >> 3;
	int val, p = 0;

	while (1)
	{
		if (!readBit(bio))
		{
			if (bpk->kr)
			{
				val = (p << bpk->kr) + readBits(bio, bpk->kr);
			}
			else
			{
				val = p;
			}
			break;
		}
		++p;
	}

	if (p == 0)
	{
		bpk->krp -= 2;
		if (bpk->krp < 0)
			bpk->krp = 0;
	}
	else if (p > 1)
	{
		bpk->krp += p;
		if (bpk->krp > 120)
			bpk->krp = 120;
	}
	
	return val;
}


int entropyRLGR(const unsigned char* pCompressed, int length, int* pDest, int destCount, int range)
{
	if (length <= 0 || destCount <= 0)
		return 0;

	BitIO bio =
	{
		.pBuffer = pCompressed,
		.length = length,
		.acck = 0,
		.accb = 0,
		.bytes = 0,
	};

	int k = 0;
	int kr = 0;
	if (range > 1)
	{
		for (++range; range > 1; ++kr)
			range >>= 1;
	}
	else
	{
		k = 1;
		kr = 2;
	}
	
	RLGRState bpk =
	{
		.run = 0,
		.k = k,
		.kp = k << 3,
		.kr = kr,
		.krp = kr << 3
	};

	for (int count = 0; count < destCount && bio.bytes <= length; ++count)
	{
		bpk.k = bpk.kp >> 3;
		
		if (bpk.k == 0)
		{
			const int rawVal = codeGR(&bio, &bpk);
			int val = (rawVal + 1) >> 1;

			if (val)
			{
				bpk.kp -= 3;
				if (bpk.kp < 0)
					bpk.kp = 0;
			}
			else
			{
				bpk.kp += 3;
				if (bpk.kp > 120)
					bpk.kp = 120;
			}

			if (rawVal & 1)
				val = -val;
			pDest[count] = val;
		}
		else if (!readBit(&bio))
		{
			// complete run
			bpk.run = 1 << bpk.k;
			count = runLength(&bpk, pDest, count, destCount, 0);
		}
		else
		{
			// partial run
			bpk.run = readBits(&bio, bpk.k);
			count = runLength(&bpk, pDest, count, destCount, 1);
			
			const int sign = readBit(&bio);
			const int val = codeGR(&bio, &bpk);

			if (count < destCount)
			{
				if (sign)
					pDest[count] = -(val + 1);
				else
					pDest[count] = val + 1;
			}	
		}

	}
	return flushBitIO(&bio);
}


typedef struct
{
	unsigned short run[64];
	unsigned short bit[64];
	unsigned short kp[64];
	unsigned char qzc[64];
} BLCState;


int adaptiveRLR(BitIO* bio, int c, BLCState* blc)
{

	if (blc->run[c])
	{
		if (--blc->run[c] == 0)
			return blc->bit[c];
		return 0;
	}

	int kp = blc->kp[c];
	const int k = kp >> 4;
	
	if (k)
	{
		if (readBit(bio))
		{
			const int bits = readBits(bio, k);
			blc->run[c] = (short)bits;
			if (bits)
			{
				blc->bit[c] = 1;
			}
			kp -= k + 4;
			if (kp < 0)
				kp = 0;
			blc->kp[c] = (short)kp;
			return bits ? 0 : 1;
		}

		blc->run[c] = (short)(1 << k) - 1;
		blc->bit[c] = 0;

		kp += k + 4;
		if (kp > 224)
			kp = 224;
		blc->kp[c] = (short)kp;
		return 0;
	}

	const int bit = readBit(bio);
	if (!bit)
	{
		kp += 4;
		if (kp > 224)
			kp = 224;
	}
	blc->kp[c] = (short)kp;
	return bit;
}


void initQZC(BLCState* blk)
{
	for (int i = 0; i < 64; i++)
	{
		unsigned char t = 6;
		if (i & 1)
			--t;
		if (i & 2)
			--t;
		if (i & 4)
			--t;
		if (i & 8)
			--t;
		if (i & 0x10)
			--t;
		if (i & 0x20)
			--t;
		blk->qzc[i] = 7 * t / 6;
	}
}


int entropyBLC(const unsigned char* pCompressed, int length, unsigned char *pDest, unsigned char* pLines, int blockCount, int width)
{
	if (length <= 0)
		return 0;

	if (blockCount > 16)
		return -1;

	BitIO bio =
	{
		.pBuffer = pCompressed,
		.length = length,
		.acck = 0,
		.accb = 0,
		.bytes = 0,
	};

	int blocks[16];
	memset(blocks, 0, 16 * sizeof(int));
	memset(pLines, 0, (size_t)3 * width + 18);

	BLCState blk;
	initQZC(&blk);
	for (int i = 0; i < 64; ++i)
	{
		blk.run[i] = 0;
		blk.bit[i] = 0;
		blk.kp[i] = 16;
	}

	const int blockMode = readBits(&bio, 2);
	
	if (blockMode == 0)
	{
		for (int i = 0; i < 16; ++i)
			blocks[i] = -1;
	}
	else if (blockMode == 1)
	{
		const int blockMask = (readByte(&bio) << kBitsPerByte) | readByte(&bio);
		const int blockValue = (readByte(&bio) << kBitsPerByte) | readByte(&bio);

		for (int i = 15, bitmask = 32768; i >= 0; --i, bitmask >>= 1)
		{
			if (bitmask & blockMask)
				blocks[i] = (bitmask & blockValue) != 0;
			else
				blocks[i] = -1;
		}
	}
	else // (blockMode == 2)
	{
		const int bit = readBit(&bio);

		for (int i = 0; i < 16; ++i)
			blocks[i] = bit;
	}
	
	int cr0 = 0;
	int cr1 = 0;
	unsigned char* const pRow0 = pLines;
	unsigned char* const pRow1 = pLines + width + 6;
	unsigned char* const pRow2 = pLines + 2 * width + 12;
	unsigned char* row0 = pRow0;
	unsigned char* row1 = pRow1;
	unsigned char* row2 = pRow2;
	
	for (int block = 0; block < blockCount; ++block)
	{
		if (blocks[block] < 0)
		{
			for (int i = 0; i < width; ++i)
			{
				// Figure 1 calculate c
				cr0 = row0[2 + i] + 2 * (cr0 & 1);
				cr1 = 2 * (2 * row1[i + 4] + (cr1 & 0xF));
				const int c = cr0 + cr1 + 32 * row2[i + 4];
				
				// pixel (x) = p(c) xor e(c)
				const int pc = 1 - (blk.qzc[c] >> 2); // flip
				const int x = pc ^ adaptiveRLR(&bio, c, &blk); // error
				row0[3 + i] = (unsigned char)x; // store

				// QZ(c) <= QZ(c) + q, where q = 1 if x = 0, and q = –1 if x = 1
				int qzc = blk.qzc[c] - 2 * x + 1;
				if (qzc < 0)
					qzc = 0;
				if (qzc > 7)
					qzc = 7;
				blk.qzc[c] = (unsigned char)qzc;
			}
			memcpy(pDest, &row0[3], width);
			pDest += width;

			// Cascade rows
			if (row0 == pRow0)
			{
				row0 = pRow2;
				row1 = pRow0;
				row2 = pRow1;
			}
			else if (row0 == pRow2)
			{
				row0 = pRow1;
				row1 = pRow2;
				row2 = pRow0;
				
			}
			else if (row0 == pRow1)
			{
				row0 = pRow0;
				row1 = pRow1;
				row2 = pRow2;
			}
		}
		else
		{
			memset(&row0[3], blocks[block], width);
			memcpy(pDest, &row0[3], width);
			pDest += width;
		}
	}

	return 0;
}
