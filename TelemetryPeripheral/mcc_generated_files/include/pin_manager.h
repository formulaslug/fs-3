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

#ifndef PINS_H_INCLUDED
#define PINS_H_INCLUDED

#include <avr/io.h>
#include "port.h"

//get/set PA2 aliases
#define PA2_SetHigh() do { PORTA_OUTSET = 0x4; } while(0)
#define PA2_SetLow() do { PORTA_OUTCLR = 0x4; } while(0)
#define PA2_Toggle() do { PORTA_OUTTGL = 0x4; } while(0)
#define PA2_GetValue() (VPORTA.IN & (0x1 << 2))
#define PA2_SetDigitalInput() do { PORTA_DIRCLR = 0x4; } while(0)
#define PA2_SetDigitalOutput() do { PORTA_DIRSET = 0x4; } while(0)
#define PA2_SetPullUp() do { PORTA_PIN2CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA2_ResetPullUp() do { PORTA_PIN2CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA2_SetInverted() do { PORTA_PIN2CTRL  |= PORT_INVEN_bm; } while(0)
#define PA2_ResetInverted() do { PORTA_PIN2CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA2_DisableInterruptOnChange() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA2_EnableInterruptForBothEdges() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA2_EnableInterruptForRisingEdge() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA2_EnableInterruptForFallingEdge() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA2_DisableDigitalInputBuffer() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA2_EnableInterruptForLowLevelSensing() do { PORTA.PIN2CTRL = (PORTA.PIN2CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set IO_PC3 aliases
#define IO_PC3_SetHigh() do { PORTC_OUTSET = 0x8; } while(0)
#define IO_PC3_SetLow() do { PORTC_OUTCLR = 0x8; } while(0)
#define IO_PC3_Toggle() do { PORTC_OUTTGL = 0x8; } while(0)
#define IO_PC3_GetValue() (VPORTC.IN & (0x1 << 3))
#define IO_PC3_SetDigitalInput() do { PORTC_DIRCLR = 0x8; } while(0)
#define IO_PC3_SetDigitalOutput() do { PORTC_DIRSET = 0x8; } while(0)
#define IO_PC3_SetPullUp() do { PORTC_PIN3CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define IO_PC3_ResetPullUp() do { PORTC_PIN3CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define IO_PC3_SetInverted() do { PORTC_PIN3CTRL  |= PORT_INVEN_bm; } while(0)
#define IO_PC3_ResetInverted() do { PORTC_PIN3CTRL  &= ~PORT_INVEN_bm; } while(0)
#define IO_PC3_DisableInterruptOnChange() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define IO_PC3_EnableInterruptForBothEdges() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define IO_PC3_EnableInterruptForRisingEdge() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define IO_PC3_EnableInterruptForFallingEdge() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define IO_PC3_DisableDigitalInputBuffer() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define IO_PC3_EnableInterruptForLowLevelSensing() do { PORTC.PIN3CTRL = (PORTC.PIN3CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PA1 aliases
#define PA1_SetHigh() do { PORTA_OUTSET = 0x2; } while(0)
#define PA1_SetLow() do { PORTA_OUTCLR = 0x2; } while(0)
#define PA1_Toggle() do { PORTA_OUTTGL = 0x2; } while(0)
#define PA1_GetValue() (VPORTA.IN & (0x1 << 1))
#define PA1_SetDigitalInput() do { PORTA_DIRCLR = 0x2; } while(0)
#define PA1_SetDigitalOutput() do { PORTA_DIRSET = 0x2; } while(0)
#define PA1_SetPullUp() do { PORTA_PIN1CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA1_ResetPullUp() do { PORTA_PIN1CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA1_SetInverted() do { PORTA_PIN1CTRL  |= PORT_INVEN_bm; } while(0)
#define PA1_ResetInverted() do { PORTA_PIN1CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA1_DisableInterruptOnChange() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA1_EnableInterruptForBothEdges() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA1_EnableInterruptForRisingEdge() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA1_EnableInterruptForFallingEdge() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA1_DisableDigitalInputBuffer() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA1_EnableInterruptForLowLevelSensing() do { PORTA.PIN1CTRL = (PORTA.PIN1CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set IO_PA4 aliases
#define IO_PA4_SetHigh() do { PORTA_OUTSET = 0x10; } while(0)
#define IO_PA4_SetLow() do { PORTA_OUTCLR = 0x10; } while(0)
#define IO_PA4_Toggle() do { PORTA_OUTTGL = 0x10; } while(0)
#define IO_PA4_GetValue() (VPORTA.IN & (0x1 << 4))
#define IO_PA4_SetDigitalInput() do { PORTA_DIRCLR = 0x10; } while(0)
#define IO_PA4_SetDigitalOutput() do { PORTA_DIRSET = 0x10; } while(0)
#define IO_PA4_SetPullUp() do { PORTA_PIN4CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define IO_PA4_ResetPullUp() do { PORTA_PIN4CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define IO_PA4_SetInverted() do { PORTA_PIN4CTRL  |= PORT_INVEN_bm; } while(0)
#define IO_PA4_ResetInverted() do { PORTA_PIN4CTRL  &= ~PORT_INVEN_bm; } while(0)
#define IO_PA4_DisableInterruptOnChange() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define IO_PA4_EnableInterruptForBothEdges() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define IO_PA4_EnableInterruptForRisingEdge() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define IO_PA4_EnableInterruptForFallingEdge() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define IO_PA4_DisableDigitalInputBuffer() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define IO_PA4_EnableInterruptForLowLevelSensing() do { PORTA.PIN4CTRL = (PORTA.PIN4CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PA3 aliases
#define PA3_SetHigh() do { PORTA_OUTSET = 0x8; } while(0)
#define PA3_SetLow() do { PORTA_OUTCLR = 0x8; } while(0)
#define PA3_Toggle() do { PORTA_OUTTGL = 0x8; } while(0)
#define PA3_GetValue() (VPORTA.IN & (0x1 << 3))
#define PA3_SetDigitalInput() do { PORTA_DIRCLR = 0x8; } while(0)
#define PA3_SetDigitalOutput() do { PORTA_DIRSET = 0x8; } while(0)
#define PA3_SetPullUp() do { PORTA_PIN3CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA3_ResetPullUp() do { PORTA_PIN3CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA3_SetInverted() do { PORTA_PIN3CTRL  |= PORT_INVEN_bm; } while(0)
#define PA3_ResetInverted() do { PORTA_PIN3CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA3_DisableInterruptOnChange() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA3_EnableInterruptForBothEdges() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA3_EnableInterruptForRisingEdge() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA3_EnableInterruptForFallingEdge() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA3_DisableDigitalInputBuffer() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA3_EnableInterruptForLowLevelSensing() do { PORTA.PIN3CTRL = (PORTA.PIN3CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PB4 aliases
#define PB4_SetHigh() do { PORTB_OUTSET = 0x10; } while(0)
#define PB4_SetLow() do { PORTB_OUTCLR = 0x10; } while(0)
#define PB4_Toggle() do { PORTB_OUTTGL = 0x10; } while(0)
#define PB4_GetValue() (VPORTB.IN & (0x1 << 4))
#define PB4_SetDigitalInput() do { PORTB_DIRCLR = 0x10; } while(0)
#define PB4_SetDigitalOutput() do { PORTB_DIRSET = 0x10; } while(0)
#define PB4_SetPullUp() do { PORTB_PIN4CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PB4_ResetPullUp() do { PORTB_PIN4CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PB4_SetInverted() do { PORTB_PIN4CTRL  |= PORT_INVEN_bm; } while(0)
#define PB4_ResetInverted() do { PORTB_PIN4CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PB4_DisableInterruptOnChange() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PB4_EnableInterruptForBothEdges() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PB4_EnableInterruptForRisingEdge() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PB4_EnableInterruptForFallingEdge() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PB4_DisableDigitalInputBuffer() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PB4_EnableInterruptForLowLevelSensing() do { PORTB.PIN4CTRL = (PORTB.PIN4CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PA6 aliases
#define PA6_SetHigh() do { PORTA_OUTSET = 0x40; } while(0)
#define PA6_SetLow() do { PORTA_OUTCLR = 0x40; } while(0)
#define PA6_Toggle() do { PORTA_OUTTGL = 0x40; } while(0)
#define PA6_GetValue() (VPORTA.IN & (0x1 << 6))
#define PA6_SetDigitalInput() do { PORTA_DIRCLR = 0x40; } while(0)
#define PA6_SetDigitalOutput() do { PORTA_DIRSET = 0x40; } while(0)
#define PA6_SetPullUp() do { PORTA_PIN6CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA6_ResetPullUp() do { PORTA_PIN6CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA6_SetInverted() do { PORTA_PIN6CTRL  |= PORT_INVEN_bm; } while(0)
#define PA6_ResetInverted() do { PORTA_PIN6CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA6_DisableInterruptOnChange() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA6_EnableInterruptForBothEdges() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA6_EnableInterruptForRisingEdge() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA6_EnableInterruptForFallingEdge() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA6_DisableDigitalInputBuffer() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA6_EnableInterruptForLowLevelSensing() do { PORTA.PIN6CTRL = (PORTA.PIN6CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PA5 aliases
#define PA5_SetHigh() do { PORTA_OUTSET = 0x20; } while(0)
#define PA5_SetLow() do { PORTA_OUTCLR = 0x20; } while(0)
#define PA5_Toggle() do { PORTA_OUTTGL = 0x20; } while(0)
#define PA5_GetValue() (VPORTA.IN & (0x1 << 5))
#define PA5_SetDigitalInput() do { PORTA_DIRCLR = 0x20; } while(0)
#define PA5_SetDigitalOutput() do { PORTA_DIRSET = 0x20; } while(0)
#define PA5_SetPullUp() do { PORTA_PIN5CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA5_ResetPullUp() do { PORTA_PIN5CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA5_SetInverted() do { PORTA_PIN5CTRL  |= PORT_INVEN_bm; } while(0)
#define PA5_ResetInverted() do { PORTA_PIN5CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA5_DisableInterruptOnChange() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA5_EnableInterruptForBothEdges() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA5_EnableInterruptForRisingEdge() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA5_EnableInterruptForFallingEdge() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA5_DisableDigitalInputBuffer() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA5_EnableInterruptForLowLevelSensing() do { PORTA.PIN5CTRL = (PORTA.PIN5CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PA7 aliases
#define PA7_SetHigh() do { PORTA_OUTSET = 0x80; } while(0)
#define PA7_SetLow() do { PORTA_OUTCLR = 0x80; } while(0)
#define PA7_Toggle() do { PORTA_OUTTGL = 0x80; } while(0)
#define PA7_GetValue() (VPORTA.IN & (0x1 << 7))
#define PA7_SetDigitalInput() do { PORTA_DIRCLR = 0x80; } while(0)
#define PA7_SetDigitalOutput() do { PORTA_DIRSET = 0x80; } while(0)
#define PA7_SetPullUp() do { PORTA_PIN7CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PA7_ResetPullUp() do { PORTA_PIN7CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PA7_SetInverted() do { PORTA_PIN7CTRL  |= PORT_INVEN_bm; } while(0)
#define PA7_ResetInverted() do { PORTA_PIN7CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PA7_DisableInterruptOnChange() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PA7_EnableInterruptForBothEdges() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PA7_EnableInterruptForRisingEdge() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PA7_EnableInterruptForFallingEdge() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PA7_DisableDigitalInputBuffer() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PA7_EnableInterruptForLowLevelSensing() do { PORTA.PIN7CTRL = (PORTA.PIN7CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PC0 aliases
#define PC0_SetHigh() do { PORTC_OUTSET = 0x1; } while(0)
#define PC0_SetLow() do { PORTC_OUTCLR = 0x1; } while(0)
#define PC0_Toggle() do { PORTC_OUTTGL = 0x1; } while(0)
#define PC0_GetValue() (VPORTC.IN & (0x1 << 0))
#define PC0_SetDigitalInput() do { PORTC_DIRCLR = 0x1; } while(0)
#define PC0_SetDigitalOutput() do { PORTC_DIRSET = 0x1; } while(0)
#define PC0_SetPullUp() do { PORTC_PIN0CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PC0_ResetPullUp() do { PORTC_PIN0CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PC0_SetInverted() do { PORTC_PIN0CTRL  |= PORT_INVEN_bm; } while(0)
#define PC0_ResetInverted() do { PORTC_PIN0CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PC0_DisableInterruptOnChange() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PC0_EnableInterruptForBothEdges() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PC0_EnableInterruptForRisingEdge() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PC0_EnableInterruptForFallingEdge() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PC0_DisableDigitalInputBuffer() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PC0_EnableInterruptForLowLevelSensing() do { PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PB1 aliases
#define PB1_SetHigh() do { PORTB_OUTSET = 0x2; } while(0)
#define PB1_SetLow() do { PORTB_OUTCLR = 0x2; } while(0)
#define PB1_Toggle() do { PORTB_OUTTGL = 0x2; } while(0)
#define PB1_GetValue() (VPORTB.IN & (0x1 << 1))
#define PB1_SetDigitalInput() do { PORTB_DIRCLR = 0x2; } while(0)
#define PB1_SetDigitalOutput() do { PORTB_DIRSET = 0x2; } while(0)
#define PB1_SetPullUp() do { PORTB_PIN1CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PB1_ResetPullUp() do { PORTB_PIN1CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PB1_SetInverted() do { PORTB_PIN1CTRL  |= PORT_INVEN_bm; } while(0)
#define PB1_ResetInverted() do { PORTB_PIN1CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PB1_DisableInterruptOnChange() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PB1_EnableInterruptForBothEdges() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PB1_EnableInterruptForRisingEdge() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PB1_EnableInterruptForFallingEdge() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PB1_DisableDigitalInputBuffer() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PB1_EnableInterruptForLowLevelSensing() do { PORTB.PIN1CTRL = (PORTB.PIN1CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PC2 aliases
#define PC2_SetHigh() do { PORTC_OUTSET = 0x4; } while(0)
#define PC2_SetLow() do { PORTC_OUTCLR = 0x4; } while(0)
#define PC2_Toggle() do { PORTC_OUTTGL = 0x4; } while(0)
#define PC2_GetValue() (VPORTC.IN & (0x1 << 2))
#define PC2_SetDigitalInput() do { PORTC_DIRCLR = 0x4; } while(0)
#define PC2_SetDigitalOutput() do { PORTC_DIRSET = 0x4; } while(0)
#define PC2_SetPullUp() do { PORTC_PIN2CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PC2_ResetPullUp() do { PORTC_PIN2CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PC2_SetInverted() do { PORTC_PIN2CTRL  |= PORT_INVEN_bm; } while(0)
#define PC2_ResetInverted() do { PORTC_PIN2CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PC2_DisableInterruptOnChange() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PC2_EnableInterruptForBothEdges() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PC2_EnableInterruptForRisingEdge() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PC2_EnableInterruptForFallingEdge() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PC2_DisableDigitalInputBuffer() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PC2_EnableInterruptForLowLevelSensing() do { PORTC.PIN2CTRL = (PORTC.PIN2CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PB0 aliases
#define PB0_SetHigh() do { PORTB_OUTSET = 0x1; } while(0)
#define PB0_SetLow() do { PORTB_OUTCLR = 0x1; } while(0)
#define PB0_Toggle() do { PORTB_OUTTGL = 0x1; } while(0)
#define PB0_GetValue() (VPORTB.IN & (0x1 << 0))
#define PB0_SetDigitalInput() do { PORTB_DIRCLR = 0x1; } while(0)
#define PB0_SetDigitalOutput() do { PORTB_DIRSET = 0x1; } while(0)
#define PB0_SetPullUp() do { PORTB_PIN0CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PB0_ResetPullUp() do { PORTB_PIN0CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PB0_SetInverted() do { PORTB_PIN0CTRL  |= PORT_INVEN_bm; } while(0)
#define PB0_ResetInverted() do { PORTB_PIN0CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PB0_DisableInterruptOnChange() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PB0_EnableInterruptForBothEdges() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PB0_EnableInterruptForRisingEdge() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PB0_EnableInterruptForFallingEdge() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PB0_DisableDigitalInputBuffer() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PB0_EnableInterruptForLowLevelSensing() do { PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

