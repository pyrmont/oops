#include "oops.h"

/* C Functions */

static Janet cfun_oops_emit_type(int32_t argc, Janet *argv) {
    janet_arity(argc, 2, 3);

    const char *name = janet_getcstring(argv, 0);
    const Janet *fields = janet_gettuple(argv, 1);

    JanetDictView methods = {NULL, 0, 0};
    if (argc == 3) {
        methods = janet_getdictionary(argv, 2);
    }

    if ((size_t)methods.len > OOPS_ABSTRACT_METHODS)
        janet_panicf("too many methods in definition");

    oops_type_t *type = oops_type(name, fields, methods);
    oops_instance_register_type(name);

    return janet_wrap_abstract(type);
}

static Janet cfun_oops_emit_instance(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    oops_type_t *type = (oops_type_t *)janet_getabstract(argv, 0, &oops_type_type);

    JanetView fields = janet_getindexed(argv, 1);

    if ((size_t)fields.len != type->field_count)
        janet_panicf("incorrect number of fields in constructor");

    oops_instance_t *instance = oops_instance(type, fields);

    return janet_wrap_abstract(instance);
}

static Janet cfun_oops_emit_protocol(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 2);

    const char *name = janet_getcstring(argv, 0);
    const Janet *function_details = janet_gettuple(argv, 1);

    oops_protocol_t *protocol = (oops_protocol_t *)janet_abstract(&oops_protocol_type, sizeof(oops_protocol_t));
    protocol->name = name;
    protocol->function_count = janet_tuple_length(function_details);
    protocol->function_details = (Janet *)function_details;

    return janet_wrap_abstract(protocol);
}

static const JanetReg cfuns[] = {
    {"emit-type", cfun_oops_emit_type, NULL},
    {"emit-instance", cfun_oops_emit_instance, NULL},
    {"emit-protocol", cfun_oops_emit_protocol, NULL},
    {NULL, NULL, NULL}
};

/* Environment Registration */

void oops_register_types(JanetTable *env) {
    (void) env;
    oops_type_register_type();
    oops_protocol_register_type();
}

void oops_register_functions(JanetTable *env) {
    janet_cfuns(env, "oops", cfuns);
}

JANET_MODULE_ENTRY(JanetTable *env) {
    oops_register_types(env);
    oops_register_functions(env);
}
