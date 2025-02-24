/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
*/
/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/


#include "../include/adc1.h"

adc_irq_cb_t ADC1_window_cb = NULL;

/**
 * \brief Initialize ADC interface
 */
int8_t ADC1_Initialize()
{
     
    //DUTYCYC DUTY50; 
	ADC1.CALIB = 0x00;

    //SAMPNUM ACC1; 
	ADC1.CTRLB = 0x00;

    //SAMPCAP disabled; REFSEL INTREF; PRESC DIV2; 
	ADC1.CTRLC = 0x00;

    //INITDLY DLY0; ASDV ASVOFF; SAMPDLY 0; 
	ADC1.CTRLD = 0x00;

    //WINCM NONE; 
	ADC1.CTRLE = 0x00;

    //DBGRUN disabled; 
	ADC1.DBGCTRL = 0x00;

    //STARTEI disabled; 
	ADC1.EVCTRL = 0x00;

    //WCMP disabled; RESRDY disabled; 
	ADC1.INTCTRL = 0x00;

    //MUXPOS AIN0; 
	ADC1.MUXPOS = 0x00;

    //SAMPLEN 1; 
	ADC1.SAMPCTRL = 0x01;

    // Window comparator high threshold 
	ADC1.WINHT = 0x00;

    // Window comparator low threshold 
	ADC1.WINLT = 0x00;

    //RUNSTBY disabled; RESSEL 10BIT; FREERUN disabled; ENABLE enabled; 
	ADC1.CTRLA = 0x01;


	return 0;
}

void ADC1_Enable()
{
	ADC1.CTRLA |= ADC_ENABLE_bm;
}

void ADC1_Disable()
{
	ADC1.CTRLA &= ~ADC_ENABLE_bm;
}

void ADC1_EnableAutoTrigger()
{
	ADC1.EVCTRL |= ADC_STARTEI_bm;
}

void ADC1_DisableAutoTrigger()
{
	ADC1.EVCTRL &= ~ADC_STARTEI_bm;
}

void ADC1_SetWindowHigh(adc_result_t high)
{
	ADC1.WINHT = high;
}

void ADC1_SetWindowLow(adc_result_t low)
{
	ADC1.WINLT = low;
}

void ADC1_SetWindowMode(adc1_window_mode_t mode)
{
	ADC1.CTRLE = mode;
}

void ADC1_SetWindowChannel(adc_0_channel_t channel)
{
	ADC1.MUXPOS = channel;
}

void ADC1_StartConversion(adc_0_channel_t channel)
{
	ADC1.MUXPOS  = channel;
	ADC1.COMMAND = ADC_STCONV_bm;
}

bool ADC1_IsConversionDone()
{
	return (ADC1.INTFLAGS & ADC_RESRDY_bm);
}

adc_result_t ADC1_GetConversionResult(void)
{
	return (ADC1.RES);
}

bool ADC1_GetWindowResult(void)
{
	bool temp     = (ADC1.INTFLAGS & ADC_WCMP_bm);
	ADC1.INTFLAGS = ADC_WCMP_bm; // Clear intflag if set
	return temp;
}

adc_result_t ADC1_GetConversion(adc_0_channel_t channel)
{
	adc_result_t res;

	ADC1_StartConversion(channel);
	while (!ADC1_IsConversionDone());
	res           = ADC1_GetConversionResult();
	ADC1.INTFLAGS = ADC_RESRDY_bm;
	return res;
}

uint8_t ADC1_GetResolution()
{
	return (ADC1.CTRLA & ADC_RESSEL_bm) ? 10 : 12;
}

void ADC1_RegisterWindowCallback(adc_irq_cb_t f)
{
	ADC1_window_cb = f;
}

ISR(ADC1_WCOMP_vect)
{

	if (ADC1_window_cb != NULL) {
		ADC1_window_cb();
	}
        
	// Clear the interrupt flag
	ADC1.INTFLAGS = ADC_WCMP_bm;
}

ISR(ADC1_RESRDY_vect)
{
    /* Insert your ADC result ready interrupt handling code here */

    /* The interrupt flag has to be cleared manually */
    ADC1.INTFLAGS = ADC_RESRDY_bm;
}