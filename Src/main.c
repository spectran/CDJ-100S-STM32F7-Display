/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "fatfs.h"
#include "i2c.h"
#include "ltdc.h"
#include "sai.h"
#include "sdmmc.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <wchar.h>
#include "rekordbox.h"
#include "display.h"
#include "mp3dec.h"
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery_ts.h"
#include "waveplayer.h"
#include "mp3player.h"
#include "ff.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t ActiveLayer = 0;
int Track_number = 0;
FATFS SDFatFs;  /* File system object for SD card logical drive */
FIL MyFile;     /* File object */
char SDPath[4]; /* SD card logical drive path */
FRESULT res;
DIR dir;
static FILINFO fno;
TCHAR new_path[255] = {0};
TCHAR old_path[255] = {0};
uint16_t bytesread = 0;
uint32_t file_pos = 0;
uint32_t file_pos_wide = 0;
extern volatile int32_t position;
uint8_t lowp_wavebuffer[400];
extern RekordboxTypeDef rekordbox;
extern DisplayTypeDef display;
uint16_t Total_tracks = 0;
char TrackTable[100][255];
char TrackPaths[100][255];
int8_t halffr = 0;
int16_t fr = 0;
int8_t sec = 0;
int8_t min = 0;
int16_t rfr = 0;
int8_t rsec = 0;
int8_t rmin = 0;
uint8_t tim7_flag = 0;
extern int8_t menu_mode;
extern int beat;
extern int bar;

uint8_t spi_tx[4] = {0x08, 0x90, 0xFF, 0x00};
uint8_t spi_rx[4] = {0};

TrackTypeDef trak;

HMP3Decoder	hMP3Decoder;

TS_StateTypeDef ts_State;

AUDIO_OUT_BufferTypeDef BufferCtl;

volatile UINT bOutOfData = 0;
volatile uint32_t unDmaBufMode = 0;
uint8_t volume = 60;
uint8_t acue_sensitivity = 30;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
FRESULT scan_files();
FRESULT find_file(uint16_t track_number);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  
  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_LTDC_Init();
  MX_FMC_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_DMA2D_Init();
  MX_SAI2_Init();
  MX_I2C3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_SPI2_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
  SDRAM_Init(); // MT48LC4M32B2B5-6A SDRAM initialization
  BSP_LCD_DisplayOff();
  HAL_LTDC_SetAddress(&hltdc, LCD_FB_START_ADDRESS_0, 0); // set layer 0 framebuffer address
  HAL_LTDC_SetAddress(&hltdc, LCD_FB_START_ADDRESS_1, 1); // set layer 1 framebuffer address
  ClearLayer(); // clear framebuffer 0
  HAL_LTDC_SetAlpha_NoReload(&hltdc, 0, ActiveLayer++);
  HAL_LTDC_SetAlpha_NoReload(&hltdc, 255, ActiveLayer--);
  ChangeLayers();
  ClearLayer(); // clear framebuffer 1
  HAL_TIM_Base_Start_IT(&htim4); // start display refresh timer
  if(BSP_SD_IsDetected() != SD_PRESENT) {
	  menu_mode = 3;
	  BSP_LCD_DisplayOn();
	  while(1);
  }
  BSP_TS_Init(480, 272); // touchscreen initialization
  BSP_TS_ITClear();
  BSP_TS_ITConfig();
  f_mount(&SDFatFs, (TCHAR const*)SDPath, 0); // SD card disk mount
  hMP3Decoder = MP3InitDecoder(); // mp3 decoder initialization
  scan_files(); // get total track number
  HAL_TIM_Base_Start_IT(&htim5); // start jog speed counting timer
  HAL_SPI_TransmitReceive_IT(&hspi2, spi_tx, spi_rx, 4);
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, volume, (uint32_t)(AUDIO_FREQUENCY_22K)*(1 + trak.percent));
  BSP_AUDIO_OUT_SetVolume(0);
  BSP_LCD_DisplayOn();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  find_file(Track_number);
	  fr = 0;
	  sec = 0;
	  min = 0;
	  rfr = 0;
	  rsec = 0;
	  rmin = 0;
	  file_pos_wide = 0;
	  position = 0;
	  rekordbox.state = 0;
	  display.loop = 0;
	  if(display.cuemode == 1) rekordbox.autocue = 1;
	  else rekordbox.autocue = 0;
	  rekordbox.currentcue = 0;
	  DecodeRekordboxFiles(new_path);
	  menu_mode = 0;
	  bar = 0;
	  beat = 0;
	  res = f_open(&MyFile, rekordbox.filename, FA_READ);
	  if(res == FR_OK)
	  {
		  trak.state = PLAYING;
		  spi_tx[2] &= ~(1 << 0);
		  if(rekordbox.autocue == 2) spi_tx[2] &= ~(1 << 1);
		  else spi_tx[2] |= (1 << 1);
		  spi_tx[2] &= ~(1 << 3);
		  if((strcmp(rekordbox.filetype, ".wav") == 0) || (strcmp(rekordbox.filetype, ".WAV") == 0)) {
			  PlayWavFile();
		  }
		  if((strcmp(rekordbox.filetype, ".mp3") == 0) || (strcmp(rekordbox.filetype, ".MP3") == 0)) {
			  PlayMp3File();
		  }
		  spi_tx[2] |= (1 << 3);
		  trak.state = STOPPED;
		  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
	  }
	  if(file_pos_wide >= rekordbox.spectrum_size) {
		  Track_number++;
	  }
	  file_pos_wide = 0;
	  menu_mode = 4;
	  f_close(&MyFile);
	  f_closedir(&dir);
	  if(Track_number >= Total_tracks) Track_number = 0;
	  if(Track_number < 0) Track_number = Total_tracks - 1;
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_SAI2
                              |RCC_PERIPHCLK_I2C3|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 100;
  PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLP_DIV2;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 240;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 6;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV6;
  PeriphClkInitStruct.PLLI2SDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
  PeriphClkInitStruct.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
