#define ENABLE_STORAGE



// indicator LED
#define PIN_LED     PIN_PD4

// i2c pins (unsure if these actually used anywhere?)
#define PIN_SDA     PIN_PC1
#define PIN_SCL     PIN_PC0

#define ENABLE_SCREEN

#define OLED_ADDR -1
//#define OLED_ADDR 0x78
#define TFT_SSOLED

#define ENABLE_ADS_24V
#define ENABLE_ADS_x48

#define ENABLE_ENCODER
#define PIN_BUTTON_B        PIN_PC7

//#define ENCODER_USE_INTERRUPTS
//#define PIN_ENCODER_BUTTON  PIN_PC5
//#define ENCODER_KNOB_R      PIN_PC6 //PCINT22 //PIN_PC6     //PCINT24
//#define ENCODER_KNOB_L      PIN_PC7 //PCINT23 //PIN_PC7     //PCINT23
//#define PIN_BUTTON_A        PIN_ENCODER_BUTTON
//#define ENCODER_STEP_DIVISOR    4   // needs to be 4 on usb_midi_clocker on teensy, 1 on my sid prototype, not sure why?