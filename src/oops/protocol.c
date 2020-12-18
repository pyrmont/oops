#include "../oops.h"

/* Deinitialising */

static int oops_protocol_gc(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Marking */

static int oops_protocol_mark(void *p, size_t size) {
    (void) p;
    (void) size;
    return 0;
}

/* Type Definition */

const JanetAbstractType oops_protocol_type = {
    "oops/protocol",
    oops_protocol_gc,
    oops_protocol_mark,
    JANET_ATEND_GCMARK
};

/* Registration Function */

void oops_protocol_register_type() {
    janet_register_abstract_type(&oops_protocol_type);
}
