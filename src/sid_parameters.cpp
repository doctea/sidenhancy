#include "sid6581.h"

#include "ads.h"

#include "FrequencyParameter.h"
//#include "ParameterInput.h"
#include "ADSParameterInput.h"

#include "LinkedList.h"

FrequencyParameter param_overall_pitch          = FrequencyParameter<SID6581,double>(&sid, &SID6581::setAllFrequency);
Parameter param_overall_pulsewidth_modulation   = Parameter<SID6581,double> (&sid, &SID6581::modulateAllPulseWidths);

ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_A = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 0, &param_overall_pitch);
ADSParameterInput<ADS1115,Parameter<SID6581,double>>        input_B = ADSParameterInput<ADS1115,Parameter<SID6581,double>>(&ADS_OBJECT, 1, &param_overall_pulsewidth_modulation);

//LinkedList<ParameterInput<BaseParameter>*> param_inputs = LinkedList<ParameterInput*>();
LinkedList<BaseParameterInput*> param_inputs = LinkedList<BaseParameterInput*>();

void setup_parameters() {
    //input_A.setTarget(&param_overall_pitch);
    //input_B.setTarget(&param_overall_pulsewidth_modulation);

    param_inputs.add(&input_A);
    param_inputs.add(&input_B);
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




//LinkedList<ParameterInput<Parameter>*> param_inputs = LinkedList<ParameterInput*>();
//ADSParameterInput<ADS1115, Parameter<SID6581,double>,double> pitch_input = ADSParameterInput<ADS1115,Parameter<SID6581,double>,double>(&ADS_OBJECT, 0, &param_overall_pitch);
//ADSParameterInput<ADS1115, Parameter<SID6581,double>,double> pitch_input = ADSParameterInput<ADS1115,Parameter<SID6581,double>,double>(&ADS_OBJECT); //, 0);
//ADSParameterInput<ADS1115, Parameter<SID6581,double>,double> pitch_input = ADSParameterInput<ADS1115,Parameter<SID6581,double>,double>(); //, 0);
//ADSParameterInput<ADS1115, Parameter<SID6581>> pulse_width_input  = ADSParameterInput<ADS1115,Parameter<SID6581,double>,double>(&ADS, 1, &param_overall_pulsewidth_modulation);

//ADSParameterInput<ADS1115, Parameter<SID6581,double>,double> input_A      = ADSParameterInput<ADS1115,Parameter<SID6581,double>,double>();
//ADSParameterInput<ADS1115, Parameter<SID6581,float>,float>   input_B      = ADSParameterInput<ADS1115,Parameter<SID6581,float>,float>();


//no matching function for call to 'ADSParameterInput<ADS1115, Parameter<SID6581, double>, double> :: ADSParameterInput(ADS1115*, int, Parameter<SID6581, double>*)'

/*ParameterInput *parameter_inputs[2] = {
    &pitch_input, //ADSParameterInput<double,ADS1115>(&ADS, 0, &overall_pitch),
    &pulse_width_input, //&ADSParameterInput<float,ADS1115>(&ADS, 1, &overall_pulsewidth_modulation)
};*/

//LinkedList<ParameterInput*> param_inputs = LinkedList<ParameterInput*>();

/*ADSParameterInput parameter_inputs[2] = {
    pitch_input,
    pulse_width_input,
};*/