/*
  Collection of non-cryptographic string hashing functions.
  All code is Public Domain.
*/

#include "stringhash.h"

hash_t add_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h += data[i];
 }
 return h;
}

hash_t xor_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h ^= data[i];
 }
 return h;
}

hash_t rot_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h = (h<<4) ^ (h>>28) ^ data[i];
 }
 return h;
}

// Bernstein's hash
hash_t djb_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h = 33*h + data[i];
 }
 return h;
}

// modified Bernstein's hash
hash_t djb2_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h = 33*h ^ data[i];
 }
 return h;
}

hash_t sax_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h ^= (h<<5) + (h>>2) + data[i];
 }
 return h;
}

// Fowler / Noll / Vo (FNV) Hash
hash_t fnv_hash(const data_t *data, const size_t size)
{
 hash_t h = 2166136261UL;
 for (size_t i = 0; i < size; i++ )
 {
  h = (h*16777619) ^ data[i];
 }
 return h;
}

// One-at-a-Time Hash
hash_t oat_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 for (size_t i = 0; i < size; i++)
 {
  h += data[i];
  h += (h<<10);
  h ^= (h>>6);
 }
 h += (h<<3);
 h ^= (h>>11);
 h += (h<<15);
 return h;
}

hash_t jsw_hash(const data_t *data, const size_t size, const hash_t *magic)
{
 hash_t h = 16777551;
 for (size_t i = 0; i < size; i++)
 {
  h = ( h << 1 | h >> 31 ) ^ magic[data[i]];
 }
 return h;
}

hash_t elf_hash(const data_t *data, const size_t size)
{
 hash_t h = 0, g;
 for (size_t i = 0; i < size; i++)
 {
  h = (h<<4) + data[i];
  g = h & 0xf0000000L;
  if (0!=g)
  {
   h ^= (g>>24);
  }
  h &= ~g;
 }
 return h;
}

void jen_mix(hash_t& a, hash_t& b, hash_t& c)
{
 a -= b; a -= c; a ^= (c>>13);
 b -= c; b -= a; b ^= (a<<8);
 c -= a; c -= b; c ^= (b>>13);
 a -= b; a -= c; a ^= (c>>12);
 b -= c; b -= a; b ^= (a<<16);
 c -= a; c -= b; c ^= (b>>5);
 a -= b; a -= c; a ^= (c>>3);
 b -= c; b -= a; b ^= (a<<10);
 c -= a; c -= b; c ^= (b>>15);
}

hash_t jen_hash(const data_t *data, const size_t size, const hash_t magic)
{
 const hash_t d = 0x9e3779b9;
 hash_t a = d, b = d;
 hash_t c = magic;
 size_t l = size;
 data_t *k = (data_t *)data;

 while (l>=12)
 {
  a += (data[0] + ((hash_t)k[1]<<8) + ((hash_t)k[2]<<16) + ((hash_t)k[3]<<24));
  b += (data[4] + ((hash_t)k[5]<<8) + ((hash_t)k[6]<<16) + ((hash_t)k[7]<<24));
  c += (k[8] + ((hash_t)k[9]<<8) + ((hash_t)k[10]<<16) + ((hash_t)k[11]<<24));
  jen_mix(a,b,c);
  k += 12; l -= 12;
 }
 c += size;
 switch (l)
 {
  case 11: c += ((hash_t)k[10]<<24);
  case 10: c += ((hash_t)k[9]<<16);
  case 9 : c += ((hash_t)k[8]<<8);
  // first byte of c reserved for length
  case 8 : b += ((hash_t)k[7]<<24);
  case 7 : b += ((hash_t)k[6]<<16);
  case 6 : b += ((hash_t)k[5]<<8);
  case 5 : b += k[4];
  case 4 : a += ((hash_t)k[3]<<24);
  case 3 : a += ((hash_t)k[2]<<16);
  case 2 : a += ((hash_t)k[1]<<8);
  case 1 : a += k[0];
 }
 jen_mix(a,b,c);
 return c;
}

hash_t sdbm_hash(const data_t *data, const size_t size)
{
 hash_t h = 0;
 //data_t c;
 for (size_t i = 0; i < size; i++)
 {
  h -= (h<<6) + (h<<16) +data[i];
 }
 return h;
}
