#ifndef OOPS_H
#define OOPS_H

#include <janet.h>

void oops_register_types(JanetTable *env);
void oops_register_functions(JanetTable *env);

#endif /* end of include guard */
