// PNGDecompress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "PNG.h"
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
//TODO:replace malloc with Arenas
#include "Engine\System\Memory\Arena.h"
#pragma warning(disable :4996)



#pragma pack(push,1)
struct BMP_HEADER 
{
	unsigned char Sig[2] = { 'B','M' };
	unsigned int Size = 0;
	short a = 0;
	short b = 0;
	unsigned int Offset = 0;
	unsigned int HeaderSize = 40;
	unsigned int Width;
	unsigned int Height;
	short Planes = 1;
	short BitsPerPixel;
	unsigned int Compression = 0;
	unsigned int ImageSize;
	unsigned int XPPM = (unsigned int)(96 * 39.375);
	unsigned int YPPM = (unsigned int)(96 * 39.375);
	unsigned int NumColors = 0;
	unsigned int ImportantColors = 0;
};
struct ZlibCmpFlags
{
	char CM;
	char CINFO;
	char FCHECK;
	char FDICT;
	char FLEVEL;
};
struct PNG_IDAT_Header
{
	unsigned char CompressionMethod;
	unsigned char AdditionalFlags;
};
struct PNG_Header
{
	unsigned char Signiture[8];
	unsigned int Length;
	unsigned char Name[4];
	unsigned int Width;
	unsigned int Height;
	unsigned char BitDepth;
	unsigned char ColorType;
	unsigned char CmpressType;
	unsigned char Filter;
	unsigned char Interlace;
	unsigned int CRC;
};
struct PNG_ChunkHeader
{
	unsigned int Length;
	unsigned char Name[4] = {0};
};
struct Byte
{
	unsigned char data;
	int NumOfBitsRead = 0;
};
#pragma pack(pop)
struct IDATData
{
	int DataSize;
	void* Data;
	//int BitsRead = 0;
	int CurrentByte = 0;
	int BitsLeft = 0;
};
struct File
{
	int Size;
	void* Data;
};
void LoadEntireFile(FILE* f,File *EntireFile)
{
	fseek(f,0,SEEK_END);
	EntireFile->Size = ftell(f);
	fseek(f, 0, SEEK_SET);
	EntireFile->Data = malloc(EntireFile->Size);
	fread(EntireFile->Data,1,EntireFile->Size,f);
}
void SwapBytes(unsigned int *Number)
{
	*Number = (*Number << 24) | (*Number >> 24) | ((*Number & 0x00ff0000) >> 8) | ((*Number & 0x0000ff00) << 8);
}
inline int GetBit(IDATData* ByteStream, int NumberOfBits, bool Peek)
{

	while (NumberOfBits > ByteStream->BitsLeft)
	{
		int NextByte = *(unsigned char*)ByteStream->Data;

		ByteStream->CurrentByte |= NextByte << ByteStream->BitsLeft;
		ByteStream->BitsLeft += 8;
		
		
		ByteStream->Data = (unsigned char*)ByteStream->Data + 1;
		
	}



	int Result = (ByteStream->CurrentByte & ((1 << NumberOfBits) - 1));
	if (!Peek)
	{
		ByteStream->BitsLeft -= NumberOfBits;
		ByteStream->CurrentByte = ByteStream->CurrentByte >> NumberOfBits;
	}
	return Result;
}
int SwapBits(int val)
{
	unsigned int Result = 0;
	for (int i = 0;i<sizeof(unsigned int)*4;i++)
	{
		Result |= ((val >> i) & 1) << (31 - i);
	}
	return Result;
}
int HuffmanbitOrder(unsigned int val,int Length) 
{
	unsigned int result = 0;
	int TempVal = val;
	for (int i = 0; i < Length; i++)
	{
		result <<= 1;
		result |= (TempVal & 1);
		TempVal >>= 1;
	}
	return result;
}
void AdvancePointer(File* EntireFile,int Size)
{
	EntireFile->Data = (char*)EntireFile->Data + Size;
	EntireFile->Size -= Size;
}
bool CompareString(const char* str1, const char* str2)
{
	
	for (int i = 0;i<4;i++)
	{
		if (str1[i] != str2[i])
		{
			return false;
		}
	}
	return true;
}
int GetMaxBitLength(int* CodeLengths,int ArrayLength)
{
	int NumberOfAssignedCodes = 0;
	for (int i = 0; i < ArrayLength;i++)
	{
		
		if (NumberOfAssignedCodes < CodeLengths[i])
		{
			NumberOfAssignedCodes = CodeLengths[i];
		}
	}
	return NumberOfAssignedCodes;
}

