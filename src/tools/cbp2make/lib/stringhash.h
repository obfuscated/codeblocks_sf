/*
  Collection of non-cryptographic string hashing functions.
  All code is Public Domain.
*/

#ifndef STRINGHASH_H
#define STRINGHASH_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t data_t;
typedef uint32_t hash_t;

hash_t add_hash(const data_t *data, const size_t size);
hash_t xor_hash(const data_t *data, const size_t size);
hash_t rot_hash(const data_t *data, const size_t size);
hash_t djb_hash(const data_t *data, const size_t size);
hash_t djb2_hash(const data_t *data, const size_t size);
hash_t sax_hash(const data_t *data, const size_t size);
hash_t fnv_hash(const data_t *data, const size_t size);
hash_t oat_hash(const data_t *data, const size_t size);
hash_t jsw_hash(const data_t *data, const size_t size, const hash_t *magic);
hash_t elf_hash(const data_t *data, const size_t size);
hash_t jen_hash(const data_t *data, const size_t size, const hash_t magic);
hash_t sdbm_hash(const data_t *data, const size_t size);

#endif
