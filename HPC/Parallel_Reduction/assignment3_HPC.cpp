// ============================================================
// Assignment 3: Parallel Reduction (Min, Max, Sum, Average)
// Fixed Version (Compatible OpenMP GCC)
// ============================================================

#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <random>
using namespace std;

// ────────────────────────────────────────────────────────────
// SEQUENTIAL
// ────────────────────────────────────────────────────────────

int seq_min(const vector<int>& arr) {
    int mn = INT_MAX;
    for (int x : arr) mn = min(mn, x);
    return mn;
}

int seq_max(const vector<int>& arr) {
    int mx = INT_MIN;
    for (int x : arr) mx = max(mx, x);
    return mx;
}

long long seq_sum(const vector<int>& arr) {
    long long s = 0;
    for (int x : arr) s += x;
    return s;
}

double seq_avg(const vector<int>& arr) {
    return (double)seq_sum(arr) / arr.size();
}

// ────────────────────────────────────────────────────────────
// PARALLEL (SAFE VERSION - NO min/max reduction dependency)
// ────────────────────────────────────────────────────────────

int par_min(const vector<int>& arr) {
    int mn = INT_MAX;

    #pragma omp parallel
    {
        int local_min = INT_MAX;

        #pragma omp for nowait
        for (int i = 0; i < (int)arr.size(); i++) {
            if (arr[i] < local_min)
                local_min = arr[i];
        }

        #pragma omp critical
        {
            if (local_min < mn)
                mn = local_min;
        }
    }
    return mn;
}

int par_max(const vector<int>& arr) {
    int mx = INT_MIN;

    #pragma omp parallel
    {
        int local_max = INT_MIN;

        #pragma omp for nowait
        for (int i = 0; i < (int)arr.size(); i++) {
            if (arr[i] > local_max)
                local_max = arr[i];
        }

        #pragma omp critical
        {
            if (local_max > mx)
                mx = local_max;
        }
    }
    return mx;
}

long long par_sum(const vector<int>& arr) {
    long long s = 0;

    #pragma omp parallel for reduction(+:s)
    for (int i = 0; i < (int)arr.size(); i++) {
        s += arr[i];
    }

    return s;
}

double par_avg(const vector<int>& arr) {
    return (double)par_sum(arr) / arr.size();
}

// ────────────────────────────────────────────────────────────
// TIMER
// ────────────────────────────────────────────────────────────

template<typename Func>
double timeIt(Func f) {
    auto s = chrono::high_resolution_clock::now();
    f();
    auto e = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(e - s).count();
}

// ────────────────────────────────────────────────────────────
// MAIN
// ────────────────────────────────────────────────────────────

int main() {
    const int N = 10'000'000;

    vector<int> arr(N);

    mt19937 rng(99);
    uniform_int_distribution<int> dist(1, 1000000);

    for (auto &x : arr)
        x = dist(rng);

    cout << "=====================================================\n";
    cout << " Parallel Reduction (Fixed Version)\n";
    cout << " Array size: " << N << "\n";
    cout << " Threads: " << omp_get_max_threads() << "\n";
    cout << "=====================================================\n";

    int smin, pmin;
    double t1 = timeIt([&]{ smin = seq_min(arr); });
    double t2 = timeIt([&]{ pmin = par_min(arr); });

    cout << "\nMIN:\n";
    cout << "Seq: " << smin << " | Par: " << pmin << "\n";
    cout << "Speedup: " << t1/t2 << "x\n";

    int smax, pmax;
    t1 = timeIt([&]{ smax = seq_max(arr); });
    t2 = timeIt([&]{ pmax = par_max(arr); });

    cout << "\nMAX:\n";
    cout << "Seq: " << smax << " | Par: " << pmax << "\n";
    cout << "Speedup: " << t1/t2 << "x\n";

    long long ssum, psum;
    t1 = timeIt([&]{ ssum = seq_sum(arr); });
    t2 = timeIt([&]{ psum = par_sum(arr); });

    cout << "\nSUM:\n";
    cout << "Seq: " << ssum << " | Par: " << psum << "\n";
    cout << "Speedup: " << t1/t2 << "x\n";

    double savg, pavg;
    t1 = timeIt([&]{ savg = seq_avg(arr); });
    t2 = timeIt([&]{ pavg = par_avg(arr); });

    cout << "\nAVG:\n";
    cout << "Seq: " << savg << " | Par: " << pavg << "\n";
    cout << "Speedup: " << t1/t2 << "x\n";

    cout << "\n=====================================================\n";

    return 0;
}




