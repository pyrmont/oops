#include "../oops.h"

/* Deinitialising */

static int oops_type_gc(void *p, size_t size) {
    (void) size;
    oops_type_t *type = (oops_type_t *)p;
    free((char *)type->name);
    return 0;
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
