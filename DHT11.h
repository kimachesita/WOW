//DHT11.h

sbit Data at RA0_bit;                   //1-Wire Protocol
sbit DataDir at TRISA0_bit;             //Channel Declaration

//Debuging Aid
sbit LED_INITIALIZE_SIGNAL at RC2_bit;  //Send Signal
sbit LED_ERROR_INDICATOR at RC3_bit;   //Error Indicator
sbit LED_ERROR_CHECKSUM at RC4_bit;   //Error Indicator

unsigned short TOUT = 0;

void StartSignal(){
  DataDir = 0;     // Data port is output
  Data    = 0;     //Data = 0
  Delay_ms(25);
  Data    = 1;
  Delay_us(30);
  DataDir = 1;     // Data port is input
  LED_INITIALIZE_SIGNAL = 1;
}

signed short CheckResponse(){
  TOUT = 0;
  TMR2 = 0;
  T2CON.TMR2ON = 1;      // start timer
  while(!Data && !TOUT);
  if (TOUT){
     return 0;
  }else{
     TMR2 = 0;
     while(Data && !TOUT);
     if (TOUT){
        return 0;
     }else{
        T2CON.TMR2ON = 0; //stoptimer
        return 1;
     }
  }
}

unsigned short ReadByte(){
  unsigned short num = 0, i;
  DataDir = 1;
  for (i=0; i<8; i++){
   while(!Data);
   TMR2 = 0;
   T2CON.TMR2ON = 1;
   while(Data);
   T2CON.TMR2ON = 0;
   if(TMR2 > 40) num |= 1<<(7-i);  // If time > 40us, Data is 1
  }
  return num;
}

void interrupt(){
  if(PIR1.TMR2IF){
   TOUT = 1;
   T2CON.TMR2ON = 0; // stop timer
   PIR1.TMR2IF  = 0; // Clear TMR0 interrupt flag
  }
}
