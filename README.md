
A tiny hashtable-ish workload to understand the latency of random memory
writes. The program creates a hashtable structure (aligned kv pairs) and writes
to them randomly. We measure the time taken for a single random write using `rdtsc`

## Build

```
make
```

## Run

```
./minimap
```
