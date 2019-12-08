/*
 * player.h
 *
 *  Created on: 9 ���. 2018 �.
 *      Author: teren
 */

#ifndef WAVEPLAYER_H_
#define WAVEPLAYER_H_

#include "stdint.h"

typedef struct
{
	uint32_t ChunkID;       /* 0 */
	uint32_t FileSize;      /* 4 */
	uint32_t FileFormat;    /* 8 */
	uint32_t SubChunk1ID;   /* 12 */
	uint32_t SubChunk1Size; /* 16*/
	uint16_t AudioFormat;   /* 20 */
	uint16_t NbrChannels;   /* 22 */
	uint32_t SampleRate;    /* 24 */

	uint32_t ByteRate;      /* 28 */
	uint16_t BlockAlign;    /* 32 */
	uint16_t BitPerSample;  /* 34 */
	uint32_t SubChunk2ID;   /* 36 */
	uint32_t SubChunk2Size; /* 40 */
}WavHeaderTypeDef;

uint8_t ReadWavHeader(WavHeaderTypeDef *waveformat);
void GetTrackTime();
void TrackTime();
void PlayWavFile();
void GoToPosition(uint32_t position);

#endif /* WAVEPLAYER_H_ */
