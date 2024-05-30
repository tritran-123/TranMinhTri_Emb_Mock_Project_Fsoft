
#ifndef S32K144_H
#define S32K144_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PCC_base_address_PORTx (0x40065000 + 0x124)          /* offset 124h start address PCC port A */
#define PCC_base_address_LPUARTx (0x40065000 + 0x1A8)        /* offset 1A8h start address PCC LPUART0 */
#define PCC_LPIT (*(volatile uint32_t *)(0x40065000 + 0xDC)) /* offset DCh start address PCC LPIT */
#define PCC_ADC (*(volatile uint32_t *)(0x40065000 + 0xEC))  /* offset ECh start address PCC ADC0 */
#define PCC_LPSPI0 (*(volatile uint32_t *)(0x40065000 + 0xB0))
#define PCC_LPSPI1 (*(volatile uint32_t *)(0x40065000 + 0xB4))
#define PCC_LPSPI2 (*(volatile uint32_t *)(0x40065000 + 0xB8))

/* PCR address in 5 port  */
#define PORTA_base_address 0x40049000
#define PORTB_base_address 0x4004A000
#define PORTC_base_address 0x4004B000
#define PORTD_base_address 0x4004C000
#define PORTE_base_address 0x4004D000

/* GPIO BAse address */
#define GPIOA_address 0x400FF000
#define GPIOB_adress (0x400FF000 + 0x40)
#define GPIOC_adress (0x400FF000 + 0x80)
#define GPIOD_adress (0x400FF000 + 0xC0)
#define GPIOE_adress (0x400FF000 + 0x100)


/* LPUART memory map */
#define LPUART0_base_address (0x4006A000 + 0x10)
#define LPUART1_base_address (0x4006B000 + 0x10)
#define LPUART2_base_address (0x4006C000 + 0x10)

/* LPIT memorry map */
#define LPIT_base_address (0x40037000)

/* ADC memory map */
#define ADC0_base_address (0x4003B000)

#define ADC0_SC2 (*(volatile uint32_t *)(ADC0_base_address + 0x90)) /* offset 90h start address ADC SC2 Registers  */
#define ADC0_SC3 (*(volatile uint32_t *)(ADC0_base_address + 0x94)) /* offset 94h start address ADC SC3 Registers  */
#define ADC0_RA (*(volatile uint32_t *)(ADC0_base_address + 0x48))  /* offset 48h start address ADC RA Registers  */

/* LPSPI memorymap */
#define LPSPI0_base_address (0x4002C000)
#define LPSPI1_base_address (0x4002D000)
#define LPSPI2_base_address (0x4002E000)

/* plus register */
#define SCG_FIRDIV (*(volatile uint32_t *)(0x40064000 + 0x304))
#define NVIC_ISER_1 (*(volatile uint32_t *)(0xE000E100u + 4 * 1))
#define NVIC_ISER_0 (*(volatile uint32_t *)(0xE000E100u))

#define NVIC_IPR12 (*(volatile uint32_t *)(0xE000E400 + 4*12))


typedef struct
{
    volatile uint32_t PCC_PORTA;
    volatile uint32_t PCC_PORTB;
    volatile uint32_t PCC_PORTC;
    volatile uint32_t PCC_PORTD;
    volatile uint32_t PCC_PORTE;

} Config_PCC_PORTx;
#define PCC_PORTx ((volatile Config_PCC_PORTx *)(PCC_base_address_PORTx))

typedef struct
{
    volatile uint32_t PCC_LPUART0;
    volatile uint32_t PCC_LPUART1;
    volatile uint32_t PCC_LPUART2;
} Config_PCC_LPUARTx;
#define PCC_LPUARTx ((Config_PCC_LPUARTx *)(PCC_base_address_LPUARTx))

typedef struct
{
    volatile uint32_t PORT_PCR0;
    volatile uint32_t PORT_PCR1;
    volatile uint32_t PORT_PCR2;
    volatile uint32_t PORT_PCR3;
    volatile uint32_t PORT_PCR4;
    volatile uint32_t PORT_PCR5;
    volatile uint32_t PORT_PCR6;
    volatile uint32_t PORT_PCR7;
    volatile uint32_t PORT_PCR8;
    volatile uint32_t PORT_PCR9;
    volatile uint32_t PORT_PCR10;
    volatile uint32_t PORT_PCR11;
    volatile uint32_t PORT_PCR12;
    volatile uint32_t PORT_PCR13;
    volatile uint32_t PORT_PCR14;
    volatile uint32_t PORT_PCR15;
    volatile uint32_t PORT_PCR16;

} Config_PCR_PORTx;
#define PCR_PORTA ((volatile Config_PCR_PORTx *)(PORTA_base_address))
#define PCR_PORTB ((volatile Config_PCR_PORTx *)(PORTB_base_address))
#define PCR_PORTC ((volatile Config_PCR_PORTx *)(PORTC_base_address))
#define PCR_PORTD ((volatile Config_PCR_PORTx *)(PORTD_base_address))

