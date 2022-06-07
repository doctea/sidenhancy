#include "Config.h"
//#include "storage.h"
#ifdef ENABLE_SCREEN

//#include "tft.h"
#include "io.h"
#include "mymenu.h"
#include "menu.h"
#include "menuitems.h"

#include "ads.h"

#include "i2cencoder.h"

#include "sid_parameters.h"

DisplayTranslator *tft;// = DisplayTranslator_SS_OLED();
Menu *menu;// = Menu(&tft);

/*#ifdef ENCODER_KNOB_L
    Encoder knob(ENCODER_KNOB_L, ENCODER_KNOB_R);
    //extern Encoder knob;
#endif
#ifdef PIN_BUTTON_A
    Bounce pushButtonA = Bounce(PIN_BUTTON_A, 10); // 10ms debounce
    //extern Bounce pushButtonA;
#endif*/

#ifdef PIN_BUTTON_B
    Bounce pushButtonB = Bounce(PIN_BUTTON_B, 10); // 10ms debounce
    //extern Bounce pushButtonB; 
#endif
#ifdef PIN_BUTTON_C
    Bounce pushButtonC = Bounce(PIN_BUTTON_C, 10); // 10ms debounce
    //extern Bounce pushButtonC;
#endif

/*PositionIndicator posbar = PositionIndicator();
//LooperStatus mpk49_looper = LooperStatus();
#ifdef ENABLE_BEATSTEP
    HarmonyStatus beatstep_notes =          HarmonyStatus("Beatstep harmony",   &last_beatstep_note,          &current_beatstep_note);
#endif
#ifdef ENABLE_BASS_TRANSPOSE
    NumberControl bass_transpose_control =  NumberControl("Bass octave", &bass_transpose_octave, bass_transpose_octave, 1, 4, &bass_transpose_changed);
#endif
#ifdef ENABLE_SEQUENCER
    SequencerStatus sequencer_status =      SequencerStatus();
#endif
#ifdef ENABLE_LOOPER
    LooperStatus    mpk49_looper_status =   LooperStatus();
    LooperQuantizeControl quantizer_setting = LooperQuantizeControl("Loop quant",   &mpk49_loop_track);   // todo: make this part of the LooperStatus object
    HarmonyStatus looper_harmony_status =   HarmonyStatus("Loop harmony",           &mpk49_loop_track.last_note, &mpk49_loop_track.current_note); // todo: make this part of the LooperStatus object
    LooperTransposeControl transpose_control =    LooperTransposeControl("Loop transpose",      &mpk49_loop_track); // todo: make this part of the LooperStatus object
#endif

#ifdef ENABLE_USB
    USBDevicesPanel usbdevices_panel = USBDevicesPanel();
#endif*/

#include "sid6581.h"

void change_filter_cutoff(int last_value, int new_value) {
    Serial.printf(F("change_filter_cutoff from %i to %i\n"), last_value, new_value);
    sid.setCutoff((uint16_t)new_value);
}

DirectNumberControl filter_cutoff_control("Filter cutoff", &sid.cutoff, sid.cutoff, 0, MAX_CUTOFF, change_filter_cutoff); //void (*on_change_handler)(int lastValue, int new_value)) 
//DirectNumberControl pulse_width_control("Pulse width", &sid.cutof, sid.cutoff, 0, MAX_CUTOFF, change_filter_cutoff); //void (*on_change_handler)(int lastValue, int new_value)) 
//NumberControl(const char* label, int *in_target_variable, int start_value, int min_value, int max_value, void (*on_change_handler)(int lastValue, int new_value)) 


// pulse width base level stuff
int all_pulsewidth = 0;
int getAllPulseWidth() {
    //return sid.voice[0].pulseWidth * 2048;
    return all_pulsewidth;
}
void setAllPulseWidth(int pw) {
    all_pulsewidth = pw;
    sid.setAllPulseWidths(pw / 2048.0f);
}
void change_pulsewidth(int new_value, int old_value) {}

DirectNumberControl pulse_width_control("Pulse width", getAllPulseWidth, setAllPulseWidth, 0, MAX_CUTOFF, change_pulsewidth);

//NumberControl CV1InputPanel("CV1 input", &ads_values[0], ads_values[0], 0, 4095, nullptr); //null_func);
//NumberControl CV2InputPanel("CV2 input", &ads_values[1], ads_values[1], 0, 4095, nullptr); //null_func);
/*MenuItem test_item_1 = MenuItem("test 1");
MenuItem test_item_2 = MenuItem("test 2");
MenuItem test_item_3 = MenuItem("test 3");*/

void setup_menu() {
    setup_encoder();

    #ifdef PIN_BUTTON_A
        pinMode(PIN_BUTTON_A, INPUT_PULLUP);
    #endif
    #ifdef PIN_BUTTON_B
        pinMode(PIN_BUTTON_B, INPUT_PULLUP);
    #endif
    #ifdef PIN_BUTTON_C
        pinMode(PIN_BUTTON_C, INPUT_PULLUP);
    #endif

    //menu = Menu(tft);        

    Serial.println(F("Instantiating DisplayTranslator_SS_OLED..")); Serial.flush();
    tft = new DisplayTranslator_SS_OLED();
    delay(50);
    Serial.println(F("Finished DisplayTranslator_SS_OLED constructor")); Serial.flush();
    Serial.println(F("Creating Menu object..")); Serial.flush();
    menu = new Menu(tft);
    Serial.printf(F("Done, free ram="));
    menu->debug_free_ram();
    Serial.println(F("Created Menu object..")); Serial.flush();

    filter_cutoff_control.setStep(32);
    pulse_width_control.setStep(256);

    //CV1InputPanel.setReadOnly(true);
    //CV2InputPanel.setReadOnly(true);

    //menu->add(&CV1InputPanel);
    //menu->add(&CV2InputPanel);

    /*Serial.println(F("Adding test menu items.."));
    menu->add(&test_item_1);
    menu->add(&test_item_2);
    menu->add(&test_item_3);
    Serial.println(F("Added test menu items!"));*/

    menu->add(&filter_cutoff_control);
    menu->add(&pulse_width_control);

    menu->mode = Menu::DISPLAY_ONE;

    setup_parameter_menu();

}

#endif