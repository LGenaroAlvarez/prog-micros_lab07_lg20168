/*
 * File:   lab07_main-20168.c
 * Author: Luis Genaro Alvarez Sulecio 20168
 *
 * Created on April 5, 2022, 10:52 AM
 */
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>

//DEFINICION DE FRECUENCIA PARA DELAY
#define _XTAL_FREQ 4000000

//DEFINICION DE BOTONES
#define incB PORTBbits.RB0
#define decB PORTBbits.RB1

//PROTO FUNCIONES
void setup(void);

//CONFIGURACION PRINCIPAL
void setup(void){    
    //IO CONFIG
    ANSEL = 0;                  
    ANSELH = 0;                 // SALIDAS Y ENTRADAS DIGITALES
    
    TRISA = 0;                  // PORTA COMO SALIDA
    PORTA = 0;                  // LIMPIEZA DE PORTA
    TRISC = 0;                  // PORTC COMO SALIDA
    PORTC = 0;                  // LIMPIEZA DE PORTC
    
    //CONFIG PUSHBUTTONS EN PORTB
    TRISBbits.TRISB0 = 1;       // RB0 COMO INPUT
    TRISBbits.TRISB1 = 1;       // RB1 COMO INPUT
    OPTION_REGbits.nRBPU = 0;   // HABILITAR WEAK PULLUP EN PUERTO B
    WPUBbits.WPUB0 = 1;         // HABILITAR RESISTENCIA EN RB0
    WPUBbits.WPUB1 = 1;         // HABILITAR RESISTENCIA EN RB1
    
    //CONFIG DE INTERRUPCIONES
    INTCONbits.GIE = 1;         // HABILITAR INTERRUPCIONES GLOBALES
    INTCONbits.RBIE = 1;        // HABILITAR INTERRUPCIONES EN PORTB
    IOCBbits.IOCB0 = 1;         // HABILITAR INTERRUPCION EN CAMBIO PARA RB0
    IOCBbits.IOCB1 = 1;         // HABILITAR INTERRUPCION EN CAMBIO PARA RB1
    INTCONbits.RBIF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN PORTB
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0110;   // FRECUENCIA DE OSCILADOR INTERNO (4MHz)
    OSCCONbits.SCS  = 1;        // RELOJ INTERNO
    return;
}

//INTERRUPCIONES
void __interrupt() isr(void){
    
    if(INTCONbits.RBIF){
        if (!incB){             // REVISAR SI RB0 FUE PRESIONADO
            while(!incB);       // ANTIRREBOTES EN RB0
            PORTA++;            // INCREMENTAR PORTA
        }
        else if(!decB){         // REVISAR SI RB1 FUE PRESIONADO
            while(!decB);       // ANTIRREBOTES EN RB1
            PORTA--;            // DECREMENTAR PORTA
        }
        INTCONbits.RBIF = 0;    // LIMPIAR BANDERA DE INTERRUPCION EN PORTB
    }
    return;
}

//CICLO MAIN
void main(void) {
    //EJECUCION CONFIG
    setup();
    
    //LOOP MAIN
    while(1){
    }
    return;
}
