/*
    \file   main.c
    
    \brief  DAC reading with ADC
    
    (c) 2019 Microchip Technology Inc. and its subsidiaries.
    
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

/* 4 MHz (needed for delay function) */
#define F_CPU                   (4000000UL)

#include <avr/io.h>
#include <util/delay.h>

/* VREF start-up time */
#define VREF_STARTUP_TIME       (50)
/* Mask needed to get the 2 LSb for DAC Data Register */
#define LSB_MASK                (0x03)

static void VREF_init(void);
static void DAC0_init(void);
static void DAC0_setVal(uint16_t val);
static void ADC0_init(void);
static uint16_t ADC0_read(void);

static void VREF_init(void)
{
    VREF.DAC0REF = VREF_REFSEL_2V048_gc /* Select the 2.048V Internal Voltage Reference for DAC */
                 | VREF_ALWAYSON_bm;    /* Set the Voltage Reference in Always On mode */   
    VREF.ADC0REF = VREF_REFSEL_2V048_gc /* Select the 2.048V Internal Voltage Reference for ADC */
                 | VREF_ALWAYSON_bm;    /* Set the Voltage Reference in Always On mode */
    /* Wait VREF start-up time */
    _delay_us(VREF_STARTUP_TIME);
}

static void DAC0_init(void)
{
    /* Enable DAC */
    DAC0.CTRLA = DAC_ENABLE_bm;
}

static void DAC0_setVal(uint16_t value)
{
    /* Store the two LSbs in DAC0.DATAL */
    DAC0.DATAL = (value & LSB_MASK) << 6;
    /* Store the eight MSbs in DAC0.DATAH */
    DAC0.DATAH = value >> 2;
}

static void ADC0_init(void)
{
    /* CLK_PER divided by 2 */
    ADC0.CTRLC = ADC_PRESC_DIV2_gc;
    ADC0.CTRLA = ADC_ENABLE_bm          /* Enable ADC */
               | ADC_RESSEL_12BIT_gc;   /* Use 12-bit resolution */
    /* Select ADC channel */
    ADC0.MUXPOS = ADC_MUXPOS_DAC0_gc;
}

static uint16_t ADC0_read(void)
{
    /* Start conversion */
    ADC0.COMMAND = ADC_STCONV_bm;
    /* Wait until ADC conversion is done */
    while(!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }
    /* The interrupt flag is cleared when the conversion result is accessed */
    return ADC0.RES;
}


int main(void)
{
    uint16_t dacVal = 0;
    volatile uint16_t adcVal = 0;
    
    VREF_init();
    DAC0_init();
    ADC0_init();
    
    while (1)
    {
        adcVal = ADC0_read();
        
        /* do something with adcVal */
        
        dacVal++;
        DAC0_setVal(dacVal);
    }
}