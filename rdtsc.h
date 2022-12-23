#ifndef RDTSC_H
#define RDTSC_H

#include <stddef.h>
#include <stdint.h>

inline uint64_t RDTSC_START (void) {
	unsigned cycles_low, cycles_high;
	asm volatile ( "RDTSC\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t"
		: "=r" (cycles_high), "=r" (cycles_low)::
		"%rax", "%rbx", "%rcx", "%rdx");
	return ((uint64_t) cycles_high << 32) | cycles_low;
}

#endif
