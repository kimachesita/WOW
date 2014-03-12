//WEATHER ON WATCH v1.0 HOST FIRMWARE
//kimachesita
//March 18,2013

//Message Convention
//$STATUS,TEMP,HUMIDITY,REV_PER_TIME_UNIT,TIC_PER_TIME_UNIT,checksumbyte
// EX. $G,30.0,71.0,004,006,7,##
// Gd. 0123456789012345678901234
//Status: B -> Bad, G ->Delay_us Good
// REV_PER_TIME_UNIT is in DEC byte
// TIC_PER_TIME_UNIT is in DEC byte

#include "DHT11.h"


char message[] = "$B,00.0,00.0,000,000,0,##";
unsigned short CheckSum ;
unsigned short T_Byte1, T_Byte2, RH_Byte1, RH_Byte2;
unsigned short C_Byte1 , C_Byte2;

//initialize indicators
sbit LED_IND_ON at RC0_bit;
sbit LED_UART_TRANSMIT at RC1_bit;

//function declarations
 void resetMessageValues();
 
void main() {
  unsigned short check;
  TRISB = 0xFF;      //Make PORT B as digital input -> Anenometer
  TRISD = 0xFF;      //Make PORT D as digital input -> Rain gauge
  TRISC = 0x00;      //MAke PORT C as digital output

  ADCON1 = 0b00000110;//Make Port A digital Input
  CMCON = 7;         //Reset Comparators
  UART1_Init(9600);  // Initialize UART1 module at 9600 bps
  INTCON.GIE = 1;    //Enable global interrupt
  INTCON.PEIE = 1;   //Enable peripheral interrupt

  // Configure Timer2 module
  PIE1.TMR2IE = 1;  // Enable Timer2 interrupt
  PIR1.TMR2IF = 0;  // Clear TMR INT Flag bit
  T2CON = 0;        // Prescaler 1:1, and Timer2 is off initially
  TMR2 = 0;
  LED_IND_ON = 1;
  LED_UART_TRANSMIT = 0;
  Delay_ms(1000);
  //start main loop
  do {
    StartSignal();
    check = CheckResponse();
    if(check == 0){
       LED_ERROR_INDICATOR = 1;
       UART1_Write_Text(message);
    }
    else{
     LED_ERROR_INDICATOR = 0;
     RH_Byte1 = ReadByte();
     RH_Byte2 = ReadByte();
     T_Byte1 = ReadByte();
     T_Byte2 = ReadByte();
     CheckSum = ReadByte();
     C_Byte1 = PORTB;
     C_Byte2 = PORTD;
     // Check for error in Data reception
     if (CheckSum == ((RH_Byte1 + RH_Byte2 + T_Byte1 + T_Byte2) & 0xFF))
     {
        LED_ERROR_CHECKSUM = 0;
        message[1] = 'G';
        message[3] =  T_Byte1/10 + 48;
        message[4] =  T_Byte1%10 + 48;
        message[6] =  T_Byte2/10 + 48;
        message[8]  = RH_Byte1/10 + 48;
        message[9]  = RH_Byte1%10 + 48;
        message[11] =  RH_Byte2/10 + 48;
        message[13] =  C_Byte1/100 + 48;
        message[14] =  (C_Byte1%100)/10 + 48;
        message[15] =  (C_Byte1%100)%10 + 48;
        message[17] =  C_Byte2/100 + 48;
        message[18] =  (C_Byte2%100)/10 + 48;
        message[19] =  (C_Byte2%100)%10 + 48;
        message[21] =  message[3] + message[4] + message[6] + message[8] + message[9] + message[11] + message[13] + message[14] + message[15] + message[17] + message[18] + message[19];
        UART1_Write_Text(message);
        resetMessageValues();

      }
      else{
        LED_ERROR_CHECKSUM = 1;
        UART1_Write_Text(message);
      }
    }
    LED_UART_TRANSMIT = 1;
    Delay_ms(1000);
    LED_INITIALIZE_SIGNAL = 0;
    LED_ERROR_INDICATOR = 0;
    LED_ERROR_CHECKSUM = 0;
    LED_UART_TRANSMIT = 0;
    Delay_ms(500);
  }while(1);

}

void resetMessageValues(){
        message[1] =   'B';
        message[3] =   '0';
        message[4] =   '0';
        message[6] =   '0';
        message[8]  =  '0';
        message[9]  =  '0';
        message[11] =  '0';
        message[13] =  '0';
        message[14] =  '0';
        message[15] =  '0';
        message[17] =  '0';
        message[18] =  '0';
        message[19] =  '0';
        message[21] =  '0';
}
