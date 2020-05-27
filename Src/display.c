/*
 * display.c
 *
 *  Created on: 30 ��� 2018 �.
 *      Author: teren
 */

#include "ltdc.h"
#include "dma2d.h"
#include "rekordbox.h"
#include "cdj2000_fonts.h"
#include "display.h"
#include "string.h"
#include "stm32746g_discovery_audio.h"
#include <stdio.h>

extern uint8_t lowp_wavebuffer[400];
extern uint32_t file_pos;
extern uint32_t file_pos_wide;
extern RekordboxTypeDef rekordbox;
extern uint8_t ActiveLayer;
extern int Track_number;
extern int16_t rfr;
extern int8_t rsec;
extern int8_t rmin;
extern int16_t fr;
extern int8_t sec;
extern int8_t min;
extern uint8_t TrackTable[100][255];
extern int str_offset;
extern char String_number;
extern char Mark_number;
extern int str_increment;
extern uint16_t Total_tracks;

float stretch = 1;
int8_t menu_mode = 0;

int beat = 0;
uint32_t first_beat = 0;
int bar = 0;
uint32_t color = 0, height = 0;
volatile int32_t position = 0;
volatile int32_t check = 0;

DisplayTypeDef display;
extern TrackTypeDef trak;
extern uint8_t inbuf[AUDIO_OUT_BUFFER_SIZE];
extern uint8_t volume;
extern float jog_sensitivity;
extern uint8_t acue_sensitivity;

// converts ARGB8888 to RGB565
static uint16_t Color_Convertion(uint32_t in_color)
{
	uint16_t red = (in_color & 0x00FF0000) >> 16;
	uint16_t green = (in_color & 0x0000FF00) >> 8;
	uint16_t blue = (in_color & 0x000000FF);
	uint16_t out_color = ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
	return out_color;
}

// clears selected ActiveLayer
void ClearLayer()
{
	hdma2d.Init.OutputOffset = 0;
	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
		if(HAL_DMA2D_Start(&hdma2d, 0, hltdc.LayerCfg[ActiveLayer].FBStartAdress,
				hltdc.LayerCfg[ActiveLayer].ImageWidth, hltdc.LayerCfg[ActiveLayer].ImageHeight) == HAL_OK) {
			/* Polling For DMA transfer */
			HAL_DMA2D_PollForTransfer(&hdma2d, 100);
		}
	}
}

// changes ActiveLayers
void ChangeLayers(void)
{
	if(ActiveLayer == 0) {
		HAL_LTDC_SetAlpha_NoReload(&hltdc, 255, ActiveLayer++);
		HAL_LTDC_SetAlpha_NoReload(&hltdc, 0, ActiveLayer);
	}
	else {
		HAL_LTDC_SetAlpha_NoReload(&hltdc, 255, ActiveLayer--);
		HAL_LTDC_SetAlpha_NoReload(&hltdc, 0, ActiveLayer);
	}
	HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
}

