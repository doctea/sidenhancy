#include "Arduino.h"

#include "sid6581.h"
#include "ads.h"

//#include "mtof.h"

SID6581 sid = SID6581();

void setup() {
    Serial.begin(115200);
    //while (!Serial);
    // todo: MIDI on Serial1

    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_LED, HIGH);

    Serial.println("eurosid starting up..");

    Serial.println("Initialising sid..");
    sid.setup();
    Serial.println("sid initialised");

    Serial.println("exiting setup()");

    setup_ads();

    //sid.test_tones();
    digitalWrite(PIN_LED, HIGH);
    delay(500);
    digitalWrite(PIN_LED, LOW);
}


void unused_loop() {
  /*sid.playNote(2, 38);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 39);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 40);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 41);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 42);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 43);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 44);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 45);
  delay(1000);
  sid.stopNote(2);
  delay(1000);
  sid.playNote(2, 46);
  delay(1000);
  sid.stopNote(2);
  delay(1000);*/

  sid.playNote(2,40);
  delay(1000);
  sid.playNote(2,41);
  delay(1000);
  sid.playNote(2,42);
  delay(1000);
  //sid.stopNote(2);
  //delay(1000);

}

void adc_loop() {
  static int last_pitch;
  int pitch = get_midi_pitch_for_voltage(read_voltage(0));
  if (pitch!=last_pitch) {
    Serial.printf("got pitch %i\n", pitch);
    sid.playNote(0,pitch);
    sid.playNote(1,pitch);
    sid.playNote(2,pitch);
  }
  last_pitch = pitch;
}

void adc_direct_freq_loop() {
  static int last_freq;
  int freq = get_frequency_for_voltage(read_voltage(0));
  if (freq!=last_freq) {
    //Serial.printf("got freq %i\n", freq);
    /*sid.playNote(0,pitch);
    sid.playNote(1,pitch);
    sid.playNote(2,pitch);*/
    
    sid.voice[0].setADSR(1,1,15,1);
    sid.voice[0].gateOn(); 
    sid.voice[0].setFrequency(freq); 
    sid.voice[1].setADSR(1,1,15,1);
    sid.voice[1].gateOn(); 
    sid.voice[1].setFrequency(freq); 
    sid.voice[2].setADSR(1,1,15,1);
    sid.voice[2].gateOn();
    sid.voice[2].setFrequency(freq);
  }
  last_freq = freq;
}

void clumsy_adc_loop() {

  //Serial.printf("starting read..");
  ADS.requestADC(0);
  //uint16_t value = constrain(ADS.readADC(0), 0, INT16_MAX);
  uint16_t value = ADS.getValue();

  float maxVoltage = ADS.getMaxVoltage();

  static uint16_t last_value;
  if (value!=last_value) {
    float voltageFromAdc = ADS.toVoltage(value);
    Serial.printf("voltage from value %i*1000\tis %i\t", value, voltageFromAdc*1000.0);

    if (voltageFromAdc>0.0) {
      //float scaled_volt = voltageFromAdc / maxVoltage;
      //float scaled_volt = voltageFromAdc;

      float pitch_offset = 24;
      //float max_voltage_of_adc = maxVoltage;
      //float voltage_division_ratio = 1.0;
      //float notes_per_octave = 12;
      //float volts_per_octave = 1;
      float scaler = 0.97;

      //float mapping_upper_limit = (maxVoltage / voltage_division_ratio) * notes_per_octave * volts_per_octave * scaler;

      //int pitch = pitch_offset + map(value, 0, INT16_MAX, 0.0, mapping_upper_limit);
      //int pitch = pitch_offset + map(scaled_volt, 0, 1.0, 0.0, mapping_upper_limit);
      
      int pitch = pitch_offset + (scaler * voltageFromAdc * 12.0);
      //float freq = mtof.toFrequency(pitch);

      Serial.printf("maxVoltage is %i :: \t", (int)(maxVoltage*100.0));
      //Serial.printf("mapping_upper_limit = %i\t :: ", (int)(mapping_upper_limit*100.0));
      Serial.printf("for value\t%i got pitch\t%i and voltage %i\n", value, pitch, (int)(voltageFromAdc*1000));

      //Serial.printf("sounding freq %i\n", value);
      //sid.voice[0].setADSR(0,0,15,0);
      //sid.voice[1].setADSR(0,0,15,0);
      //sid.voice[2].setADSR(0,0,15,0);
      sid.playNote(0,pitch);
      sid.playNote(1,pitch);
      sid.playNote(2,pitch);
      //sid.voice[0].gateOn();
      //sid.voice[1].gateOn();
      //sid.voice[2].gateOn();

      /*sid.voice[0].setFrequency(freq, true);
      sid.voice[1].setFrequency(freq, true);
      sid.voice[2].setFrequency(freq, true);*/
    }
  }
  last_value = value;



}

void note_loop() {
    //sid.test_tones();
    Serial.println("Looped!");
    static int value = 33;

    if (value>=80)
      value = 33;

    Serial.printf("Sounding note %i...", value);

    static int a,d,s,r;

    //seed(analogRead(A1));
    a = random(15);
    d = random(15);
    s = random(15);
    r = random(15);
    sid.voice[0].setADSR((a+d)%15,(d+s)%15,(s+r)%15,(r+a)%15);
    sid.voice[1].setADSR((d+d)%15,(a+s)%15,(r+r)%15,(s+a)%15);
    sid.voice[2].setADSR((a+r)%15,(d+r)%15,(s+a)%15,(r+d)%15);
    /*sid.voice[1].setADSR(a,d,s,r);
    sid.voice[2].setADSR((d+d)%15,(a+s)%15,(r+r)%15,(s+a)%15);
    sid.voice[3].setADSR((a+r)%15,(d+r)%15,(s+a)%15,(r+d)%15);*/

    digitalWrite(PIN_LED, HIGH);
    sid.playNote(0, value);
    sid.playNote(1, value+4);
    sid.playNote(2, value+7);
    delay(1000);

    digitalWrite(PIN_LED, LOW);
    Serial.printf("Stopping note %i.", value);
    sid.stopNote(0);
    sid.stopNote(1);
    sid.stopNote(2);
    delay(3000);

    value += 1;
    //sid.test_tones();
}


void loop() {
  static int mode = 0;

  if (Serial.available()) {
    char i = Serial.read();
    if (i=='D') {
      mode = 0;
      Serial.println("Switching to Direct mode");
    } else if (i=='T') {
      mode = 1;
      Serial.println("Switching to Table mode");
    } else if (i=='N') {
      mode = 2;
      Serial.println("Switching to Note loop mode");
    }
    sid.voice[0].stopNote();
    sid.voice[1].stopNote();
    sid.voice[2].stopNote();
  }

  if (mode==0) 
    adc_direct_freq_loop();
  else if (mode==1)
    adc_loop();
  else if (mode==2)
    note_loop();
}
