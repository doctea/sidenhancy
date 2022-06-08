#include "sid6581.h"

#include "ads.h"

#include "FrequencyParameter.h"
//#include "ParameterInput.h"
#include "ADSParameterInput.h"

#include "mymenu.h"
#include "ParameterMenuItems.h"
#include "menu_parameter_selector.h"

#include "LinkedList.h"

BaseParameter param_none                        = BaseParameter("None");

FrequencyParameter param_overall_pitch          = FrequencyParameter<SID6581,double> ("Overall pitch\0", &sid, &SID6581::setAllFrequency);
Parameter param_overall_pulsewidth_modulation   = Parameter<SID6581,double> ("Overall PW Mod\0", &sid, &SID6581::modulateAllPulseWidths);

FrequencyParameter param_osc_1_pitch          = FrequencyParameter<Voice,double> ("Osc 1 pitch\0", &sid.voice[0], &Voice::setFrequency);
FrequencyParameter param_osc_2_pitch          = FrequencyParameter<Voice,double> ("Osc 2 pitch\0", &sid.voice[1], &Voice::setFrequency);
FrequencyParameter param_osc_3_pitch          = FrequencyParameter<Voice,double> ("Osc 3 pitch\0", &sid.voice[2], &Voice::setFrequency);

Parameter param_filter_cutoff                   = Parameter<SID6581,double> ("Filter cutoff\0", &sid, &SID6581::setCutoff);

ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_C = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 1, &param_filter_cutoff);

ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_D = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_1_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_E = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_2_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_F = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_3_pitch);

//LinkedList<ParameterInput<BaseParameter>*> param_inputs = LinkedList<ParameterInput*>();
LinkedList<BaseParameterInput*> param_inputs = LinkedList<BaseParameterInput*>();
LinkedList<BaseParameter*>      available_parameters   = LinkedList<BaseParameter*>();

ParameterMenuItem PulseWidthModulationPanel("Pulse width mod", &param_overall_pulsewidth_modulation);
//MenuItem testitem("SID Params test");

void setup_parameters() {
    Serial.println(F("==== begin setup_parameters ===="));
    //input_A.setTarget(&param_overall_pitch);
    //input_B.setTarget(&param_overall_pulsewidth_modulation);

    available_parameters.add(&param_none);
    available_parameters.add(&param_overall_pitch);
    available_parameters.add(&param_overall_pulsewidth_modulation);
    available_parameters.add(&param_filter_cutoff);
    available_parameters.add(&param_osc_1_pitch);
    available_parameters.add(&param_osc_2_pitch);
    available_parameters.add(&param_osc_3_pitch);

    input_D.setInverted();
    input_F.setInverted();    

    param_inputs.add(&input_A);
    param_inputs.add(&input_B);
    param_inputs.add(&input_C);
    param_inputs.add(&input_D);
    param_inputs.add(&input_E);
    param_inputs.add(&input_F);

    Serial.println("==== end setup_parameters ====");
}

void update_parameters() {
    //Serial.println("update_parameters..");
    //input_A.loop();
    //input_B.loop();
    for (int i = 0 ; i < param_inputs.size() ; i++) {
        param_inputs.get(i)->loop();
        //parameter_inputs[i]->loop();
    }
}

void setup_parameter_menu() {
    Serial.println("==== setup_parameter_menu starting ====");
    menu->add(&PulseWidthModulationPanel);
    //menu->add(&testitem);

    Serial.println("Adding ParameterSelectorControls for param_inputs...");
    for (int i = 0 ; i < param_inputs.size() ; i++) {
        char input_label[20];
        sprintf(input_label, "Input %c=>", i+'A');
        Serial.printf("%i: Adding control labelled '%s'...\n", i, input_label);
        ParameterSelectorControl *ctrl = new ParameterSelectorControl(input_label);
        Serial.printf("%i:\tConfiguring it with param_inputs item target '%s'..\n", param_inputs.get(i)->target_parameter->label);
        BaseParameterInput *param_input = param_inputs.get(i);
        if (!param_input) {
            Serial.printf("\tDidn't get an item for index %i from param_inputs?  Yet size is %i?\n", i, param_inputs.size());
        }
        if (!param_input->target_parameter) {
            Serial.printf("\ttarget_parameter doesn't give a valid pointer? (%i)\n", param_input->target_parameter);
        }
        ctrl->configure(param_input);
        Serial.println("\tConfigured!  Adding..");
        menu->add(ctrl);
        Serial.println("\tAdded!");
    }
    Serial.println("setup_parameter_menu done ==================");
}

int find_parameter_index_for_label(char *label) {
    int size = available_parameters.size();
    for (int i = 0 ; i < size ; i++) {
        if (!strcmp(available_parameters.get(i)->label, label))
            return i;
    }
    Serial.printf("WARNING: find_parameter_index_for_label: didn't find one for %s?\n", label);
    return -1;
}

