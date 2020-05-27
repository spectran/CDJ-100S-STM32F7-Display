/*
 * mp3player.c
 *
 *  Created on: 12 ���. 2018 �.
 *      Author: teren
 */

#include "mp3player.h"
#include "rekordbox.h"
#include "stdint.h"
#include "string.h"
#include "ff.h"
#include "mp3dec.h"
#include "stm32746g_discovery_audio.h"
#include "waveplayer.h"
#include "display.h"
#include "tim.h"
#include "main.h"

extern FIL MyFile;
extern AUDIO_OUT_BufferTypeDef BufferCtl;
extern RekordboxTypeDef rekordbox;
extern DisplayTypeDef display;
extern volatile UINT bOutOfData;
extern volatile uint32_t unDmaBufMode;
extern HMP3Decoder hMP3Decoder;
extern TrackTypeDef trak;

uint32_t id3tagsize;

Mp3HeaderTypeDef mp3header;

MP3FrameInfo mp3FrameInfo;

uint16_t g_pMp3OutBuffer[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP]; // output samples from decoder
uint16_t* g_pMp3OutBufferPtr = NULL;
uint16_t g_pMp3DmaBuffer[MP3_DMA_BUFFER_SIZE]; // samples to be send to audio codec
uint16_t* g_pMp3DmaBufferPtr = NULL;
int unInDataLeft = 0;
uint32_t unDmaBufferSpace = 0;
UINT unFramesDecoded = 0;
int nDecodeRes = ERR_MP3_NONE;
uint8_t *pInData = BufferCtl.buff;

extern uint8_t volume;

/*
 * Taken from
 * http://www.mikrocontroller.net/topic/252319
 */
uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize)
{
	UINT unRead = 0;
	BYTE byEncoding = 0;
	if((f_read(pInFile, &byEncoding, 1, &unRead) == FR_OK) && (unRead == 1))
	{
		unDataLen--;
		if(unDataLen <= (unBufferSize - 1))
		{
			if((f_read(pInFile, pszBuffer, unDataLen, &unRead) == FR_OK) ||
					(unRead == unDataLen))
			{
				if(byEncoding == 0)
				{
					// ISO-8859-1 multibyte
					// just add a terminating zero
					pszBuffer[unDataLen] = 0;
				}
				else if(byEncoding == 1)
				{
					// UTF16LE unicode
					uint32_t r = 0;
					uint32_t w = 0;
					if((unDataLen > 2) && (pszBuffer[0] == 0xFF) && (pszBuffer[1] == 0xFE))
					{
						// ignore BOM, assume LE
						r = 2;
					}
					for(; r < unDataLen; r += 2, w += 1)
					{
						// should be acceptable for 7 bit ascii
						pszBuffer[w] = pszBuffer[r];
					}
					pszBuffer[w] = 0;
				}
			}
			else
			{
				return 1;
			}
		}
		else
		{
			// we won't read a partial text
			if(f_lseek(pInFile, f_tell(pInFile) + unDataLen) != FR_OK)
			{
				return 1;
			}
		}
	}
	else
	{
		return 1;
	}
	return 0;
}

/*
 * Taken from
 * http://www.mikrocontroller.net/topic/252319
 */
uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize)
{
	pszArtist[0] = 0;
	pszTitle[0] = 0;

	BYTE id3hd[10];
	UINT unRead = 0;
	// read ID3 tag header (10 bytes)
	if((f_read(pInFile, id3hd, 10, &unRead) != FR_OK) || (unRead != 10))
	{
		return 1;
	}
	else
	{
		uint32_t unSkip = 0;
		id3tagsize = 0;
		if((unRead == 10) &&
				(id3hd[0] == 'I') &&
				(id3hd[1] == 'D') &&
				(id3hd[2] == '3'))
		{
			// include ID3 header size (10 bytes)
			unSkip += 10;
			// ID3 tag full size
			unSkip = ((id3hd[6] & 0x7f) << 21) | ((id3hd[7] & 0x7f) << 14) | ((id3hd[8] & 0x7f) << 7) | (id3hd[9] & 0x7f);
			id3tagsize = unSkip;
			// try to get some information from the tag
			// skip the extended header, if present
			uint8_t unVersion = id3hd[3];
			if(id3hd[5] & 0x40)
			{
				BYTE exhd[4];
				f_read(pInFile, exhd, 4, &unRead);
				size_t unExHdrSkip = ((exhd[0] & 0x7f) << 21) | ((exhd[1] & 0x7f) << 14) | ((exhd[2] & 0x7f) << 7) | (exhd[3] & 0x7f);
				unExHdrSkip -= 4;
				if(f_lseek(pInFile, f_tell(pInFile) + unExHdrSkip) != FR_OK)
				{
					return 1;
				}
			}
			uint32_t nFramesToRead = 2;
			while(nFramesToRead > 0)
			{
				char frhd[10];
				if((f_read(pInFile, frhd, 10, &unRead) != FR_OK) || (unRead != 10))
				{
					return 1;
				}
				if((frhd[0] == 0) || (strncmp(frhd, "3DI", 3) == 0))
				{
					break;
				}
				char szFrameId[5] = {0, 0, 0, 0, 0};
				memcpy(szFrameId, frhd, 4);
				uint32_t unFrameSize = 0;
				uint32_t i = 0;
				for(; i < 4; i++)
				{
					if(unVersion == 3)
					{
						// ID3v2.3
						unFrameSize <<= 8;
						unFrameSize += frhd[i + 4];
					}
					if(unVersion == 4)
					{
						// ID3v2.4
						unFrameSize <<= 7;
						unFrameSize += frhd[i + 4] & 0x7F;
					}
				}

				if(strcmp(szFrameId, "TPE1") == 0)
				{
					// artist
					if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszArtist, unArtistSize) != 0)
					{
						break;
					}
					nFramesToRead--;
				}
				else if(strcmp(szFrameId, "TIT2") == 0)
				{
					// title
					if(Mp3ReadId3V2Text(pInFile, unFrameSize, pszTitle, unTitleSize) != 0)
					{
						break;
					}
					nFramesToRead--;
				}
				else
				{
					if(f_lseek(pInFile, f_tell(pInFile) + unFrameSize) != FR_OK)
					{
						return 1;
					}
				}
			}
		}
		if(f_lseek(pInFile, unSkip) != FR_OK)
		{
			return 1;
		}
	}

	return 0;
}

uint8_t ReadMp3Header(Mp3HeaderTypeDef *mp3format) {
	uint32_t bytesread;
	uint16_t readbuffer;
	FRESULT res;
	do {
		while(f_read(&MyFile, &readbuffer, 2, (void *)&bytesread) != FR_OK);
		if((readbuffer & 0xFFF0) == 0xFFF0) break;
		f_lseek(&MyFile, -1);
	}
	while(f_eof(&MyFile) == 0);
	if(f_eof(&MyFile) == 1) return 1;
	mp3format->MPEGVersion = readbuffer & 0x0008;
	mp3format->Layer = readbuffer & 0x0006;
	res = f_read(&MyFile, &readbuffer, 2, (void *)&bytesread);
	if(mp3format->MPEGVersion == 0x08) {
		if(mp3format->Layer == 0x02) {
			switch(readbuffer & 0xF000) {
			case 0x1000: {
				mp3format->BitRate = 32;
				break;
			}
			case 0x2000: {
				mp3format->BitRate = 40;
				break;
			}
			case 0x3000: {
				mp3format->BitRate = 48;
				break;
			}
			case 0x4000: {
				mp3format->BitRate = 56;
				break;
			}
			case 0x5000: {
				mp3format->BitRate = 64;
				break;
			}
			case 0x6000: {
				mp3format->BitRate = 80;
				break;
			}
			case 0x7000: {
				mp3format->BitRate = 96;
				break;
			}
			case 0x8000: {
				mp3format->BitRate = 112;
				break;
			}
			case 0x9000: {
				mp3format->BitRate = 128;
				break;
			}
			case 0xA000: {
				mp3format->BitRate = 160;
				break;
			}
			case 0xB000: {
				mp3format->BitRate = 192;
				break;
			}
			case 0xC000: {
				mp3format->BitRate = 224;
				break;
			}
			case 0xD000: {
				mp3format->BitRate = 256;
				break;
			}
			case 0xE000: {
				mp3format->BitRate = 320;
				break;
			}
			default: break;
			}
			switch(readbuffer & 0x0C00) {
			case 0x0000: {
				mp3format->AudioFrequency = 44100;
				break;
			}
			case 0x0400: {
				mp3format->AudioFrequency = 48000;
				break;
			}
			case 0x0C00: {
				mp3format->AudioFrequency = 32000;
				break;
			}
			default: break;
			}
		}
	}
	return res;
}

