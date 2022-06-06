#include "sid6581.h"

#include "ads.h"

#include "FrequencyParameter.h"
//#include "ParameterInput.h"
#include "ADSParameterInput.h"

#include "mymenu.h"
#include "ParameterMenuItems.h"

#include "LinkedList.h"

FrequencyParameter param_overall_pitch          = FrequencyParameter<SID6581,double> (&sid, &SID6581::setAllFrequency);
Parameter param_overall_pulsewidth_modulation   = Parameter<SID6581,double> (&sid, &SID6581::modulateAllPulseWidths);

FrequencyParameter osc_1_pitch          = FrequencyParameter<Voice,double> (&sid.voice[0], &Voice::setFrequency);
FrequencyParameter osc_2_pitch          = FrequencyParameter<Voice,double> (&sid.voice[1], &Voice::setFrequency);
FrequencyParameter osc_3_pitch          = FrequencyParameter<Voice,double> (&sid.voice[2], &Voice::setFrequency);

Parameter param_filter_cutoff                   = Parameter<SID6581,double> (&sid, &SID6581::setCutoff);

ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pulsewidth_modulation);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_C = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 1, &param_filter_cutoff);

ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_D = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &osc_1_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_E = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &osc_2_pitch);
ADSParameterInput<ADS1115,Parameter<Voice,double>>        input_F = ADSParameterInput<ADS1115,Parameter<Voice,double>>(&ADS_OBJECT, 1, &osc_3_pitch);


//LinkedList<ParameterInput<BaseParameter>*> param_inputs = LinkedList<ParameterInput*>();
LinkedList<BaseParameterInput*> param_inputs = LinkedList<BaseParameterInput*>();

ParameterMenuItem PulseWidthModulationPanel("Pulse width mod", &param_overall_pulsewidth_modulation);
//MenuItem testitem("SID Params test");

void setup_parameters() {
    Serial.println(F("==== begin setup_parameters ===="));
    //input_A.setTarget(&param_overall_pitch);
    //input_B.setTarget(&param_overall_pulsewidth_modulation);

    input_D.setInverted();
    input_F.setInverted();    

    //param_inputs.add(&input_A);
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
    Serial.println("==== menu->add ====");
    menu->add(&PulseWidthModulationPanel);
    //menu->add(&testitem);
}
