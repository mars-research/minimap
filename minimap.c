/*
 * minimap.c
 *  Analyze the latency of a random memory write (for hashtable like workload)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "rdtsc.h"

#define NUM_ELEMENTS    (1 << 26)
#define IDX_MASK        (NUM_ELEMENTS - 1)
#define CACHELINE_SIZE  64

#ifndef NDEBUG
  #define DEBUG(...)    fprintf(stderr, __VA_ARGS__)
#else
  #define DEBUG(...)    do { } while (0)
#endif

typedef struct {
  uint64_t k;
  uint64_t v;
} __attribute__((aligned)) kv_pair_t;

struct minimap {
  kv_pair_t kv;
  uint8_t padding[CACHELINE_SIZE - sizeof(kv_pair_t)];
} __attribute__((aligned(64)));


/* From https://en.wikipedia.org/wiki/Xorshift */
/* xorshift64s, variant A_1(12,25,27) with multiplier M_32 from line 3 of table 5 */
static uint64_t x;
/* Quick RNG */
uint64_t xorshift64star(void) {
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * 0x2545F4914F6CDD1DULL;
}

int main() {
  size_t sz = NUM_ELEMENTS * sizeof(struct minimap);
  struct minimap *hashmap = (struct minimap *) malloc(sz);
  uint64_t start, elapsed_cycles;
  uint64_t sum = 0;
  int i;

  if (!hashmap) {
    perror("malloc: ");
    exit(-1);
  }

  x = time(NULL); /* initial seed must be nonzero, don't use a static variable for the state if multithreaded */

  fprintf(stderr, "Allocated %zu bytes for hashmap (%u elements)\n", sz, NUM_ELEMENTS);

  start = RDTSC_START();

  for (i = 0; i < NUM_ELEMENTS; i++) {
    int idx = xorshift64star();
    DEBUG("writing to idx %u\n", idx & IDX_MASK);
    hashmap[idx & IDX_MASK].kv.k = idx;
  }

  elapsed_cycles = RDTSC_START() - start;

  fprintf(stdout, "%u random writes to the hashtable took %lu cycles (%lu per access)\n",
            NUM_ELEMENTS, elapsed_cycles, elapsed_cycles / NUM_ELEMENTS);

  /* To avoid compiler optimization, use the hashmap values */
  for (i = 0; i < NUM_ELEMENTS; i++) {
    sum += hashmap[i].kv.k;
  }

  fprintf(stderr, "*Discard this* sum %lu\n", sum);

  return 0;
}