/*

commands - 

g++ -O2 -fopenmp assignment3_HPC.cpp -o assignment3_HPC.exe

.\assignment3_HPC.exe

============================================================
 ASSIGNMENT 3: PARALLEL REDUCTION USING OPENMP
============================================================

WHAT IS REDUCTION?
------------------------------------------------------------
Reduction is a technique in parallel computing where a large
dataset is reduced into a single value using an operation.

Examples:
- Sum of all elements
- Minimum / Maximum
- Average

OpenMP provides a special "reduction" clause to do this safely
without race conditions.

============================================================
OPENMP REDUCTION CLAUSE
============================================================

Syntax:
#pragma omp parallel for reduction(op : variable)

Where:
- op = +, *, min, max, logical AND, OR etc.
- variable = shared result variable

Example:
#pragma omp parallel for reduction(+:sum)

============================================================
WHY REDUCTION IS NEEDED?
============================================================

If multiple threads update same variable without protection,
it causes:

❌ Race condition
❌ Incorrect results
❌ Non-deterministic output

Reduction ensures:
✔ Each thread has private copy
✔ Final safe merge operation
✔ No locking required

============================================================
TIME COMPLEXITY
============================================================

Sequential:
- Min / Max / Sum → O(N)

Parallel:
- O(N / P) + synchronization overhead

Where:
N = number of elements
P = number of threads

============================================================
SPACE COMPLEXITY
============================================================

Sequential:
- O(1)

Parallel:
- O(P) due to thread-local copies

============================================================
WHY PARALLEL MAY BE SLOWER?
============================================================

1. Thread creation overhead
2. Reduction merge overhead
3. Memory bandwidth bottleneck
4. Cache coherence cost
5. False sharing
6. Small dataset size (not enough work per thread)

============================================================
IMPORTANT CONCEPTS (HPC CORE)
============================================================

✔ Race Condition:
When multiple threads modify shared data simultaneously.

✔ Critical Section:
Only one thread can execute at a time (locks).

✔ Reduction:
Automatic parallel-safe aggregation.

✔ False Sharing:
Threads update different variables but same cache line.

✔ Cache Coherence:
Keeping CPU caches consistent across cores.

✔ Load Imbalance:
Some threads do more work than others.

============================================================
ADVANCED HPC CONCEPTS
============================================================

✔ Amdahl’s Law:
Maximum speedup is limited by sequential portion.

✔ Gustafson’s Law:
Speedup increases with problem size.

✔ Scalability:
How performance improves with more threads.

✔ Strong Scaling:
Fixed problem size, more threads.

✔ Weak Scaling:
Increase problem size with threads.

============================================================
APPLICATIONS
============================================================

- Big Data Analytics (Hadoop-style operations)
- Machine Learning (loss / gradient computation)
- Scientific simulations
- Financial modeling
- Image processing (pixel reduction)
- Sensor data aggregation

============================================================
INTERVIEW / VIVA QUESTIONS WITH ANSWERS
============================================================

1. What is reduction in OpenMP?
------------------------------------------------------------
Ans:
It combines partial results from threads into a final result
using an associative operation.

---

2. Why is reduction better than mutex/critical?
------------------------------------------------------------
Ans:
Because it avoids locking and allows parallel execution.

---

3. What is race condition?
------------------------------------------------------------
Ans:
When multiple threads access shared data without synchronization.

---

4. Difference: mutex vs reduction?
------------------------------------------------------------
Ans:
Mutex:
- Blocking
- Slower

Reduction:
- Lock-free
- Faster

---

5. Why is sum operation safe for parallel reduction?
------------------------------------------------------------
Ans:
Because addition is associative and commutative.

---

6. What operations are valid for reduction?
------------------------------------------------------------
Ans:
+ , * , min , max , logical AND/OR

---

7. Why is parallel reduction not always faster?
------------------------------------------------------------
Ans:
Because overhead > computation for small inputs.

---

8. What is false sharing?
------------------------------------------------------------
Ans:
Multiple threads modify variables on same cache line causing slowdown.

---

9. What is cache line?
------------------------------------------------------------
Ans:
Smallest unit of data transferred between RAM and CPU cache.

---

10. What is OpenMP?
------------------------------------------------------------
Ans:
API for shared-memory parallel programming using compiler directives.

---

11. What is thread?
------------------------------------------------------------
Ans:
Small execution unit inside a process.

---

12. What is speedup?
------------------------------------------------------------
Ans:
Speedup = Sequential Time / Parallel Time

---

13. What is efficiency?
------------------------------------------------------------
Ans:
Efficiency = Speedup / Number of Threads

---

14. What is Amdahl’s Law?
------------------------------------------------------------
Ans:
Speedup limited by sequential part of program.

---

15. What is Gustafson’s Law?
------------------------------------------------------------
Ans:
Speedup increases when problem size increases.

---

16. What is load balancing?
------------------------------------------------------------
Ans:
Even distribution of work among threads.

---

17. What is synchronization overhead?
------------------------------------------------------------
Ans:
Time wasted in coordinating threads.

---

18. What is memory bottleneck?
------------------------------------------------------------
Ans:
CPU waits for data from memory due to limited bandwidth.

---

19. Why min/max work in reduction?
------------------------------------------------------------
Ans:
Because they are associative operations.

---

20. Why average is computed using reduction?
------------------------------------------------------------
Ans:
Because sum is reduced in parallel then divided.

============================================================
KEY OBSERVATIONS
============================================================

✔ Reduction is best for large datasets
✔ Parallelism improves with computation-heavy tasks
✔ Memory access pattern is critical

============================================================
IMPORTANT FORMULAS
============================================================

Speedup = Ts / Tp

Efficiency = Speedup / P

Parallel Time = Computation + Overhead

============================================================
END OF THEORY
============================================================
*/