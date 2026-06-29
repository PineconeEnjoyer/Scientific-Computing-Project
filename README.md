# Scientific Computing Project 💻

Welcome to the Scientific Computing Project! This GitHub repository demonstrates parallel programming techniques by implementing the Producer-Consumer Problem. In this project, we use computational load balancing to study properties of concurrency and synchronization in various parallel systems.

---

## Project Overview

Scientific computing often requires heavy parallelization to efficiently process large datasets. This particular project aims at investigating two major types of parallel programming: shared-memory and distributed memory parallelism. By building a producer-consumer system using standard C++ threads and the Message Passing Interface (MPI), the project evaluates the speed and efficiency of sorting large arrays using blocking routines, non-blocking routines, and thread synchronization elements.

---

## Repository Structure

The project code is divided into modular C++ files, each representing a different approach to the parallel computing assignment.

| File Name | Description |
| :--- | :--- |
| **`SC_Threads.cpp`** | Implementation of the Producer-Consumer model using shared-memory C++ standard threads, a custom thread-safe FIFO queue, mutexes, and atomic variables. |
| **`SC_MPI.cpp`** | Implementation of the distributed-memory model using standard blocking MPI routines (`MPI_Send`, `MPI_Recv`). |
| **`SC_MPI_NON.cpp`** | Optimized distributed-memory model utilizing non-blocking MPI routines (`MPI_Isend`, `MPI_Irecv`) to increase parallelism. |
| **`SC-proj-plan-new.docx`** | The documented methodology, laboratory tasks, and project guidelines. |

---

## Built With

* **C++** - The primary language used for system-level parallel programming.
* **C++ Standard Library** - Utilizing `<thread>`, `<mutex>`, and `<atomic>` for shared-memory synchronization.
* **MS MPI** - Microsoft Message Passing Interface used to manage distributed-memory communications.

---

## How It Works

1. **Initialization:** A maximum array size and total number of arrays are defined. For the threaded version, a thread-safe FIFO queue is established.
2. **Producer Execution:** A producer entity (a dedicated thread or MPI rank 0) generates arrays filled with random integers and pushes them to the queue or sends them directly to consumers.
3. **Consumer Execution:** Consumer entities (worker threads or MPI ranks 1 to N) receive the arrays, sort the elements in memory, and track the total number of arrays they have successfully processed.
4. **Performance Measurement:** The system logs the time taken by the consumers to evaluate the efficiency and hardware scaling of the different parallelization strategies.

---

## Acknowledgments

Thank you for checking out this project! Happy computing! 🚀
