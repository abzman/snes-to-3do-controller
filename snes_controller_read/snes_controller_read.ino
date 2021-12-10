/*
  ================================================================================

    File........... NES Controller Test Code
    Purpose........ To demonstrate how to interface to an NES controller
    Author......... Joseph Corleto
    E-mail......... corleto.joseph @ gmail.com
    Started........ 04/13/2016
    Finished....... 04/14/2016
    Updated........ --/--/----

  ================================================================================
   Notes
  ================================================================================
  - The NES controller contains one 8-bit 4021 shift register inside.

  - This register takes parallel inputs and converts them into a serial output.

  - This code first latches the data and then shifts in the first bit on the data line.
  Then it clocks and shifts in on the data line until all bits are received.

  - What is debugged are the button states of the NES controller.

  - A logical "1" means the button is not pressed. A logical "0" means the button is
  pressed.

  - This code shifts the first bit of data into the LSB.

  - The order of shifting for the buttons is shown in the table below:

        Bit# | Button
        --------------
          0  |   A
        --------------
          1  |   B
        --------------
          2  | Select
        --------------
                      3  | Start
        --------------
          4  |   Up
        --------------
          5  |  Down
        --------------
          6  |  Left
        --------------
          7  | Right
        --------------

  - The NES controller pinout is shown below (looking into controllers
  connector end):
    __________
   /      |
  /       O 1 | 1 - Ground
        |           | 2 - Clock
  | 7 O   O 2 |   3 - Latch
  |           | 4 - Data Out
  | 6 O   O 3 | 5 - No Connection
  |           |   6 - No Connection
  | 5 O   O 4 |   7 - +5V
  |___________|

  - Please visit www.allaboutcircuits.com to search for complete article!

*/
//===============================================================================
//  Constants
//===============================================================================
// Here we have a bunch of constants that will become clearer when we look at the
// readSnesController() function. Basically, we will use these contents to clear
// a bit. These are chosen according to the table above.
#define B_BUTTON          0
#define Y_BUTTON          1
#define SELECT_BUTTON     2
#define START_BUTTON      3
#define UP_BUTTON         4
#define DOWN_BUTTON       5
#define LEFT_BUTTON       6
#define RIGHT_BUTTON      7
#define A_BUTTON          8
#define X_BUTTON          9
#define L_BUTTON          10
#define R_BUTTON          11
#define NONE1_BUTTON      12
#define NONE2_BUTTON      13
#define NONE3_BUTTON      14
#define NONE4_BUTTON      15

//===============================================================================
//  Variables
//===============================================================================
uint16_t snesRegister  = 0;    // We will use this to hold current button states

//===============================================================================
//  Pin Declarations
//===============================================================================
//Inputs:
#define snesData 4    // The data pin for the SNES controller

//Outputs:
#define snesClock 2    // The clock pin for the SNES controller
#define snesLatch 3    // The latch pin for the SNES controller

//primitives for reading and writing fast
#define data_in() (PIND & 0x10) >> 4

#define clock_on()  PORTD |= 0x04
#define clock_off() PORTD &= 0xfb

#define latch_on()  PORTD |= 0x08
#define latch_off() PORTD &= 0xf7
//===============================================================================
//  Initialization
//===============================================================================
void setup()
{
  // Initialize serial port speed for the serial terminal
  Serial.begin(115200);

  // Set appropriate pins to inputs
  pinMode(snesData, INPUT);

  // Set appropriate pins to outputs
  pinMode(snesClock, OUTPUT);
  pinMode(snesLatch, OUTPUT);

  // Set initial states
  clock_off();
  latch_off();
}

//===============================================================================
//  Main
//===============================================================================
void loop()
{
  // This function call will return the states of all SNES controller's register
  // in a nice 16 bit variable format. Remember to refer to the table and
  // constants above for which button maps where!
  //unsigned long temptime = micros();
  snesRegister = readSnesController();
  //Serial.println(micros() - temptime);
  // Slight delay before we debug what was pressed so we don't spam the
  // serial monitor.
  //unsigned long temptime = micros();
  //__builtin_avr_delay_cycles(180*16);  // 180us
  //Serial.println(micros() - temptime);
  //delay(200);
  //Serial.println(snesRegister, DEC);

  // If any of these bits are set that means the controller is not present (the pin has a pull down, the controller pulls them high)
  if (!bitRead(snesRegister, NONE1_BUTTON) || !bitRead(snesRegister, NONE2_BUTTON) || !bitRead(snesRegister, NONE3_BUTTON) || !bitRead(snesRegister, NONE4_BUTTON)) {
    //Serial.println("no controller connected");
    
  } else {
    // To give you an idea on how to use this data to control things for your
    // next project, look through the serial terminal code below. Basically,
    // just choose a bit to look at and decide what to do whether 1 (not pushed)
    // or 0 (pushed). What is nice about this test code is that we mapped all
    // of the bits to the actual button name so no useless memorizing!

    if (!bitRead(snesRegister, A_BUTTON))
      Serial.println("A");

    if (!bitRead(snesRegister, B_BUTTON))
      Serial.println("B");

    if (!bitRead(snesRegister, START_BUTTON))
      Serial.println("start");

    if (!bitRead(snesRegister, SELECT_BUTTON))
      Serial.println("select");

    if (!bitRead(snesRegister, UP_BUTTON))
      Serial.println("up");

    if (!bitRead(snesRegister, DOWN_BUTTON))
      Serial.println("down");

    if (!bitRead(snesRegister, LEFT_BUTTON))
      Serial.println("left");

    if (!bitRead(snesRegister, RIGHT_BUTTON))
      Serial.println("right");

    if (!bitRead(snesRegister, X_BUTTON))
      Serial.println("X");

    if (!bitRead(snesRegister, Y_BUTTON))
      Serial.println("Y");

    if (!bitRead(snesRegister, L_BUTTON))
      Serial.println("L");

    if (!bitRead(snesRegister, R_BUTTON))
      Serial.println("R");
  }

}

//===============================================================================
//  Functions
//===============================================================================
///////////////////////
// readSnesController //
///////////////////////
uint16_t readSnesController()
{
  // Pre-load a variable with all 0's which assumes all buttons are not
  // pressed. But while we cycle through the bits, if we detect a 1
  // we set that bit. In the end, we find all the buttons states at once.
  uint16_t tempData = 0;

  // Quickly pulse the nesLatch pin so that the register grab what it see on
  // its parallel data pins.
  latch_on();
  latch_off();
  // Upon latching, the first bit is available to look at, which is the state
  // of the B button. We see if it is high, and if it is, we set the variable's
  // first bit to indicate this is so.
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) >> 4;
  // Clock the next bit which is the Y button and determine its state just like
  // we did above.
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) >> 3;
  // Now do this for the rest of them!

  // Select button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) >> 2;

  // Start button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) >> 1;

  // Up button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10);

  // Down button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 1;

  // Left button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 2;

  // Right button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 3;

  // A button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 4;

  // X button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 5;

  // L button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 6;

  // R button
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 7;

  // constantly tied high
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 8;

  // constantly tied high
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 9;

  // constantly tied high
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 10;

  // constantly tied high
  clock_on();
  clock_off();
  __builtin_avr_delay_cycles(1);  // 1/16us
  tempData |= (PIND & 0x10) << 11;

  // After all of this, we now have our variable all bundled up
  // with all of the SNES button states.*/
  return tempData;
}
