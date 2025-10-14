#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void seed_once(void) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
}

static int rand_in_range(int lo, int hi) {
    if (hi < lo) { int t = lo; lo = hi; hi = t; }
    return lo + (rand() % (hi - lo + 1));
}

int generate_random_csv(const char* filepath,
                        int num_processes,
                        int min_arrival, int max_arrival,
                        int min_burst, int max_burst)
{
    if (!filepath || num_processes <= 0) return 1;
    FILE* f = fopen(filepath, "w");
    if (!f) return 2;

    seed_once();

    for (int pid = 1; pid <= num_processes; ++pid) {
        int arrival = rand_in_range(min_arrival, max_arrival);
        int burst   = rand_in_range(min_burst, max_burst);
        fprintf(f, "%d,%d,%d\n", pid, arrival, burst);
    }

    fclose(f);
    return 0;
}