void DrawMenu()
{
	uint32_t backcolor = 0;
	if(menu_mode == 0) {
		char string[255] = " { ";
		DrawString(strcat(string, rekordbox.file), 0, 20, 0x00FFFFFF, 0x000000FF);
	}
	else if(menu_mode == 1) {
		if(Total_tracks > 7) {
			uint16_t scroll_lenght = (140/(float)Total_tracks)*7;
			uint16_t step_scroll = (140/(float)Total_tracks)*(float)str_increment;
			if(scroll_lenght < 5) scroll_lenght = 5;
			VLine(1, 21, 140, 0x00FFFFFF);
			VLine(3, 22+step_scroll, scroll_lenght-2, 0x00FFFFFF);
			VLine(4, 21+step_scroll, scroll_lenght, 0x00FFFFFF);
			VLine(5, 21+step_scroll, scroll_lenght, 0x00FFFFFF);
			VLine(6, 21+step_scroll, scroll_lenght, 0x00FFFFFF);
			VLine(7, 22+step_scroll, scroll_lenght-2, 0x00FFFFFF);
			VLine(9, 21, 140, 0x00FFFFFF);
		}
		DrawString(" [FILES]", 0, 20, 0x00FFFFFF, 0x000000FF);
		int i = 0;
		while(21+20*(i+1) < 165) {
			char string[255] = "{ ";
			if(i == Track_number - str_increment) {
				if(i == Mark_number) {
					DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00000000, 0x00FFFFFF);
				}
				else {
					DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00FFFFFF, 0x000000FF);
				}
				backcolor += 0x00202020;
				if(backcolor > 0x00404040) backcolor = 0;
			}
			else if(i == String_number) {
				if(str_offset > 10) {
					DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11 + str_offset, 21+20*(i+1), 0x00FFFFFF, 0x00008F00);
				}
				else {
					if(i == Mark_number) {
						DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00000000, 0x00FFFFFF);
					}
					else {
						DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00FFFFFF, backcolor);
					}
				}
				backcolor += 0x00202020;
				if(backcolor > 0x00404040) backcolor = 0;
			}
			else {
				if(i == Mark_number) {
					DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00000000, 0x00FFFFFF);
				}
				else {
					DrawString(strcat(string, (char*)TrackTable[i+str_increment]), 11, 21+20*(i+1), 0x00FFFFFF, backcolor);
				}
				backcolor += 0x00202020;
				if(backcolor > 0x00404040) backcolor = 0;
			}
			i++;
			if(i >= Total_tracks) break;
		}
	}
	else if(menu_mode == 2) {
		DrawString(" [SETTINGS]", 0, 20, 0x00FFFFFF, 0x000000FF);
		backcolor += 0x00202020;
		char stri[50] = {0};
		sprintf(stri, "%s %d %s", " Volume", volume, "%");
		DrawString(stri, 0, 40, 0x00FFFFFF, backcolor);
		VLine(40, 41, 18, 0x00FFFFFF);
		VLine(440, 41, 18, 0x00FFFFFF);
		HLine(40, 41, 400, 0x00FFFFFF);
		HLine(40, 59, 401, 0x00FFFFFF);
		uint32_t vol_pos = (float)volume/100*400;
		if(vol_pos != 0) Rectangle(40, 42, vol_pos, 17, 0x00FFFFFF);
		backcolor += 0x00202020;
		uint32_t jogsens_pos = jog_sensitivity * 10000;
		sprintf(stri, "%s %d %s", " Jog sensitivity", (int)jogsens_pos, "%");
		DrawString(stri, 0, 80, 0x00FFFFFF, backcolor);
		VLine(40, 81, 18, 0x00FFFFFF);
		VLine(440, 81, 18, 0x00FFFFFF);
		HLine(40, 81, 400, 0x00FFFFFF);
		HLine(40, 99, 401, 0x00FFFFFF);
		jogsens_pos *= 4;
		if(jogsens_pos != 0) Rectangle(40, 82, jogsens_pos, 17, 0x00FFFFFF);
		backcolor += 0x00202020;
		sprintf(stri, "%s %d %s", " Auto cue sensitivity", (int)((float)acue_sensitivity/250*100), "%");
		DrawString(stri, 0, 120, 0x00FFFFFF, backcolor);
		VLine(40, 121, 18, 0x00FFFFFF);
		VLine(440, 121, 18, 0x00FFFFFF);
		HLine(40, 121, 400, 0x00FFFFFF);
		HLine(40, 139, 401, 0x00FFFFFF);
		uint32_t acu_pos = (float)acue_sensitivity/250*400;
		if(acu_pos != 0) Rectangle(40, 122, acu_pos, 17, 0x00FFFFFF);
	}
	else if(menu_mode == 3) {
		DrawString("ERROR! INSERT SD CARD AND RESTART!", 0, 20, 0x00FFFFFF, 0x00FF0000);
	}
}