//get/set PC1 aliases
#define PC1_SetHigh() do { PORTC_OUTSET = 0x2; } while(0)
#define PC1_SetLow() do { PORTC_OUTCLR = 0x2; } while(0)
#define PC1_Toggle() do { PORTC_OUTTGL = 0x2; } while(0)
#define PC1_GetValue() (VPORTC.IN & (0x1 << 1))
#define PC1_SetDigitalInput() do { PORTC_DIRCLR = 0x2; } while(0)
#define PC1_SetDigitalOutput() do { PORTC_DIRSET = 0x2; } while(0)
#define PC1_SetPullUp() do { PORTC_PIN1CTRL  |= PORT_PULLUPEN_bm; } while(0)
#define PC1_ResetPullUp() do { PORTC_PIN1CTRL  &= ~PORT_PULLUPEN_bm; } while(0)
#define PC1_SetInverted() do { PORTC_PIN1CTRL  |= PORT_INVEN_bm; } while(0)
#define PC1_ResetInverted() do { PORTC_PIN1CTRL  &= ~PORT_INVEN_bm; } while(0)
#define PC1_DisableInterruptOnChange() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x0 ; } while(0)
#define PC1_EnableInterruptForBothEdges() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x1 ; } while(0)
#define PC1_EnableInterruptForRisingEdge() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x2 ; } while(0)
#define PC1_EnableInterruptForFallingEdge() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x3 ; } while(0)
#define PC1_DisableDigitalInputBuffer() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x4 ; } while(0)
#define PC1_EnableInterruptForLowLevelSensing() do { PORTC.PIN1CTRL = (PORTC.PIN1CTRL & ~PORT_ISC_gm) | 0x5 ; } while(0)

