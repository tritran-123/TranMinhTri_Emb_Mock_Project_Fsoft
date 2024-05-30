/***********************************************************************************/
/*                                EMB09-TRAN MINH TRI                              */
/*                          	     Mock Project                                  */
/*               THANK YOU TO MR NHAN FOR BRINGING A VERY USEFUL COURSE            */
/*                                                                                 */
/***********************************************************************************/
#include "S32K144EVB.h"

/* prototype function */
void Initalize_LPSPI0_Master(void);
void LPSPI_TransmitData(uint16_t u8Data);
void Initalize_Slave_8DigitLed_MAX7219(void);
void Initialize_LPIT(void);
void Display_Time_In_Led(void);
void Display_Date_In_Led(void);
uint8_t Leap_Year(uint32_t year_check);
void LPIT0_Ch3_IRQHandler(void);
void Initialize_Button(void);
void PORTC_IRQHandler(void);
void Handler_Feature_Two_Button(void);
void Initialize_LPUART1(void);
void LPUART1_RxTx_IRQHandler(void);
void Initialize_ADC0(void);
void ADC_Module(void);

/* global variable */
static volatile uint16_t Hour = 0; /* variable to control hour */
static volatile uint16_t Min = 0;  /* variable to control min */
static volatile uint16_t Sec = 0;  /* variable to control second */

static volatile uint16_t Day = 1;     /* variable to control day ,first value = 1*/
static volatile uint16_t Month = 1;   /* variable to control month ,first value = 1*/
static volatile uint32_t Year = 1971; /* variable to control year  ,first value = 1971*/

static volatile uint8_t Flag_Button = 1;    /* variable to control value of button (have 2 buttons, each button has 2 state)*/
static volatile uint32_t State_Button1 = 0; /* variable to control state of button 1 */
static volatile uint32_t State_Button2 = 0; /* variable to control state of button 1 */

static volatile uint8_t Check_Data_Receiver;     /* variable to check each character received by uart*/
static uint8_t Check_String_Receiver[20] = {0};  /* string receiver */
static volatile uint8_t Receiver_String_Len = 0; /*lenght string */

static volatile uint32_t Receiver_Data_ADC = 0; /* variable to receiver read ADC data */
static volatile uint16_t Intensity = 0;         /* variable to control intensity of led by ADC */

int main(void)

{
    /* call functions to initial the module */
    Initalize_LPSPI0_Master();
    Initalize_Slave_8DigitLed_MAX7219();
    Initialize_LPIT();
    Initialize_Button();
    Initialize_LPUART1();
    Initialize_ADC0();
    while (1)
    {
        ADC_Module();                 /* polling continuously to express intensity of led */
        Handler_Feature_Two_Button(); /* call func to handler requirements */
    }
}

/* func initalize master (LPSPI0) */
void Initalize_LPSPI0_Master(void)
{
    /* 1. Setting SCK/PCS/SOUT/SIN pin */
    /*+ Enable clock for PORT: PCC[CGC]  LPSPI0_PCS0 (PTB0) muX = 0000_0011*/
    PCC_PORTx->PCC_PORTB |= (1 << 30);
    /* Set alternate function pin: PORT_PCR[MUX] (PCS[0] : PTB0) (SOUT : PTB1) (SCK : PTB2) (SIN : PTB3) */
    PCR_PORTB->PORT_PCR0 |= (3 << 8); /* PCS */
    PCR_PORTB->PORT_PCR1 |= (3 << 8); /* SOUT */
    PCR_PORTB->PORT_PCR2 |= (3 << 8); /* SCK */
    PCR_PORTB->PORT_PCR3 |= (3 << 8); /* SIN */

    /*2. Select source LPSPI */
    /*+ Peripheral Clock Source Select: PCC[PCS] */
    PCC_LPSPI0 |= (3 << 24);
    /* SCG_FIRC bit 8-10 = 001 Divide by 1 */
    SCG_FIRDIV |= (1 << 8);
    /* + Enable clock for LPSPI: PCC[CGC]*/
    PCC_LPSPI0 |= (1 << 30);

    /* 3. Setting Clock */
    /*+ Set Prescaler Value: TCR[PRESCALE] */
    LPSPI0->TCR |= (3 << 27); /* 011b - Divide by 8 */
    /* Set divide ratio of the SCK pin: CCR[SCKDIV] */
    LPSPI0->CCR = (4 << 0);

    /* 4. Configures Clock Phase and Polarity */
    /* + Set Clock Polarity: TCR[CPOL] = 0b - The inactive state value of SCK is low */
    /*+ Set Clock Phase: TCR[CPHA] = 0b - Data is captured on the leading edge of SCK and changed on the following edge of SCK */

    /*5. Setting Frame data */
    /*+ Set Frame Size: TCR[FRAMESZ] */
    LPSPI0->TCR &= ~((uint32_t)31 << 0);
    LPSPI0->TCR |= (7 << 0);
    /* + Set type of transfer data: TCR[LSBF] */
    // LPSPI0->TCR &= ~(1 << 23); /* 0b - Data is transferred MSB first */

    /* 6. Configures the peripheral chip
     00b - Transfer using LPSPI_PCS[0] */

    /* 7. Setting Transmit/Receive FIFO
    + Set Receive FIFO Watermark: FCR[RXWATER]    //00b = 1word
    + Set Transmit FIFO Watermark: FCR[TXWATER]   //00b = 1word
    + Set FIFO underrun: CFGR1[NOSTALL]    //00b = 1word */
    LPSPI0->CFGR1 |= (1 << 3); /* 01b = nostall*/

    /* 8. Configures LPSPI mode
      Set mode: CFGR1[MASTER] */
    LPSPI0->CFGR1 |= (1 << 0);
    /* Debug Enable: CR[DBGEN] */
    LPSPI0->CR |= (1 << 3); /*1b - LPSPI module is enabled in debug mode*/

    /* 9. Enable LPSPI module
    + Module Enable: CR[MEN] */
    /* Enable module when need to tranfer data */
    // LPSPI0->CR |= (1 << 0); /*1b - Module is enabled*/
}

