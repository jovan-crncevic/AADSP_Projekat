#include "wavhdr.h"
#include <stdbool.h>

#pragma pack(1)	// needed for proper calculation for: sizeof(struct) - round to BYTE /zp1

int32_t WavHeader_Read(FILE* refFile, WavHeader* inWAVhdr)
{

	uint8_t tempBuff[256];

	inWAVhdr->headerSize = 0;

	//get Ref WAV header
	fread(&inWAVhdr->riff, 1, sizeof(WavHeader_RiffChunk), refFile);
	fread(&inWAVhdr->fmt, 1, sizeof(WavHeader_FmtChunk), refFile);
	if ((inWAVhdr->riff.chunkID[0] == 'R') &&
		(inWAVhdr->riff.chunkID[1] == 'I') &&
		(inWAVhdr->riff.chunkID[2] == 'F') &&
		(inWAVhdr->riff.chunkID[3] == 'F')
		&&
		(inWAVhdr->riff.format[0] == 'W') &&
		(inWAVhdr->riff.format[1] == 'A') &&
		(inWAVhdr->riff.format[2] == 'V') &&
		(inWAVhdr->riff.format[3] == 'E')
		&&
		(inWAVhdr->fmt.subChunk1ID[0] == 'f') &&
		(inWAVhdr->fmt.subChunk1ID[1] == 'm') &&
		(inWAVhdr->fmt.subChunk1ID[2] == 't') &&
		(inWAVhdr->fmt.subChunk1ID[3] == ' ')
		&&
		(inWAVhdr->fmt.audioFormat == 1)		// not PCM content
		&&
		(inWAVhdr->fmt.bitsPerSample != 0)
		&&
		(inWAVhdr->fmt.numChannels != 0)
		&&
		(inWAVhdr->fmt.sampleRate != 0)
		&&
		(inWAVhdr->fmt.blockAlign != 0))
	{
		// read any additional fmt data - not really elegant...
		if (inWAVhdr->fmt.subChunk1Size > 16)
		{
			fread(tempBuff, 1, inWAVhdr->fmt.subChunk1Size - 16, refFile);
		}
		//get Ref WAV header - Data
		fread(&inWAVhdr->data, 1, sizeof(WavHeader_DataChunk), refFile);
		if ((inWAVhdr->data.subChunk2ID[0] == 'd') &&
			(inWAVhdr->data.subChunk2ID[1] == 'a') &&
			(inWAVhdr->data.subChunk2ID[2] == 't') &&
			(inWAVhdr->data.subChunk2ID[3] == 'a')
			&&
			(inWAVhdr->data.subChunk2Size != 0))
		{
			//Get RefWavHeader Size
			inWAVhdr->headerSize = ftell(refFile);
		}
	}

	return true;
}
//==============================================================================


int32_t WavHeader_Write(FILE* refFile, WavHeader* inWAVhdr)
{

	fwrite(inWAVhdr, 1, inWAVhdr->headerSize, refFile);
	fflush(refFile);

	return true;
}
//==============================================================================

FILE* WavHeader_Fopen(char* fileName, char* stMode)
{

	FILE* file = fopen(fileName, stMode); /// stMod=="rb" ili "wb"
	if (file == NULL)
	{
		printf("Cannot open file %s\n", fileName);
		return 0;
	}

	return file;
}
//==============================================================================
