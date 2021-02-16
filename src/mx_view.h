#include "Common.hpp"

void displayProgramOptions_view();
void displayProgramOptions_shape();
void parseProgramOptions_view(int argc, char *argv[], MX_opt &opt);
bool validateProgramOptions_view(MX_opt &opt);
void mx_view(MX_opt &opt);
void mx_shape(MX_opt &opt);