#include "Arduino.h"

#include "Config.h"

#include "sid6581.h"
#include "ads.h"

//#include "display_ss.h"
#ifdef ENABLE_SCREEN
  //#include "io.h"
  #include "mymenu.h"
  //#include "menu_io.h"
  void setup_menu();
#endif

//#include "mtof.h"

SID6581 sid = SID6581();

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n\n======================= sidenhancy starting up! >>>>>>>>>>>>>>>>>>>>>\n");
    //while (!Serial);
    // todo: MIDI on Serial1

    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_LED, HIGH);

    Serial.println("\tInitialising sid..");
    sid.setup();
    /*sid.voice[0].pulseOn();
    sid.voice[1].pulseOn();
    sid.voice[2].pulseOn();
    sid.voice[0].sawOn();
    sid.voice[2].sawOn();*/
    Serial.println("\tsid initialised!");

    setup_ads();

    #ifdef ENABLE_SCREEN
      Serial.println("\tSetting up display..");
      setup_menu();
      //setup_display();
      Serial.println("\tFinished initialising display!");
      //tft_print("hello", 0, 0);
    #endif
    
    Serial.println("exiting setup()\n\n");
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
  //Serial.println("adc_direct_freq_loop().."); Serial.flush();
  int freq = get_frequency_for_voltage(read_voltage(0));
  //Serial.printf("Read channel 0: %i\n", freq); Serial.flush();
  //int pulseWidth = freq; // read_voltage(1);
  //Serial.printf("Read channel 1: %i\n", pulseWidth); Serial.flush();

  if (freq!=last_freq) {
    //Serial.printf("got freq %i\n", freq);
    /*sid.playNote(0,pitch);
    sid.playNote(1,pitch);
    sid.playNote(2,pitch);*/
    
    sid.voice[0].setADSR(1,1,15,1);
    sid.voice[0].gateOn(); 
    sid.voice[0].setFrequency(freq); 
    //sid.voice[0].setPulseWidth(pulseWidth);
    sid.voice[0].setPulseWidth(freq);
    sid.voice[1].setADSR(1,1,15,1);
    sid.voice[1].gateOn(); 
    sid.voice[1].setFrequency(freq); 
    sid.voice[1].setPulseWidth(freq);
    sid.voice[2].setADSR(1,1,15,1);
    sid.voice[2].gateOn();
    sid.voice[2].setFrequency(freq);
    sid.voice[2].setPulseWidth(freq);
  }
}

void note_loop() {
    //sid.test_tones();
    Serial.println("Looped!"); Serial.flush();
    static int value = 33;

    if (value>=80)
      value = 33;

    Serial.printf("Sounding chord %i...\n", value); Serial.flush();

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
    Serial.printf("Stopping note %i.\n", value);
    sid.stopNote(0);
    sid.stopNote(1);
    sid.stopNote(2);
    delay(3000);

    value += 1;
    //sid.test_tones();
}

