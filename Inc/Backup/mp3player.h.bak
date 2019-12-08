/*
 * mp3player.h
 *
 *  Created on: 12 θών. 2018 γ.
 *      Author: teren
 */

#ifndef MP3PLAYER_H_
#define MP3PLAYER_H_

#include "stdint.h"
#include "ff.h"

typedef struct
{
	uint8_t MPEGVersion;
	uint8_t Layer;
	uint16_t BitRate;
	uint32_t AudioFrequency;
}Mp3HeaderTypeDef;


uint32_t Mp3ReadId3V2Text(FIL* pInFile, uint32_t unDataLen, char* pszBuffer, uint32_t unBufferSize);
uint32_t Mp3ReadId3V2Tag(FIL* pInFile, char* pszArtist, uint32_t unArtistSize, char* pszTitle, uint32_t unTitleSize);
uint8_t ReadMp3Header(Mp3HeaderTypeDef *mp3format);
void PlayMp3File();

#endif /* MP3PLAYER_H_ */
