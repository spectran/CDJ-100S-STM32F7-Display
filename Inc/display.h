/*
 * display.h
 *
 *  Created on: 30 ��� 2018 �.
 *      Author: teren
 */

typedef struct {
	uint8_t timemode;
	uint8_t pitchmode;
	uint8_t cuemode;
	uint8_t memorymode;
	uint8_t trackbarmode;
	uint8_t mastertempomode;
	uint32_t loopstart;
	uint32_t loopend;
	uint8_t loop;
	uint8_t quantize;
}DisplayTypeDef;

typedef struct {
	uint8_t state; // track state: playing, stopped, cue set...
	uint16_t pitch; // pitch position in range 0...32768
	float percent; // pitch changing in percent
	uint16_t bitrate; // current file original bit rate
}TrackTypeDef;

typedef enum {
	PLAYING,
	STOPPED,
	SETCUE
}TrackStateTypeDef;

void ClearLayer();
void ChangeLayers(void);
void DrawMenu();
void DrawLowSpectrum();
void SetBeatGrid();
void DrawElement(uint16_t width, uint16_t height, uint16_t xpos, uint16_t ypos, uint32_t color, const uint16_t* element);
void DrawSpectrum();
void VLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint32_t color);
void HLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint32_t color);
void Rectangle(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint32_t color);
void Pixel(uint16_t Xpos, uint16_t Ypos, uint32_t color);
void DrawDigit(uint8_t digit, uint16_t Xpos, uint16_t Ypos, uint8_t width, uint32_t color);
void DrawString(char* str, uint16_t Xpos, uint16_t Ypos, uint32_t color, uint32_t fontcolor);
uint32_t QuantizePosition(int beat_offset);
//uint16_t GetLoopLength();