void CreateHuffman(int* CodeLengths, int* Codes,int ArrayLength)
{
	//this is wrong pass in the bit length.
	int BitLength = GetMaxBitLength(CodeLengths, ArrayLength) ;

	//Count all the code lengths.
	int Freq[20] = {};
	for (int i = 0; i < ArrayLength; i++)
	{
		Freq[CodeLengths[i]]++;
	}
	Freq[0] = 0;

	//Comput the min value for each code length.
	int SmallestLength[20] = {0};
	int Smallest[20] = { 0 };
	int Code = 0;
	for (int bits = 1;bits<= BitLength;bits++)
	{
		SmallestLength[bits] = (SmallestLength[bits - 1] + Freq[(bits - 1)]) << 1;
	}
	for (int i = 0; i < ArrayLength; i++)
	{
		if (CodeLengths[i])
		{
			Codes[i] = SmallestLength[CodeLengths[i]]++;
		}
	}

}
int DecodeHuffman(IDATData* BitStream,int Length,int* HuffmanCodes,int* CodeLengths)
{
	unsigned int Code = 0;
	for (int i = 0;i<Length;i++)	
	{
		//unsigned int CurrentBytes = ((unsigned int*)BitStream->Data)[(int)(BitStream->BitsRead / 32)] >> (BitStream->BitsRead % (sizeof(unsigned int) * 8));
		Code = GetBit(BitStream, CodeLengths[i], true);
		int TempVal = Code;
		Code = 0;
		for (int k = 0; k < CodeLengths[i]; k++)
		{
			Code <<= 1;
			Code |= (TempVal & 1);
			TempVal >>= 1;
		}
		//Code = HuffmanbitOrder(Code,CodeLengths[i]);
		if (CodeLengths[i])
		{
			if (Code == HuffmanCodes[i])
			{
				GetBit(BitStream, CodeLengths[i], false);
				return i;
			}
		}
	}
	return 0;
}
char base_length_extra_bit[] = {
	0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
	1, 1, 1, 1, //265 - 268
	2, 2, 2, 2, //269 - 273 
	3, 3, 3, 3, //274 - 276
	4, 4, 4, 4, //278 - 280
	5, 5, 5, 5, //281 - 284
	0           //285
};

int base_lengths[] = {
	3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
	11, 13, 15, 17,          //265 - 268
	19, 23, 27, 31,          //269 - 273 
	35, 43, 51, 59,          //274 - 276
	67, 83, 99, 115,         //278 - 280
	131, 163, 195, 227,      //281 - 284
	258                      //285
};
unsigned int dist_bases[] = {
	/*0*/ 1, 2, 3, 4,    //0-3
	/*1*/ 5, 7,          //4-5
	/*2*/ 9, 13,         //6-7
	/*3*/ 17, 25,        //8-9
	/*4*/ 33, 49,        //10-11
	/*5*/ 65, 97,        //12-13
	/*6*/ 129, 193,      //14-15
	/*7*/ 257, 385,      //16-17
	/*8*/ 513, 769,      //18-19
	/*9*/ 1025, 1537,    //20-21
	/*10*/ 2049, 3073,   //22-23
	/*11*/ 4097, 6145,   //24-25
	/*12*/ 8193, 12289,  //26-27
	/*13*/ 16385, 24577, //28-29
			 0   , 0      //30-31, error, shouldn't occur
};

unsigned int dist_extra_bits[] = {
	/*0*/ 0, 0, 0, 0, //0-3
	/*1*/ 1, 1,       //4-5
	/*2*/ 2, 2,       //6-7
	/*3*/ 3, 3,       //8-9
	/*4*/ 4, 4,       //10-11
	/*5*/ 5, 5,       //12-13
	/*6*/ 6, 6,       //14-15
	/*7*/ 7, 7,       //16-17
	/*8*/ 8, 8,       //18-19
	/*9*/ 9, 9,       //20-21
	/*10*/ 10, 10,    //22-23
	/*11*/ 11, 11,    //24-25
	/*12*/ 12, 12,    //26-27
	/*13*/ 13, 13,     //28-29
			0 , 0      //30-31 error, they shouldn't occur
};
int ColorTypesLengths[7] = { 1,0,3,1,2,0,4 };
unsigned char PaethPredictor(int a,int b,int c) 
{
	
	int pa = abs(b - c);
	int pb = abs(a - c);
	int pc = abs(a + b - (2*c));
	if (pb < pa) 
	{
		a = b;
		pa = pb;
	}
	return (pc < pa) ? c : a;
}

