#include "../oops.h"

/* Deinitialising */

static int oops_value_gc(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Marking */

static int oops_value_mark(void *p, size_t size) {
    (void) size;
    oops_value_t *value = (oops_value_t *)p;
    /* TODO: Make sure this is how you iterate over a tuple */
    for (size_t i = 0; i < value->type->field_count; i++) {
        Janet field = *(value->fields + (i * sizeof(Janet)));
        janet_mark(field);
    }
    return 0;
}

/* Accessing */

static int oops_value_get_field_pos(oops_value_t *value, Janet key) {
    const uint8_t *target = janet_unwrap_keyword(key);
    size_t ret = -1;
    for (size_t i = 0; i < value->type->field_count; i++) {
        const uint8_t *name = janet_getkeyword(value->type->field_names, i);
        if (0 == strcmp((char *)target, (char *)name)) {
            ret = i;
            break;
        }
    }
    return ret;
}

static int oops_value_get(void *p, Janet key, Janet *out) {
    oops_value_t *value = (oops_value_t *)p;

    int pos = oops_value_get_field_pos(value, key);
    if (pos == -1) return 0;

    Janet ret = *(value->fields + (pos * sizeof(Janet)));
    *out = ret;
    return 1;
}

/* Type Retrieval */

oops_value_t *oops_value(oops_type_t *type) {
    const JanetAbstractType *at = janet_get_abstract_type(janet_csymbolv(type->name));
    if (at == NULL)
        janet_panicf("no abstract type in registry with name %s", type->name);
    oops_value_t *value = (oops_value_t *)janet_abstract(at, sizeof(oops_value_t));
    return value;
}

/* Registration Function */

void oops_value_register_type(const char *type_name) {
    /* Don't free this because janet_get_abstract_type() needs it */
    JanetAbstractType *at = malloc(sizeof(JanetAbstractType));

    *at = (JanetAbstractType){
        type_name,
        oops_value_gc,
        oops_value_mark,
        oops_value_get,
        JANET_ATEND_GET
    };

    janet_register_abstract_type(at);
}

