/**
 ******************************************************************************
 * @file       ab_display.h
 * @author     MEMS Application Team
 * @brief      Header for ab_display.c
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H
#define __DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief  Serial message array indexes enumeration based on DataLogCustomLite application
 */
typedef enum
{
  DEST        = 0,
  SOURCE      = DEST      + 1,
  COMMAND     = SOURCE    + 1,
  TIMESTAMP   = COMMAND   + 1,
  STREAM_DATA = TIMESTAMP + 6,
} MSG_INDEXES;

typedef struct {
  uint8_t info_type;
  uint8_t info_index;
  uint16_t variable_count;
  uint8_t variable_type;
  uint16_t stream_position;
  const char *config_string;
  uint8_t already_processed;
  void *p_node;
} sDISPLAY_INFO,*psDISPLAY_INFO;

typedef struct {
  uint8_t var_count;
  uint8_t var_type;
  uint8_t conn_size;
  uint8_t conn[1];
} sCONFIG_RECORD;

/* Exported macro ------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
#define VAR_TYPE_FLOAT    1
#define VAR_TYPE_INT32    2
#define VAR_TYPE_BIT      3
#define VAR_TYPE_INPUT    0x80

#define INFO_TYPE_ABITS   1
#define INFO_TYPE_AINT32  2
#define INFO_TYPE_AFLOAT  3
#define INFO_TYPE_GRAPH   4
#define INFO_TYPE_LOGIC   5
#define INFO_TYPE_SCATTER 6
#define INFO_TYPE_FUSION  7
#define INFO_TYPE_INT32   8
#define INFO_TYPE_FLOAT   9
#define INFO_TYPE_BAR     10
#define INFO_TYPE_HISTO   11
#define INFO_TYPE_PLOT3D  12
#define INFO_TYPE_FFT     13
#define INFO_TYPE_ANGLE	  14

/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Display_Update(void *source, sDISPLAY_INFO *info);

void Datalog_Init(char *header, uint32_t sync_period);
void Datalog_Update(float *data, uint32_t size);

#if ((defined (USE_NUCLEO)) || (defined (USE_SENSORTILE)))
/* Not supported feature */
#elif  ((defined (USE_SENSORTILEBOX)) || (defined (USE_STWIN)))
void Datalog_Sync_Handler(void);
void Datalog_Set_RTC_Handle(void *handle);
void Datalog_Open(void);
void Datalog_Close(void);  
#else
#error Not supported platform
#endif

#endif /* __DISPLAY_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