void* RemoveFilter(PNG_Header Header,void* Image,int* Size)
{
	int BytesPerPixel = ColorTypesLengths[Header.ColorType];
	unsigned char* row = (unsigned char*)Image;
	int stride = Header.Width*BytesPerPixel;
	*Size = Header.Width*Header.Height*BytesPerPixel;
	
	int count = 0;
	void* FinalImage = malloc(Header.Width*Header.Height*BytesPerPixel );
	memset(FinalImage, 0, *Size);
	unsigned char* ImagePointer = (unsigned char*)FinalImage;
	unsigned char* Prev = 0;
	for (unsigned int i = 0;i<Header.Height;i++)
	{
		
		int Filter = *row++;
		
		switch (Filter)
		{
			case 0:
			{

				memcpy(ImagePointer,row,stride);
				break;
			}
			case 1:
			{
				memcpy(ImagePointer, row, BytesPerPixel);
				for (int j = BytesPerPixel;j< stride;j++)
				{
					ImagePointer[j] = row[j] + ImagePointer[j - BytesPerPixel];
				}
				break;
			}
			case 2:
			{
				if (Prev)
				{
					for (int j = 0; j < stride; j++)
					{
						ImagePointer[j] = Prev[j] + row[j];
					}
				}
				else
				{
					for (int j = 0; j < stride; j++)
					{
						ImagePointer[j] = row[j];
					}
				}
				
				break;
			}
			case 3:
			{
				if (Prev)
				{
					for (int j = 0; j != BytesPerPixel; ++j)
					{
						ImagePointer[j] = row[j] + (Prev[j] >> 1);
					}
					for (int j = BytesPerPixel; j < stride; ++j)
					{
						
						ImagePointer[j] = row[j] + ((ImagePointer[j - BytesPerPixel] + Prev[j]) >> 1);
					}
				}
				else
				{
					memcpy(ImagePointer, row, BytesPerPixel);
					for (int j = BytesPerPixel; j < stride; j++)
					{
						ImagePointer[j] = row[j] + (ImagePointer[j - BytesPerPixel] >> 1);
					}

				}
				break;
			}
			case 4:
			{
				if (Prev)
				{
					for (int j = 0; j != BytesPerPixel; ++j)
					{
						ImagePointer[j] = (row[j] + Prev[j]); 
					}
					for (int j = BytesPerPixel; j < stride; ++j)
					{
						int Test = PaethPredictor(ImagePointer[j - BytesPerPixel], Prev[j], Prev[j - BytesPerPixel]);
						ImagePointer[j] = (row[j] + Test);
					}
				}
				else
				{
					memcpy(ImagePointer, row, BytesPerPixel);
					for (int j = BytesPerPixel; j < stride; ++j) 
					{
						
						ImagePointer[j] = (row[j] + ImagePointer[j - BytesPerPixel]);
					}
				}
				break;
			
			}
		}
		Prev = ImagePointer;
		row += stride;
		count += stride;
		ImagePointer += stride;
	}
	
	int i = 0;
	return FinalImage;
}
void LZ77Decompress(IDATData* IDATChunk,int HLIT,int HDIST,int*LiteralTree,int*DistTree, int* LitBitLengths,int* DistBitLengths,unsigned char* DecompressedData,unsigned int *StartPos)
{
	int Count = *StartPos;
	while (true)
	{
		int Code = DecodeHuffman(IDATChunk, HLIT, LiteralTree, LitBitLengths);
		if (Code == 256)
		{
			break;
		}
		else if (Code < 256)
		{
			DecompressedData[Count++] = Code;
		}
		else if (Code < 286 && Code > 256)
		{
			int Index = Code - 257;
			int DupedLength = base_lengths[Index] + GetBit(IDATChunk, base_length_extra_bit[Index], false);

			int Distance = DecodeHuffman(IDATChunk, HDIST, DistTree, DistBitLengths);

			int Length = dist_bases[Distance] + GetBit(IDATChunk, dist_extra_bits[Distance], false);

			int BackPointer = Count - Length;
			//change this to memcpy
			while (DupedLength--)
			{
				DecompressedData[Count++] = DecompressedData[BackPointer++];
			}
		}
	}
	*StartPos += (Count - *StartPos);
};

