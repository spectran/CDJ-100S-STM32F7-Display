/*
 * rekordbox.h
 *
 *  Created on: 14 ��� 2018 �.
 *      Author: teren
 */

#ifndef REKORDBOX_H_
#define REKORDBOX_H_

#include "ff.h"

typedef struct
{
	TCHAR filename[255]; // full filename with path
	char file[255]; // filename without path
	char filetype[5]; // file extension
	uint32_t timezones; // number of time zone entries
	uint16_t bpm[1000]; // current bpm
	uint16_t phase[1000]; // current beat phase
	uint32_t timeindex[1000]; // time index
	uint8_t beat_grid_offset;
	uint32_t lowp_spectrum_size;
	uint32_t spectrum_size;
	uint16_t remminticks[64]; // remain minutes ticks positions
	uint16_t elminticks[64]; // elapsed minutes ticks positions
	uint8_t minutes; // number of minutes
	int32_t cues; // number of cuepoints
	uint32_t cue_number[100];
	uint32_t cue_active[100];
	int32_t cue_type[100]; // 1 - hot cue, 0 - memory
	int32_t cue_start_position[100];
	int32_t cue_end_position[100];
	uint8_t cue_singleloop[100];
	uint8_t autocue;
	int32_t currentcue;
	uint8_t state;
}RekordboxTypeDef;

uint8_t DecodeRekordboxFiles(TCHAR *path);
uint8_t GetFileName(TCHAR *path);

#endif /* REKORDBOX_H_ */
