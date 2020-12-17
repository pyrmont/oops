#include "oops.h"

/* C Functions */

static Janet cfun_oops_emit_type(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    const char *name = janet_getcstring(argv, 0);
    const Janet *field_names = janet_gettuple(argv, 1);

    oops_value_register_type(name);
    oops_type_t *type = (oops_type_t *)janet_abstract(&oops_type_type, sizeof(oops_type_t));
    type->name = name;
    type->field_count = janet_tuple_length(field_names);
    type->field_names = (Janet *)field_names;

    return janet_wrap_abstract(type);
}

static Janet cfun_oops_emit_instance(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    oops_type_t *type = (oops_type_t *)janet_getabstract(argv, 0, &oops_type_type);
    const Janet *fields = janet_gettuple(argv, 1);

    if ((size_t)janet_tuple_length(fields) != type->field_count)
        janet_panicf("incorrect number of fields in constructor");

    oops_value_t *value = oops_value(type);
    value->type = type;
    value->fields = (Janet *)fields;

    return janet_wrap_abstract(value);
}

static const JanetReg cfuns[] = {
    {"emit-type", cfun_oops_emit_type, NULL},
    {"emit-instance", cfun_oops_emit_instance, NULL},
    {NULL, NULL, NULL}
};

/* Environment Registration */

void oops_register_types(JanetTable *env) {
    (void) env;
    oops_type_register_type();
}

void oops_register_functions(JanetTable *env) {
    janet_cfuns(env, "oops", cfuns);
}

JANET_MODULE_ENTRY(JanetTable *env) {
    oops_register_types(env);
    oops_register_functions(env);
}
