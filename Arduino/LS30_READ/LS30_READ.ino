//SNK LS-30 Rotary joystick SNAC adapter for the MiSTer FPGA.
//Author: @RndMnkIII.
//Date:   29/07/2022

//LS-30 Rotary Joystick connector schematic (one for each player):
//     ---------------------------------------------------------------
//CN1 | 13   12   11   10    9    8   7    6    5    4    3    2    1 |   +--+--+--+------ 3V3    +-+-+-+-- GND
//     ---------------------------------------------------------------    |  |  |  |              | | | |
//     P12  P11  P10   P9   P8   P7  GND  P6   P5   P4   P3   P2   P1     <  <  <  < R Network    | | | |
//       |    |    |    |    |   |    |    |    |    |    |    |    |     >  >  >  > 10K,1/4w x4  = = = = 10nF,16V x4 
//       |    |    |    |    |   |    |    |    |    |    |    |    |     <  <  <  <              | | | |
//       |    |    |    |    |   |    |    |    |    |    |    |    |     |  |  |  |              | | | |
//       +----|----|----|----+---|----|----|----|----+----|----|----|-----+--|--|--|---v^v^-------+-|-|-|------- P1D3
//            +----|----|--------+----|----|----|---------+----|----|--------+--|--|---v^v^---------+-|-|------- P1D2
//                 +----|-------------|----+----|--------------+----|-----------+--|---v^v^-----------+-|------- P1D1
//                      +-------------|---------+-------------------+--------------+---v^v^-------------+------- P1D0
//                                    +----------------------------------- GND         1K,1/4W x4
//
////   ---------------------------------------------------------------
//CN2 | 13   12   11   10    9    8   7    6    5    4    3    2    1 |   +--+--+--+------ 3V3    +-+-+-+-- GND
//     ---------------------------------------------------------------    |  |  |  |              | | | |
//     P12  P11  P10   P9   P8   P7  GND  P6   P5   P4   P3   P2   P1     <  <  <  < R Network    | | | |
//       |    |    |    |    |   |    |    |    |    |    |    |    |     >  >  >  > 10K,1/4w x4  = = = = 10nF,16V x4 
//       |    |    |    |    |   |    |    |    |    |    |    |    |     <  <  <  <              | | | |
//       |    |    |    |    |   |    |    |    |    |    |    |    |     |  |  |  |              | | | |
//       +----|----|----|----+---|----|----|----|----+----|----|----|-----+--|--|--|---v^v^-------+-|-|-|------- P2D3
//            +----|----|--------+----|----|----|---------+----|----|--------+--|--|---v^v^---------+-|-|------- P2D2
//                 +----|-------------|----+----|--------------+----|-----------+--|---v^v^-----------+-|------- P2D1
//                      +-------------|---------+-------------------+--------------+---v^v^-------------+------- P2D0
//                                    +----------------------------------- GND         1K,1/4W x4
// Legend: 
// --- cable
// + cable join                               |             <
// v^v^ resistor                              |             >
// = ceramic capacitor (non polarized)        | cable       < resistor  
//      
//STM32F103CT8 "Blue Pill" pinout:
//STM32 Pin      Connect to
//--------------------------------------
//PA0            P1D1/Button D         P
//PA1            P1D0/Button C         L
//PA2            Button A switch       A
//PA3            Button B switch       Y
//PA4            Left     switch       E
//PA5            Right    switch       R
//PA6            Down     switch 
//PA7            Up       switch       1
//PA8            P1D3/Button F
//PA9            P1D2/Button E
//PA10           Coin  switch
//PA11           Start switch
//PA12           Unused 
//PA15           Unused
//--------------------------------------
//PB0            Unused
//PB1            Unused
//PB3            Unused
//PB4            P2D1/Button D         P
//PB5            P2D0/Button C         L
//PB6            Button A switch       A
//PB7            Button B switch       Y
//PB8            Left     switch       E
//PB9            Right    switch       R
//PB10           Down     switch 
//PB11           Up       switch       2
//PB12           P1D3/Button F
//PB13           P1D2/Button E
//PB14           Coin  switch
//PB15           Start switch
//---------------------------------------
//PC13           pin8 TX-              U  P
//PC14           pin3 D+               S  O
//PC15           pin2 D-               B  R
//5V             pin1 VBUS             3  T
//GND            pin4 GND

