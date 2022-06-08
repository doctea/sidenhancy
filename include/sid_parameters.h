#ifndef SID_PARAMETERS__INCLUDED
#define SID_PARAMETERS__INCLUDED

#include "ads.h"

#include "Parameter.h"
#include "ParameterInput.h"
#include <LinkedList.h>

void setup_parameters();
void update_parameters();

void setup_parameter_menu();

/*struct parameter_option {
    char label[20];
    BaseParameter *parameter;
};*/

extern LinkedList<BaseParameterInput*> available_inputs;
extern LinkedList<BaseParameter*> available_parameters;

#endif