// draws low resolution spectrum on the selected ActiveLayer
void DrawLowSpectrum()
{
	uint32_t i, color, height;
	for(i = 0; i < 400; i++)
	{
		color = (uint32_t)(lowp_wavebuffer[i] & 0xE0); // read color data - first 3 bits
		color <<= 8;
		color |= (uint32_t)(lowp_wavebuffer[i] & 0xE0);
		color <<= 8;
		color |= 0x000000FF;
		height = (uint32_t)(lowp_wavebuffer[i]);
		height &= 0x1F; //read height data - last 5 bits
		VLine(i + 40, 250 - height, height, color);
	}
	// draw bpm frame
	VLine(397, 174, 32, 0x00FFFFFF);
	VLine(453, 174, 32, 0x00FFFFFF);
	HLine(399, 172, 53, 0x00FFFFFF);
	HLine(399, 207, 53, 0x00FFFFFF);
	Pixel(398, 173, 0xFFFF);
	Pixel(452, 173, 0xFFFF);
	Pixel(398, 206, 0xFFFF);
	Pixel(452, 206, 0xFFFF);
	DrawElement(20, 5, 430, 205, 0x00FFFFFF, bpm);

	// draw bottom frame
	// draw left border
	VLine(40, 252, 4, 0x00FFFFFF);
	// draw right border
	VLine(440, 252, 4, 0x00FFFFFF);
	// draw upper border
	HLine(40, 252, 400, 0x00FFFFFF);
	// draw bottom border
	HLine(40, 256, 401, 0x00FFFFFF);
	if(display.timemode == 0) {
		// draw remain indicator
		DrawElement(37, 5, 84, 191, 0x00FFFFFF, remain);
	}
	if(display.trackbarmode == 0) {
		// remain
		Rectangle(file_pos+40, 253, 400-file_pos, 3, 0x00FFFFFF);
	}
	else {
		// elapsed
		Rectangle(40, 253, file_pos, 3, 0x00FFFFFF);
	}
	// draw minute ticks
	i = 0;
	while(i < rekordbox.minutes) {
		if(display.timemode == 0) {
			VLine(40 + rekordbox.remminticks[i], 260, 4, 0x00FFFFFF);
		}
		else {
			VLine(40 + rekordbox.elminticks[i], 260, 4, 0x00FFFFFF);
		}
		i++;
	}
	// draw cue point
	//DrawElement(7, 4, 40, 264, 0x00FFFF00, cue_pointer);
	// draw percent
	DrawElement(7, 8, 375, 204, 0x00FFFFFF, percent);
	if(trak.percent < 0) DrawElement(10, 2, 308, 195, 0x00FFFFFF, minus);
	else DrawElement(10, 10, 308, 199, 0x00FFFFFF, plus);
	uint16_t percent = (abs)(trak.percent * 10000);
	if(display.pitchmode == 0) {
		// draw wide pitch
		DrawElement(34, 7, 445, 230, 0x00FF0000, pitch_wide);
		if(percent / 10000 > 0) {
			DrawDigit(percent / 10000, 315, 186, 10, 0x00FFFFFF);
		}
		if(percent / 1000 > 0) {
			DrawDigit(((percent / 100) % 100) / 10, 330, 186, 10, 0x00FFFFFF);
		}
		DrawDigit(((percent / 100) % 100) % 10, 345, 186, 10, 0x00FFFFFF);
		DrawElement(2, 2, 356, 204, 0x00FFFFFF, small_point);
		if(percent % 100 >= 5) DrawDigit(5, 360, 186, 10, 0x00FFFFFF);
		else DrawDigit(0, 360, 186, 10, 0x00FFFFFF);
	}
	else if(display.pitchmode == 1) {
		// draw 16% pitch
		DrawElement(34, 9, 445, 241, 0x00FFFFFF, pitch16);
		if(percent / 1000 > 0) {
			DrawDigit(((percent / 100) % 100) / 10, 315, 186, 10, 0x00FFFFFF);
		}
		DrawDigit(((percent / 100) % 100) % 10, 330, 186, 10, 0x00FFFFFF);
		DrawElement(2, 2, 341, 204, 0x00FFFFFF, small_point);
		DrawDigit((percent / 10) % 10, 345, 186, 10, 0x00FFFFFF);
		DrawDigit((percent % 2) * 5, 360, 186, 10, 0x00FFFFFF);
	}
	else if(display.pitchmode == 2) {
		// draw 10% pitch
		DrawElement(34, 9, 445, 252, 0x00FF0000, pitch10);
		if(percent / 1000 > 0) {
			DrawDigit(((percent / 100) % 100) / 10, 315, 186, 10, 0x00FFFFFF);
		}
		DrawDigit(((percent / 100) % 100) % 10, 330, 186, 10, 0x00FFFFFF);
		DrawElement(2, 2, 341, 204, 0x00FFFFFF, small_point);
		DrawDigit((percent / 10) % 10, 345, 186, 10, 0x00FFFFFF);
		DrawDigit((percent % 2) * 5, 360, 186, 10, 0x00FFFFFF);
	}
	else if(display.pitchmode == 3) {
		// draw 6% pitch
		DrawElement(34, 9, 445, 263, 0x0000FF00, pitch6);
		DrawDigit(((percent / 100) % 100) % 10, 330, 186, 10, 0x00FFFFFF);
		DrawElement(2, 2, 341, 204, 0x00FFFFFF, small_point);
		DrawDigit((percent / 10) % 10, 345, 186, 10, 0x00FFFFFF);
		DrawDigit((percent % 5) * 2, 360, 186, 10, 0x00FFFFFF);
	}
	// draw track indicator
	DrawElement(34, 5, 40, 178, 0x00FFFFFF, track);
	// draw tempo indicator
	DrawElement(31, 5, 310, 178, 0x00FFFFFF, tempo);
	// draw auto cue indicator
	if(display.cuemode == 1) {
		DrawElement(34, 11, 86, 207, 0x00FF0000, auto_cue);
	}
	// draw quantize indicator
	if(display.quantize == 1) {
		DrawElement(48, 5, 200, 178, 0x00FF0000, quantize);
	}
	// draw master tempo indicator
	if(display.mastertempomode == 1) {
		DrawElement(34, 11, 355, 183, 0x00FF0000, master_tempo);
	}
	// draw minutes
	DrawElement(15, 16, 165, 204, 0x00FFFFFF, minutes);
	// draw seconds
	DrawElement(6, 7, 213, 204, 0x00FFFFFF, seconds);
	// draw frames
	DrawElement(5, 6, 252, 204, 0x00FFFFFF, frames);
	DrawElement(3, 4, 252, 204, 0x00FFFFFF, point);

	DrawDigit((Track_number + 1) / 10, 40, 183, 13, 0x00FFFFFF);
	DrawDigit((Track_number + 1) % 10, 60, 183, 13, 0x00FFFFFF);
	if(display.timemode == 0) {
		DrawDigit(rmin / 10, 134, 184, 11, 0x00FFFFFF);
		DrawDigit(rmin % 10, 148, 184, 11, 0x00FFFFFF);

		DrawDigit(rsec / 10, 185, 184, 11, 0x00FFFFFF);
		DrawDigit(rsec % 10, 199, 184, 11, 0x00FFFFFF);

		DrawDigit(rfr / 20, 224, 184, 11, 0x00FFFFFF);
		DrawDigit((rfr / 2) % 10, 238, 184, 11, 0x00FFFFFF);
	}
	else {
		DrawDigit(min / 10, 134, 184, 11, 0x00FFFFFF);
		DrawDigit(min % 10, 148, 184, 11, 0x00FFFFFF);

		DrawDigit(sec / 10, 185, 184, 11, 0x00FFFFFF);
		DrawDigit(sec % 10, 199, 184, 11, 0x00FFFFFF);

		DrawDigit(fr / 20, 224, 184, 11, 0x00FFFFFF);
		DrawDigit((fr / 2) % 10, 238, 184, 11, 0x00FFFFFF);
	}

	uint16_t bpm = rekordbox.bpm[beat] * (1 + trak.percent);
	if(bpm / 10000 > 0) {
		DrawDigit(bpm / 10000, 400, 176, 10, 0x00FFFFFF);
	}
	if(bpm / 1000 > 0) {
		DrawDigit(((bpm / 100) % 100) / 10, 415, 176, 10, 0x00FFFFFF);
	}
	DrawDigit(((bpm / 100) % 100) % 10, 430, 176, 10, 0x00FFFFFF);
	DrawElement(2, 2, 442, 194, 0x00FFFFFF, small_point);
	DrawDigit((bpm / 10) % 10, 445, 182, 7, 0x00FFFFFF);

	if(rekordbox.state == 1) {
		if(rekordbox.cue_start_position[0] != 0) {
			DrawElement(9, 5, 36+(rekordbox.cue_start_position[0]
				*400/rekordbox.spectrum_size), 219, 0x00FFFF00, hot_cue);
		}
		// draw cue indicator
		if(rekordbox.cue_start_position[0] != 0) {
			DrawElement(36, 11, 1, 260, 0x00FF0000, cue_ind);
		}
	}

	if(rekordbox.cues > 1) {
		// draw memory indicator
		DrawElement(36, 11, 1, 230, 0x00FF0000, memory_ind);
		for(i = 1; i < rekordbox.cues; i++) {
			uint16_t start_pos = rekordbox.lowp_spectrum_size*
					(float)rekordbox.cue_start_position[i]/rekordbox.spectrum_size;
			if(rekordbox.cue_singleloop[i] == 1) {
				DrawElement(9, 5, 37+start_pos, 219, 0x00FF0000, hot_cue);
			}
			if(rekordbox.cue_singleloop[i] == 2) {
				uint16_t end_pos = rekordbox.lowp_spectrum_size*
					(float)rekordbox.cue_end_position[i]/rekordbox.spectrum_size;
				VLine(40+start_pos, 220, 40, 0x0000FF00);
				Pixel(41+start_pos, 220, 0x0000FF00);
				Pixel(41+start_pos, 259, 0x0000FF00);
				VLine(40+end_pos, 220, 40, 0x0000FF00);
				Pixel(39+end_pos, 220, 0x0000FF00);
				Pixel(39+end_pos, 259, 0x0000FF00);
			}
		}
	}
	// draw loop borders and loop indicator
	if(display.loop == 1) {
		DrawElement(22, 11, 270, 199, 0x0000FF00, loop_l);
		uint16_t start_pos = rekordbox.lowp_spectrum_size*
				(float)display.loopstart/rekordbox.spectrum_size;
		uint16_t end_pos = rekordbox.lowp_spectrum_size*
				(float)display.loopend/rekordbox.spectrum_size;
		VLine(40+start_pos, 220, 40, 0x0000FF00);
		Pixel(41+start_pos, 220, 0x0000FF00);
		Pixel(41+start_pos, 259, 0x0000FF00);
		VLine(40+end_pos, 220, 40, 0x0000FF00);
		Pixel(39+end_pos, 220, 0x0000FF00);
		Pixel(39+end_pos, 259, 0x0000FF00);
	}
	// draw cursor
	VLine(40 + file_pos, 220, 40, 0x00FFFFFF);
	VLine(41 + file_pos, 220, 40, 0x00FFFFFF);
}

