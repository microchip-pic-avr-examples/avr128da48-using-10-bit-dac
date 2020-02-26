/*
    \file   main.c
    
    \brief  DAC generating sine wave
    
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
#include <math.h>

/* VREF start-up time */
#define VREF_STARTUP_TIME       (50)
/* Mask needed to get the 2 LSb for DAC Data Register */
#define LSB_MASK                (0x03)
/* Number of samples for a sine wave period */
#define SINE_PERIOD_STEPS       (100)
/* Sine wave amplitude */
#define SINE_AMPLITUDE          (511)
/* Sine wave DC offset */
#define SINE_DC_OFFSET          (512)
/* Frequency of the sine wave */
#define SINE_FREQ               (100)
/* Step delay for the loop */
#define STEP_DELAY_TIME         ((1000000 / SINE_FREQ) / SINE_PERIOD_STEPS)

static void sineWaveInit(void);
static void VREF_init(void);
static void DAC0_init(void);
static void DAC0_setVal(uint16_t value);

/* Buffer to store the sine wave samples */
uint16_t sineWave[SINE_PERIOD_STEPS];

static void sineWaveInit(void)
{
    uint8_t i;
    for(i = 0; i < SINE_PERIOD_STEPS; i++)
    {
        sineWave[i] = SINE_DC_OFFSET + SINE_AMPLITUDE * sin(2 * M_PI * i / SINE_PERIOD_STEPS);
    }
}

static void VREF_init(void)
{
    VREF.DAC0REF = VREF_REFSEL_2V048_gc /* Select the 2.048V Internal Voltage Reference for DAC */
                 | VREF_ALWAYSON_bm;    /* Set the Voltage Reference in Always On mode */
    /* Wait VREF start-up time */
    _delay_us(VREF_STARTUP_TIME);
}

static void DAC0_init(void)
{
    /* Disable digital input buffer */
    PORTD.PIN6CTRL &= ~PORT_ISC_gm;
    PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
    /* Disable pull-up resistor */
    PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;
    DAC0.CTRLA = DAC_ENABLE_bm          /* Enable DAC */
               | DAC_OUTEN_bm           /* Enable output buffer */
               | DAC_RUNSTDBY_bm;       /* Enable Run in Standby mode */
}

static void DAC0_setVal(uint16_t value)
{
    /* Store the two LSbs in DAC0.DATAL */
    DAC0.DATAL = (value & LSB_MASK) << 6;
    /* Store the eight MSbs in DAC0.DATAH */
    DAC0.DATAH = value >> 2;
}

int main(void)
{
    uint8_t sineIndex = 0;
    
    VREF_init();
    DAC0_init();
    
    sineWaveInit();
    
    while (1)
    {
        DAC0_setVal(sineWave[sineIndex++]);
        if(sineIndex == SINE_PERIOD_STEPS)
            sineIndex = 0;
        _delay_us(STEP_DELAY_TIME);
    }
}