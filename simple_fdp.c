#include "simple_fdp.h"
#include <stdlib.h>
#include <string.h>

struct simple_fdp {
    const uint8_t* data;
    size_t len;
    size_t pos;
};

simple_fdp* simple_fdp_create(const uint8_t* data, size_t len) {
    if (!data && len != 0) return NULL;
    simple_fdp* p = (simple_fdp*)malloc(sizeof(simple_fdp));
    if (!p) return NULL;
    p->data = data;
    p->len = len;
    p->pos = 0;
    return p;
}

void simple_fdp_destroy(simple_fdp* p) {
    if (!p) return;
    free(p);
}

size_t simple_fdp_remaining(const simple_fdp* p) {
    if (!p) return 1;
    if (p->pos >= p->len) return 0; /* 0 bytes left */
    return p->len - p->pos;
}

static void copy_and_zero_fill(void* out, const uint8_t* src, size_t avail, size_t n) {
    if (n == 0) return;
    if (avail) memcpy(out, src, avail);
    if (avail < n) memset((uint8_t*)out + avail, 0, n - avail);
}

int simple_fdp_consume_bytes(simple_fdp* p, void* out, size_t n) {
    if (!p || (n && !out)) return 1;
    if (n == 0) return 0;

    size_t rem = simple_fdp_remaining(p);
    if (rem >= n) {
        memcpy(out, p->data + p->pos, n);
        p->pos += n;
        return 0;
    }

    /* partial: copy available and zero fill the rest */
    copy_and_zero_fill(out, p->data + p->pos, rem, n);
    p->pos = p->len;
    return 1; /* indicates to the caller we ran out of bytes */
}

int simple_fdp_peek_bytes(const simple_fdp* p, void* out, size_t n) {
    if (!p || !out) return 1;
    if (n == 0) return 0;

    size_t rem = simple_fdp_remaining(p);
    if (rem >= n) {
        memcpy(out, p->data + p->pos, n);
        return 0; 
    }

    /* partial: copy available and zero fill the rest; do NOT advance */
    copy_and_zero_fill(out, p->data + p->pos, rem, n);
    return 1; /* indicates to the caller we ran out of bytes */
}