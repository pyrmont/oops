#include "../oops.h"

/* Utility Functions */

static Janet oops_instance_invoke_method(JanetFunction *method, int32_t argc, JanetTuple argv) {
    JanetFiber *fiber = janet_fiber(method, 64, argc, argv);
    Janet out;
    JanetSignal sig = janet_continue(fiber, janet_wrap_nil(), &out);
    if (sig != JANET_SIGNAL_OK) {
        /* This stacktrace could be improved */
        janet_stacktrace(fiber, out);
        janet_panic(NULL);
    }
    return out;
}

/* Deinitialising */

static int oops_instance_gc(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Initialising  */

oops_instance_t *oops_instance(oops_type_t *type, JanetView fields) {
    const JanetAbstractType *at = janet_get_abstract_type(janet_csymbolv(type->name));
    if (at == NULL)
        janet_panicf("no abstract type in registry with name %s", type->name);
    oops_instance_t *instance = (oops_instance_t *)janet_abstract(at, sizeof(oops_instance_t));

    instance->type = type;

    instance->fields = malloc(type->field_count * sizeof(Janet*));
    for (size_t i = 0; i < type->field_count; i++) {
        instance->fields[i] = fields.items[i];
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
}

/* Stringifying */

static void oops_instance_tostring(void *p, JanetBuffer *buf) {
    oops_instance_t *instance = (oops_instance_t *)p;

    if (janet_checktype(instance->type->methods[OOPS_ABSTRACT_TOSTRING], JANET_FUNCTION)) {
        JanetFunction *method = janet_unwrap_function(instance->type->methods[OOPS_ABSTRACT_TOSTRING]);
        Janet tup[1] = {janet_wrap_abstract(instance)};
        JanetTuple args = janet_tuple_n(tup, 1);
        Janet out = oops_instance_invoke_method(method, 1, args);
        if (!janet_checktypes(out, JANET_TFLAG_BYTES)) {
            janet_panic("function must return a string, symbol, buffer or keyword");
        }
        janet_buffer_push_string(buf, janet_unwrap_string(out));
    } else {
        janet_buffer_push_cstring(buf, "<");
        janet_buffer_push_cstring(buf, instance->type->name);
        janet_buffer_push_cstring(buf, ">");
    }
}

/* Comparing */

static int oops_instance_compare(void *lhs, void *rhs) {
    oops_instance_t *a = (oops_instance_t *)lhs;
    oops_instance_t *b = (oops_instance_t *)rhs;

    if (janet_checktype(a->type->methods[OOPS_ABSTRACT_COMPARE], JANET_FUNCTION)) {
        JanetFunction *method = janet_unwrap_function(a->type->methods[OOPS_ABSTRACT_COMPARE]);
        Janet tup[2] = {janet_wrap_abstract(a), janet_wrap_abstract(b)};
        JanetTuple args = janet_tuple_n(tup, 2);
        Janet out = oops_instance_invoke_method(method, 2, args);
        if (!janet_checkint(out)) {
            janet_panic("function must return an integer");
        }
        return janet_unwrap_integer(out);
    } else {
        return a == b;
    }
}

/* Traversing */

static Janet oops_instance_next(void *p, Janet key) {
    (void)p;
    (void)key;
    return janet_wrap_nil();
}

/* Calling */

/* static Janet oops_instance_call(void *p, int32_t argc, Janet *argv) { */
/*     (void)p; */
/*     (void)argc; */
/*     (void)argv; */
/*     return janet_wrap_nil(); */
/* } */

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
        oops_instance_compare,
        NULL, /* hashing */
        oops_instance_next,
        JANET_ATEND_NEXT
    };

    janet_register_abstract_type(at);
}