FRESULT scan_files()
{
    DIR folder;
    UINT i = 0;
    UINT folders = 0;
    UINT subfolders = 0;
    TCHAR relative_path[255] = {0};
    while(1) {
    	res = f_opendir(&folder, L"/PIONEER/USBANLZ");	/* Open the directory */
    	i = 0;
    	do {
    		res = f_readdir(&folder, &fno);	/* Read a directory item */
    	}
    	while(i++ < folders);
    	if (res != FR_OK || fno.fname[0] == 0) {
    		f_closedir(&folder);
    		break;	/* Break on error or end of dir */
    	}
    	if (fno.fattrib & AM_DIR) {	/* It is a directory */
    		folders++;
    		wcscpy(new_path, L"/PIONEER/USBANLZ/");
    		wcscat(new_path, fno.fname);
    	  	for(i = 0; i < 255; i++) relative_path[i] = 0;
    		wcscpy(relative_path, fno.fname);
    		wcscpy(old_path, new_path);
    		f_closedir(&folder);
    		subfolders = 0;
    		while(1) {
    			res = f_opendir(&folder, old_path);
    			for(i = 0; i < 255; i++) new_path[i] = 0;
    			wcscpy(new_path, old_path);
    			i = 0;
    			do {
    				res = f_readdir(&folder, &fno);	/* Read a directory item */
    			}
    			while(i++ < subfolders);
    			if (res != FR_OK || fno.fname[0] == 0) {
    				f_closedir(&folder);
    				break;  /* Break on error or end of dir */
    			}
    			if (fno.fattrib & AM_DIR) {	/* It is a directory */
    				subfolders++;
    				wcscat(new_path, L"/");
    				wcscat(new_path, fno.fname);
    				wcscat(relative_path, L"/");
    				wcscat(relative_path, fno.fname);
    				f_closedir(&folder);
    				res = f_opendir(&folder, new_path);
    				if(res == FR_OK) {
    					res = f_readdir(&folder, &fno);
    					if (!(fno.fattrib & AM_DIR)) {
    						for(i=0; i<255; i++) TrackPaths[Total_tracks][i] = ff_convert(new_path[i], 0);
    						wcscat(new_path, L"/");
    						wcscat(new_path, fno.fname);
    						GetFileName(new_path);
    						for(i = 0; i < 255; i++) TrackTable[Total_tracks][i] = rekordbox.file[i];
    						Total_tracks++;
    						f_closedir(&folder);
    					}
    				}
    			}
    		}
    	}
    }
    return res;
}

FRESULT find_file(uint16_t track_number)
{
    DIR folder;
    TCHAR path[255] = {0};
    for(int i=0; i<255; i++) path[i] = ff_convert(TrackPaths[Track_number][i], 1);
    wcscpy(new_path, path);
	res = f_opendir(&folder, new_path);
	if(res == FR_OK) {
		res = f_readdir(&folder, &fno);
		if (!(fno.fattrib & AM_DIR)) {
		}
	}
    return res;
}

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{

  /* Disables the MPU */
  HAL_MPU_Disable();
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
