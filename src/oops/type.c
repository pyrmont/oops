#include "../oops.h"

/* Deinitialising */

static int oops_type_gc(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Initialising */

oops_type_t *oops_type(const char *name, const Janet *fields, JanetDictView methods) {
    oops_type_t *type = (oops_type_t *)janet_abstract(&oops_type_type, sizeof(oops_type_t));
    type->name = name;
    type->field_count = janet_tuple_length(fields);
    type->field_names = (Janet *)fields;

    Janet method;

    method = janet_dictionary_get(methods.kvs, methods.cap, janet_ckeywordv("tostring"));
    if (janet_checktype(method, JANET_FUNCTION)) {
        type->methods[OOPS_ABSTRACT_TOSTRING] = method;
    }
    method = janet_dictionary_get(methods.kvs, methods.cap, janet_ckeywordv("compare"));
    if (janet_checktype(method, JANET_FUNCTION)) {
        type->methods[OOPS_ABSTRACT_COMPARE] = method;
    }

    return type;
}

/* Marking */

static int oops_type_mark(void *p, size_t size) {
    (void) size;
    oops_type_t *type = (oops_type_t *)p;
    /* TODO: Make sure this is how you iterate over a tuple */
    for (size_t i = 0; i < type->field_count; i++) {
        Janet field_name = *(type->field_names + (i * sizeof(Janet)));
        janet_mark(field_name);
    }
    return 0;
}

/* Type Definition */

const JanetAbstractType oops_type_type = {
    "oops/type",
    oops_type_gc,
    oops_type_mark,
    JANET_ATEND_GCMARK
};

/* Registration Function */

void oops_type_register_type() {
    janet_register_abstract_type(&oops_type_type);
}