//For instructions about how to connect and program the STM32F103C using ST LINK V2 programmer see:
// https://www.youtube.com/watch?v=saolJ_7E7hc
volatile uint32_t var_portA;
volatile uint32_t var_portB;
volatile uint32_t data_out;

/* This ISR is triggered on LOAD falling edge and read inputs controls data in parallel
 *  using direct port manipulation. Then the port data is formated and stored into data_out.
 *  Measured about 7.69K reads per second or one read each 130us. You can sense this input lag in your controls???
 */
void load_ISR(void)
{
  noInterrupts();
  var_portA = (GPIOA->IDR);
  var_portB = (GPIOB->IDR);

  //data_out format:
  //        23  22  21   20  19  18  17  16   15  14  13  12   11  10   9   8   7   6   5   4   3   2   1   0
  //---------------------------------------------------------------------------------------------------------
  //PLAYER1                                   ST  CO D2/E D3/F                  U   D   L   R   A   B D0/C D1/D
  //PLAYER2  A  B D0/C  D1/D ST  CO D2/E D3/F                   U   D   L   R
  //data_out is used as PISO register
  data_out = 0xFF000000                | 
              (var_portA & 0x00ff    ) |
             ((var_portB & 0x0f00)   ) |
             ((var_portA & 0x0f00)<<4) |
             ((var_portB & 0xf000)<<4) |
             ((var_portB & 0x00f0)<<16);
  interrupts();
}

/* This ISR is triggered on CLK falling edge and outputs serial data on DATA pin and right shift data_out.
 *  Measured about 200K reads per second or one read each 5us.
 */
void clk_ISR(void){
  GPIOC->ODR =  (GPIOC->ODR & 0xdfff) | (data_out & 0x1)<<13;
  data_out = data_out >> 1;
}

void setup() {
  //Free PA11, PA12 from USB Serial port.
  Serial.end();

  //PLAYER1 control set: port A pins 0,1,2,3,4,5,6,7,8,9,10,11
  //Setup port A[11:0] as input at max speed.
  //direct port manipulation to setup pins.
  GPIOA->CRL = (GPIOA->CRL & 0x00000000) | 0x88888888;
  GPIOA->CRH = (GPIOA->CRH & 0xffff0000) | 0x00008888;
  GPIOA->ODR = (GPIOA->ODR & 0xf000) | 0x0CFC; //input for pins 0,1,8,9 input pull-up for pins 2,3,4,5,6,7,10,11

  //PLAYER2 control set: port B pins 4,5,6,7,8,9,10,11,12,13,14,15
  
  //direct port manipulation to setup pins.
//  GPIOB->CRL = (GPIOB->CRL & 0x0000ffff) | 0x88880000;
//  GPIOB->CRH = (GPIOB->CRH & 0x00000000) | 0x88888888;
//  GPIOB->ODR = (GPIOB->ODR & 0x000f) | 0xCFC0; //input for pins 4,5,12,13 input pull-up for pins 6,7,8,9,10,11,14,15
    //Arduino way to setup pins.
  pinMode(PB4,INPUT_PULLUP);
  pinMode(PB5,INPUT_PULLUP);
  pinMode(PB6,INPUT_PULLUP);
  pinMode(PB7,INPUT_PULLUP);
  pinMode(PB8,INPUT_PULLUP);
  pinMode(PB9,INPUT_PULLUP);
  pinMode(PB10,INPUT_PULLUP);
  pinMode(PB11,INPUT_PULLUP);
  pinMode(PB12,INPUT_PULLUP);
  pinMode(PB13,INPUT_PULLUP);
  pinMode(PB14,INPUT_PULLUP);
  pinMode(PB15,INPUT_PULLUP);

  //Setup CLK,LOAD,DATA pins for USERIO port interface, port C pins:
  pinMode(PC13,OUTPUT); //DATA
  pinMode(PC14,INPUT);  //LATCH
  pinMode(PC15,INPUT);  //CLK

  delay(5);
  data_out = 0xFFFFFFFF;
  digitalWrite(PC13,0x1);
  
  attachInterrupt(digitalPinToInterrupt(PC14), load_ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(PC15), clk_ISR, FALLING);
}

void loop() {
}
