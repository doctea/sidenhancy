#include "sid6581.h"

#include "ads.h"

#include "FrequencyParameter.h"
//#include "ParameterInput.h"
#include "ADSParameterInput.h"

#include "mymenu.h"
#include "ParameterMenuItems.h"

#include "LinkedList.h"

// Parameters, ie wrappers around destination object
LinkedList<BaseParameter*>      available_parameters    = LinkedList<BaseParameter*>();

BaseParameter       param_none                              = BaseParameter("None");
FrequencyParameter  param_overall_pitch                     = FrequencyParameter<SID6581,double> ("Overall pitch", &sid, &SID6581::setAllFrequency);
Parameter           param_overall_pulsewidth_modulation     = Parameter<SID6581,double> ("Overall PW Mod", &sid, &SID6581::modulateAllPulseWidths);
Parameter           param_overall_pitch_modulation          = Parameter<SID6581,double> ("Overall Pitch Mod", &sid, &SID6581::modulateAllPitches);

FrequencyParameter  param_osc_1_pitch                       = FrequencyParameter<Voice,double> ("Osc 1 pitch\0", &sid.voice[0], &Voice::setFrequency);
FrequencyParameter  param_osc_2_pitch                       = FrequencyParameter<Voice,double> ("Osc 2 pitch\0", &sid.voice[1], &Voice::setFrequency);
FrequencyParameter  param_osc_3_pitch                       = FrequencyParameter<Voice,double> ("Osc 3 pitch\0", &sid.voice[2], &Voice::setFrequency);
Parameter           param_filter_cutoff                     = Parameter<SID6581,double> ("Filter cutoff\0", &sid, &SID6581::setCutoff);


// ParameterInputs, ie wrappers around input mechanism, assignable to a Parameter
LinkedList<BaseParameterInput*> available_inputs            = LinkedList<BaseParameterInput*>();

/*ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0); //&param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_C = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0); //&param_filter_cutoff);*/
ADSParameterInput<ADS1115,BaseParameter> input_A = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_B = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0); //&param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,BaseParameter> input_C = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0); //&param_filter_cutoff);

/*ADSParameterInput<ADS1115,Parameter<Voice,double>> input_D = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_1_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>> input_E = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_2_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>> input_F = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_3_pitch);*/

ADSParameterInput<ADS1115,BaseParameter> input_D = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1, &param_overall_pitch_modulation); //, &param_osc_1_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_E = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1); //, &param_osc_2_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_F = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1); //, &param_osc_3_pitch);

// menu item for direct control over a Parameter via menu
//ParameterMenuItem PulseWidthModulationPanel("Pulse width mod", &param_overall_pulsewidth_modulation);

void setup_parameters() {
    Serial.println(F("==== begin setup_parameters ===="));
    //input_A.setTarget(&param_overall_pitch);
    //input_B.setTarget(&param_overall_pulsewidth_modulation);

    // add the available parameters to a list used globally and later passed to each selector menuitem 
    available_parameters.add(&param_none);
    available_parameters.add(&param_overall_pitch);
    available_parameters.add(&param_overall_pitch_modulation);
    available_parameters.add(&param_overall_pulsewidth_modulation);
    available_parameters.add(&param_filter_cutoff);
    available_parameters.add(&param_osc_1_pitch);
    available_parameters.add(&param_osc_2_pitch);
    available_parameters.add(&param_osc_3_pitch);

    //param_overall_pitch.debug = true;

    input_C.setInverted();
    input_F.setInverted();

    input_D.map_unipolar = true;    

    available_inputs.add(&input_A);
    available_inputs.add(&input_B);
    available_inputs.add(&input_C);
    available_inputs.add(&input_D);
    available_inputs.add(&input_E);
    available_inputs.add(&input_F);

    Serial.println("==== end setup_parameters ====");
}

// called every loop to update the inputs; maybe in future we also eventually want to process internal LFOs or other properties of parameter here
void update_parameters() {
    //Serial.println("update_parameters..");
    //input_A.loop();
    //input_B.loop();
    for (int i = 0 ; i < available_inputs.size() ; i++) {
        available_inputs.get(i)->loop();
        //parameter_inputs[i]->loop();
    }
}

// initialise the menus for the ParameterInputs
void setup_parameter_menu() {
    Serial.println("==== setup_parameter_menu starting ====");
    //menu->add(&PulseWidthModulationPanel);
    //menu->add(&testitem);

    Serial.println("Adding ParameterSelectorControls for available_inputs...");
    for (int i = 0 ; i < available_inputs.size() ; i++) {
        char input_label[20];
        sprintf(input_label, "Input %c=>", i+'A');

        Serial.printf("%i: Creating control labelled '%s'...\n", i, input_label);
        ParameterSelectorControl *ctrl = new ParameterSelectorControl(input_label);

        Serial.printf("%i:\tConfiguring it with available_inputs item target '%s'..\n", available_inputs.get(i)->target_parameter->label);
        BaseParameterInput *param_input = available_inputs.get(i);
        ctrl->configure(param_input, &available_parameters);

        Serial.println("\tConfigured!  Adding to menu..");
        menu->add(ctrl);
        Serial.println("\tAdded!");
    }
    Serial.println("setup_parameter_menu done ==================");

    for (int i = 0 ; i < available_parameters.size() ; i++) {
        BaseParameter *p = available_parameters.get(i);
        ParameterMenuItem *ctrl = new ParameterMenuItem(p->label, p);
        menu->add(ctrl);
    }
}
