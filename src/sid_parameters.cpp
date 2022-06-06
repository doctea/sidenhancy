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

ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 1, &param_overall_pulsewidth_modulation);

//LinkedList<ParameterInput<BaseParameter>*> param_inputs = LinkedList<ParameterInput*>();
LinkedList<BaseParameterInput*> param_inputs = LinkedList<BaseParameterInput*>();

ParameterMenuItem PulseWidthModulationPanel("Pulse width mod", &param_overall_pulsewidth_modulation);
//MenuItem testitem("SID Params test");

void setup_parameters() {
    Serial.println(F("==== begin setup_parameters ===="));
    //input_A.setTarget(&param_overall_pitch);
    //input_B.setTarget(&param_overall_pulsewidth_modulation);

    param_inputs.add(&input_A);
    param_inputs.add(&input_B);

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
