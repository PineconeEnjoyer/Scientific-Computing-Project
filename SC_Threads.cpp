#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <array>
#include <mutex>
#include <atomic>
#include <random>
#include <chrono> // Timing
#include <algorithm> // Sorting
#include <memory>

// Initialization of array size, 'constexpr' makes it a const 
// and can be calculated during compilation
constexpr size_t ARRSIZE = 100000;
using ArrayType = std::array<int, ARRSIZE>;
using ArrayPtr = std::unique_ptr<ArrayType>;
std::mutex mtx;

class queueFIFO{
	std::queue<ArrayPtr> queue;
	size_t capasity;

public:
	queueFIFO(size_t cap) : capasity(cap) {}

	// Add elements to the end of the queue
	bool push(ArrayPtr arr) {
		std::lock_guard<std::mutex> lock(mtx);
		if (queue.size() >= capasity) // Queue is full!
			return false;
		queue.push(std::move(arr));
		return true;
	}

	// Remove elements from the beggining
	ArrayPtr pop() {
		std::lock_guard<std::mutex> lock(mtx);
		if (queue.empty()) // Queue is empty!
			return nullptr;
		auto arr = std::move(queue.front()); // Auto - complicated data type
		queue.pop();
		return arr;
	}

	// Chceck if the queue is empty
	bool empty() {
		std::lock_guard<std::mutex> lock(mtx);
		return queue.empty();
	}
};

class producer{
	queueFIFO& queue; // Reference to the queue
	size_t num; //Number of arrays to be produced 

public:
	producer(queueFIFO& q, size_t n) : queue(q), num(n) {}

	// Operator makes an object callable
	void operator()() {
		std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> dist(0, ARRSIZE);
		for (size_t i = 0; i < num; i++) {
			// Create a new array and fill it with random numbers
			auto arr = std::make_unique<ArrayType>();
			for (auto& x : *arr)
				x = dist(rng);
		
			// Yield if the queue is full
			while (!queue.push(std::move(arr))) {
				std::this_thread::yield();
			}
		}
	}
};

class consumer{
	queueFIFO& queue;
	size_t num = 0; //Number of sorted arrays
	std::atomic<bool>& done; 
	int id; // ID for output
	std::chrono::duration<double>& duration;

public:
	consumer(queueFIFO& q, std::atomic<bool>& d, int i, std::chrono::duration<double>& dur) : queue(q), done(d), id(i), duration(dur) {}

	void operator()() {
		auto start = std::chrono::high_resolution_clock::now();
		while (true) {
			auto arr = queue.pop();
			if (!arr) {
				// Yield if the queue is empty or done
				if (queue.empty() && done)
					break;
				std::this_thread::yield();
				continue;
			}
			// Sorting and checksum
			std::sort(arr->begin(), arr->end());
			long long checksum = 0;
			for (auto x : *arr) checksum += x;
			num++;
		}
		auto end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		std::lock_guard<std::mutex> lock(mtx);
		std::cout << "Consumer " << id << " sorted " << num << " arrays.\n";

	}
};

int main()
{
	const int numConsumers = 64;
	int numArrays = 10000;
	int capQueue = 10 * numConsumers;

	queueFIFO queue(capQueue);
	std::atomic<bool> done{false};

	std::cout << "\nArray Size: " << ARRSIZE;
	std::cout << "\nNumber of Arrays: " << numArrays;
	std::cout << "\nQueue Capasity: " << capQueue;
	std::cout << "\nRunning test with " << numConsumers << " consumer threads...\n";

	auto start = std::chrono::high_resolution_clock::now();

	std::thread prod(producer(queue, numArrays));

	std::vector<std::chrono::duration<double>> durations(numConsumers);
	std::vector<std::thread> consumers;

	for (size_t i = 0; i < numConsumers; ++i)
		consumers.emplace_back(consumer(queue, done, static_cast<int>(i), durations[i]));

	prod.join();
	done = true;

	for (auto& c : consumers)
		c.join();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;

	std::cout << "Total time with " << numConsumers << " consumers: " << elapsed.count() << " seconds\n";

	for (size_t i = 0; i < numConsumers; ++i)
		std::cout << "Consumer " << i << " thread time: " << durations[i].count() << " seconds\n";

	return 0;
}
