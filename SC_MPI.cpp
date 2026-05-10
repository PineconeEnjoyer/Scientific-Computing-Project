#include <mpi.h>
#include <iostream>
#include <array>
#include <random>
#include <algorithm> // Sorting
#include <numeric> // Accumulate

// Initialization of array size for MPI_Send
const int ARRSIZE = 100000;

// Producer, rank 0
class producer {
    int num;   // Number of arrays to be produced 
    int cons;  // Number of consumers that will process the arrays

public:
    producer(int n, int c) : num(n), cons(c) {}

    void run() {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, 1000);
        //std::array<int, ARRSIZE> buffer;
        std::vector<std::array<int, ARRSIZE>> buffer(num);

        // Fill the array with random numbers ahead of time
        for (auto& arr : buffer) {
            for (auto& x : arr) {
                x = dist(rng);
            }
        }

        // Send arrays to consumers
        for (int i = 0; i < num; ++i) {
            // Arrays distribution in round-robin way to consumers 1 to N  
            int cons_rank = (i % cons) + 1;

            //std::cout << "Producer sending array " << i + 1 << " to consumer " << cons_rank << std::endl;
            MPI_Send(buffer[i].data(), ARRSIZE, MPI_INT, cons_rank, 0, MPI_COMM_WORLD);
        }

        // Termination message to each consumer  
        for (int rank = 1; rank <= cons; ++rank) {
            MPI_Send(nullptr, 0, MPI_INT, rank, 1, MPI_COMM_WORLD);
        }
    }
};

// Consumer, ranks 1 to N
class consumer {
    int rank;

public:
    consumer(int r) : rank(r) {}

    void run() {
        std::array<int, ARRSIZE> buffer;
        int num = 0;
        MPI_Status status;
        auto start = MPI_Wtime();

        // Receive arrays from producer and process them
        while (true) {
            MPI_Recv(buffer.data(), ARRSIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Check if the termination message is received
            if (status.MPI_TAG == 1)
                break;

            // Sorting
            std::sort(buffer.begin(), buffer.end());
            num++;
        }

        auto end = MPI_Wtime();
        std::cout << "Consumer " << rank << " sorted " << num << " arrays in " << (end - start) << " seconds.\n";
    }
};

int main(int argc, char* argv[]) {
    // Initialization of MPI environment
    MPI_Init(&argc, &argv);

    // Number of processes and process's rank
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int numConsumers = size - 1; // All ranks except 0 are consumers
    const int totalArrays = 1000;

	auto t0 = MPI_Wtime();

    // Producer initialization
    if (rank == 0) {
        producer producerInitialize(totalArrays, numConsumers);
        producerInitialize.run();
    }
    // Consumer initialization
    else {
        consumer consumerInitialize(rank);
        consumerInitialize.run();
    }

	auto t1 = MPI_Wtime();
    auto local_runtime = t1 - t0;
    auto total_runtime = 0.0;

    // Maximum runtime across all processes
    MPI_Reduce(&local_runtime, &total_runtime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

	if (rank == 0) {
        std::cout << "Total runtime for all consumers: " << total_runtime << " seconds.\n";
	}

    MPI_Finalize();
    return 0;
}
