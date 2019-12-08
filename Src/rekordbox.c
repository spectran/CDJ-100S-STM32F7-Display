/*
 * rekordbox.c
 *
 *  Created on: 14 ��� 2018 �.
 *      Author: teren
 */

#include "ff.h"
#include "rekordbox.h"
#include <wchar.h>
#include <string.h>
#include "main.h"

char wave_token[5] = "PWAV";
char wv2_token[5] = "PWV2";
char wv3_token[5] = "PWV3";
char cob_token[5] = "PCOB";
char cue_token[5] = "PCPT";
char path_token[5] = "PPTH";
char qtz_token[5] = "PQTZ";

char tag[5];

RekordboxTypeDef rekordbox;

extern uint8_t lowp_wavebuffer[400];

extern AUDIO_OUT_BufferTypeDef  BufferCtl;

extern FIL MyFile;     /* File object */
extern uint16_t bytesread;
extern uint8_t acue_sensitivity;

static uint8_t FindToken (char *token);
static int32_t GetLongNumber ();
static int32_t GetBigEndianLongNumber ();

// finds section token in the file. 0 - token is found, 1 - end of file
static uint8_t FindToken (char *token) {
	for(int i = 0; i < 5; i++) tag[i] = 0;
	while(f_tell(&MyFile) < f_size(&MyFile)) {
		while(f_tell(&MyFile) < f_size(&MyFile)) {
			while(f_read(&MyFile, &tag[0], 1, (void *)&bytesread) != FR_OK);
			if(tag[0] == 'P') break;
		}
		while(f_read(&MyFile, &tag[1], 3, (void *)&bytesread) != FR_OK);
		tag[4] = '\0';
		if(strcmp(tag, token) == 0) return 0;
	}
	return 1;
}

static int32_t GetLongNumber () {
	uint8_t i = 0;
	int32_t out_data = 0;
	for(int i = 0; i < 5; i++) tag[i] = 0;
	while(f_read(&MyFile, &tag[0], 4, (void *)&bytesread) != FR_OK);
	for(i = 0; i < 4; i++) {
		out_data |= tag[i];
		out_data <<= 8;
	}
	out_data >>= 8;
	return out_data;
}

static int32_t GetBigEndianLongNumber () {
	int i = 0;
	int32_t out_data = 0;
	for(int i = 0; i < 5; i++) tag[i] = 0;
	while(f_read(&MyFile, &tag[0], 4, (void *)&bytesread) != FR_OK);
	for(i = 3; i >= 0; i--) {
		out_data |= tag[i];
		out_data <<= 8;
	}
	out_data >>= 8;
	return out_data;
}

