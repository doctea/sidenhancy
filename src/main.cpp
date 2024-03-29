#include "Arduino.h"

#include "Config.h"

#include "sid6581.h"

#include "sid_parameters.h"

#ifdef ENABLE_STORAGE
  #include "storage.h"
#endif

#include "ads.h"

//#include "display_ss.h"
#ifdef ENABLE_SCREEN
  #include "i2cencoder.h"
  //#include "io.h"
  #include "mymenu.h"
  //#include "menu_io.h"
  void setup_menu();
#endif

//#include "mtof.h"

SID6581 sid = SID6581();

void setup() {
    Serial.begin(115200);
    Serial.println(F("\n\n\n======================= sidenhancy starting up! >>>>>>>>>>>>>>>>>>>>>\n"));
    #ifdef WAIT_FOR_SERIAL
      while (!Serial);
    #endif
    // todo: MIDI on Serial1

    pinMode(PIN_LED, OUTPUT);

    digitalWrite(PIN_LED, HIGH);

    Serial.println(F("\tInitialising sid.."));
    sid.setup();
    /*sid.voice[0].pulseOn();
    sid.voice[1].pulseOn();
    sid.voice[2].pulseOn();
    sid.voice[0].sawOn();
    sid.voice[2].sawOn();*/
    Serial.println(F("\tsid initialised!"));

    //setup_ads();
    setup_voltage_sources();

    #ifdef ENABLE_SCREEN
      Serial.println(F("\tSetting up display.."));
      setup_menu();
      //setup_display();
      Serial.println(F("\tFinished initialising display!"));
      //tft_print("hello", 0, 0);
    #endif
    
    Serial.println(F("Setting up parameters.."));
    setup_parameters();
    Serial.println(F("Parameter setup done!"));

    #ifdef ENABLE_STORAGE
      setup_storage();
    #endif

    //save_parameter_settings(&available_parameters, &available_inputs, DEFAULT_SLOT);
    #ifdef LOAD_PARAMETER_SETTINGS
      load_parameter_settings(&available_parameters, &available_inputs, 0); // restore previous settings
    #endif

    #ifdef ENABLE_SCREEN
      setup_parameter_menu();
    #endif

    sid.allGateOn();  // turn on all gates so that we can use this like an oscillator

    Serial.println(F("exiting setup()\n\n"));
}

void note_loop() {
    //sid.test_tones();
    Serial.println(F("Looped!")); Serial.flush();
    static int value = 33;

    //return;

    if (value>=80)
      value = 33;

    Serial.printf(F("Sounding chord %i...\n"), value); Serial.flush();

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
    Serial.printf(F("Stopping note %i.\n"), value);
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
  bool debug = true;
  static int mode = 0;
  static bool paused = false;

  if (debug) {  Serial.println(F("Start loop()!")); Serial.flush(); }

  if (debug) { Serial.println(F("updating inputs()..")); Serial.flush(); }
  #ifdef ENABLE_SCREEN
    menu->update_inputs();
    update_encoder();
  #endif

  update_parameters();

  #ifdef ENABLE_SCREEN
    static unsigned long last_drawn = millis();
    if (millis() - last_drawn > 50) {
      if (debug) { Serial.println(F("updating display()!"));  Serial.flush(); }
      menu->display();
      if (debug) { Serial.println(F("done display()!"));      Serial.flush(); }
      last_drawn = millis();
    }
  #endif

  //static unsigned long last_milli = 0;
  //if (millis()!=last_milli) {
    update_voltage_sources();
    //last_milli = millis();
  //}

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
      if (paused) Serial.println(F("Paused"));
      return;
    }
    if (i=='S') {
      if (sid.resonance==0)
        sid.setResonance((byte)MAX_RESONANCE);
      else
        sid.setResonance((byte)0);
      return;
    }
    if (i=='F') {
      Serial.println(F("Setting filtervoices")); // and random oscmask");
      for (int i = 0 ; i < 3 ; i++) {
        //sid.voice[i].setOscMask(random(15));
        sid.setFilterVoice(i,false);
      }
      sid.updateFilterVoice();
      return;
    }
    if (i=='f') {
      Serial.println(F("Unsetting filterVoice"));
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
      sid.setResonance((byte)15);
      for (int i = MAX_CUTOFF ; i > 0 ; i--) {
        sid.setCutoff((uint16_t)i);
        //Serial.printf("Cutoff=%i\n",i);
        //delay(1);
      }
    }
    if (i=='M') {
      static int oscMask = 1;
      static int oscNum = 0;
      sid.voice[oscNum].unsetOscMask(oscMask<<4);
      oscMask = oscMask << 1;
      if (oscMask>15)
        oscMask = 1;
      oscNum++; oscNum %= 3;
      Serial.printf(F("setting osc %i to oscMask %i\n"), oscNum, oscMask);
      sid.voice[oscNum].setOscMask(oscMask<<4);
      return;
    }
    if(i=='+') {
      if (sid.cutoff>=MAX_CUTOFF) sid.cutoff = MAX_CUTOFF/2;
      sid.setCutoff((uint16_t)(sid.getCutoff()+1));
      return;
    }
    if(i=='-') {
      if (sid.cutoff<=0) sid.cutoff = MAX_CUTOFF;
      sid.setCutoff((uint16_t)(sid.getCutoff()-1));
      return;
    }
    if (i=='U') {
      bool orig_debug = debug_sid;
      Serial.println(F("calling sid->updateAll()"));
      debug_sid = true;

      sid.updateAll();
      debug_sid = orig_debug;
      Serial.println(F("---"));
    }
    if (i=='_') {
      debug_sid = !debug_sid;
      Serial.printf(F("debug set to %i\n------\n"), debug_sid);
      return;
    }
    if (i=='E') {
      Serial.println(F("test_tones:"));
      //hw.write(23,0xF1); //1ch filter
      //hw.write(24,0x0F); //volume
      /*sid.setCutoff(MAX_CUTOFF);
      sid.setFilterVoice(0);
      sid.setResonance(0);
      sid.unsetFilterType(0x0F);
      sid.setFilterType(filter_lowpass);*/
      //sid.updateAll();
      sid.test_tones();
      Serial.println(F("finished test_tones!"));
      return;
    }
    if (i=='1') sid.voice[0].gateOff();
    if (i=='!') sid.voice[0].gateOn();
    if (i=='2') sid.voice[1].gateOff();
    if (i=='"') sid.voice[1].gateOn();
    if (i=='3') sid.voice[2].gateOff();
    if (i=='4') sid.voice[2].gateOn();

    #ifdef ENABLE_SCREEN
      if (i=='0') setup_encoder();
    #endif

    if (i=='R') {
      Serial.println(F("Resetting.."));
      sid.resetChip();
      sid.voice[0].setADSR(0,0,15,5);
      sid.voice[1].setADSR(0,0,15,5);
      sid.voice[2].setADSR(0,0,15,5);
      sid.voice[0].setFrequency(MAX_FREQ/2,true);
      sid.voice[1].setFrequency(MAX_FREQ/2,true);
      sid.voice[2].setFrequency(MAX_FREQ/2,true);
      //sid.unsetFilterType(triMask | sawMask | pulseMask);
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
      Serial.println(F("Reset and should have beeped?"));
      return;
    }
    if (i=='D') {
      mode = 0;
      Serial.println(F("Switching to Direct mode"));
    } else if (i=='T') {
      mode = 1;
      Serial.println(F("Switching to Table mode"));
    } else if (i=='N') {
      mode = 2;
      Serial.println(F("Switching to Note loop mode"));
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
}
