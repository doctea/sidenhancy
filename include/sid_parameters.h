#ifndef SID_PARAMETERS__INCLUDED
#define SID_PARAMETERS__INCLUDED

#include "ads.h"

#include "parameters/Parameter.h"
#include "parameter_inputs/ParameterInput.h"
#include <LinkedList.h>

void setup_parameters();
void update_parameters();

void setup_parameter_menu();

/*struct parameter_option {
    char label[20];
    BaseParameter *parameter;
};*/

void setup_voltage_sources();
void update_voltage_sources();

extern LinkedList<BaseParameterInput*> available_inputs;
extern LinkedList<BaseParameter*> available_parameters;

#endif