void SetBeatGrid()
{
	if(position != file_pos_wide) {
		position = file_pos_wide;
		if(stretch > 1) {
			while((position % (int32_t)(stretch)) != (check % (int32_t)(stretch))) {
				position++;
			}
		}
	}
	beat = 0;
	first_beat = 0;
	int32_t check = position + (-240)*stretch;
	if(check > 0) {
		while(((int32_t)(rekordbox.timeindex[first_beat]*150/1000 <= check)) && (first_beat <= rekordbox.timezones)) {
			first_beat++;
		}
	}
	if(position > 0) {
		while((rekordbox.timeindex[beat]*150/1000 <= position) && (beat <= rekordbox.timezones)) {
			beat++;
		}
		beat--;
	}
}

// draws element
void DrawElement(uint16_t width, uint16_t height, uint16_t xpos, uint16_t ypos, uint32_t color, const uint16_t* element)
{
	uint16_t i = 0;
	while(i < width) {
		uint16_t k = 0;
		while(k < height) {
			if(((element[i] >> k) & 1) == 1) {
				*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((ypos - k) * hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = Color_Convertion(color);
			}
			else {
				*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((ypos - k) * hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = 0;
			}
			k++;
		}
		i++;
	}
}

// draws high resolution spectrum on the selected ActiveLayer
void DrawSpectrum()
{
	uint8_t spectrum_frame[480] = {0};
	uint32_t new_cues[rekordbox.cues];
	int i, k;
	// set new high resolution spectrum start position
	if(position != file_pos_wide) {
		position = file_pos_wide;
		// synchronize old and new spectrum positions
		if(stretch > 1) {
			while((position % (int32_t)(stretch)) != (check % (int32_t)(stretch))) {
				position++;
			}
		}
	}
	// fill new cue positions buffer
	for(k = 0; k < rekordbox.cues; k++) {
		new_cues[k] = 0;
		i = 0;
		check = position + (int32_t)((i-240)*stretch);
		if(check < rekordbox.cue_start_position[k]) {
			while(check < rekordbox.cue_start_position[k]) {
				i++;
				check = position + (int32_t)((i-240)*stretch);
			}
			if(new_cues[k] != check) new_cues[k] = check;
		}
	}
	// fill the spectrum frame buffer
	for(i = 0; i < 480; i++) {
		check = position + (int32_t)((i-240)*stretch);
		if(check > 0) {
			spectrum_frame[i] = *(__IO uint8_t*)(WAVE_BUFFER+check);
		}
	}
	// draw spectrum
	for(i = 0; i < 480; i++)
	{
		check = position + (int32_t)((i-240)*stretch);
		if(check > 0) {
			// draw loop above the high resolution spectrum
			if(display.loop == 1) {
				if((check >= display.loopstart) && (check <= display.loopend)) {
					VLine(i, 75, 90, 0x008F8F8F);
				}
			}
			color = spectrum_frame[i] & 0xE0; // read color data - first 3 bits
			color <<= 8;
			color = spectrum_frame[i] & 0xE0;
			color <<= 8;
			color |= 0x000000FF;
			height = spectrum_frame[i] & 0x1F; //read height data - last 5 bits
			VLine(i, 120-height, height*2, color);
		}
	}
	// draw beat grid and other signs
	for(i = 0; i < 480; i++)
	{
		check = position + (int32_t)((i-240)*stretch);
		if(check > 0) {
			// draw beat ticks
			if(first_beat <= rekordbox.timezones) {
				if(check >= (int32_t)(rekordbox.timeindex[first_beat]*150/1000)) {
					if(rekordbox.phase[first_beat] == 1) {
						VLine(i, 75, 4, 0x00FF0000);
						VLine(i, 161, 4, 0x00FF0000);
					}
					else {
						VLine(i, 75, 4, 0x00FFFFFF);
						VLine(i, 161, 4, 0x00FFFFFF);
					}
					first_beat++;
				}
			}
			if(rekordbox.state == 1) {
				if(check == new_cues[0]) {
					if((i > 10) && (i < 472))
						DrawElement(15, 5, i - 11, 72, 0x00FFFF00, hot_cue_wide);
				}
			}
			for(int k = 1; k < rekordbox.cues; k++) {
				if(check == new_cues[k]) {
					if((i > 10) && (i < 472))
						DrawElement(15, 5, i - 11, 72, 0x00FF0000, hot_cue_wide);
				}
			}
		}
	}
	// draw central cursor
	VLine(239, 75, 90, 0x00FFFFFF);
	VLine(240, 75, 90, 0x00FFFFFF);
	// draw bar display
	bar = (beat + rekordbox.beat_grid_offset) / 4;
	if(bar / 100 > 0) {
		DrawDigit(bar / 100, 305, 51, 7, 0x000000FF);
		DrawDigit(bar % 100 / 10 , 314, 51, 7, 0x000000FF);
	}
	else if(bar / 10 > 0) DrawDigit(bar / 10, 314, 51, 7, 0x000000FF);
	DrawDigit(bar % 10, 323, 51, 7, 0x000000FF);
	DrawElement(2, 2, 332, 63, 0x000000FF, small_point);
	DrawDigit(rekordbox.phase[beat], 336, 51, 7, 0x000000FF);
	DrawElement(19, 6, 348, 63, 0x000000FF, bars);
	// draw beatframes
	VLine(171, 54, 9, 0x000000FF);
	HLine(172, 54, 28, 0x000000FF);
	HLine(172, 62, 28, 0x000000FF);
	VLine(200, 54, 9, 0x000000FF);
	VLine(201, 57, 3, 0x000000FF);
	VLine(202, 57, 3, 0x000000FF);
	VLine(203, 57, 3, 0x000000FF);
	VLine(204, 54, 9, 0x000000FF);
	HLine(205, 54, 28, 0x000000FF);
	HLine(205, 62, 28, 0x000000FF);
	VLine(233, 54, 9, 0x000000FF);
	VLine(234, 57, 3, 0x000000FF);
	VLine(235, 57, 3, 0x000000FF);
	VLine(236, 57, 3, 0x000000FF);
	VLine(237, 54, 9, 0x000000FF);
	HLine(238, 54, 28, 0x000000FF);
	HLine(238, 62, 28, 0x000000FF);
	VLine(266, 54, 9, 0x000000FF);
	VLine(267, 57, 3, 0x000000FF);
	VLine(268, 57, 3, 0x000000FF);
	VLine(269, 57, 3, 0x000000FF);
	VLine(270, 54, 9, 0x000000FF);
	HLine(271, 54, 28, 0x000000FF);
	HLine(271, 62, 28, 0x000000FF);
	VLine(299, 54, 9, 0x000000FF);
	if(rekordbox.phase[beat] > 0) Rectangle(172 + (rekordbox.phase[beat] - 1) * 33, 55, 28, 7, 0x000000FF);
}

void VLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint32_t color)
{
	uint32_t Xaddress = 0;
    /* Get the line address */
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(hltdc.LayerCfg[ActiveLayer].ImageWidth*Ypos + Xpos);
    /* Write line */
    hdma2d.Init.Mode = DMA2D_R2M;
    /* RGB565 format */
    hdma2d.Init.ColorMode = DMA2D_RGB565;
    hdma2d.Init.OutputOffset = hltdc.LayerCfg[ActiveLayer].ImageWidth - 1;
    hdma2d.Instance = DMA2D;
    /* DMA2D Initialization */
    if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
    	if(HAL_DMA2D_ConfigLayer(&hdma2d, ActiveLayer) == HAL_OK) {
    		if (HAL_DMA2D_Start(&hdma2d, color, Xaddress, 1, Length) == HAL_OK) {
    			/* Polling For DMA transfer */
    			HAL_DMA2D_PollForTransfer(&hdma2d, 10);
    		}
    	}
    }
}

void HLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint32_t color)
{
	uint32_t Xaddress = 0;
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(hltdc.LayerCfg[ActiveLayer].ImageWidth*Ypos + Xpos);
	/* Write line */
	hdma2d.Init.Mode = DMA2D_R2M;
	/* RGB565 format */
	hdma2d.Init.ColorMode = DMA2D_RGB565;
	hdma2d.Init.OutputOffset = 0;
	hdma2d.Instance = DMA2D;
	/* DMA2D Initialization */
	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
	 	if(HAL_DMA2D_ConfigLayer(&hdma2d, ActiveLayer) == HAL_OK) {
	   		if (HAL_DMA2D_Start(&hdma2d, color, Xaddress, Length, 1) == HAL_OK) {
	   			/* Polling For DMA transfer */
	   			HAL_DMA2D_PollForTransfer(&hdma2d, 10);
	   		}
	   	}
	}
}

void Rectangle(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint32_t color)
{
	uint32_t xpos = hltdc.LayerCfg[ActiveLayer].FBStartAdress+2*(hltdc.LayerCfg[ActiveLayer].ImageWidth*Ypos+Xpos);
	hdma2d.Init.OutputOffset = hltdc.LayerCfg[ActiveLayer].ImageWidth - Xsize;
	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) {
		if (HAL_DMA2D_Start(&hdma2d, color, xpos, Xsize, Ysize) == HAL_OK) {
			HAL_DMA2D_PollForTransfer(&hdma2d, 100);
		}
	}

}