void PIN_MANAGER_Initialize();
void PORTA_PA2_DefaultInterruptHandler(void);
void PORTA_PA2_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTC_IO_PC3_DefaultInterruptHandler(void);
void PORTC_IO_PC3_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_PA1_DefaultInterruptHandler(void);
void PORTA_PA1_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_IO_PA4_DefaultInterruptHandler(void);
void PORTA_IO_PA4_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_PA3_DefaultInterruptHandler(void);
void PORTA_PA3_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTB_PB4_DefaultInterruptHandler(void);
void PORTB_PB4_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_PA6_DefaultInterruptHandler(void);
void PORTA_PA6_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_PA5_DefaultInterruptHandler(void);
void PORTA_PA5_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTA_PA7_DefaultInterruptHandler(void);
void PORTA_PA7_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTC_PC0_DefaultInterruptHandler(void);
void PORTC_PC0_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTB_PB1_DefaultInterruptHandler(void);
void PORTB_PB1_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTC_PC2_DefaultInterruptHandler(void);
void PORTC_PC2_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTB_PB0_DefaultInterruptHandler(void);
void PORTB_PB0_SetInterruptHandler(void (* interruptHandler)(void)) ;
void PORTC_PC1_DefaultInterruptHandler(void);
void PORTC_PC1_SetInterruptHandler(void (* interruptHandler)(void)) ;
#endif /* PINS_H_INCLUDED */