typedef struct
{
    volatile uint32_t BAUD;
    volatile uint32_t STAT;
    volatile uint32_t CTRL;
    volatile uint32_t DATA;
} LPUART_Type;
#define LPUART0 ((volatile LPUART_Type *)(LPUART0_base_address))
#define LPUART1 ((volatile LPUART_Type *)(LPUART1_base_address))
#define LPUART2 ((volatile LPUART_Type *)(LPUART2_base_address))

typedef struct
{
    volatile uint32_t PDOR;
    volatile uint32_t PSOR;
    volatile uint32_t PCOR;
    volatile uint32_t PTOR;
    volatile uint32_t PDIR;
    volatile uint32_t PDDR;
    volatile uint32_t PIDR;
} Config_GPIO_PORTx;
#define GPIOD ((volatile Config_GPIO_PORTx *)(GPIOD_adress))
#define GPIOC ((volatile Config_GPIO_PORTx *)(GPIOC_adress))
#define GPIOB ((volatile Config_GPIO_PORTx *)(GPIOB_adress))
#define GPIOA ((volatile Config_GPIO_PORTx *)(GPIOA_adress))


typedef struct
{
    volatile uint32_t VERID;
    volatile uint32_t PARAM;
    volatile uint32_t MCR;
    volatile uint32_t MSR;
    volatile uint32_t MIER;
    volatile uint32_t SETTEN;
    volatile uint32_t CLRTEN;
    volatile uint32_t Reserved0;
    volatile uint32_t TVAL0;
    volatile uint32_t CVAL0;
    volatile uint32_t TCTRL0;
    volatile uint32_t Reserved1;
    volatile uint32_t TVAL1;
    volatile uint32_t CVAL1;
    volatile uint32_t TCTRL1;
    volatile uint32_t Reserved2;
    volatile uint32_t TVAL2;
    volatile uint32_t CVAL2;
    volatile uint32_t TCTRL2;
    volatile uint32_t Reserved3;
    volatile uint32_t TVAL3;
    volatile uint32_t CVAL3;
    volatile uint32_t TCTRL3;

} Config_LPIT;
#define LPIT ((Config_LPIT *)(LPIT_base_address))

typedef struct
{
    volatile uint32_t SC1A;
    volatile uint32_t SC1B;
    volatile uint32_t SC1C;
    volatile uint32_t SC1D;
    volatile uint32_t SC1E;
    volatile uint32_t SC1F;
    volatile uint32_t SC1G;
    volatile uint32_t SC1H;
    volatile uint32_t SC1I;
    volatile uint32_t SC1J;
    volatile uint32_t SC1K;
    volatile uint32_t SC1L;
    volatile uint32_t SC1M;
    volatile uint32_t SC1N;
    volatile uint32_t SC1O;
    volatile uint32_t SC1P;

    volatile uint32_t CFG1;
    volatile uint32_t CFG2;
} Config_ADC;
#define ADC0 ((Config_ADC *)(ADC0_base_address))

typedef struct
{
    volatile uint32_t aRA;
    volatile uint32_t aRB;
    volatile uint32_t aRC;
    volatile uint32_t aRD;
    volatile uint32_t aRE;
    volatile uint32_t aRF;
    volatile uint32_t aRG;
    volatile uint32_t aRH;
    volatile uint32_t aRI;
    volatile uint32_t aRJ;
    volatile uint32_t aRK;
    volatile uint32_t aRL;
    volatile uint32_t aRM;
    volatile uint32_t aRN;
    volatile uint32_t aRO;
    volatile uint32_t aRP;
} Config_ADC_Data_Register;
#define ADC0_Data_Register ((Config_ADC_Data_Register *)(ADC0_base_address + 0x188)) /* offset 188h start address ADC Data Result Registers (RA - aRP) */

typedef struct
{
    volatile uint32_t VERID;
    volatile uint32_t PARAM;
    volatile uint32_t Reserved0;
    volatile uint32_t Reserved12;
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t IER;
    volatile uint32_t DER;
    volatile uint32_t CFGR0;
    volatile uint32_t CFGR1;
    volatile uint32_t Reserved1;
    volatile uint32_t Reserved2;
    volatile uint32_t DMR0;
    volatile uint32_t DMR1;
    volatile uint32_t Reserved3;
    volatile uint32_t Reserved4;
    volatile uint32_t CCR;
    volatile uint32_t Reserved5;
    volatile uint32_t Reserved6;
    volatile uint32_t Reserved7;
    volatile uint32_t Reserved8;
    volatile uint32_t Reserved9;
    volatile uint32_t FCR;
    volatile uint32_t FSR;
    volatile uint32_t TCR;
    volatile uint32_t TDR;
    volatile uint32_t Reserved10;
    volatile uint32_t Reserved11;
    volatile uint32_t RSR;
    volatile uint32_t RDR;
} LPSPI_Type;
#define LPSPI0 ((LPSPI_Type *)(LPSPI0_base_address))
#define LPSPI1 ((LPSPI_Type *)(LPSPI1_base_address))
#define LPSPI2 ((LPSPI_Type *)(LPSPI2_base_address))


#endif
