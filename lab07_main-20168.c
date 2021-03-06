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
#include<stdio.h>
#include <stdint.h>

//DEFINICION DE FRECUENCIA PARA DELAY
#define _XTAL_FREQ 4000000

//DEFINICION DE ALIAS PARA PINES
#define incB PORTBbits.RB0      
#define decB PORTBbits.RB1
#define disp1 PORTDbits.RD2
#define disp2 PORTDbits.RD1
#define disp3 PORTDbits.RD0  

//DEFINICIONES GENERALES
#define tmr0_val 248            // PARA INTERRUPCIONES CADA 2mS

//VARIABLES GLOBALES
uint8_t contador1 = 0;          // VARIABLE DE CONTADOR CON PUSHBUTTONS
uint8_t mod = 0;                // VARIABLE DE MODULO
uint8_t A = 1;                  // VARIABLES PARA ANTIRREBOTE 
uint8_t B = 1;
uint8_t C = 1;
uint8_t D = 1;
int huns = 0;                   // VARIABLE PARA CENTENAS (HUNDREDS)
int tens = 0;                   // VARIABLE PARA DECENAS (TENTHS)
int ones = 0;                   // VARIABLE PARA UNIDADES (ONES)
int disp_flag = 0;              // BANDERA PARA DISPLAYS

//7SEG DISPLAY INDEX
char index[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 
0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111};

//PROTO FUNCIONES
void setup(void);

void tmr0_setup(void);

int digits(void);

//CONFIGURACION PRINCIPAL
void setup(void){    
    //IO CONFIG
    ANSEL = 0;                  
    ANSELH = 0;                 // SALIDAS Y ENTRADAS DIGITALES
    
    TRISA = 0;                  // PORTA COMO SALIDA
    PORTA = 0;                  // LIMPIEZA DE PORTA
    TRISC = 0;                  // PORTC COMO SALIDA
    PORTC = 0;                  // LIMPIEZA DE PORTC
    TRISD = 0b11111000          // PORTD RD0-RD2 COMO SALIDA
    PORTD = 0;                  // LIMPIEZA DE PORTD
    
    //CONFIG PUSHBUTTONS EN PORTB
    TRISBbits.TRISB0 = 1;       // RB0 COMO INPUT
    TRISBbits.TRISB1 = 1;       // RB1 COMO INPUT
    OPTION_REGbits.nRBPU = 0;   // HABILITAR WEAK PULLUP EN PUERTO B
    WPUBbits.WPUB0 = 1;         // HABILITAR RESISTENCIA EN RB0
    WPUBbits.WPUB1 = 1;         // HABILITAR RESISTENCIA EN RB1
    
    //CONFIG DE INTERRUPCIONES
    INTCONbits.GIE = 1;         // HABILITAR INTERRUPCIONES GLOBALES
    INTCONbits.RBIE = 1;        // HABILITAR INTERRUPCIONES EN PORTB
    INTCONbits.T0IE = 1;        // HABILITAR INTERRUPCIONES DE TMR0
    IOCBbits.IOCB0 = 1;         // HABILITAR INTERRUPCION EN CAMBIO PARA RB0
    IOCBbits.IOCB1 = 1;         // HABILITAR INTERRUPCION EN CAMBIO PARA RB1
    INTCONbits.RBIF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN PORTB
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    
    //OSCCONFIC
    OSCCONbits.IRCF = 0b0110;   // FRECUENCIA DE OSCILADOR INTERNO (4MHz)
    OSCCONbits.SCS  = 1;        // RELOJ INTERNO
    return;
}

//CONFIGURACION TMR0
void tmr0_setup(void){
    OPTION_REGbits.T0CS = 0;    // UTILIZAR CICLO INTERNO
    OPTION_REGbits.PSA = 0;     // CAMBIAR PRESCALER A TMR0
    OPTION_REGbits.PS0 = 1;     // COLOCAR PRESCALER EN 1:256
    OPTION_REGbits.PS1 = 1;     // 
    OPTION_REGbits.PS2 = 1;     // 
    
    INTCONbits.T0IF = 0;        // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
    TMR0 = tmr0_val;            // VALOR DE TMR0
    return;
}
//
int digits(void){
    mod = contador1%100;        // CALCULO DEL MODULO DEL CONTADOR PARA AISLAR DECENAS Y UNIDADES
    huns = contador1/100;       // DIVISION DE CONTADOR PARA DETERMINAR CENTENAS
    tens = mod/10;              // DIVISION DEL MODULO DEL CONTADOR PARA DETERMINAR DECENAS
    ones = mod%10;              // MODULO DEL MODULO DEL CONTADOR PARA DETERMINAR UNIDADES
}

//INTERRUPCIONES
void __interrupt() isr(void){
    
    if(INTCONbits.RBIF){
        A = 1;
        C = 1;
        if (!incB){             // REVISAR SI RB0 FUE PRESIONADO
            A = 0;
            B = A;        
        }
        if (B != A){            // REVISAR SI RB0 FUE LIBERADO
            B = A;
            contador1++;        // INCREMENTAR PORTA
        }
        else if(!decB){         // REVISAR SI RB1 FUE PRESIONADO
            C = 0;
            D = C;         
        }
        if (D != C){            // REVISAR SI RB1 FUE LIBERADO
            D = C;
            contador1--;        // DECREMENTAR PORTA
        }
        INTCONbits.RBIF = 0;    // LIMPIAR BANDERA DE INTERRUPCION EN PORTB
    }
    
    else if(T0IF){              // INTERRUPCION DE TMR0 ACTIVADA
        INTCONbits.T0IF = 0;    // LIMPIAR BANDERA DE INTERRUPCION EN TMR0
        TMR0 = tmr0_val;        // REINICIAR TMR0
        PORTD = 0;              // PREPARAR PORTD PARA RECIVIR VALORES A DISP
        if (disp_flag == 0){            //
            PORTC = (index[ones]);      //
            disp3 = 0;                  //
            disp1 = 1;                  //
            disp_flag = 1;              //
        }
        else if (disp_flag == 1){       //
            PORTC = (index[tens]);      //
            disp1 = 0;                  //
            disp2 = 1;                  //
            disp_flag = 2;              //
        }
        else if (disp_flag == 2){       //
            PORTC = (index[huns]);      //
            disp2 = 0;                  //
            disp3 = 1;                  //
            disp_flag = 0;              //
        }
    }
    return;
}

//CICLO MAIN
void main(void) {
    //EJECUCION CONFIG
    setup();
    tmr0_setup();
    
    //LOOP MAIN
    while(1){
        PORTA = contador1;      // ACTUALIZAR CONSTANTEMENTE PORTC
        digits();               // SEPARAR DIGITOS DE CUENTA
    }
    return;
}
