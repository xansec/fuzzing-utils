#ifndef SIMPLE_FDP_H
#define SIMPLE_FDP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque provider type */
typedef struct simple_fdp simple_fdp;

/* Create/destroy provider. */
simple_fdp* simple_fdp_create(const uint8_t* data, size_t len);
void simple_fdp_destroy(simple_fdp* p);

/* Query remaining bytes */
size_t simple_fdp_remaining(const simple_fdp* p);

/* Consume `n` bytes into out (out must have space for n bytes).
   Returns 1 on success (and advances cursor), 0 on failure (insufficient bytes, cursor unchanged). */
int simple_fdp_consume_bytes(simple_fdp* p, void* out, size_t n);

/* Peek n bytes without advancing cursor. Returns 1 on success, 0 if not enough bytes. */
int simple_fdp_peek_bytes(const simple_fdp* p, void* out, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_FDP_H */