#include "p30F3013.h"


//_FOSC(CSW_FSCM_OFF & FRC); 
#pragma config FOSFPR = FRC             // Oscillator (Internal Fast RC (No change to Primary Osc Mode bits))
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)
/********************************************************************************/
/* SE DESACTIVA EL WAvoid habilitarPerifericos()TCHDOG														*/
//_FWDT(WDT_OFF); 
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)
/********************************************************************************/
//_FBORPOR( PBOR_ON & BORV27 & PWRT_16 & MCLR_EN ); 
// FBORPOR
#pragma config FPWRT  = PWRT_16          // POR Timer Value (16ms)
#pragma config BODENV = BORV20           // Brown Out Voltage (2.7V)
#pragma config BOREN  = PBOR_ON          // PBOR Enable (Enabled)
#pragma config MCLRE  = MCLR_EN          // Master Clear Enable (Enabled)
/********************************************************************************/
//_FGS(CODE_PROT_OFF)
// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

/********************************************************************************/
/* SECCI?N DE DECLARACI?N DE CONSTANTES CON DEFINE								*/
/********************************************************************************/
#define EVER 1
#define N           6
#define DATA_FORMAT 0X31
#define BW_RATE     0X2C
#define POWER_CTL   0X2D
#define DATAX0      0X32
#define DATAX1      0X33
#define DATAY0      0X34
#define DATAY1      0X35
#define DATAZ0      0X36
#define DATAZ1      0X37

//#define MUESTRAS 64

/********************************************************************************/
/* DECLARACIONES GLOBALES														*/
/********************************************************************************/
/*DECLARACI?N DE LA ISR DEL TIMER 1 USANDO __attribute__						*/
/********************************************************************************/
void __attribute__((__interrupt__)) _INT0Interrupt( void );

void configurarPuertos();  
void configurarUART();
void configurarTIMER();
void configurarSPI1();
void configurarIntTimer();
void configurarInt();


extern void INI_LCD_8BITS( );
extern void BF_LCD( );
extern void DATO_LCD( unsigned short int Dato );
extern void COMANDO_LCD( unsigned short int Dato );
extern void printLCD( char *cad );
extern void writeADXL345(unsigned short int reg, unsigned short int dato);
extern short int readADXL345(unsigned short int reg);
extern void bin2cad(short int num, char cad[N]);

short int muestra, X0, X1, X, Y0, Y1, Y, Z0, Z1, Z;
char cadX[N], cadY[N], cadZ[N];

int main(void) 
{    
    configurarPuertos();
    INI_LCD_8BITS();
    printLCD("X:");
    BF_LCD();
    COMANDO_LCD(0X89);
    printLCD("Y:");
    BF_LCD();
    COMANDO_LCD(0XC4);
    printLCD("Z:");
    configurarUART();
    configurarTIMER();
    configurarSPI1();
    writeADXL345(POWER_CTL, 0X00);
    writeADXL345(DATA_FORMAT, 0X0B);
    writeADXL345(BW_RATE, 0X0D);
    writeADXL345(POWER_CTL, 0X08);
    X0=X1=X = 777;
    Y0=Y1=Y = 777;
    Z0=Z1=Z = 777;
    muestra = 0;
    configurarIntTimer();
    configurarInt();
    for(;EVER;)
    {
        if(muestra == 1){
            X0 = readADXL345(DATAX0);
            U1TXREG = X0;
            X1 = readADXL345(DATAX1);
            U1TXREG = X1;
            X = X1<<8|X0;
            bin2cad(X,cadX);
            BF_LCD();
            COMANDO_LCD(0X82);
            printLCD(cadX);
            //Y
            Y0 = readADXL345(DATAY0);
            U1TXREG = Y0;
            Y1 = readADXL345(DATAY1);
            U1TXREG = Y1;
            Y = Y1<<8|Y0;
            bin2cad(Y,cadY);
            BF_LCD();
            COMANDO_LCD(0X8B);
            printLCD(cadY);
            //Z
            Z0 = readADXL345(DATAZ0);
            U1TXREG = Z0;
            Z1 = readADXL345(DATAZ1);
            U1TXREG = Z1;
            Z = Z1<<8|Z0;
            bin2cad(Z,cadZ);
            BF_LCD();
            COMANDO_LCD(0XC6);
            printLCD(cadZ);
            muestra = 0;
            
        }
        asm("nop");
    }
    
    return 0;
}

void configurarInt()
{
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
    T1CONbits.TON = 1;
}

void configurarIntTimer()
{
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
}

void configurarSPI1(){
    SPI1CON = 0X047F;
    SPI1STAT = 0X8000;
}

void configurarTIMER()
{
    TMR1 = 0X0000;
    PR1 = 0X7080;
    T1CON = 0X0020;   // 8 para activar TIMER1 y 2 para usar la pre-escala de 64
}

void configurarUART()
{
    U1MODE = 0X0420;
    U1STA = 0X8000;
    U1BRG = 1;
}

void configurarPuertos()
{
    PORTB = 0;
    asm("nop");
    LATB = 0;
    asm("nop");
    TRISB = 0;
    asm("nop");
    
    ADPCFG = 0XFFFF;
    asm("nop");
    
    PORTD = 0;
    asm("nop");             //ENSAMBLADOR EN LINEA
    LATD = 0;
    asm("nop");
    TRISDbits.TRISD8 = 0;
    asm("nop");
    
    PORTF = 0;
    asm("nop");             //ENSAMBLADOR EN LINEA
    LATF = 0;
    asm("nop");
    TRISF = 0;
    asm("nop");
    TRISFbits.TRISF2 = 1;
    asm("nop");

    PORTC = 0;
    asm("nop");             //ENSAMBLADOR EN LINEA
    LATC = 0;
    asm("nop");
    TRISC = 0;
    asm("nop");
    TRISCbits.TRISC14 = 1;
    asm("nop");
}
/**  @brief: CONFIGURACION DEL UART 1. EL UART 1 SE COMUNICA CON 
 *           EL FT232 PARA COMUNICACION CON PC
 *   @baudrate: 115200 BAUDIOS
 *   @frame: 8 BITS X DATO, SIN PARIDAD, 1 BIT DE PARO
 */

/********************************************************************************/
/* DESCRICION:	ISR (INTERRUPT SERVICE ROUTINE) DE RECEPCION DEL UART 2			*/
/* LA RUTINA TIENE QUE SER GLOBAL PARA SER UNA ISR								*/	
/* SE USA PUSH.S PARA GUARDAR LOS REGISTROS W0, W1, W2, W3, C, Z, N Y DC EN LOS */
/* REGISTROS SOMBRA																*/
/********************************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _INT0Interrupt( void )
{
//    LATD = LATD ^ 0x0200;
//    LATDbits.LATD9 = ~LATDbits.LATD9;
    //asm("btg latd, #latd9");
        
    IFS0bits.T1IF = 0;
}
//      T1IF=1,   T1IF=1
//      ________
//_____|        |_______|