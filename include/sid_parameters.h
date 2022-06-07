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

extern LinkedList<BaseParameterInput*> param_inputs;

extern LinkedList<BaseParameter*> available_parameters;

#define NUM_AVAILABLE_PARAMETERS (available_parameters.size())

int find_parameter_index_for_label(char *label);

#endif