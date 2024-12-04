#include <stdio.h>
#include <stdint.h>

typedef struct
{
	char		chunkID[4];			// "RIFF"
	uint32_t	chunkSize;			// = 36 + SubChunk2Size
	char		format[4];			// "WAVE"
} WavHeader_RiffChunk;

typedef struct
{
	char		subChunk1ID[4];		// "fmt "
	uint32_t	subChunk1Size;		// 16 for PCM
	uint16_t	audioFormat;		// PCM = 1 (i.e. Linear quantization)
	uint16_t	numChannels;
	uint32_t	sampleRate;
	uint32_t	byteRate;			// = SampleRate * NumChannels * BitsPerSample/8
	uint16_t	blockAlign;			// = NumChannels * BitsPerSample/8
	uint16_t	bitsPerSample;
} WavHeader_FmtChunk;

typedef struct
{
	char		subChunk2ID[4];		// "data"
	uint32_t	subChunk2Size;		// = NumSamples * NumChannels * BitsPerSample/8
} WavHeader_DataChunk;

typedef struct
{
	WavHeader_RiffChunk		riff;
	WavHeader_FmtChunk		fmt;
	WavHeader_DataChunk		data;
	long					headerSize;
} WavHeader;

FILE*	WavHeader_Fopen(char* fileName, char* stMode);
int32_t WavHeader_Read(FILE* refFile, WavHeader* inWAVhdr);
int32_t WavHeader_Write(FILE* refFile, WavHeader* inWAVhdr);
