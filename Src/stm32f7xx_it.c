/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
  
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include "display.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_audio.h"
#include "ff.h"
#include "mp3player.h"
#include "waveplayer.h"
#include "rekordbox.h"
#include "spi.h"

extern UINT bOutOfData;
extern UINT unInDataLeft;
extern uint32_t unDmaBufMode;
extern TS_StateTypeDef ts_State;
extern AUDIO_OUT_BufferTypeDef  BufferCtl;
extern FIL MyFile;     /* File object */
extern DisplayTypeDef display;
extern RekordboxTypeDef rekordbox;
extern SPI_HandleTypeDef hspi2;
extern WavHeaderTypeDef wavfile;

uint8_t flag = 0;
uint8_t touch_count = 0;
extern int8_t menu_mode;
float touch_wide = 0;
uint16_t prev_x = 0;
int str_offset = 0;
int String_number = -1;
int str_increment = 0;
extern float stretch;
extern uint16_t Total_tracks;
extern int Track_number;
extern uint32_t file_pos_wide;
extern uint32_t file_pos;
int Mark_number = -1;
uint16_t button_flag = 0;
extern int beat;

extern TrackTypeDef trak;

extern uint8_t spi_rx[4];
extern uint8_t spi_tx[4];

extern uint32_t wavtagsize;
extern uint32_t id3tagsize;
extern uint8_t volume;
extern uint8_t acue_sensitivity;

float scale_pitch = 1;
int pulses = 0; // jog pulses counter
int old_pulses = 0;
float jog_sensitivity = 0.003; // jog sensitivity; must be 0.005 ... 0.01

volatile uint32_t New_frequency = AUDIO_FREQUENCY_22K;

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_sai2_a;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim9;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */
	HAL_SPI_TransmitReceive_IT(&hspi2, spi_tx, spi_rx, 4);
  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */

  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_IRQn 1 */

  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles TIM1 break interrupt and TIM9 global interrupt.
  */
void TIM1_BRK_TIM9_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_BRK_TIM9_IRQn 0 */
	if(button_flag & (1 << 0)) {
		if(display.cuemode == 0) display.cuemode = 1;
		else display.cuemode = 0;
		button_flag ^= (1 << 0);
	}
	else if(button_flag & (1 << 1)) {
		// turn on Master Tempo
		button_flag ^= (1 << 1);
	}
	else if(button_flag & (1 << 3)) {
		if(display.quantize == 0) display.quantize = 1;
		else display.quantize = 0;
		button_flag ^= (1 << 3);
	}
	HAL_TIM_Base_Stop_IT(&htim9);
  /* USER CODE END TIM1_BRK_TIM9_IRQn 0 */
  HAL_TIM_IRQHandler(&htim9);
  /* USER CODE BEGIN TIM1_BRK_TIM9_IRQn 1 */

  /* USER CODE END TIM1_BRK_TIM9_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
	ClearLayer();
	SetBeatGrid();
	if(menu_mode == 0) DrawSpectrum();
	if(menu_mode != 3) 	DrawLowSpectrum();
	DrawMenu();
	//DrawFrequencySpectrum();
	ChangeLayers();
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles SPI2 global interrupt.
  */