void PlayMp3File() {
	BufferCtl.filetype = 1;
	char szArtist[120];
	char szTitle[120];
	id3tagsize = 0;
	Mp3ReadId3V2Tag(&MyFile, szArtist, sizeof(szArtist), szTitle, sizeof(szTitle));
	bOutOfData = 0;
	memset(BufferCtl.buff, 0, AUDIO_OUT_BUFFER_SIZE);
	//memset(g_pMp3DmaBuffer, 0, MP3_DMA_BUFFER_SIZE);
	g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;
	unInDataLeft = 0;
	unDmaBufferSpace = 0;
	unFramesDecoded = 0;
	nDecodeRes = ERR_MP3_INDATA_UNDERFLOW;
	unDmaBufMode = 0;
	do {
		// fill the whole buffer for the first time
		if(unInDataLeft < (2 * MAINBUF_SIZE)) {
			UINT unRead = Mp3FillReadBuffer(pInData, unInDataLeft, &MyFile);
			unInDataLeft += unRead;
			pInData = BufferCtl.buff;
		}
		// find start of next MP3 frame - assume EOF if no sync found
		int nOffset = MP3FindSyncWord(pInData, unInDataLeft);
		if(nOffset < 0) {
			unInDataLeft = 0;
			//bOutOfData = 1;
			//break;
		}
		else {
			pInData += nOffset;
			unInDataLeft -= nOffset;
			// decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame
			nDecodeRes = MP3Decode(hMP3Decoder, &pInData, (int*)&unInDataLeft, (short*)g_pMp3OutBuffer, 0);
			switch(nDecodeRes) {
			case ERR_MP3_NONE:
			{
				MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
				unFramesDecoded++;
				g_pMp3OutBufferPtr = g_pMp3OutBuffer;
				uint32_t unOutBufferAvail = mp3FrameInfo.outputSamps;
				while(unOutBufferAvail > 0) {
					// fill up the whole dma buffer
					uint32_t unDmaBufferSpace = 0;
					if(unDmaBufMode == 0) {
						// fill the whole buffer
						// dma buf ptr was reset to beginning of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}
					else if(unDmaBufMode == 1) {
						// fill the first half of the buffer
						// dma buf ptr was reset to beginning of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2) - g_pMp3DmaBufferPtr;
					}
					else {
						// fill the last half of the buffer
						// dma buf ptr was reset to middle of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}
					uint32_t unCopy = unDmaBufferSpace > unOutBufferAvail ? unOutBufferAvail : unDmaBufferSpace;
					if(unCopy > 0) {
						memcpy(g_pMp3DmaBufferPtr, g_pMp3OutBufferPtr, unCopy * sizeof(uint16_t));
						unOutBufferAvail -= unCopy;
						g_pMp3OutBufferPtr += unCopy;
						unDmaBufferSpace -= unCopy;
						g_pMp3DmaBufferPtr += unCopy;
					}
					if(unDmaBufferSpace == 0) {
						// dma buffer full
						// see if this was the first run
						if(unDmaBufMode == 0) {
							// on the first buffer fill up,
							// start the dma transfer
							if(mp3FrameInfo.samprate > 0) trak.bitrate = mp3FrameInfo.samprate;
							else trak.bitrate = AUDIO_FREQUENCY_44K;
							BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, (uint32_t)(trak.bitrate / 2)
									*(1 + trak.percent), NULL);
							BSP_AUDIO_OUT_Play(g_pMp3DmaBuffer, MP3_DMA_BUFFER_SIZE * sizeof(uint16_t));
							BSP_AUDIO_OUT_SetVolume(volume);
							if((rekordbox.autocue == 2) && (trak.state == PLAYING)) {
								BSP_AUDIO_OUT_Pause();
								trak.state = STOPPED;
								HAL_TIM_Base_Start_IT(&htim8);
								GoToPosition(rekordbox.cue_start_position[0]);
								TrackTime();
							}
						}
						// we must wait for the dma stream tx interrupt here
						while(unDmaBufMode == 0);
					}
				}
				break;
			}
			case ERR_MP3_MAINDATA_UNDERFLOW:
			{
				// do nothing - next call to decode will provide more mainData
				break;
			}
			case ERR_MP3_INDATA_UNDERFLOW:
			{
				if(nOffset == 0)
				// something's really wrong here, frame had to fit
					bOutOfData = 1;
				else {
					unInDataLeft = 0;
				}
				break;
			}
			default:
			{
				// just try to skip the offending frame...
				if(unInDataLeft > 0) {
					unInDataLeft-=MP3_BYTES_SKIP;
					pInData+=MP3_BYTES_SKIP;
				}
				break;
			}
			}
		}
	}
	while(!bOutOfData);
	BSP_AUDIO_OUT_SetVolume(0);
	while(unDmaBufMode < 3);
}

// after an error, try to restart MP3 decoding
void RereadMp3File()
{
	bOutOfData = 0;
	memset(BufferCtl.buff, 0, AUDIO_OUT_BUFFER_SIZE);
	g_pMp3DmaBufferPtr = g_pMp3DmaBuffer;
	unInDataLeft = 0;
	unDmaBufferSpace = 0;
	unFramesDecoded = 0;
	nDecodeRes = ERR_MP3_INDATA_UNDERFLOW;
	unDmaBufMode = 0;
	do {
		// fill the whole buffer for the first time
		if(unInDataLeft < (2 * MAINBUF_SIZE)) {
			UINT unRead = Mp3FillReadBuffer(pInData, unInDataLeft, &MyFile);
			unInDataLeft += unRead;
			pInData = BufferCtl.buff;
		}
		// find start of next MP3 frame - assume EOF if no sync found
		int nOffset = MP3FindSyncWord(pInData, unInDataLeft);
		if(nOffset < 0) {
			unInDataLeft = 0;
			//bOutOfData = 1;
			//break;
		}
		else {
			pInData += nOffset;
			unInDataLeft -= nOffset;
			// decode one MP3 frame - if offset < 0 then bytesLeft was less than a full frame
			nDecodeRes = MP3Decode(hMP3Decoder, &pInData, (int*)&unInDataLeft, (short*)g_pMp3OutBuffer, 0);
			switch(nDecodeRes) {
			case ERR_MP3_NONE:
			{
				MP3GetLastFrameInfo(hMP3Decoder, &mp3FrameInfo);
				unFramesDecoded++;
				g_pMp3OutBufferPtr = g_pMp3OutBuffer;
				uint32_t unOutBufferAvail = mp3FrameInfo.outputSamps;
				while(unOutBufferAvail > 0) {
					// fill up the whole dma buffer
					uint32_t unDmaBufferSpace = 0;
					if(unDmaBufMode == 0) {
						// fill the whole buffer
						// dma buf ptr was reset to beginning of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}
					else if(unDmaBufMode == 1) {
						// fill the first half of the buffer
						// dma buf ptr was reset to beginning of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + (MP3_DMA_BUFFER_SIZE / 2) - g_pMp3DmaBufferPtr;
					}
					else {
						// fill the last half of the buffer
						// dma buf ptr was reset to middle of the buffer
						unDmaBufferSpace = g_pMp3DmaBuffer + MP3_DMA_BUFFER_SIZE - g_pMp3DmaBufferPtr;
					}
					uint32_t unCopy = unDmaBufferSpace > unOutBufferAvail ? unOutBufferAvail : unDmaBufferSpace;
					if(unCopy > 0) {
						memcpy(g_pMp3DmaBufferPtr, g_pMp3OutBufferPtr, unCopy * sizeof(uint16_t));
						unOutBufferAvail -= unCopy;
						g_pMp3OutBufferPtr += unCopy;
						unDmaBufferSpace -= unCopy;
						g_pMp3DmaBufferPtr += unCopy;
					}
					if(unDmaBufferSpace == 0) {
						// dma buffer full
						// see if this was the first run
						if(unDmaBufMode == 0) {
							// on the first buffer fill up,
							// start the dma transfer
							if(mp3FrameInfo.samprate > 0) trak.bitrate = mp3FrameInfo.samprate;
							else trak.bitrate = AUDIO_FREQUENCY_44K;
							BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, (uint32_t)(trak.bitrate / 2)
									*(1 + trak.percent), NULL);
							BSP_AUDIO_OUT_Play(g_pMp3DmaBuffer, MP3_DMA_BUFFER_SIZE * sizeof(uint16_t));
							BSP_AUDIO_OUT_SetVolume(volume);
							if((rekordbox.autocue == 2) && (trak.state == PLAYING)) {
								BSP_AUDIO_OUT_Pause();
								trak.state = STOPPED;
								HAL_TIM_Base_Start_IT(&htim8);
								GoToPosition(rekordbox.cue_start_position[0]);
								TrackTime();
							}
						}
						// we must wait for the dma stream tx interrupt here
						while(unDmaBufMode == 0);
					}
				}
				break;
			}
			case ERR_MP3_MAINDATA_UNDERFLOW:
			{
				// do nothing - next call to decode will provide more mainData
				// just need another frame
				unInDataLeft += MP3_BYTES_SKIP;
				break;
			}
			case ERR_MP3_INDATA_UNDERFLOW:
			{
				//if(nOffset == 0)
				// something's really wrong here, frame had to fit
				//bOutOfData = 1;
				//else {
				unInDataLeft = 0;
				//}
				break;
			}
			default:
			{
				// just try to skip the offending frame...
				if(unInDataLeft > 0) {
					unInDataLeft--;
					pInData++;
				}
				break;
			}
			}
		}
	}
	while(!bOutOfData);
	BSP_AUDIO_OUT_SetVolume(0);
	while(unDmaBufMode < 3);
}