void Pixel(uint16_t Xpos, uint16_t Ypos, uint32_t color)
{
	*(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*hltdc.LayerCfg[ActiveLayer].ImageWidth + Xpos))) = Color_Convertion(color);
}

void DrawDigit(uint8_t digit, uint16_t Xpos, uint16_t Ypos, uint8_t width, uint32_t color)
{
	switch(digit) {
	case 0: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// e
		VLine(Xpos, Ypos + width, width - 2, color);
		VLine(Xpos + 1, Ypos + width + 1, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 1: {
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 2: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// e
		VLine(Xpos, Ypos + width, width - 2, color);
		VLine(Xpos + 1, Ypos + width + 1, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 3: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 4: {
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 5: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		break;
	}
	case 6: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// e
		VLine(Xpos, Ypos + width, width - 2, color);
		VLine(Xpos + 1, Ypos + width + 1, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		break;
	}
	case 7: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 8: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// e
		VLine(Xpos, Ypos + width, width - 2, color);
		VLine(Xpos + 1, Ypos + width + 1, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	case 9: {
		// a
		HLine(Xpos + 1, Ypos, width - 2, color);
		HLine(Xpos + 2, Ypos + 1, width - 4, color);
		// f
		VLine(Xpos, Ypos + 1, width - 2, color);
		VLine(Xpos + 1, Ypos + 2, width - 4, color);
		// d
		HLine(Xpos + 1, Ypos + (width * 2) - 2, width - 2, color);
		HLine(Xpos + 2, Ypos + (width * 2) - 3, width - 4, color);
		// g
		HLine(Xpos + 1, Ypos + width - 1, width - 2, color);
		HLine(Xpos + 2, Ypos + width - 2, width - 4, color);
		// c
		VLine(Xpos + width - 1, Ypos + width, width - 2, color);
		VLine(Xpos + width - 2, Ypos + width + 1, width - 4, color);
		// b
		VLine(Xpos + width - 1, Ypos + 1, width - 2, color);
		VLine(Xpos + width - 2, Ypos + 2, width - 4, color);
		break;
	}
	}
}

void DrawString(char* str, uint16_t Xpos, uint16_t Ypos, uint32_t color, uint32_t fontcolor)
{
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_RGB565;
	hdma2d.Init.OutputOffset = Xpos;
	hdma2d.Instance = DMA2D;
	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
	{
		if(HAL_DMA2D_ConfigLayer(&hdma2d, ActiveLayer) == HAL_OK)
		{
			if (HAL_DMA2D_Start(&hdma2d, fontcolor, hltdc.LayerCfg[ActiveLayer].FBStartAdress + 2*(480*(Ypos-20)+Xpos),
					480-Xpos, 20) == HAL_OK)
			{
				HAL_DMA2D_PollForTransfer(&hdma2d, 10);
			}
		}
	}
	int xpos = Xpos + 5;
	Ypos -= 4;
	uint16_t k = 0;
	int i = 0;
	int l = 0;
	while(str[l] != 0) {
		if((str[l] > 32) && (str[l] < 128)) {
			i = 0;
			do {
				k = 0;
				if(xpos < 470) {
					while(k < 13) {
						if(((font[(str[l]-33)*12+i] >> k) & 1) == 1) {
							*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((Ypos - k)
									* hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = color;
						}
						else {
							*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((Ypos - k)
									* hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = Color_Convertion(fontcolor);
						}
						k++;
					}
				}
				if(font[(str[l]-33)*12+i] == 0) {
					if(font[(str[l]-33)*12+i+1] == 0) {
						i++;
						break;
					}
					i++;
				}
				else {
					i++;
				}
			}
			while(i < 12);
			xpos += i;
		}
		if(str[l] > 191) {
			i = 0;
			do {
				k = 0;
				if(xpos < 470) {
					while(k < 13) {
						if(((ru_font[(str[l]-192)*12+i] >> k) & 1) == 1) {
							*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((Ypos - k)
									* hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = color;
						}
						else {
							*(__IO uint16_t*)(hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2 * ((Ypos - k)
									* hltdc.LayerCfg[ActiveLayer].ImageWidth + xpos + i))) = Color_Convertion(fontcolor);
						}
						k++;
					}
				}
				if(ru_font[(str[l]-192)*12+i] == 0) {
					if(ru_font[(str[l]-192)*12+i+1] == 0) {
						i++;
						break;
					}
					i++;
				}
				else {
					i++;
				}
			}
			while(i < 12);
			xpos += i;
		}
		if(str[l] == 32) {
			xpos += 5;
		}
		l++;
	}
}

// snaps file position on the spectra to the nearest beat
uint32_t QuantizePosition(int beat_offset) {
	uint32_t quantized_pos = file_pos_wide;
	int32_t time1 = file_pos_wide - rekordbox.timeindex[beat]*150/1000;
	int32_t time2 = rekordbox.timeindex[beat+1]*150/1000 - file_pos_wide;
	if(time1 > time2) {
		quantized_pos = rekordbox.timeindex[beat+beat_offset+1]*150/1000;							;
	}
	else {
		quantized_pos = rekordbox.timeindex[beat+beat_offset]*150/1000;
	}
	return quantized_pos;
}

// get loop length in the units of 1/32 beat
/*uint16_t GetLoopLength() {
	uint16_t loop_length = 0;
	uint32_t beatlength = (rekordbox.timeindex[beat+1] -
			rekordbox.timeindex[beat])*150/1000;
	display.looplength = display.loopend - display.loopstart;
	loop_length = (uint16_t)((float)display.looplength /
			(float)beatlength * 32) / 32;
	return loop_length * 32;
}*/