void SPI2_IRQHandler(void)
{
  /* USER CODE BEGIN SPI2_IRQn 0 */

  /* USER CODE END SPI2_IRQn 0 */
  HAL_SPI_IRQHandler(&hspi2);
  /* USER CODE BEGIN SPI2_IRQn 1 */

  /* USER CODE END SPI2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
	BSP_TS_GetState(&ts_State);
	if(ts_State.touchDetected == 1) {
		if((ts_State.touchX[0] > 40) && (ts_State.touchX[0] < 440) && (ts_State.touchY[0] > 220) && (ts_State.touchY[0] < 260)) {
			touch_count = 0;
			GoToPosition(((float)ts_State.touchX[0] - 40) * (float)rekordbox.spectrum_size / rekordbox.lowp_spectrum_size);
			if(trak.state == PLAYING) BSP_AUDIO_OUT_Resume();
			else {
				GetTrackTime();
				if(spi_tx[2] & (1 << 0)) spi_tx[2] |= (1 << 1);
				else spi_tx[2] &= ~(1 << 1);
				trak.state = SETCUE;
			}
		}
		else if((ts_State.touchY[0] > 20) && (ts_State.touchY[0] < 165)) {
			if(menu_mode == 0) {
				if((ts_State.touchY[0] > 75) && (ts_State.touchY[0] < 165)) {
					HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
					__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
					__HAL_TIM_SET_COUNTER(&htim6, 0);
					HAL_TIM_Base_Start_IT(&htim6);
					touch_count = 0;
					if(ts_State.touchX[0] > 280) {
						if(stretch > 1) stretch--;
						else stretch /= 2;
						if(stretch < 0.1) stretch = 0.125;
					}
					else if(ts_State.touchX[0] < 200) {
						if(stretch >= 1) stretch++;
						else stretch *= 2;
						if(stretch > 4) stretch = 4;
					}
					else stretch = 1;
				}
			}
			if(menu_mode == 1) {
				if(touch_count == 0) {
					str_offset = 0;
					prev_x = ts_State.touchX[0];
					String_number = ts_State.touchY[0] / 20 - 1;
					Mark_number = String_number;
					__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
					__HAL_TIM_SET_COUNTER(&htim6, 0);
					HAL_TIM_Base_Start_IT(&htim6);
				}
				else {
					__HAL_TIM_SET_COUNTER(&htim6, 0);
					str_offset = ts_State.touchX[0] - prev_x;
				}
				touch_count++;
				if((String_number != ts_State.touchY[0] / 20 - 1)) {
					if(Total_tracks > 7) {
						if(String_number > (ts_State.touchY[0] / 20 - 1)) {
							str_increment += abs((ts_State.touchY[0] / 20 - 1) - String_number);
							if(str_increment > Total_tracks - 7) str_increment = Total_tracks - 7;
						}
						else {
							str_increment -= abs((ts_State.touchY[0] / 20 - 1) - String_number);
							if(str_increment < 0) str_increment = 0;
						}
					}
					str_offset = 0;
					touch_count = 0;
					String_number = -1;
				}
				if(ts_State.touchX[0] > 380) {
					str_offset = 0;
					touch_count = 0;
					bOutOfData = 1;
					unDmaBufMode = 3;
					Track_number = String_number + str_increment;
					if(Track_number >= Total_tracks) Track_number = 0;
					String_number = -1;
				}
			}
			if(menu_mode == 2) {
				touch_count = 0;
				if((ts_State.touchX[0] >= 40) && (ts_State.touchX[0] <= 440)) {
					if((ts_State.touchY[0] > 40) && (ts_State.touchY[0] < 60))	{
						volume = 100 * (float)(ts_State.touchX[0]-40) / 400;
						BSP_AUDIO_OUT_SetVolume(volume);
					}
					if((ts_State.touchY[0] > 80) && (ts_State.touchY[0] < 100)) {
						jog_sensitivity = 0.01 * (float)(ts_State.touchX[0]-40) / 400;
					}
					if((ts_State.touchY[0] > 120) && (ts_State.touchY[0] < 140)) {
						acue_sensitivity = 250 * (float)(ts_State.touchX[0]-40) / 400;
					}
				}
			}
		}
		else if((ts_State.touchY[0] > 0) && (ts_State.touchY[0] < 20)) {
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim6, 0);
			HAL_TIM_Base_Start_IT(&htim6);
			touch_count = 0;
			if(ts_State.touchX[0] < 200) {
				menu_mode--;
				if(menu_mode < 0) menu_mode = 2;
			}
			else if(ts_State.touchX[0] > 280) {
				menu_mode++;
				if(menu_mode > 2) menu_mode = 0;
			}
		}
		else if((ts_State.touchX[0] > 80) && (ts_State.touchX[0] < 280) && (ts_State.touchY[0] > 180) && (ts_State.touchY[0] < 210)) {
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim6, 0);
			HAL_TIM_Base_Start_IT(&htim6);
			touch_count = 0;
			if(display.timemode == 0) {
				display.timemode = 1;
				display.trackbarmode = 1;
			}
			else {
				display.timemode = 0;
				display.trackbarmode = 0;
			}
		}
		else {
			touch_count = 0;
		}
	}
	/*if((ts_State.touchDetected == 2) && (menu_mode == 0)) {
		if((ts_State.touchY[0] > 75) && (ts_State.touchY[0] < 165)) {
			if(touch_count == 0) {
				touch_wide = abs(ts_State.touchX[0] - ts_State.touchX[1]);
			}
			else {
				stretch = touch_wide/abs(ts_State.touchX[0] - ts_State.touchX[1]);
				if(stretch < 0.0001) stretch = 0.0001;
				if(stretch > 4) stretch = 4;
			}
			touch_count++;
		}
	}*/
	BSP_TS_ResetTouchData(&ts_State);
	BSP_TS_ITClear();
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles TIM8 update interrupt and TIM13 global interrupt.
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 0 */
	if(trak.state == STOPPED) {
		spi_tx[2] ^= (1 << 0);
	}
	if(trak.state == SETCUE) {
		spi_tx[2] ^= (1 << 0);
		spi_tx[2] ^= (1 << 1);
	}
  /* USER CODE END TIM8_UP_TIM13_IRQn 0 */
  HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_UP_TIM13_IRQn 1 */

  /* USER CODE END TIM8_UP_TIM13_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
	if(pulses != old_pulses) {
		if(pulses != 0) {
			New_frequency = (uint32_t)((trak.bitrate / 2)
					*(1 + trak.percent + jog_sensitivity*pulses));
		}
		else {
			New_frequency = (uint32_t)((trak.bitrate / 2)*(1 + trak.percent));
		}
		BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, New_frequency, NULL);
		old_pulses = pulses;
	}
	pulses = 0;
  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_TIM_Base_Stop_IT(&htim6);
	touch_count = 0;

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	if(display.trackbarmode == 0) display.trackbarmode = 1;
	else display.trackbarmode = 0;
  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream4 global interrupt.
  */
void DMA2_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream4_IRQn 0 */

  /* USER CODE END DMA2_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sai2_a);
  /* USER CODE BEGIN DMA2_Stream4_IRQn 1 */

  /* USER CODE END DMA2_Stream4_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	switch(spi_rx[1] & 0xF0) {
	case 0x90: {
		switch(spi_rx[2]) {
		case JET: {
			if(display.quantize == 1) {
				display.loopstart = QuantizePosition(0);
			}
			else {
				display.loopstart = file_pos_wide;
			}
			break;
		}
		case JOG: {
			if(menu_mode == 1) {
				bOutOfData = 1;
				unDmaBufMode = 3;
				HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
				__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
				__HAL_TIM_SET_COUNTER(&htim6, 0);
				HAL_TIM_Base_Start_IT(&htim6);
				Track_number = Mark_number + str_increment;
				if(Track_number >= Total_tracks) Track_number = 0;
				String_number = -1;
			}
			break;
		}
		case ZIP: {
			if(display.quantize == 1) {
				display.loopend = QuantizePosition(0);
			}
			else {
				display.loopend = file_pos_wide;
			}
			if(display.loop == 0) display.loop = 1;
			GoToPosition(display.loopstart);
			BSP_AUDIO_OUT_Resume();
			break;
		}
		case WAH: {
			button_flag ^= (1 << 2);
			__HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim9, 0);
			HAL_TIM_Base_Start_IT(&htim9);
			break;
		}
		case HOLD: {
			button_flag ^= (1 << 3);
			__HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim9, 0);
			HAL_TIM_Base_Start_IT(&htim9);
			break;
		}
		case TIME: {
			button_flag ^= (1 << 0);
			__HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim9, 0);
			HAL_TIM_Base_Start_IT(&htim9);
			break;
		}
		case MASTERTEMPO: {
			button_flag ^= (1 << 1);
			__HAL_TIM_CLEAR_IT(&htim9, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim9, 0);
			HAL_TIM_Base_Start_IT(&htim9);
			break;
		}
		case TRACKBACK: {
			if(!(button_flag & (1 << 2))) {
				bOutOfData = 1;
				unDmaBufMode = 3;
				Track_number--;
			}
			else {
				display.loopend = (display.loopend+display.loopstart)/2;
				if(display.loopend - display.loopstart < 1)
					display.loopend = display.loopstart + 1;
				if(file_pos_wide >= display.loopend) {
					GoToPosition(display.loopstart);
					BSP_AUDIO_OUT_Resume();
				}
			}
			break;
		}
		case TRACKFORWARD: {
			if(!(button_flag & (1 << 2))) {
				bOutOfData = 1;
				unDmaBufMode = 3;
				Track_number++;
			}
			else {
				display.loopend = display.loopend*2-display.loopstart;
				if(display.loopend >= rekordbox.spectrum_size)
					display.loopend = rekordbox.spectrum_size - 1;
			}
			break;
		}
		case SCANBACK: {
			if(!(button_flag & (1 << 2))) {
				if(display.quantize == 1) {
					GoToPosition(QuantizePosition(-1));
				}
				else {
					GoToPosition((float)(f_tell(&MyFile) - 120000)
							/(float)(f_size(&MyFile)-id3tagsize-wavtagsize)
							*rekordbox.spectrum_size);
				}
				if(trak.state == PLAYING) BSP_AUDIO_OUT_Resume();
				else {
					GetTrackTime();
					if(!spi_tx[2] & (1 << 0)) spi_tx[2] |= (1 << 1);
					else spi_tx[2] &= ~(1 << 1);
					trak.state = SETCUE;
				}
			}
			else {
				rekordbox.currentcue++;
				if(rekordbox.currentcue >= rekordbox.cues)
					rekordbox.currentcue = 0;
				GoToPosition(rekordbox.cue_start_position[rekordbox.currentcue]);
				if(trak.state == PLAYING) {
					BSP_AUDIO_OUT_Resume();
					HAL_TIM_Base_Stop_IT(&htim8); //stop blinking
					spi_tx[2] &= ~(1 << 0); //turn on PLAY led
				}
				else {
					GetTrackTime();
				}
				// if CUE is set
				if(rekordbox.cue_start_position[rekordbox.currentcue] != 0) {
					spi_tx[2] &= ~(1 << 1); //turn on CUE led
				}
				else {
					spi_tx[2] |= (1 << 1); //turn off CUE led
				}
			}
			break;
		}
		case SCANFORWARD: {
			if(!(button_flag & (1 << 2))) {
				if(display.quantize == 1) {
					GoToPosition(QuantizePosition(1));
				}
				else {
					GoToPosition((float)(f_tell(&MyFile) + 60000)
							/(float)(f_size(&MyFile)-id3tagsize-wavtagsize)
							*rekordbox.spectrum_size);
				}
				if(trak.state == PLAYING) BSP_AUDIO_OUT_Resume();
				else {
					GetTrackTime();
					if(!spi_tx[2] & (1 << 0)) spi_tx[2] |= (1 << 1);
					else spi_tx[2] &= ~(1 << 1);
					trak.state = SETCUE;
				}
			}
			else {
				rekordbox.currentcue--;
				if(rekordbox.currentcue < 0)
					rekordbox.currentcue = rekordbox.cues - 1;
				GoToPosition(rekordbox.cue_start_position[rekordbox.currentcue]);
				if(trak.state == PLAYING) {
					BSP_AUDIO_OUT_Resume();
					HAL_TIM_Base_Stop_IT(&htim8); //stop blinking
					spi_tx[2] &= ~(1 << 0); //turn on PLAY led
				}
				else {
					GetTrackTime();
				}
				// if CUE is set
				if(rekordbox.cue_start_position[rekordbox.currentcue] != 0) {
					spi_tx[2] &= ~(1 << 1); //turn on CUE led
				}
				else {
					spi_tx[2] |= (1 << 1); //turn off CUE led
				}
			}
			break;
		}
		case PLAY: {
			if(trak.state == PLAYING) {
				BSP_AUDIO_OUT_Pause();
				if(!spi_tx[2] & (1 << 0)) spi_tx[2] |= (1 << 1);
				else spi_tx[2] &= ~(1 << 1);
				HAL_TIM_Base_Start_IT(&htim8); //start blinking
				if(file_pos_wide != rekordbox.cue_start_position[0]) {
					trak.state = SETCUE;
				}
				else {
					trak.state = STOPPED;
				}
			}
			else {
				BSP_AUDIO_OUT_Resume();
				trak.state = PLAYING;
				HAL_TIM_Base_Stop_IT(&htim8); //stop blinking
				spi_tx[2] &= ~(1 << 0); //turn on PLAY led
				// if CUE is set
				if(rekordbox.cue_start_position[rekordbox.currentcue] != 0) {
					spi_tx[2] &= ~(1 << 1); //turn on CUE led
				}
				else {
					spi_tx[2] |= (1 << 1); //turn off CUE led
				}
			}
			break;
		}
		case CUE: {
			if(trak.state == STOPPED) {
				BSP_AUDIO_OUT_Resume();
				trak.state = PLAYING;
				HAL_TIM_Base_Stop_IT(&htim8); //stop blinking
				spi_tx[2] &= ~(1 << 0); //turn on PLAY led
			}
			else if(trak.state == SETCUE) {
				if(display.quantize == 1) {
					rekordbox.cue_start_position[0] = QuantizePosition(0);
				}
				else {
					rekordbox.cue_start_position[0] = file_pos_wide;
				}
				rekordbox.currentcue = 0;
				spi_tx[2] &= ~(1 << 1); //turn on CUE led
				trak.state = STOPPED;
			}
			break;
		}
		default: break;
		}
		break;
		}
		case 0x80: {
			switch(spi_rx[2]) {
			case CUE: {
				if(trak.state == PLAYING) {
					BSP_AUDIO_OUT_Pause();
					trak.state = STOPPED;
					HAL_TIM_Base_Start_IT(&htim8); //start blinking
					GoToPosition(rekordbox.cue_start_position[rekordbox.currentcue]);
					TrackTime();
				}
				break;
			}
			case TIME: {
				if((__HAL_TIM_GET_COUNTER(&htim9) < 65535)
						&& (__HAL_TIM_GET_COUNTER(&htim9) > 0)) {
					HAL_TIM_Base_Stop_IT(&htim9);
					if(display.timemode == 0) {
						display.timemode = 1;
						display.trackbarmode = 1;
					}
					else {
						display.timemode = 0;
						display.trackbarmode = 0;
					}
				}
				break;
			}
			case MASTERTEMPO: {
				if((__HAL_TIM_GET_COUNTER(&htim9) < 65535)
						&& (__HAL_TIM_GET_COUNTER(&htim9) > 0)) {
					HAL_TIM_Base_Stop_IT(&htim9);
					display.pitchmode++;
					if(display.pitchmode > 3) display.pitchmode = 0;
					if(display.pitchmode == 0) scale_pitch = 1; //wide
					else if(display.pitchmode == 1) scale_pitch = 0.16; //16%
					else if(display.pitchmode == 2) scale_pitch = 0.1; //10%
					else if(display.pitchmode == 3) scale_pitch = 0.06; //6%
					trak.percent = ((float)trak.pitch - 16384) / 16384 * scale_pitch;
					New_frequency = (uint32_t)((trak.bitrate / 2)*(1 + trak.percent));
					BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, New_frequency, NULL);
				}
				break;
			}
			case HOLD: {
				if((__HAL_TIM_GET_COUNTER(&htim9) < 65535)
						&& (__HAL_TIM_GET_COUNTER(&htim9) > 0)) {
					HAL_TIM_Base_Stop_IT(&htim9);
					if(display.loop == 1) display.loop = 0;
				}
			}
			case WAH: {
				button_flag &= (0 << 2);
				break;
			}
			default: break;
			}
			break;
		}
		case 0xB0: {
			HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
			__HAL_TIM_CLEAR_IT(&htim6, TIM_IT_UPDATE);
			__HAL_TIM_SET_COUNTER(&htim6, 0);
			HAL_TIM_Base_Start_IT(&htim6);
			if(spi_rx[2] == 0x24) {
				if(spi_rx[3] == 0x01) {
					if(menu_mode == 1) {
						String_number++;
						Mark_number = String_number;
						if(Total_tracks < 7) {
							if(String_number + 1 > Total_tracks) {
								String_number = 0;
								Mark_number = 0;
							}
						}
						else {
							if(String_number > 6) {
								String_number = 0;
								Mark_number = 0;
								str_increment += 7;
								if(str_increment >= Total_tracks - 7)
									str_increment = Total_tracks - 7;
							}
						}
					}
					else {
						//jog forward
						if(trak.state == PLAYING) {
							pulses++;
						}
						else if(trak.state == SETCUE) {
							GoToPosition(file_pos_wide + 2);
							GetTrackTime();
						}
					}
				}
				else if(spi_rx[3] == 0x7F) {
					if(menu_mode == 1) {
						String_number--;
						Mark_number = String_number;
						if(String_number < 0) {
							if(Total_tracks > 7) {
								String_number = 6;
								Mark_number = 6;
								str_increment -= 7;
								if(str_increment == Total_tracks - 7)
									str_increment = (str_increment / 7) * 7;
								if(str_increment <= 0)
									str_increment = 0;
							}
							else {
								String_number = Total_tracks - 1;
								Mark_number = Total_tracks - 1;;
							}
						}
					}
					else {
						//jog back
						if(trak.state == PLAYING) {
							pulses--;
						}
						else if(trak.state == SETCUE) {
							GoToPosition(file_pos_wide - 2);
							GetTrackTime();
						}
					}
				}
			}
			break;
		}
		case 0xE0: {
			uint16_t pitch_rx = spi_rx[3];
			pitch_rx <<= 8;
			pitch_rx |= spi_rx[2];
			if(flag == 0) {
				if(trak.pitch == 0) trak.pitch = pitch_rx;
				flag = 1;
			}
			int delta = trak.pitch - pitch_rx;
			if(delta < 0) delta = pitch_rx - trak.pitch;
			if(pitch_rx < 64) pitch_rx = 64;
			if(delta < 16384) {
				trak.percent = ((float)pitch_rx - 16383) / 16384 * scale_pitch;
				New_frequency = (uint32_t)((trak.bitrate / 2)*(1 + trak.percent));
				BSP_AUDIO_OUT_ClockConfig(&hsai_BlockA2, New_frequency, NULL);
				trak.pitch = pitch_rx;
			}
			break;
		}
		default: break;
		}
	for(int i = 0; i < 4; i++) spi_rx[i] = 0;
	HAL_SPI_TransmitReceive_IT(&hspi2, spi_tx, spi_rx, 4);
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
