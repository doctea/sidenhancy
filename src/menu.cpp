#include "Config.h"
//#include "storage.h"
#ifdef ENABLE_SCREEN

#define TFT_SSOLED

//#include "tft.h"
#include "io.h"
#include "mymenu.h"
#include "menu.h"
#include "menuitems.h"

DisplayTranslator *tft;// = DisplayTranslator_SS_OLED();
Menu *menu;// = Menu(&tft);

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

MenuItem test_item_1 = MenuItem("test 1");
MenuItem test_item_2 = MenuItem("test 2");
MenuItem test_item_3 = MenuItem("test 3");

void setup_menu() {
    /*menu.add(&posbar);
    //menu.add(&mpk49_looper);
    menu.add(&beatstep_notes);
    menu.add(&bass_transpose_control);  // beatstep transposed to neutron control
    menu.add(&sequencer_status);
    menu.add(&mpk49_looper_status);
    menu.add(&quantizer_setting);       // todo: make this part of the LooperStatus object
    menu.add(&looper_harmony_status);   // todo: make this part of the LooperStatus object
    menu.add(&transpose_control);
    menu.add(&usbdevices_panel);*/

    pinMode(PIN_BUTTON_A, INPUT_PULLUP);
    #ifdef PIN_BUTTON_B
        pinMode(PIN_BUTTON_B, INPUT_PULLUP);
    #endif
    #ifdef PIN_BUTTON_C
        pinMode(PIN_BUTTON_C, INPUT_PULLUP);
    #endif

    //menu = Menu(tft);        

    Serial.println("Instantiating DisplayTranslator_SS_OLED..");
    tft = new DisplayTranslator_SS_OLED();
    delay(50);
    Serial.println("Finished DisplayTranslator_SS_OLED constructor");
    Serial.flush();
    Serial.println("Creating Menu object..");
    Serial.flush();
    menu = new Menu(tft);
    Serial.println("Created Menu object..");
    Serial.flush();

    Serial.println("Adding test menu items..");
    menu->add(&test_item_1);
    menu->add(&test_item_2);
    menu->add(&test_item_3);
    Serial.println("Added test menu items!");

}

#endif