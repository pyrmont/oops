#ifndef OOPS_H
#define OOPS_H

#include <janet.h>

/* Oops Type Type */

extern const JanetAbstractType oops_type_type;

typedef struct {
    const char *name;
    size_t field_count;
    Janet *field_names;
} oops_type_t;

void oops_type_register_type();

/* Oops Value Type */

typedef struct {
    oops_type_t *type;
    Janet *fields;
} oops_value_t;

oops_value_t *oops_value(oops_type_t *type);
void oops_value_register_type(const char *type_name);

/* Registration Functions */

void oops_register_types(JanetTable *env);
void oops_register_functions(JanetTable *env);

#endif /* end of include guard */
