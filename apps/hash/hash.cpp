#include <iostream>
#include <cstdint>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>

#include <sparsehash/dense_hash_map>
#include <libdune/dune.h>

using hmap = google::dense_hash_map<uint64_t, uint64_t>;
using keys = std::vector<uint64_t>;
using cycles = std::vector<uint64_t>;

inline uint64_t start_counter() {
    uint32_t high, low;
    asm volatile(
            "cpuid\n"
            "rdtsc\n"
            "mov %%edx, %0\n"
            "mov %%eax, %1\n"
            : "=g" (high), "=g" (low)
            :
            : "%rax", "%rbx", "%rcx", "%rdx"
            );
    return ((uint64_t) high << 32) | low;
}

inline uint64_t stop_counter() {
    uint32_t high, low;
    asm volatile(
            "rdtscp\n"
            "mov %%edx, %0\n"
            "mov %%eax, %1\n"
            "cpuid\n"
            : "=g" (high), "=g" (low)
            :
            : "%rax", "%rbx", "%rcx", "%rdx"
            );
    return ((uint64_t) high << 32) | low;
}

uint64_t measure_tsc_overhead() {
    constexpr size_t COUNT = 10000;

    uint64_t overhead = UINT64_MAX;
    uint64_t start, stop;

    for (size_t i = 0; i < COUNT; ++i) {
        start = start_counter();
        asm volatile("");
        stop = stop_counter();
        if (stop - start < overhead) {
            overhead = stop - start;
        }
    }

    return overhead;
}

void do_insert(hmap& hm, keys& build, uint64_t overhead, cycles& cs) {

    uint64_t start, stop;

    for (size_t i = 0; i < build.size(); ++i) {
        start = start_counter();
        hm[build[i]] = i;
        stop = stop_counter();
        cs[i] = stop - start - overhead;
    }
}

void do_lookup(hmap& hm, keys& probe, uint64_t overhead, uint64_t& sum, cycles& cs) {

    uint64_t start, stop;
    uint64_t lsum = 0;

    for (size_t i = 0; i < probe.size(); ++i) {
        start = start_counter();
        lsum += hm[probe[i]];
        stop = stop_counter();
        cs[i] = stop - start - overhead;
    }

    sum = lsum;
}

void generate_random_keys(keys& build, keys& probe, float hit_probability) {

    const int seed = 10;

    std::default_random_engine generator(seed);
    std::uniform_int_distribution<uint64_t> uniform(0, UINT64_MAX);
    std::uniform_int_distribution<uint64_t> uniform_probe(0, build.size() - 1);
    std::bernoulli_distribution bernoulli(hit_probability);

    for (size_t i = 0; i < build.size(); ++i) {
        build[i] = uniform(generator);
    }

    for (size_t i = 0; i < probe.size(); ++i) {
        bool hit = bernoulli(generator);
        probe[i] = hit ?
            build[uniform_probe(generator)]
            :
            uniform(generator);
    }
}

void usage() {
    extern char *__progname;

    fprintf(stderr, "Usage: %s <dune true/false> <size> <num probes> <hit probability>\n", __progname);

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {

    if (argc != 5) {
        usage();
    }

    bool do_dune_mode = atoi(argv[1]);
    uint64_t num_elements = strtoul(argv[2], NULL, 10);
    uint64_t num_probes = strtoul(argv[3], NULL, 10);
    float hit_probability = strtof(argv[4], NULL);

    int ret;

    keys build(num_elements);
    keys probe(num_probes);
    hmap hm;
    cycles build_cycles(num_elements);
    cycles probe_cycles(num_probes);

    uint64_t overhead = measure_tsc_overhead();
    printf("TSC overhead: %lu\n", overhead);

    if (do_dune_mode) {
        ret = dune_init_and_enter();
        if (ret) {
            dune_printf("Failed to initialize Dune\n");
            exit(ret);
        }

        dune_printf("Entering Dune mode\n");
    }

    hm.set_empty_key(UINT64_MAX);
    hm.max_load_factor(0.5);

    generate_random_keys(build, probe, hit_probability);
    do_insert(hm, build, overhead, build_cycles);
    uint64_t tmp;
    do_lookup(hm, probe, overhead, tmp, probe_cycles);

    auto prt_insert = do_dune_mode ?
        [](uint64_t x){ dune_printf("Dune insert: %lu\n", x); }
    :
        [](uint64_t x){ printf("Native insert: %lu\n", x); };

    auto prt_lookup = do_dune_mode ?
        [](uint64_t x){ dune_printf("Dune lookup: %lu\n", x); }
    :
        [](uint64_t x){ printf("Native lookup: %lu\n", x); };

    std::for_each(build_cycles.begin(), build_cycles.end(), prt_insert);
    std::for_each(probe_cycles.begin(), probe_cycles.end(), prt_lookup);

    exit(EXIT_SUCCESS);
}
