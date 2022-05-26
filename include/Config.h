
// indicator LED
#define PIN_LED     PIN_PD4

#define ENABLE_SCREEN
#define TFT_SSOLED


//#define ENCODER_USE_INTERRUPTS
#define PIN_ENCODER_BUTTON  PIN_PC5
#define ENCODER_KNOB_R      PIN_PC6 //PCINT22 //PIN_PC6     //PCINT24
#define ENCODER_KNOB_L      PIN_PC7 //PCINT23 //PIN_PC7     //PCINT23
#define PIN_BUTTON_A        PIN_ENCODER_BUTTON
#define PIN_BUTTON_B        PIN_PC4
#define ENCODER_STEP_DIVISOR    4   // needs to be 4 on usb_midi_clocker on teensy, 1 on my sid prototype, not sure why?