#include "sid6581.h"

#include "Config.h"

#include "ads.h"

#include "voltage_sources/VoltageSource.h"
#include "voltage_sources/ADSVoltageSource.h"
#include "voltage_sources/ADS24vVoltageSource.h"

#include "parameters/FrequencyParameter.h"
#include "parameters/ToggleParameter.h"

//#include "ParameterInput.h"
#include "parameter_inputs/VoltageParameterInput.h"

#include "mymenu.h"
#include "mymenu_items/ParameterMenuItems.h"
#include "mymenu_items/ToggleMenuItems.h"

#include "LinkedList.h"

#include "ADS1X15.h"

LinkedList<VoltageSourceBase*> voltage_sources = LinkedList<VoltageSourceBase*> ();

/////// PARAMETER INPUTS & VOLTAGE SOURCES

#define ENABLE_ADS_24V
#define ENABLE_ADS_x48

#ifdef ENABLE_ADS_24V
    #define MAX_INPUT_VOLTAGE_24V 10.0
    ADS1015 ADS_OBJECT_24V(0x49);
    ADS24vVoltageSource<ADS1015> voltage_source_1_channel_0 = ADS24vVoltageSource<ADS1015>(&ADS_OBJECT_24V, 0, MAX_INPUT_VOLTAGE_24V);
    ADS24vVoltageSource<ADS1015> voltage_source_1_channel_1 = ADS24vVoltageSource<ADS1015>(&ADS_OBJECT_24V, 1, MAX_INPUT_VOLTAGE_24V);
    ADS24vVoltageSource<ADS1015> voltage_source_1_channel_2 = ADS24vVoltageSource<ADS1015>(&ADS_OBJECT_24V, 2, MAX_INPUT_VOLTAGE_24V);
#endif

#ifdef ENABLE_ADS_x48
    #define MAX_INPUT_VOLTAGE_x48 5.0
    ADS1115 ADS_OBJECT_x48(0x48);
    ADSVoltageSource<ADS1115> voltage_source_2_channel_0 = ADSVoltageSource<ADS1115>(&ADS_OBJECT_x48, 0, MAX_INPUT_VOLTAGE_x48);
    ADSVoltageSource<ADS1115> voltage_source_2_channel_1 = ADSVoltageSource<ADS1115>(&ADS_OBJECT_x48, 1, MAX_INPUT_VOLTAGE_x48);
    ADSVoltageSource<ADS1115> voltage_source_2_channel_2 = ADSVoltageSource<ADS1115>(&ADS_OBJECT_x48, 2, MAX_INPUT_VOLTAGE_x48);
    ADSVoltageSource<ADS1115> voltage_source_2_channel_3 = ADSVoltageSource<ADS1115>(&ADS_OBJECT_x48, 3, MAX_INPUT_VOLTAGE_x48);
#endif

void setup_voltage_sources() {
    Serial.println("setup_voltage_sources...");
    #ifdef ENABLE_ADS_x48
        Serial.println("Beginning ADS_x48..");
        ADS_OBJECT_x48.begin();
        ADS_OBJECT_x48.setGain(0);
        voltage_sources.add(&voltage_source_2_channel_0);
        voltage_sources.add(&voltage_source_2_channel_1);
        voltage_sources.add(&voltage_source_2_channel_2);
        voltage_sources.add(&voltage_source_2_channel_3);
    #endif
    
    #ifdef ENABLE_ADS_24V
    Serial.println("Beginning ADS_24V..");
        ADS_OBJECT_24V.begin();
        ADS_OBJECT_24V.setGain(2);
        voltage_sources.add(&voltage_source_1_channel_0);
        voltage_sources.add(&voltage_source_1_channel_1);
        voltage_sources.add(&voltage_source_1_channel_2);
    #endif
    Serial.println("done setup_voltage_sources!");
}

void update_voltage_sources() {
    for (int i = 0 ; i < voltage_sources.size() ; i++) {
        voltage_sources.get(i)->update();
    }
}

// ParameterInputs, ie wrappers around input mechanism, assignable to a Parameter
LinkedList<BaseParameterInput*> available_inputs            = LinkedList<BaseParameterInput*>();


// Parameters, ie wrappers around destination object
LinkedList<DataParameter*>      available_parameters    = LinkedList<DataParameter*>();

DataParameter       param_none                              = DataParameter("None");
FrequencyParameter  param_overall_pitch                     = FrequencyParameter<SID6581,double> ("Overall pitch", &sid, 0.0, &SID6581::setAllFrequency);
Parameter           param_overall_pulsewidth_modulation     = Parameter<SID6581,double> ("Overall PW Mod", &sid, 0.0, &SID6581::modulateAllPulseWidths);
Parameter           param_overall_pitch_modulation          = Parameter<SID6581,double> ("Overall Pitch Mod", &sid, 0.0, &SID6581::modulateAllPitches);

Parameter           param_filter_cutoff                     = Parameter<SID6581,double> ("Filter cutoff", &sid, 0.0, &SID6581::setCutoff);
Parameter           param_filter_resonance                  = Parameter<SID6581,double> ("Filter res", &sid, 0.0, &SID6581::setResonanceD);

ToggleParameter     param_filter_lowpass   = ToggleParameter<SID6581,bool> ("Filter LP", &sid, sid.isFilterTypeLP(), &SID6581::setFilterTypeLP);
ToggleParameter     param_filter_bandpass  = ToggleParameter<SID6581,bool> ("Filter BP", &sid, sid.isFilterTypeBP(), &SID6581::setFilterTypeBP);
ToggleParameter     param_filter_highpass  = ToggleParameter<SID6581,bool> ("Filter HP", &sid, sid.isFilterTypeHP(), &SID6581::setFilterTypeHP);
ToggleParameter     param_filter_off3      = ToggleParameter<SID6581,bool> ("Filter Off3", &sid, sid.isFilterTypeOff3(), &SID6581::setFilterTypeOff3);

