#include "sid6581.h"

#include "ads.h"

#include "FrequencyParameter.h"
#include "ToggleParameter.h"

//#include "ParameterInput.h"
#include "ADSParameterInput.h"

#include "mymenu.h"
#include "ParameterMenuItems.h"
#include "ToggleMenuItems.h"

#include "LinkedList.h"

// Parameters, ie wrappers around destination object
LinkedList<DataParameter*>      available_parameters    = LinkedList<DataParameter*>();

DataParameter       param_none                              = DataParameter("None");
FrequencyParameter  param_overall_pitch                     = FrequencyParameter<SID6581,double> ("Overall pitch", &sid, &SID6581::setAllFrequency);
Parameter           param_overall_pulsewidth_modulation     = Parameter<SID6581,double> ("Overall PW Mod", &sid, &SID6581::modulateAllPulseWidths);
Parameter           param_overall_pitch_modulation          = Parameter<SID6581,double> ("Overall Pitch Mod", &sid, &SID6581::modulateAllPitches);

Parameter           param_filter_cutoff                     = Parameter<SID6581,double> ("Filter cutoff", &sid, &SID6581::setCutoff);
Parameter           param_filter_resonance                  = Parameter<SID6581,double> ("Filter res", &sid, &SID6581::setResonanceD);

/*FrequencyParameter  param_osc_1_pitch                       = FrequencyParameter<Voice,double> ("Osc 1 pitch", &sid.voice[0], &Voice::setFrequency);
FrequencyParameter  param_osc_2_pitch                       = FrequencyParameter<Voice,double> ("Osc 2 pitch", &sid.voice[1], &Voice::setFrequency);
FrequencyParameter  param_osc_3_pitch                       = FrequencyParameter<Voice,double> ("Osc 3 pitch", &sid.voice[2], &Voice::setFrequency);*/

/*Parameter           param_osc_1_waveforms                   = Parameter<Voice,double> ("Osc 1 wave", &sid.voice[0], &Voice::setOsc);
Parameter           param_osc_2_waveforms                   = Parameter<Voice,double> ("Osc 2 wave", &sid.voice[1], &Voice::setOsc);
Parameter           param_osc_3_waveforms                   = Parameter<Voice,double> ("Osc 3 wave", &sid.voice[2], &Voice::setOsc);*/

void add_voice_parameters(LinkedList<DataParameter*> *available_parameters, int voice_number) {
    char label[20] = "            ";
    Voice *voice = &sid.voice[voice_number];
    ToggleParameter<Voice,bool> *param;

    sprintf(label, "Osc %i %s", voice_number+1, "Triangle");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::triMask), &Voice::triOn, &Voice::triOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Pulse");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::pulseMask), &Voice::pulseOn, &Voice::pulseOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Saw");    
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::sawMask), &Voice::sawOn, &Voice::sawOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Noise");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::noiseMask), &Voice::noiseOn, &Voice::noiseOff);    
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Pitch");
    FrequencyParameter<Voice,double> *param_freq = new FrequencyParameter<Voice,double> (label, voice, &Voice::setFrequency);
    available_parameters->add(param_freq);

    sprintf(label, "Osc %i %s", voice_number+1, "Pitch Mod");
    DataParameter *param3 = new Parameter<Voice,double> (label, voice, &Voice::modulatePulseWidth);
    available_parameters->add(param3);

    sprintf(label, "Osc %i %s", voice_number+1, "Freq mult");
    DataParameter *param2 = new Parameter<Voice,double> (label, voice, &Voice::setFrequencyMultiplier);
    param2->minimum_value = 0.0;
    param2->maximum_value = 400; // actuall 4x?
    available_parameters->add(param2);
}

// ParameterInputs, ie wrappers around input mechanism, assignable to a Parameter
LinkedList<BaseParameterInput*> available_inputs            = LinkedList<BaseParameterInput*>();

/*ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0); //&param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,Parameter<SID6581,double>> input_C = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0); //&param_filter_cutoff);*/
ADSParameterInput<ADS1115,BaseParameter> input_A = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_B = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0); //&param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,BaseParameter> input_C = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0); //&param_filter_cutoff);
ADSParameterInput<ADS1115,BaseParameter> input_D = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 0); //&param_filter_cutoff);

/*ADSParameterInput<ADS1115,Parameter<Voice,double>> input_D = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_1_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>> input_E = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_2_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>> input_F = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &param_osc_3_pitch);*/

ADSParameterInput<ADS1115,BaseParameter> input_E = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1, &param_overall_pitch_modulation); //, &param_osc_1_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_F = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1); //, &param_osc_2_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_G = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1); //, &param_osc_3_pitch);
ADSParameterInput<ADS1115,BaseParameter> input_H = ADSParameterInput<ADS1115,BaseParameter>(&ADS_OBJECT, 1); //, &param_osc_3_pitch);

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
    available_parameters.add(&param_filter_resonance);
    /*available_parameters.add(&param_osc_1_pitch);
    available_parameters.add(&param_osc_2_pitch);
    available_parameters.add(&param_osc_3_pitch);*/

    /*available_parameters.add(&param_osc_1_waveforms);
    available_parameters.add(&param_osc_2_waveforms);
    available_parameters.add(&param_osc_3_waveforms);*/

    /*available_parameters.add(&param_osc_1_triangle);
    available_parameters.add(&param_osc_1_saw);
    available_parameters.add(&param_osc_1_pulse);*/

    //param_overall_pitch.debug = true;

    add_voice_parameters(&available_parameters, 0);
    add_voice_parameters(&available_parameters, 1);
    add_voice_parameters(&available_parameters, 2);    

    input_D.setInverted();
    input_H.setInverted();

    input_D.map_unipolar = true;    

    available_inputs.add(&input_A);
    available_inputs.add(&input_B);
    available_inputs.add(&input_C);
    available_inputs.add(&input_D);
    available_inputs.add(&input_E);
    available_inputs.add(&input_F);
    available_inputs.add(&input_G);        
    available_inputs.add(&input_H);

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
        BaseParameterInput *param_input = available_inputs.get(i);
        BaseParameter *param = param_input->target_parameter;

        char input_label[20];
        sprintf(input_label, "Input %i => %c", i<4?0:1, i+'A');

        Serial.printf("%i: Creating control labelled '%s'...\n", i, input_label);
        ParameterSelectorControl *ctrl = new ParameterSelectorControl(input_label);

        Serial.printf("%i:\tConfiguring it with available_inputs item target '%s'..\n", i, param->label);
        //BaseParameterInput *param_input = available_inputs.get(i);
        ctrl->configure(param_input, &available_parameters);

        Serial.println("\tConfigured!  Adding to menu..");
        menu->add(ctrl);
        Serial.println("\tAdded!");
    }

    Serial.println("Starting add available_parameters loop..."); Serial.flush();
    for (int i = 0 ; i < available_parameters.size() ; i++) {
        DataParameter *p = available_parameters.get(i);
        Serial.printf("\tinstantiating ParameterMenuItem for item %i aka %s\n", i, p->label); Serial.flush();
        MenuItem *ctrl = p->makeControl();
        Serial.printf("\tadding it to menu...\n", i, p->label); Serial.flush();
        menu->add(ctrl);
    }
    Serial.println("Finished adding available_parameters loop"); Serial.flush();

    Serial.println("setup_parameter_menu done ==================");
}
