#include "oops.h"

/* Structs */

typedef struct {
    const char *name;
    size_t fields_count;
    Janet *fields;
} oops_type_t;

typedef struct {
    oops_type_t *type;
    Janet *fields;
} oops_value_t;

/* Utility Functions */

static void oops_register_abstract_type(const char *type_name) {
    /* Don't free this because janet_get_abstract_type() needs it */
    JanetAbstractType *at = malloc(sizeof(JanetAbstractType));

    *at = (JanetAbstractType){
        type_name,
        JANET_ATEND_NAME
    };

    janet_register_abstract_type(at);
}

static oops_value_t *oops_value(oops_type_t *type) {
    const JanetAbstractType *at = janet_get_abstract_type(janet_csymbolv(type->name));
    if (at == NULL)
        janet_panicf("no abstract type in registry with name %s", type->name);
    oops_value_t *value = (oops_value_t *)janet_abstract(at, sizeof(oops_value_t));
    return value;
}

/* Type Definition */

static const JanetAbstractType oops_type_type = {
    "oops/type",
    JANET_ATEND_NAME
};

/* Functions */

static Janet cfun_oops_emit_type(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    const char *name = janet_getcstring(argv, 0);
    oops_register_abstract_type(name);

    oops_type_t *type = (oops_type_t *)janet_abstract(&oops_type_type, sizeof(oops_type_t));
    type->name = name;
    type->fields_count = 0;

    return janet_wrap_abstract(type);
}

static Janet cfun_oops_emit_instance(int32_t argc, Janet *argv) {
    janet_fixarity(argc, 1);

    oops_type_t *type = (oops_type_t *)janet_getabstract(argv, 0, &oops_type_type);
    oops_value_t *value = oops_value(type);

    value->type = type;
    value->fields = NULL;

    return janet_wrap_abstract(value);
}

static const JanetReg cfuns[] = {
    {"emit-type", cfun_oops_emit_type, NULL},
    {"emit-instance", cfun_oops_emit_instance, NULL},
    {NULL, NULL, NULL}
};

/* Environment Registration */

void oops_register_type(JanetTable *env) {
    (void) env;
    janet_register_abstract_type(&oops_type_type);
}

void oops_register_functions(JanetTable *env) {
    janet_cfuns(env, "oops", cfuns);
}

JANET_MODULE_ENTRY(JanetTable *env) {
    oops_register_functions(env);
}