void add_voice_parameters(LinkedList<DataParameter*> *available_parameters, int voice_number) {
    char label[40] = "            ";
    Voice *voice = &sid.voice[voice_number];
    ToggleParameter<Voice,bool> *param;

    sprintf(label, "Osc %i %s", voice_number+1, "Triangle");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::triMask), &Voice::toggleTri); //triOn, &Voice::triOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Pulse");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::pulseMask), &Voice::togglePulse); //&Voice::pulseOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Saw");    
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::sawMask), &Voice::toggleSaw); //&Voice::sawOff);
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Noise");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::noiseMask), &Voice::toggleNoise);    
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Ring");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::ringMask), &Voice::toggleRing);    
    available_parameters->add(param);

    sprintf(label, "Osc %i %s", voice_number+1, "Sync");
    param = new ToggleParameter<Voice,bool>(label, voice, voice->isWaveform(ENVOSC::ringMask), &Voice::toggleRing);    
    available_parameters->add(param);

    /*sprintf(label, "Osc %i %s", voice_number+1, "Pitch");
    FrequencyParameter<Voice,double> *param_freq = new FrequencyParameter<Voice,double> (label, voice, &Voice::setFrequency);
    available_parameters->add(param_freq);*/

    sprintf(label, "Osc %i %s", voice_number+1, "Pitch Mod");
    Parameter<Voice,double> *pitch_freq = new Parameter<Voice,double> (label, voice, &Voice::modulatePitch, &Voice::getPitchMod);
    available_parameters->add(pitch_freq);

    sprintf(label, "Osc %i %s", voice_number+1, "PW Mod");
    DataParameter *param3 = (new Parameter<Voice,double> (label, voice, &Voice::modulatePulseWidth, &Voice::getModulatedPulseWidth))
                            ->initialise_values(NULL,0,4095);
    available_parameters->add(param3);

    sprintf(label, "Osc %i %s", voice_number+1, "Freq mult");
    DataParameter *param2 = (new Parameter<Voice,double> (label, voice, &Voice::setFrequencyMultiplier, &Voice::getFrequencyMultiplier))
                            ->initialise_values(NULL,0,400);
    //param2->minimum_value = 0.0;
    //param2->maximum_value = 400; // actually 4x?
    available_parameters->add(param2);
}

void setup_parameters() {
    // add the available parameters to a list used globally and later passed to each selector menuitem
    Serial.println(F("==== begin setup_parameters ===="));
    
    // global sid parameters
    available_parameters.add(&param_none);
    available_parameters.add(&param_overall_pitch);
    available_parameters.add(&param_overall_pitch_modulation);
    available_parameters.add(&param_overall_pulsewidth_modulation);
    available_parameters.add(&param_filter_cutoff);
    available_parameters.add(&param_filter_resonance);
  
    available_parameters.add(&param_filter_lowpass);
    available_parameters.add(&param_filter_bandpass);
    available_parameters.add(&param_filter_highpass);
    available_parameters.add(&param_filter_off3);

    // add the parameters for each voice
    available_parameters.add(
        new ToggleParameter<SID6581,bool>("Osc 1 Filt", &sid, sid.isFilterVoice0On(), &SID6581::changeFilterVoice0)
    );
    add_voice_parameters(&available_parameters, 0);

    available_parameters.add(
        new ToggleParameter<SID6581,bool>("Osc 2 Filt", &sid, sid.isFilterVoice1On(), &SID6581::changeFilterVoice1)
    );    
    add_voice_parameters(&available_parameters, 1);

    available_parameters.add(
        new ToggleParameter<SID6581,bool>("Osc 3 Filt", &sid, sid.isFilterVoice2On(), &SID6581::changeFilterVoice2)
    );
    add_voice_parameters(&available_parameters, 2);    

    // Dynamically create ParameterInputs based on the voltage_sources array
    char name = 'A';
    Serial.println("About to dynamically create ParameterInputs...");
    for (int i = 0 ; i < voltage_sources.size() ; i++) {
        Serial.printf("\tDynamically creating input %c...\n", name); Serial.flush();
        VoltageParameterInput<BaseParameter> *vpi = new VoltageParameterInput<BaseParameter>(name, voltage_sources.get(i));
        Serial.println("\tInstantiated, adding to available_inputs!");
        available_inputs.add(vpi);
        name++;

        /*vpi = new VoltageParameterInput<BaseParameter>(name, voltage_sources.get(i));
        vpi->setInverted();
        available_inputs.add(vpi);
        name++;*/
    }

    Serial.println("==== end setup_parameters ====");
}

// called every loop to update the inputs; maybe in future we also eventually want to process internal LFOs or other properties of parameter here
void update_parameters() {
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

        param_input->name = i+'A';

        char input_label[20];
        //sprintf(input_label, "Input %i => %c", i<4?0:1, param_input->name); //i+'A');
        sprintf(input_label, "Input %i => %c", i/2, param_input->name); //i+'A');

        Serial.printf("%i: Creating control labelled '%s'...\n", i, input_label);
        ParameterSelectorControl *ctrl = new ParameterSelectorControl(input_label);

        Serial.printf("\tConfiguring it with available_inputs item target '%s'..\n", i, param->label);
        //BaseParameterInput *param_input = available_inputs.get(i);
        ctrl->configure(param_input, &available_parameters);

        Serial.println("\tConfigured!  Adding to menu..");
        menu->add(ctrl);
        Serial.println("\tAdded!");
    }

    Serial.println("Starting add available_parameters loop..."); Serial.flush();
    Serial.printf("Got %i parameters..\n", available_parameters.size());
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