void* ReadAllChunks(File CurrentPos,int *IDATSize)
{
	File Temp = CurrentPos;

	PNG_ChunkHeader Header = *(PNG_ChunkHeader*)Temp.Data;
	void* Data = 0;
	Data = malloc(100000000);
	while (!CompareString((char*)Header.Name,"IEND"))
	{
		Header = *(PNG_ChunkHeader*)Temp.Data;;
		
		char Buff[5] = {};
		memcpy(Buff, Header.Name, 4);
		AdvancePointer(&Temp, sizeof(PNG_ChunkHeader));
		printf("%s\n", Buff);
		SwapBytes(&Header.Length);
		printf("%d\n", Header.Length);
		
		if (CompareString((char*)Header.Name, "IDAT"))
		{
			
			
			memcpy((char*)Data + *IDATSize, (char*)Temp.Data, Header.Length);
			*IDATSize += Header.Length;
			
			
		}
		AdvancePointer(&Temp, Header.Length);
		AdvancePointer(&Temp, 4);

	}
	

	return Data;
	
}

int Ary[19] = { 16, 17, 18,0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

PNG LoadPNG(const char* FileName)
{
	//Load in File
	FILE* f = fopen(FileName, "rb");
	if (!f)
	{
		printf("Error: Failed to load %s\n", FileName);
		return {};
	}
	File EntireFile;
	LoadEntireFile(f, &EntireFile);
	
	File* CurrentPos = &EntireFile;
	//---------
	PNG DecodedPNG;
	PNG_Header FileHeader = *(PNG_Header*)CurrentPos->Data;
	AdvancePointer(CurrentPos, sizeof(PNG_Header));
	SwapBytes(&FileHeader.Length);
	SwapBytes(&FileHeader.Width);
	SwapBytes(&FileHeader.Height);
	DecodedPNG.Width = FileHeader.Width;
	DecodedPNG.Height = FileHeader.Height;

	int IDATSize = 0;
	void* IDATChunksData = ReadAllChunks(EntireFile,&IDATSize);
	
	ZlibCmpFlags ZlibFlags;

	unsigned char* DecompressedData = (unsigned char*)malloc(FileHeader.Width * FileHeader.Height * 4 * 8);
	
	IDATData IDATChunk;
	IDATChunk.Data = IDATChunksData;
	IDATChunk.DataSize = IDATSize;
	//ZLIB Header	
	ZlibFlags.CM = GetBit(&IDATChunk, 4, false);
	ZlibFlags.CINFO = GetBit(&IDATChunk, 4, false);
	ZlibFlags.FCHECK = GetBit(&IDATChunk, 5, false);
	ZlibFlags.FDICT = GetBit(&IDATChunk, 1, false);
	ZlibFlags.FLEVEL = GetBit(&IDATChunk, 2, false);
			
	int BFINAL = 0;
	unsigned int StartPos = 0;
	while (BFINAL != 1)
	{
		BFINAL = GetBit(&IDATChunk, 1, false);
		int BTYPE = GetBit(&IDATChunk, 2, false);
		if (BTYPE == 3)
		{
			break;
		}
		if (BTYPE == 0)
		{
			int a = 0;
		}
		if (BTYPE == 1)
		{
			int* Dist = (int*)malloc(32 * sizeof(unsigned int));
			int* Len = (int*)malloc(288 * sizeof(unsigned int));
			int BitLen[288];
			int BitLenDist[32];
			for (int i = 0; i < 144; i++)
			{
				BitLen[i] = 8;
			}
			for (int i = 144; i < 256; i++)
			{
				BitLen[i] = 9;
			}
			for (int i = 256; i < 280; i++)
			{
				BitLen[i] = 7;
			}
			for (int i = 280; i <= 287; i++)
			{
				BitLen[i] = 8;
			}
			for (int i = 0; i < 32; i++)
			{
				BitLenDist[i] = 5;
			}
			CreateHuffman(BitLen, Len, 288);
			CreateHuffman(BitLenDist, Dist, 32);
			LZ77Decompress(&IDATChunk, 288, 32, Len, Dist, BitLen, BitLenDist, DecompressedData, &StartPos);
			free(Dist);
			free(Len);
		}
		if (BTYPE == 2)
		{
			int HLIT = GetBit(&IDATChunk, 5, false) + 257;
			int HDIST = GetBit(&IDATChunk, 5, false) + 1;
			int HCLEN = GetBit(&IDATChunk, 4, false) + 4;
			int HCLENTable[19] = { 0 };

			for (int i = 0; i < HCLEN; ++i)
			{
				HCLENTable[Ary[i]] = GetBit(&IDATChunk, 3, false);
			}
			int* HuffmanCodesForTrees = (int*)malloc(19 * sizeof(int));
			memset(HuffmanCodesForTrees, 0, 19 * sizeof(int));
			CreateHuffman(HCLENTable, HuffmanCodesForTrees, 19);
			int Count = 0;

			int* CodeLengthsForSubTrees = (int*)malloc(sizeof(int)*(HLIT + HDIST));
			memset(CodeLengthsForSubTrees, 0, sizeof(int)*(HLIT + HDIST));
			while (Count < HLIT + HDIST)
			{
				int DecodedVal = DecodeHuffman(&IDATChunk, 19, HuffmanCodesForTrees, HCLENTable);
				int RepeatCount = 0;
				int CodeToRepeat = 0;
				if (DecodedVal < 16)
				{
					CodeLengthsForSubTrees[Count++] = DecodedVal;
				}
				else
				{
					switch (DecodedVal)
					{
					case 16:
					{

						RepeatCount = 3 + GetBit(&IDATChunk, 2, false);
						CodeToRepeat = CodeLengthsForSubTrees[Count - 1];
						break;
					}
					case 17:
					{
						RepeatCount = 3 + GetBit(&IDATChunk, 3, false);
						CodeToRepeat = 0;
						break;
					}
					case 18:
					{
						RepeatCount = 11 + GetBit(&IDATChunk, 7, false);
						CodeToRepeat = 0;
						break;
					}

					}
				}
				while (RepeatCount--)
				{
					CodeLengthsForSubTrees[Count] = CodeToRepeat;
					Count++;
				}

			}
			int* LiteralLengthsHuffTree = (int*)malloc(HLIT * sizeof(int));
			memset(LiteralLengthsHuffTree, 0, sizeof(int)*HLIT);
			CreateHuffman(CodeLengthsForSubTrees, LiteralLengthsHuffTree, HLIT);
			int* DistanceHuffTree = (int*)malloc(HDIST * sizeof(int));
			memset(DistanceHuffTree, 0, sizeof(int)*HDIST);
			CreateHuffman(CodeLengthsForSubTrees + HLIT, DistanceHuffTree, HDIST);

			//Decode the 2 trees
			LZ77Decompress(&IDATChunk, HLIT, HDIST, LiteralLengthsHuffTree, DistanceHuffTree, CodeLengthsForSubTrees, CodeLengthsForSubTrees + HLIT, DecompressedData, &StartPos);




			free(LiteralLengthsHuffTree);
			free(DistanceHuffTree);
			free(CodeLengthsForSubTrees);
			free(HuffmanCodesForTrees);

		}
		

	}
	
	int DecompressedDataSize = 0;

	void* UnFilteredData = RemoveFilter(FileHeader, DecompressedData, &DecompressedDataSize);
	
	DecodedPNG.Data = malloc(FileHeader.Width * FileHeader.Height * 4 );
	
	int ImageStride = FileHeader.Width*ColorTypesLengths[FileHeader.ColorType];
	
	int StrideRGBA = FileHeader.Width * 4;
	int RGBAPos = 0;
	int RGBDataPos = 0;
	for (unsigned int y = 0; y < FileHeader.Height; y++)
	{
		//if its 3 byte color;

		if (ColorTypesLengths[FileHeader.ColorType] == 3)
		{

			//R G B R G B
			//R G B A R G B A
			
			for (u32 x = 0; x < FileHeader.Width; x++)
			{
				((unsigned char*)DecodedPNG.Data)[RGBAPos++] = ((unsigned char*)UnFilteredData)[RGBDataPos++];
				((unsigned char*)DecodedPNG.Data)[RGBAPos++] = ((unsigned char*)UnFilteredData)[RGBDataPos++];
				((unsigned char*)DecodedPNG.Data)[RGBAPos++] = ((unsigned char*)UnFilteredData)[RGBDataPos++];
				((unsigned char*)DecodedPNG.Data)[RGBAPos++] = 0xff;
				
			}
			//DecodedPNG.HasAlpha = false;
		}
	}
	if (ColorTypesLengths[FileHeader.ColorType] == 4)
	{
		//DecodedPNG.HasAlpha = true;
		memcpy((unsigned char*)DecodedPNG.Data, (unsigned char*)UnFilteredData, DecodedPNG.Height * DecodedPNG.Width * 4);
	}

	
	
	
	//memcpy((unsigned char*)DecodedPNG.Data, Temp, FileHeader.Width*FileHeader.Height * 4 );
	free(IDATChunksData);
	free(DecompressedData);
	free(UnFilteredData);
	fclose(f);
	

	
	return DecodedPNG;

}