/* func initalize master (LPSPI0) */
void LPSPI_TransmitData(uint16_t u16Data)
{
    LPSPI0->CR |= (1 << 0); /*1b - Module is enabled*/
    /* wait the number of words in the transmit FIFO is equal or less than FCR[TXWATER]  */
    while (!(LPSPI0->SR & (1 << 0)))
    {
    }
    /* transmit data */
    LPSPI0->TDR = u16Data;
    LPSPI0->CR &= ~((uint32_t)1 << 0); /*1b - Module is disabled*/
}

/* func initialize slave (8digit led MAX7219) */
void Initalize_Slave_8DigitLed_MAX7219(void)
{
    /* MSB */
    /* Decode-Mode Register Examples (Address (Hex) = 0xX9) */
    /* Code B decode for digits 7–0 : 0xff */
    LPSPI_TransmitData(0x09FF);

    /* Intensity Register Format (Address (Hex) = 0xXA) */
    /* MAX7219(3/32) : 0xX8 */
    LPSPI_TransmitData(0x0A00);

    /* Scan-Limit Register Format (Address (Hex) = 0xXB) */
    /* Display digits 0 1 2 3 4 5 6 7 : 0xX7 */
    LPSPI_TransmitData(0x0B07);

    /*Display-Test Register Format (Address (Hex) = 0xXF)*/
    /* Normal Operation 0xX0 */
    LPSPI_TransmitData(0x0F00);

    /* Shutdown Register Format (Address (Hex) = 0xXC) */
    /* Normal Operation : 0xX1 */
    LPSPI_TransmitData(0x0C01);
}

/* func print value timer into led */
void Display_Time_In_Led(void)
{
    /* Decode-Mode Register Examples (Address (Hex) = 0xX9) */
    /* Code B decode for digits 01 34 67 : 0xD8 */
    LPSPI_TransmitData(0x09DB);

    /* represents Sec's value in led digit 1 + 2*/
    LPSPI_TransmitData((Sec % 10) + 0x0100);
    LPSPI_TransmitData((Sec / 10) % 10 + 0x0200);

    LPSPI_TransmitData(0x0301); /* No decode mode represents character "-" */

    /* represents Min's value in led digit 4 + 5*/
    LPSPI_TransmitData((Min % 10) + 0x0400);
    LPSPI_TransmitData((Min / 10) % 10 + 0x0500);

    LPSPI_TransmitData(0x0601); /* No decode mode represents character "-" */

    /* represents Hour's value in led digit 7 + 8*/
    LPSPI_TransmitData((Hour % 10) + 0x0700);
    LPSPI_TransmitData((Hour / 10) % 10 + 0x0800);

    /* Enable Timer Channel 3: start counter  */
    LPIT->TCTRL3 |= (1 << 0);
}

