#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "wavhdr.h"
#include "custom_module.h"

#define BLOCK_SIZE 16
#define MAX_NUM_CHANNEL 8

double sampleBuffer[MAX_NUM_CHANNEL][BLOCK_SIZE];

int main(int argc, char* argv[])
{
	FILE* wavInput = NULL;
	FILE* wavOutput = NULL;
	char wavInputName[256];
	char wavOutputName[256];
	WavHeader inputWavHeader;
	WavHeader outputWavHeader;

	uint8_t mask_list[4] = {
		0b00011011,
		0b00111011,
		0b00011111,
		0b00111111
	};

	uint16_t numChannels_list[4] = {
		4,
		5,
		5,
		6
	};

	uint8_t mask = mask_list[atoi(argv[5])];

	// Init channel buffers
	//-------------------------------------------------
	for (int32_t i = 0; i < MAX_NUM_CHANNEL; i++)
	{
		memset(&sampleBuffer[i], 0, BLOCK_SIZE);
	}

	// Open input and output wav files
	//-------------------------------------------------
	strcpy(wavInputName, argv[1]);
	wavInput = WavHeader_Fopen(wavInputName, "rb");
	strcpy(wavOutputName, argv[2]);
	wavOutput = WavHeader_Fopen(wavOutputName, "wb");

	// Read input wav header
	//-------------------------------------------------
	WavHeader_Read(wavInput, &inputWavHeader);

	// Set up output WAV header
	//-------------------------------------------------
	memcpy(&outputWavHeader, &inputWavHeader, sizeof(WavHeader));
	outputWavHeader.fmt.numChannels = numChannels_list[atoi(argv[5])];

	int32_t oneChannelSubChunk2Size = inputWavHeader.data.subChunk2Size / inputWavHeader.fmt.numChannels;
	int32_t oneChannelByteRate = inputWavHeader.fmt.byteRate / inputWavHeader.fmt.numChannels;
	int32_t oneChannelBlockAlign = inputWavHeader.fmt.blockAlign / inputWavHeader.fmt.numChannels;

	outputWavHeader.data.subChunk2Size = oneChannelSubChunk2Size * outputWavHeader.fmt.numChannels;
	outputWavHeader.fmt.byteRate = oneChannelByteRate * outputWavHeader.fmt.numChannels;
	outputWavHeader.fmt.blockAlign = oneChannelBlockAlign * outputWavHeader.fmt.numChannels;


	// Write output WAV header to file
	//-------------------------------------------------
	WavHeader_Write(wavOutput, &outputWavHeader);

	// Custom module
	CustomModule_ArgumentsTable argumentsTable;
	CustomModule_InitializeArgumentsTable(&argumentsTable, atoi(argv[3]), atof(argv[4]), atoi(argv[5]));

	std::cout << argumentsTable.module_enabled << " " << argumentsTable.input_gain << " " << argumentsTable.output_mode << std::endl;

	// Processing loop
	//-------------------------------------------------
	{
		int32_t sample;
		int32_t bytesPerSample = inputWavHeader.fmt.bitsPerSample / 8;
		const double SAMPLE_SCALE = -(double)(1 << 31);		//2^31
		int32_t iNumSamples = inputWavHeader.data.subChunk2Size / (inputWavHeader.fmt.numChannels * inputWavHeader.fmt.bitsPerSample / 8);
		uint32_t nBlocks = (iNumSamples / BLOCK_SIZE);
		uint32_t blockSize = BLOCK_SIZE;

		// exact file length should be handled correctly...
		if (iNumSamples % BLOCK_SIZE) nBlocks++;

		for (int32_t i = 0; i < nBlocks; i++)
		{
			if (i == nBlocks - 1) { blockSize = iNumSamples % BLOCK_SIZE; }
			else { blockSize = BLOCK_SIZE; }

			for (int32_t j = 0; j < blockSize; j++)
			{
				for (int32_t k = 0; k < inputWavHeader.fmt.numChannels; k++)
				{
					fread(&sample, bytesPerSample, 1, wavInput);
					sample = sample << (32 - inputWavHeader.fmt.bitsPerSample); // force signextend
					sampleBuffer[k][j] = sample / SAMPLE_SCALE;					// scale sample to 1.0/-1.0 range
				}
			}

			//processing();
			CustomModule_Main(argumentsTable, sampleBuffer);

			//mask
			{
				uint8_t maskTemp = mask;
				int8_t i = 0;
				int8_t j = 0;
				while (i < MAX_NUM_CHANNEL)
				{
					if (maskTemp & 1)
					{
						for (int8_t k = 0; k < BLOCK_SIZE; k++)
						{
							sampleBuffer[j][k] = sampleBuffer[i][k];
						}

						j++;
					}

					i++;
					maskTemp = maskTemp >> 1;
				}
			}

			for (int32_t j = 0; j < blockSize; j++)
			{
				for (int32_t k = 0; k < outputWavHeader.fmt.numChannels; k++)
				{
					sample = sampleBuffer[k][j] * SAMPLE_SCALE;	// crude, non-rounding
					sample = sample >> (32 - inputWavHeader.fmt.bitsPerSample);
					fwrite(&sample, outputWavHeader.fmt.bitsPerSample / 8, 1, wavOutput);
				}
			}
		}
	}

	// Close files
	//-------------------------------------------------
	fclose(wavInput);
	fclose(wavOutput);

	return 0;
}
//==============================================================================
