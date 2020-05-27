/*
 * player.c
 *
 *  Created on: 10 ���. 2018 �.
 *      Author: teren
 */

#include <waveplayer.h>
#include "fatfs.h"
#include "rekordbox.h"
#include "stm32746g_discovery_audio.h"
#include "display.h"
#include "mp3player.h"
#include "tim.h"
#include "stm32f7xx_it.h"
#include <string.h>
#include "main.h"

extern FIL MyFile;
extern AUDIO_OUT_BufferTypeDef BufferCtl;
extern volatile RekordboxTypeDef rekordbox;
extern DisplayTypeDef display;
extern volatile UINT bOutOfData;
extern TrackTypeDef trak;

extern int16_t rfr;
extern int8_t rsec;
extern int8_t rmin;
extern int16_t fr;
extern int8_t sec;
extern int8_t min;
extern uint32_t file_pos;
extern uint32_t file_pos_wide;
extern uint32_t id3tagsize;
extern uint8_t tim7_flag;

extern uint8_t spi_rx[4];
extern uint8_t spi_tx[4];

extern int unInDataLeft;
extern uint32_t unDmaBufferSpace;
extern UINT unFramesDecoded;
extern uint32_t unDmaBufMode;
extern uint16_t* g_pMp3DmaBufferPtr;
extern uint16_t g_pMp3DmaBuffer[MP3_DMA_BUFFER_SIZE];
extern int Track_number;

extern uint8_t volume;
uint32_t wavtagsize = 0;
WavHeaderTypeDef wavfile;

uint8_t ReadWavHeader(WavHeaderTypeDef *waveformat) {
	uint32_t bytesread;
	FRESULT res;
	/* Fill the buffer to Send */
	res = f_read(&MyFile, waveformat, sizeof(wavfile), (void*)&bytesread);
	return res;
}

void GetTrackTime() {
	float track_lenght = (float)(f_tell(&MyFile)-id3tagsize)/
			(float)(f_size(&MyFile)-id3tagsize-wavtagsize)
			*rekordbox.spectrum_size;
	file_pos_wide = (track_lenght + 0.5);
	file_pos = (uint16_t)(rekordbox.lowp_spectrum_size*file_pos_wide/
			rekordbox.spectrum_size);
	TrackTime();
	if(display.loop == 1) {
		if(trak.state == PLAYING) {
			if(file_pos_wide >= display.loopend) {
				GoToPosition(display.loopstart);
				BSP_AUDIO_OUT_Resume();
			}
		}
	}
}

void TrackTime() {
	rfr = ((rekordbox.spectrum_size-file_pos_wide) % 9000) % 150;
	rsec = ((rekordbox.spectrum_size-file_pos_wide) % 9000) / 150;
	rmin = (rekordbox.spectrum_size-file_pos_wide) / 9000;
	fr = ((file_pos_wide) % 9000) % 150;
	sec = ((file_pos_wide) % 9000) / 150;
	min = (file_pos_wide) / 9000;
}

void PlayWavFile() {
	wavtagsize = 0;
	BufferCtl.filetype = 0;
	memset(BufferCtl.buff, 0, AUDIO_OUT_BUFFER_SIZE);
	bOutOfData = 0;
	uint16_t bytesread = 0;
	ReadWavHeader(&wavfile);
	wavtagsize = wavfile.FileSize - wavfile.SubChunk2Size;
	if(wavfile.SampleRate > 0) trak.bitrate = wavfile.SampleRate;
	else trak.bitrate = AUDIO_FREQUENCY_44K;
	BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, (uint32_t)(trak.bitrate / 2)*(1 + trak.percent), NULL);
	/* Fill whole buffer at first time */
	while(f_read(&MyFile, &BufferCtl.buff[0], 2048, (void *)&bytesread) != FR_OK);
	if(bytesread != 0) {
		BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
		BufferCtl.fptr = bytesread;
		BSP_AUDIO_OUT_SetVolume(volume);
	}
	if((rekordbox.autocue == 2) && (trak.state == PLAYING)) {
		BSP_AUDIO_OUT_Pause();
		trak.state = STOPPED;
		HAL_TIM_Base_Start_IT(&htim8);
		GoToPosition(rekordbox.cue_start_position[0]);
		TrackTime();
	}
	while((!bOutOfData) && (file_pos_wide < rekordbox.spectrum_size)) {
		if(file_pos_wide >= 0) {
			if((rmin == 0) && (rsec < 30) && (rsec > 10)) {
				if(tim7_flag == 0) {
					__HAL_TIM_SET_AUTORELOAD(&htim7, 4999);
					HAL_TIM_Base_Start_IT(&htim7);
					tim7_flag =  1;
				}
				if(tim7_flag == 2) {
					HAL_TIM_Base_Stop_IT(&htim7);
					__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
					__HAL_TIM_SET_COUNTER(&htim7, 0);
					__HAL_TIM_SET_AUTORELOAD(&htim7, 4999);
					HAL_TIM_Base_Start_IT(&htim7);
					tim7_flag = 1;
				}
			}
			else if((rmin == 0) && (rsec <= 10)) {
				if(tim7_flag == 0) {
					__HAL_TIM_SET_AUTORELOAD(&htim7, 1999);
					HAL_TIM_Base_Start_IT(&htim7);
					tim7_flag =  2;
				}
				if(tim7_flag == 1) {
					HAL_TIM_Base_Stop_IT(&htim7);
					__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
					__HAL_TIM_SET_COUNTER(&htim7, 0);
					__HAL_TIM_SET_AUTORELOAD(&htim7, 1999);
					HAL_TIM_Base_Start_IT(&htim7);
					tim7_flag = 2;
				}
			}
			else {
				if(tim7_flag > 0) {
					HAL_TIM_Base_Stop_IT(&htim7);
					__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
					__HAL_TIM_SET_COUNTER(&htim7, 0);
					tim7_flag = 0;
					if(display.timemode == 0) {
						display.trackbarmode = 0;
					}
					else {
						display.trackbarmode = 1;
					}
				}
			}
		}
	}
	BSP_AUDIO_OUT_SetVolume(0);
	wavtagsize = 0;
	HAL_TIM_Base_Stop_IT(&htim7);
	__HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim7, 0);
	tim7_flag = 0;
	if(display.timemode == 0) display.trackbarmode = 0;
	else display.trackbarmode = 1;
}

void GoToPosition(uint32_t position) {
	BSP_AUDIO_OUT_Pause();
	file_pos_wide = position;
	file_pos = (float)position / (float)rekordbox.spectrum_size *
			rekordbox.lowp_spectrum_size;
	if(BufferCtl.filetype == 0)	{
		uint16_t i = 0;
		do {
			f_lseek(&MyFile, (position*(float)(f_size(&MyFile)-id3tagsize-wavtagsize)
				/rekordbox.spectrum_size + i));
			i++;
		}
		while(((UINT)BufferCtl.buff & 1) != (f_tell(&MyFile) & 1));
	}
	else {
		f_lseek(&MyFile, (position*(float)(f_size(&MyFile)-id3tagsize-wavtagsize)
						/rekordbox.spectrum_size));
	}
	if(BufferCtl.filetype == 1) {
		unInDataLeft = 0;
		unDmaBufferSpace = 0;
		unFramesDecoded = 0;
		unDmaBufMode = 0;
	}
}
