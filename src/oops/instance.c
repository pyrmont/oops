#include "../oops.h"

/* Deinitialising */

static int oops_instance_gc(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Initialising  */

oops_instance_t *oops_instance(oops_type_t *type, JanetView fields, JanetDictView methods) {
    const JanetAbstractType *at = janet_get_abstract_type(janet_csymbolv(type->name));
    if (at == NULL)
        janet_panicf("no abstract type in registry with name %s", type->name);
    oops_instance_t *instance = (oops_instance_t *)janet_abstract(at, sizeof(oops_instance_t));

    instance->type = type;

    instance->fields = malloc(type->field_count * sizeof(Janet*));
    for (size_t i = 0; i < type->field_count; i++) {
        instance->fields[i] = fields.items[i];
    }

    Janet method;

    method = janet_dictionary_get(methods.kvs, methods.cap, janet_ckeywordv("tostring"));
    if (janet_checktype(method, JANET_FUNCTION)) {
        instance->methods[OOPS_ABSTRACT_TOSTRING] = method;
    }
    return instance;
}

/* Marking */

static int oops_instance_mark(void *p, size_t size) {
    (void) size;
    oops_instance_t *instance = (oops_instance_t *)p;
    /* TODO: Make sure this is how you iterate over a tuple */
    for (size_t i = 0; i < instance->type->field_count; i++) {
        Janet field = instance->fields[i];
        janet_mark(field);
    }
    return 0;
}

/* Accessing */

static int oops_instance_get_field_pos(oops_instance_t *instance, Janet key) {
    const uint8_t *target = janet_unwrap_keyword(key);
    size_t ret = -1;
    for (size_t i = 0; i < instance->type->field_count; i++) {
        const uint8_t *name = janet_getkeyword(instance->type->field_names, i);
        if (0 == strcmp((char *)target, (char *)name)) {
            ret = i;
            break;
        }
    }
    return ret;
}

static int oops_instance_get(void *p, Janet key, Janet *out) {
    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panic("field must be keyword");

    oops_instance_t *instance = (oops_instance_t *)p;

    int pos = oops_instance_get_field_pos(instance, key);
    if (pos == -1) return 0;

    Janet ret = instance->fields[pos];
    *out = ret;
    return 1;
}

/* Updating */

static void oops_instance_put(void *p, Janet key, Janet value) {
    if (!janet_checktype(key, JANET_KEYWORD))
        janet_panic("field must be keyword");

    oops_instance_t *instance = (oops_instance_t *)p;

    int pos = oops_instance_get_field_pos(instance, key);
    if (pos == -1)
        janet_panicf("no field %j in type", key);

    instance->fields[pos] = value;
    printf("Finished putting\n");
}

/* Stringifying */

static void oops_instance_tostring(void *p, JanetBuffer *buf) {
    oops_instance_t *instance = (oops_instance_t *)p;

    if (janet_checktype(instance->methods[OOPS_ABSTRACT_TOSTRING], JANET_FUNCTION)) {
        JanetFunction *method = janet_unwrap_function(instance->methods[OOPS_ABSTRACT_TOSTRING]);
        Janet ret = janet_call(method, 0, NULL);
        janet_buffer_push_string(buf, janet_unwrap_string(ret));
    } else {
        janet_buffer_push_cstring(buf, "<");
        janet_buffer_push_cstring(buf, instance->type->name);
        janet_buffer_push_cstring(buf, ">");
    }
}

/* Registration Function */

void oops_instance_register_type(const char *type_name) {
    /* Don't free this because janet_get_abstract_type() needs it */
    JanetAbstractType *at = malloc(sizeof(JanetAbstractType));

    *at = (JanetAbstractType){
        type_name,
        oops_instance_gc,
        oops_instance_mark,
        oops_instance_get,
        oops_instance_put,
        NULL, /* marshalling */
        NULL, /* unmarshalling */
        oops_instance_tostring,
        JANET_ATEND_TOSTRING
    };

    janet_register_abstract_type(at);
}
