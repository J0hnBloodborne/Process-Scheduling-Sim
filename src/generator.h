#pragma once

int generate_random_csv(const char* filepath,
                        int num_processes,
                        int min_arrival, int max_arrival,
                        int min_burst, int max_burst);