/* func print value (Day,Month,Year) into led */
void Display_Date_In_Led(void)
{
    /* Decode-Mode Register Examples (Address (Hex) = 0xX9) */
    /* Code B decode for digits 01234567 : 0xD8 */
    LPSPI_TransmitData(0x09FF);

    /* represents Year's value in led digit 1 + 2 + 3 + 4*/
    LPSPI_TransmitData((Year % 10) + 0x0100);
    LPSPI_TransmitData((Year / 10) % 10 + 0x0200);
    LPSPI_TransmitData((Year / 100) % 10 + 0x0300);
    LPSPI_TransmitData((Year / 1000) % 10 + 0x0400);

    /* represents Month's value in led digit 5 + 6 */
    LPSPI_TransmitData((Month % 10) + 0x0580); /* *The decimal point is set by bit D7 = 1 */
    LPSPI_TransmitData((Month / 10) % 10 + 0x0600);

    /* represents Month's value in led digit 7 + 8 */
    LPSPI_TransmitData((Day % 10) + 0x0780);
    LPSPI_TransmitData((Day / 10) % 10 + 0x0800);

    /* Enable Timer Channel 3: start counter  */
    LPIT->TCTRL3 |= (1 << 0);
}

/* func initalize LPIT */
void Initialize_LPIT(void)
{
    /* Interrupt Set Enable Register at LPIT0_Ch2_IRQHandler */
    NVIC_ISER_1 |= (3 << 18); /* IRQ_LPIT0_Ch3 = 51 + IRQ_LPIT0_Ch2 = 50*/

    /* Peripheral Clock Source Select: PCC[PCS] */
    PCC_LPIT |= (0x7 << 24); /* LPO128_CLK */
    /*Setting Clocking*/
    PCC_LPIT |= (1 << 30);

    /* Module Clock Enable: MCR[M_CEN] */
    LPIT->MCR |= (1 << 0);

    /* Enable Channel3 + 2 Timer Interrupt */
    LPIT->MIER |= (1 << 3);

    /* Run in Debug mode: MCR[DBG_EN] */
    LPIT->MCR |= (1 << 3);

    /* Timer Operation Mode: TCTR​Ln[MODE] */
    /* 00b =  32-bit Periodic Counter*/

    /* Get timer value 0xFFFFFFFF (1s in timer chanel 3)*/
    LPIT->TVAL3 = (127999);

    /* Enable Timer Channel 3 */
    /* Enable timer when value has been appear in 8digit led in function "Display_Date_In_Led && Display_Time_In_Led"*/
}

/* func check leap_year */
uint8_t Leap_Year(uint32_t year_check)
{
    /* A leap year is a year divisible by 4  and if it is divisible by 4 and 100 then it will be divisible by 400 */
    /* return 1 if this year is leap year */
    return (year_check % 4 == 0) && ((year_check % 100 == 0) ? (year_check % 400 == 0) : 1);
}