void led_on() {
  digitalWrite(PIN_LED, HIGH);
}
void led_off() {
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  bool debug = false;
  static int mode = 0;
  static bool paused = false;

  if (debug) {  Serial.println("Start loop()!"); Serial.flush(); }

  static unsigned long last_drawn;
  if (millis() - last_drawn > 50) {
    if (debug) { Serial.println("updating inputs().."); Serial.flush(); }
    menu->update_inputs();
    if (debug) { Serial.println("updating display()!"); Serial.flush(); }
    menu->display();
    if (debug) { Serial.println("done display()!"); Serial.flush(); }
    last_drawn = millis();
  }

  while(Serial.available()) {
    char i = Serial.read();
    Serial.println(i);
    /*if (i=='d') {
      tft_print("test?", random(40), random(10));
    }*/
    if (i=='#') {
      sid.printStatus();
      return;
    }
    if (i=='P') {
      paused = !paused;
      if (paused) Serial.println("Paused");
      return;
    }
    if (i=='S') {
      if (sid.resonance==0)
        sid.setResonance(MAX_RESONANCE);
      else
        sid.setResonance(0);
      return;
    }
    if (i=='F') {
      Serial.println("Setting filtervoices"); // and random oscmask");
      for (int i = 0 ; i < 3 ; i++) {
        //sid.voice[i].setOscMask(random(15));
        sid.setFilterVoice(i,false);
      }
      sid.updateFilterVoice();
      return;
    }
    if (i=='f') {
      Serial.println("Unsetting filterVoice");
      for (int i = 0 ; i < 3 ; i++) {
        sid.unsetFilterVoice(i,false);
      }
      sid.updateFilterVoice();
      return;
    }
    if (i=='G') {
      Serial.println("Setting random filter types"); // and random oscmask");
      sid.setFilterType(random(15)<<4,false);
      sid.unsetFilterType(random(15)<<4);
      /*for (int i = 0 ; i < 3 ; i++) {
        //sid.voice[i].setOscMask(random(15));
        sid.setFilterType(i,false);
      }
      sid.updateFilterVoice();
      return;*/
    }
    if (i=='f') {
      Serial.println("Unsetting filter types");
      for (int i = 0 ; i < 4 ; i++) {
        sid.unsetFilterType(1 << i,false);
      }
      sid.updateFilterType();
      return;
    }
    if (i=='Q') {
      sid.setResonance(15);
      for (int i = MAX_CUTOFF ; i > 0 ; i--) {
        sid.setCutoff(i);
        //Serial.printf("Cutoff=%i\n",i);
        //delay(1);
      }
    }
    if(i=='+') {
      if (sid.cutoff>=MAX_CUTOFF) sid.cutoff = MAX_CUTOFF/2;
      sid.setCutoff(sid.getCutoff()+1);
      return;
    }
    if(i=='-') {
      if (sid.cutoff<=0) sid.cutoff = MAX_CUTOFF;
      sid.setCutoff(sid.getCutoff()-1);
      return;
    }
    if (i=='U') {
      bool orig_debug = debug_sid;
      Serial.println("calling sid->updateAll()");
      debug_sid = true;

      sid.updateAll();
      debug_sid = orig_debug;
      Serial.println("---");
    }
    if (i=='_') {
      debug_sid = !debug_sid;
      Serial.printf("debug set to %i\n------\n", debug_sid);
      return;
    }
    if (i=='M') {
      static int oscMask = 1;
      oscMask = oscMask << 1;
      if (oscMask>15)
        oscMask = 1;
      sid.voice[0].setOscMask(oscMask<<4);
      return;
    }
    if (i=='E') {
      Serial.println("test_tones:");
      //hw.write(23,0xF1); //1ch filter
      //hw.write(24,0x0F); //volume
      /*sid.setCutoff(MAX_CUTOFF);
      sid.setFilterVoice(0);
      sid.setResonance(0);
      sid.unsetFilterType(0x0F);
      sid.setFilterType(filter_lowpass);*/
      //sid.updateAll();
      sid.test_tones();
      Serial.println("finished test_tones!");
      return;
    }
    if (i=='R') {
      Serial.println("Resetting..");
      sid.resetChip();
      sid.voice[0].setADSR(0,0,15,5);
      sid.voice[1].setADSR(0,0,15,5);
      sid.voice[2].setADSR(0,0,15,5);
      sid.voice[0].setFrequency(MAX_FREQ/2);
      sid.voice[1].setFrequency(MAX_FREQ/2);
      sid.voice[2].setFrequency(MAX_FREQ/2);
      sid.unsetFilterType(triMask | sawMask | pulseMask);
      sid.unsetFilterVoice(0,false);
      sid.unsetFilterVoice(1,false);
      sid.unsetFilterVoice(2,false);
      sid.unsetFilterVoice(3,false);
      sid.updateAll();
      led_on();
      sid.voice[0].gateOn();
      sid.voice[1].gateOn();
      sid.voice[2].gateOn();
      delay(1000);
      led_off();
      sid.voice[0].gateOff();
      sid.voice[1].gateOff();
      sid.voice[2].gateOff();
      delay(1000);
      Serial.println("Reset and should have beeped?");
      return;
    }
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
    if (i=='K') {
      sid.voice[0].stopNote();
      sid.voice[1].stopNote();
      sid.voice[2].stopNote();
    } else if (i=='k') {
      sid.voice[0].gateOn();
      sid.voice[1].gateOn();
      sid.voice[2].gateOn();
    }
  }

  if (paused) return;

  if (mode==0) 
    adc_direct_freq_loop();
  else if (mode==1)
    adc_loop();
  else if (mode==2)
    note_loop();
  else 
    Serial.printf("unknown mode %i?\n", mode);
}