uint8_t DecodeRekordboxFiles(TCHAR *path) {
	uint32_t data_size = 0;
	uint8_t k;
	uint32_t i;
	uint8_t res = FR_OK;
	for(i = 0; i < 1000; i++) {
		rekordbox.bpm[i] = 0;
		rekordbox.phase[i] = 0;
		rekordbox.timeindex[i] = 0;
	}
	rekordbox.cues = 0;
	// Clear SDRAM or fill with some pattern/color
	for(i = 0; i < 1000000; i++) *(__IO uint8_t*)(WAVE_BUFFER+i) = 0;
	for(i = 0; i < 255; i++) rekordbox.filename[i] = 0;
	char filename[255] = {0};
	TCHAR my_path[255] = {0};
	wcscpy(my_path, path);
	wcscat(my_path, L"/ANLZ0000.DAT");
	res = f_open(&MyFile, my_path, FA_READ);
	if(res == FR_OK)
	{
		if(FindToken(path_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
		data_size = GetLongNumber();
		while(f_read(&MyFile, filename, data_size, (void *)&bytesread) != FR_OK);
		i = 0;
		k = 0;
		while(i < data_size) {
			WCHAR c = filename[i];
			c <<= 8;
			i++;
			c |= filename[i];
			i++;
			rekordbox.filename[k] = c;
			filename[k] = ff_convert(c, 0);
			k++;
		}
		for(i = k; i < data_size; i++) filename[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '.')) i--;
		k = 0;
		while(k < 5)
		{
			if(filename[i] >= 32) {
				rekordbox.filetype[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.filetype[4] = 0;

		for(i = 0; i < 255; i++) rekordbox.file[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '/')) i--;
		k = 0;
		i++;
		while(i < data_size)
		{
			if(filename[i] >= 32) {
				rekordbox.file[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.file[i] = 0;

		if(FindToken(qtz_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 16, (void *)&bytesread) != FR_OK); // dummy read 16 bytes
		data_size = GetLongNumber();
		rekordbox.timezones = data_size;
		for(i=0; i<rekordbox.timezones; i++) {
			for(k=0; k<2; k++) {
				while(f_read(&MyFile, &tag[k], 1, (void *)&bytesread) != FR_OK);
			}
			rekordbox.phase[i] |= tag[0];
			rekordbox.phase[i] <<= 8;
			rekordbox.phase[i] |= tag[1];
			for(k=0; k<2; k++) {
				while(f_read(&MyFile, &tag[k], 1, (void *)&bytesread) != FR_OK);
			}
			rekordbox.bpm[i] |= tag[0];
			rekordbox.bpm[i] <<= 8;
			rekordbox.bpm[i] |= tag[1];
			rekordbox.timeindex[i] = GetLongNumber();
		}
		if(rekordbox.phase[0] == 1) rekordbox.beat_grid_offset = 4;
		else rekordbox.beat_grid_offset = rekordbox.phase[0] - 1;
		if(FindToken(wave_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
		rekordbox.lowp_spectrum_size = GetLongNumber();
		while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
		while(f_read(&MyFile, &lowp_wavebuffer[0], rekordbox.lowp_spectrum_size, (void *)&bytesread) != FR_OK);

		if(FindToken(cob_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK); // dummy read 12 bytes
		rekordbox.cues = GetLongNumber() + 1;
		for(i = 1; i < rekordbox.cues; i++) {
			if(FindToken(cue_token) != 0) return 1;
			while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
			rekordbox.cue_number[i] = GetLongNumber();
			rekordbox.cue_active[i] = GetLongNumber();
			while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
			rekordbox.cue_type[i] = GetLongNumber();
			while(f_read(&MyFile, &rekordbox.cue_singleloop[i], 1, (void *)&bytesread) != FR_OK);
			while(f_read(&MyFile, BufferCtl.buff, 3, (void *)&bytesread) != FR_OK); // dummy read 3 bytes
			rekordbox.cue_start_position[i] = (int32_t)((float)GetLongNumber()*150/1000);
			if(rekordbox.cue_singleloop[i] == 2) rekordbox.cue_end_position[i] = (int32_t)((float)GetLongNumber()*150/1000);
		}
		if(FindToken(cob_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK); // dummy read 12 bytes
		rekordbox.cues += GetLongNumber();
		k = i;
		for(k = i; k < rekordbox.cues; k++) {
			if(FindToken(cue_token) != 0) return 1;
			while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
			rekordbox.cue_number[k] = GetLongNumber();
			rekordbox.cue_active[k] = GetLongNumber();
			while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
			rekordbox.cue_type[k] = GetLongNumber();
			while(f_read(&MyFile, &rekordbox.cue_singleloop[k], 1, (void *)&bytesread) != FR_OK);
			while(f_read(&MyFile, BufferCtl.buff, 3, (void *)&bytesread) != FR_OK); // dummy read 3 bytes
			rekordbox.cue_start_position[k] = (int32_t)((float)GetLongNumber()*150/1000);
			if(rekordbox.cue_singleloop[k] == 2) rekordbox.cue_end_position[k] = (int32_t)((float)GetLongNumber()*150/1000);
		}
		f_close(&MyFile);
	}
	for(i = 0; i < 255; i++) my_path[i] = 0;
	wcscpy(my_path, path);
	wcscat(my_path, L"/ANLZ0000.EXT");
	res = f_open(&MyFile, my_path, FA_READ);
	if(res == FR_OK) {
		if(FindToken(wv3_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK);
		rekordbox.spectrum_size = GetLongNumber();
		while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
		rekordbox.cue_start_position[0] = 0;
		for(i = 0; i < rekordbox.spectrum_size; i++) {
			while(f_read(&MyFile, &tag[0], 1, (void *)&bytesread) != FR_OK);
			*(__IO uint8_t*)(WAVE_BUFFER+i) = tag[0];
			if(rekordbox.autocue == 1) {
				if(((tag[0] & 0x1F) > acue_sensitivity/10) && ((tag[0] & 0xE0) < 128)) {
					rekordbox.autocue = 2;
					rekordbox.cue_start_position[0] = i;
				}
			}
		}
		f_close(&MyFile);
	}
	for(i = 0; i < 255; i++) my_path[i] = 0;
	wcscpy(my_path, path);
	wcscat(my_path, L"/ANLZ0001.DAT");
	res = f_open(&MyFile, my_path, FA_READ);
	if(res == FR_OK)
	{
		if(FindToken(path_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
		data_size = GetLongNumber();
		while(f_read(&MyFile, filename, data_size, (void *)&bytesread) != FR_OK);
		i = 0;
		k = 0;
		while(i < data_size) {
			WCHAR c = filename[i];
			c <<= 8;
			i++;
			c |= filename[i];
			i++;
			rekordbox.filename[k] = c;
			filename[k] = ff_convert(c, 0);
			k++;
		}
		for(i = k; i < data_size; i++) filename[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '.')) i--;
		k = 0;
		while(k < 5)
		{
			if(filename[i] >= 32) {
				rekordbox.filetype[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.filetype[4] = 0;

		for(i = 0; i < 255; i++) rekordbox.file[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '/')) i--;
		k = 0;
		i++;
		while(i < data_size)
		{
			if(filename[i] >= 32) {
				rekordbox.file[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.file[i] = 0;

		if(FindToken(qtz_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 16, (void *)&bytesread) != FR_OK); // dummy read 22 bytes
		data_size = GetLongNumber();
		rekordbox.timezones = data_size;
		for(i=0; i<rekordbox.timezones; i++) {
			for(k=0; k<2; k++) {
				while(f_read(&MyFile, &tag[k], 1, (void *)&bytesread) != FR_OK);
			}
			rekordbox.phase[i] |= tag[0];
			rekordbox.phase[i] <<= 8;
			rekordbox.phase[i] |= tag[1];
			for(k=0; k<2; k++) {
				while(f_read(&MyFile, &tag[k], 1, (void *)&bytesread) != FR_OK);
			}
			rekordbox.bpm[i] |= tag[0];
			rekordbox.bpm[i] <<= 8;
			rekordbox.bpm[i] |= tag[1];
			rekordbox.timeindex[i] = GetLongNumber();
		}
		if(rekordbox.phase[0] == 1) rekordbox.beat_grid_offset = 4;
		else rekordbox.beat_grid_offset = rekordbox.phase[0] - 1;
		if(FindToken(wave_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
		rekordbox.lowp_spectrum_size = GetLongNumber();
		while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
		while(f_read(&MyFile, &lowp_wavebuffer[0], rekordbox.lowp_spectrum_size, (void *)&bytesread) != FR_OK);
		if(FindToken(cob_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK); // dummy read 12 bytes
		rekordbox.cues = GetLongNumber() + 1;
		for(i = 1; i < rekordbox.cues; i++) {
			if(FindToken(cue_token) != 0) return 1;
			while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
			rekordbox.cue_number[i] = GetLongNumber();
			rekordbox.cue_active[i] = GetLongNumber();
			while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
			rekordbox.cue_type[i] = GetLongNumber();
			while(f_read(&MyFile, &rekordbox.cue_singleloop[i], 1, (void *)&bytesread) != FR_OK);
			while(f_read(&MyFile, BufferCtl.buff, 3, (void *)&bytesread) != FR_OK); // dummy read 3 bytes
			rekordbox.cue_start_position[i] = (int32_t)((float)GetLongNumber()*150/1000);
			if(rekordbox.cue_singleloop[i] == 2) rekordbox.cue_end_position[i] = (int32_t)((float)GetLongNumber()*150/1000);
		}
		if(FindToken(cob_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK); // dummy read 12 bytes
		rekordbox.cues += GetLongNumber();
		k = i;
		for(k = i; k < rekordbox.cues; k++) {
			if(FindToken(cue_token) != 0) return 1;
			while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
			rekordbox.cue_number[k] = GetLongNumber();
			rekordbox.cue_active[k] = GetLongNumber();
			while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
			rekordbox.cue_type[k] = GetLongNumber();
			while(f_read(&MyFile, &rekordbox.cue_singleloop[k], 1, (void *)&bytesread) != FR_OK);
			while(f_read(&MyFile, BufferCtl.buff, 3, (void *)&bytesread) != FR_OK); // dummy read 3 bytes
			rekordbox.cue_start_position[k] = (int32_t)((float)GetLongNumber()*150/1000);
			if(rekordbox.cue_singleloop[k] == 2) rekordbox.cue_end_position[k] = (int32_t)((float)GetLongNumber()*150/1000);
		}
		f_close(&MyFile);
	}
	for(i = 0; i < 255; i++) my_path[i] = 0;
	wcscpy(my_path, path);
	wcscat(my_path, L"/ANLZ0001.EXT");
	res = f_open(&MyFile, my_path, FA_READ);
	if(res == FR_OK) {
		if(FindToken(wv3_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 12, (void *)&bytesread) != FR_OK);
		rekordbox.spectrum_size = GetLongNumber();
		while(f_read(&MyFile, BufferCtl.buff, 4, (void *)&bytesread) != FR_OK); // dummy read 4 bytes
		rekordbox.cue_start_position[0] = 0;
		for(i=0; i<rekordbox.spectrum_size; i++) {
			while(f_read(&MyFile, &tag[0], 1, (void *)&bytesread) != FR_OK);
			*(__IO uint8_t*)(WAVE_BUFFER+i) = tag[0];
			if(rekordbox.autocue == 1) {
				if(((tag[0] & 0x1F) > acue_sensitivity/10) && ((tag[0] & 0xE0) < 128)) {
					rekordbox.autocue = 2;
					rekordbox.cue_start_position[0] = i;
				}
			}
		}
		f_close(&MyFile);
	}
	i = 0;
	uint8_t l = 0;
	while(i++  < rekordbox.spectrum_size) {
		if((i % 9000) == 0) {
			rekordbox.remminticks[l] = (uint16_t)(rekordbox.lowp_spectrum_size * (rekordbox.spectrum_size - i) / rekordbox.spectrum_size);
			rekordbox.elminticks[l] = (uint16_t)(rekordbox.lowp_spectrum_size * i / rekordbox.spectrum_size);
			l++;
		}
	}
	rekordbox.minutes = l;
	rekordbox.state = 1;
	return 0;
}

uint8_t GetFileName(TCHAR *path)
{
	uint32_t data_size = 0;
	uint8_t k;
	uint32_t i;
	uint8_t res = FR_OK;
	for(i = 0; i < 255; i++) rekordbox.filename[i] = 0;
	char filename[255] = {0};

	res = f_open(&MyFile, path, FA_READ);
	if(res == FR_OK)
	{
		if(FindToken(path_token) != 0) return 1;
		while(f_read(&MyFile, BufferCtl.buff, 8, (void *)&bytesread) != FR_OK); // dummy read 8 bytes
		data_size = GetLongNumber();
		while(f_read(&MyFile, filename, data_size, (void *)&bytesread) != FR_OK);
		i = 0;
		k = 0;
		while(i < data_size) {
			WCHAR c = filename[i];
			c <<= 8;
			i++;
			c |= filename[i];
			i++;
			rekordbox.filename[k] = c;
			filename[k] = ff_convert(c, 0);
			k++;
		}
		for(i = k; i < data_size; i++) filename[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '.')) i--;
		k = 0;
		while(k < 5)
		{
			if(filename[i] >= 32) {
				rekordbox.filetype[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.filetype[4] = 0;

		for(i = 0; i < 255; i++) rekordbox.file[i] = 0;
		i = data_size;
		while((i > 0) && (filename[i] != '/')) i--;
		k = 0;
		i++;
		while(i < data_size)
		{
			if(filename[i] >= 32) {
				rekordbox.file[k] = filename[i];
				k++;
			}
			i++;
		}
		rekordbox.file[i] = 0;
	}
	f_close(&MyFile);
	return 0;
}
