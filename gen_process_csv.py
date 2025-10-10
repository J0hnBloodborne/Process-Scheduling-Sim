import csv
import random

def generate_process_csv(filename, num_processes):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for pid in range(1, num_processes + 1):
            arrival = random.randint(0, 40)      # Arrival time: 0-100
            burst = random.randint(2, 20)        # Burst time: 2-20
            writer.writerow([pid, arrival, burst])

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print("Usage: python gen_process_csv.py <output.csv> <num_processes>")
    else:
        generate_process_csv(sys.argv[1], int(sys.argv[2]))