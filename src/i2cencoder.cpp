#include <Wire.h>
#include <i2cEncoderLibV2.h>

#include "mymenu.h"

const int IntPin = PIN_PC3;

i2cEncoderLibV2 Encoder(0x01); /* A0 is soldered */

//Callback when the CVAL is incremented
void encoder_increment(i2cEncoderLibV2* obj) {
  menu->knob_left();
  Serial.print(F("Increment: "));
  Serial.println(Encoder.readCounterByte());
}

//Callback when the CVAL is decremented
void encoder_decrement(i2cEncoderLibV2* obj) {
  menu->knob_right();
  Serial.print(F("Decrement: "));
  Serial.println(Encoder.readCounterByte());
}

//Callback when CVAL reach MAX
void encoder_max(i2cEncoderLibV2* obj) {
  Serial.print(F("Maximum threshold: "));
  Serial.println(Encoder.readCounterByte());
}

//Callback when CVAL reach MIN
void encoder_min(i2cEncoderLibV2* obj) {
  Serial.print(F("Minimum threshold: "));
  Serial.println(Encoder.readCounterByte());
}

//Callback when the encoder is pushed
void encoder_push(i2cEncoderLibV2* obj) {
  menu->button_select();
  Serial.println(F("Encoder is pushed!"));
}

//Callback when the encoder is released
void encoder_released(i2cEncoderLibV2* obj) {
  Serial.println(F("Encoder is released"));
}

//Callback when the encoder is double pushed
void encoder_double_push(i2cEncoderLibV2* obj) {
  Serial.println(F("Encoder is double pushed!"));
}

void setup_encoder() {
    pinMode(IntPin, INPUT);

    Encoder.reset();

    Encoder.begin(
        i2cEncoderLibV2::INT_DATA | i2cEncoderLibV2::WRAP_ENABLE
        | i2cEncoderLibV2::DIRE_LEFT | i2cEncoderLibV2::IPUP_ENABLE
        | i2cEncoderLibV2::RMOD_X1 | i2cEncoderLibV2::RGB_ENCODER);

    Encoder.writeCounter((int32_t) 0); /* Reset the counter value */
    //Encoder.writeMax((int32_t) 10); /* Set the maximum threshold*/
    //Encoder.writeMin((int32_t) - 10); /* Set the minimum threshold */
    Encoder.writeStep((int32_t) 1); /* Set the step to 1*/
    Encoder.writeInterruptConfig(0xff); /* Enable all the interrupt */
    Encoder.writeAntibouncingPeriod(20); /* Set an anti-bouncing of 200ms */
    Encoder.writeDoublePushPeriod(50); /*Set a period for the double push of 500ms */

    // Definition of the events
    Encoder.onIncrement = encoder_increment;
    Encoder.onDecrement = encoder_decrement;
    Encoder.onMax = encoder_max;
    Encoder.onMin = encoder_min;
    Encoder.onButtonPush = encoder_push;
    Encoder.onButtonRelease = encoder_released;
    Encoder.onButtonDoublePush = encoder_double_push;

    /* Enable the I2C Encoder V2 interrupts according to the previus attached callback */
    Encoder.autoconfigInterrupt();
}

void update_encoder() {
    if (digitalRead(IntPin) == LOW) {
      /* Check the status of the encoder and call the callback */
      Encoder.updateStatus();
    }
}