/* func LPIT's interrupt */
/*Every 1s, In interrupt function will recalculation value of time (6 variable Sec,Min,Hour,Day,Month,Year)*/
void LPIT0_Ch3_IRQHandler(void)
{
    uint16_t Month_days[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; /* array express day in each month */
    if (LPIT->MSR & (1 << 3))                                                      /* check Channel 3 Timer Interrupt Flag */
    {
        LPIT->MSR |= (1 << 3); /*  To clear a channel timer interrupt flag, write logic 1 to it */

        if (Sec < 59) /* condition to increase Sec's value */
        {
            ++Sec; /* increase Sec's value 1 unit */
        }
        else
        {
            Sec = 0;
            if (Min < 59) /* condition to increase Min's value */
            {
                ++Min; /* increase Min's value 1 unit */
            }
            else
            {
                Min = 0;
                if (Hour < 23) /* condition to increase Hour's value */
                {
                    Hour++; /* increase Hour's value 1 unit */
                }
                else
                {
                    Day++;
                    Hour = 0;
                }
            }
        }

        if (Leap_Year(Year)) /* check leap year in currently year */
        {
            Month_days[2] = 29; /* leap year -> February has 29day */
        }
        else
        {
            Month_days[2] = 28; /* normally february */
        }

        if (Day > Month_days[Month]) /* condition to increase Month's value */
        {
            Day = 1;
            Month++; /* Increase month' value 1 unit */
        }
        if (Month > 12) /* condition to increase Year's value */
        {
            Month = 1;
            Year++; /* Increase month' value 1 unit */
        }
    }
}

/* func initalize PORTC (2 button) */
void Initialize_Button(void)
{
    /* Interrupt Set Enable Register at port C*/
    NVIC_ISER_1 |= (1 << 29);

    /* Enable clock for Port C */
    PCC_PORTx->PCC_PORTC |= (1 << 30);

    /* Configure MUX Pin 12 & 13 of Port C as GPIO */
    PCR_PORTC->PORT_PCR12 |= (1 << 8);
    PCR_PORTC->PORT_PCR13 |= (1 << 8);

    /* Set falling-edge interrupt */
    PCR_PORTC->PORT_PCR12 |= (10 << 16);
    PCR_PORTC->PORT_PCR13 |= (10 << 16);

    /* Configure Pin 12 and 13 of Port C as input */
    GPIOC->PDDR &= ~((uint32_t)1 << 12);
    GPIOC->PDDR &= ~((uint32_t)1 << 13);
}

/* func Port's interrupt */
/*Whenever push button, In interrupt function will recalculation value and state of button */
void PORTC_IRQHandler(void)
{
    if (PCR_PORTC->PORT_PCR12 & (1 << 24)) /* push bt1 will occur interrupt with failing-edge */
    {
        /* clear ISF*/
        PCR_PORTC->PORT_PCR12 |= (1 << 24);

        /* set status is bt1 */
        Flag_Button = 1;

        /* increase value state when push bt1 */
        ++State_Button1;
    }
    else if (PCR_PORTC->PORT_PCR13 & (1 << 24)) /* push bt2 will occur interrupt with failing-edge */
    {
        /* clear ISF */
        PCR_PORTC->PORT_PCR13 |= (1 << 24);

        /* set status is bt2 */
        Flag_Button = 2;

        /* increase value state when push bt2 */
        ++State_Button2;
    }
}

/* func resolves the function of each button push value (loop in main) */
void Handler_Feature_Two_Button(void)
{
    if (Flag_Button == 1) /* if currently in status of button 1*/
    {
        if (State_Button1 % 2 != 0) /* if value of state is a first duty */
        {
            Display_Date_In_Led(); /* call func to display date */
        }
        else /* if value of state is a second duty */
        {
            Display_Time_In_Led(); /* call func to display time */
        }
    }
    else if (Flag_Button == 2) /* if currently in status of button 2 */
    {
        /* turn off display mode */
        if (State_Button2 % 2 != 0)
        {
            LPSPI_TransmitData(0x0C00);
        }
        else
        {
            /* turn on display mode */
            LPSPI_TransmitData(0x0C01);
            Flag_Button = 1; /* reset to first status button = 1 to display date or time */
        }
    }
}

/* func initalize LPUART */
void Initialize_LPUART1(void)
{
    /* 0. Configure NVIC */
    NVIC_ISER_1 |= (1 << 1);

    /*----------------- 1. Setting Tx/Rx pin -----------------*/
    /* Enable clock for PORTC - CGC */
    PCC_PORTx->PCC_PORTC |= (1 << 30);
    /* MUX: Tx and Rx of LPUART1 */
    PCR_PORTC->PORT_PCR6 |= (2 << 8);
    PCR_PORTC->PORT_PCR7 |= (2 << 8);

    /*----------------- 2. Select source LPUART -----------------*/
    /* Peripheral Clock Source Select: Select FIRCDIV2_CLK = 48 MHz and dividede by 1 */
    PCC_LPUARTx->PCC_LPUART1 |= (3 << 24);
    SCG_FIRDIV |= (1 << 8);
    /* Enable clock for LPUART */
    PCC_LPUARTx->PCC_LPUART1 |= (1 << 30);

    /*----------------- 3. Setting baud rate -----------------*/
    /* Set the modulo divide rate: Baud Rate Modulo Divisor = 312,5 ~ 313 = 0x139 */
    LPUART1->BAUD = 0x139;

    /* Set OSC: default = 15 */

    /*----------------- 4. Setting Frame -----------------*/
    /* Select Stop Bit Number: default - 0b: One stop bit */

    /* Set data characters number: default - 0b: 8-bit data */

    /* Set parity bit: 0b - Parity disabled */

    /*----------------- 5. Enable transmitter & receiver -----------------*/
    /* Transmitter Enable */
    // LPUART1->CTRL |= (0x3 << 18);
    /* Receiver Enable */
    LPUART1->CTRL |= (1 << 18);

    /*----------------- 6. Enable Receiver Interrupt -----------------*/
    LPUART1->CTRL |= (1 << 21);
}

/* func LPUART's interrupt */
/* Whenever receive all value by user, it will parse and assign new value of time */
void LPUART1_RxTx_IRQHandler(void)
{
    /* receiver interrupt */
    if (LPUART1->STAT & (1 << 21)) /* interrup when RDRF is set and RDRF is unable when read data */
    {
        Check_Data_Receiver = ((uint8_t)LPUART1->DATA & 0xFF); /* received each character */
        if (Check_Data_Receiver != '\0')
        {
            Check_String_Receiver[Receiver_String_Len++] = Check_Data_Receiver; /* assign character into string and increase value of len */
        }
        else
        {
            Check_String_Receiver[Receiver_String_Len] = '\0';
        }

        /* Syntax to tranfer Date is : Day-Month-Year-('D' || 'd')*/
        if (Check_String_Receiver[strlen((const char*)Check_String_Receiver) - 1] == 'D' || Check_String_Receiver[strlen((const char*)Check_String_Receiver) - 1] == 'd')
        {
            /*parse string has receiveđ and convert value to integer type to assign into variable Day*/
            char *token = strtok((char *)Check_String_Receiver, "-");
            Day = (uint16_t)atoi(token);

            /*parse string has receiveđ and convert value to integer type to assign into variable Month*/
            token = strtok(NULL, "-");
            Month = (uint16_t)atoi(token);

            /*parse string has receiveđ and convert value to integer type to assign into variable Year*/
            token = strtok(NULL, "-");
            Year = (uint32_t)atoi(token);

            memset((void *)Check_String_Receiver, 0, sizeof(Check_String_Receiver)); /* set all value of elements in string to 0 */
            Receiver_String_Len = 0;                                                 /* reset value of lenght string*/
        }
        /* Syntax to tranfer Time is : Hour-Min-Sec-('T' || 't')*/
        if (Check_String_Receiver[strlen((const char*)Check_String_Receiver) - 1] == 'T' || Check_String_Receiver[strlen((const char*)Check_String_Receiver) - 1] == 't')
        {
            /*parse string has receiveđ and convert value to integer type to assign into variable Hour */
            char *token = strtok((char *)Check_String_Receiver, "-");
            Hour = (uint16_t)atoi(token);
            /*parse string has receiveđ and convert value to integer type to assign into variable Min */
            token = strtok(NULL, "-");
            Min = (uint16_t)atoi(token);
            /*parse string has receiveđ and convert value to integer type to assign into variable Sec */
            token = strtok(NULL, "-");
            Sec = (uint16_t)atoi(token);

            memset((void *)Check_String_Receiver, 0, sizeof(Check_String_Receiver)); /* set all value of elements in string to 0 */
            Receiver_String_Len = 0;                                                 /* reset value of lenght string*/
        }
    }
}

/* func read and handler value adc */
void ADC_Module(void)
{
    if (ADC0->SC1A & (1 << 7)) /* COCO in SC1A is also set at the completion of a calibration sequence */
    {
        Receiver_Data_ADC = ADC0_RA;                              /* assign new value of ADC */
        if (Receiver_Data_ADC < 3400 || Receiver_Data_ADC > 3599) /* condition if touch electrodes has been pressed */
        {
            if (Intensity > 15) /* if value of intensity exceeds the max value */
            {
                Intensity = 0; /* return to min value = 0*/
            }
            Intensity += 4;                         /* increase value by 4 units*/
            LPSPI_TransmitData(0x0A00 + Intensity); /* send value by LPSPI */
        }
    }
}

/* func initalize ADC_module */
void Initialize_ADC0(void)
{
    /* step 1. Select source ADC */
    PCC_ADC |= (3 << 24);   /* Peripheral Clock Source Select: PCC[PCS] */
    SCG_FIRDIV |= (1 << 8); /* SCG_FIRC bit 8-10 = 001 Divide by 1 */
    PCC_ADC |= (1 << 30);   /* Enable clock for ADC: PCC[CGC] */

    /* step 2. Configure ADC  */
    /* Select source clock: CFG1[ADICLK] :  00b - Alternate clock 1 (ALTCLK1)*/
    /* Set Prescaler Value: CFG1[ADIV] : 00b - The divide ratio is 1 and the clock rate is input clock.*/

    /*step 3. Configures scale*/
    ADC0->CFG1 |= (1 << 2); /* Set resolution: CFG1[MODE] : 01b - 12-bit conversion*/
    ADC0->CFG2 = 13;        /*  Set sampling time: CFG1[SMPLTS] : Sample time of 13 ADCK clock*/

    /*step 4. Configures mode */
    /* Set trigger mode: SC2[ADTRG] :0b - Software trigger selected*/
    /*  Set conversion mode: SC3[ADCO] : 0b one shot mode (set AVGE)*/
    ADC0_SC3 |= (1 << 3);

    /* step 5. Configures channel + Start conversion*/
    ADC0->SC1A &= ~((uint32_t)63 << 0);
    ADC0->SC1A = (8 << 0); /*  Set channel: SC1[ADCH] : PTC1 - ADC0_SE9 (chanle 9).*/
}
