#ifndef OOPS_H
#define OOPS_H

#include <janet.h>

#define OOPS_ABSTRACT_METHODS 5
#define OOPS_ABSTRACT_GET 0
#define OOPS_ABSTRACT_PUT 1
#define OOPS_ABSTRACT_TOSTRING 2
#define OOPS_ABSTRACT_COMPARE 3
#define OOPS_ABSTRACT_NEXT 4

/* Oops Type Type */

extern const JanetAbstractType oops_type_type;

typedef struct {
    const char *name;
    size_t field_count;
    Janet *field_names;
    Janet methods[OOPS_ABSTRACT_METHODS];
} oops_type_t;

oops_type_t *oops_type(const char *name, const Janet *fields, JanetDictView methods);
void oops_type_register_type();

/* Oops Instance Type */

typedef struct {
    oops_type_t *type;
    Janet *fields;
} oops_instance_t;

oops_instance_t *oops_instance(oops_type_t *type, JanetView fields);
void oops_instance_register_type(const char *type_name);

/* Oops Protocol Type */

extern const JanetAbstractType oops_protocol_type;

typedef struct {
    const char *name;
    size_t function_count;
    Janet *function_details;
    Janet implementers;
} oops_protocol_t;

void oops_protocol_register_type();

/* Registration Functions */

void oops_register_types(JanetTable *env);
void oops_register_functions(JanetTable *env);

#endif /* end of include guard */
