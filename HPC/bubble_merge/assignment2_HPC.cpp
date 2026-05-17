// ============================================================
// Assignment 2: Parallel Bubble Sort & Merge Sort (OpenMP)
// Measures and compares sequential vs parallel performance
// ============================================================

#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <chrono>
#include <random>
using namespace std;

// ────────────────────────────────────────────────────────────
// BUBBLE SORT
// ────────────────────────────────────────────────────────────

void bubbleSort_Sequential(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j+1])
                swap(arr[j], arr[j+1]);
}

// Odd-Even Transposition Sort (parallelisable bubble sort variant)
void bubbleSort_Parallel(vector<int>& arr) {
    int n = arr.size();
    for (int phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            // Even phase: compare (0,1), (2,3), (4,5) ...
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < n - 1; i += 2) {
                if (arr[i] > arr[i+1])
                    swap(arr[i], arr[i+1]);
            }
        } else {
            // Odd phase: compare (1,2), (3,4), (5,6) ...
            #pragma omp parallel for schedule(static)
            for (int i = 1; i < n - 1; i += 2) {
                if (arr[i] > arr[i+1])
                    swap(arr[i], arr[i+1]);
            }
        }
    }
}

// ────────────────────────────────────────────────────────────
// MERGE SORT
// ────────────────────────────────────────────────────────────

void merge(vector<int>& arr, int l, int m, int r) {
    vector<int> left(arr.begin()+l, arr.begin()+m+1);
    vector<int> right(arr.begin()+m+1, arr.begin()+r+1);
    int i = 0, j = 0, k = l;
    while (i < (int)left.size() && j < (int)right.size())
        arr[k++] = (left[i] <= right[j]) ? left[i++] : right[j++];
    while (i < (int)left.size())  arr[k++] = left[i++];
    while (j < (int)right.size()) arr[k++] = right[j++];
}

void mergeSort_Sequential(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    mergeSort_Sequential(arr, l, m);
    mergeSort_Sequential(arr, m+1, r);
    merge(arr, l, m, r);
}

void mergeSort_Parallel(vector<int>& arr, int l, int r, int depth = 0) {
    if (l >= r) return;
    int m = (l + r) / 2;

    if (depth < 4) { // spawn threads up to depth 4
        #pragma omp parallel sections
        {
            #pragma omp section
            mergeSort_Parallel(arr, l, m, depth+1);
            #pragma omp section
            mergeSort_Parallel(arr, m+1, r, depth+1);
        }
    } else {
        mergeSort_Sequential(arr, l, m);
        mergeSort_Sequential(arr, m+1, r);
    }
    merge(arr, l, m, r);
}

// ────────────────────────────────────────────────────────────
// UTILITY
// ────────────────────────────────────────────────────────────

vector<int> generateRandom(int n) {
    vector<int> v(n);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(1, 100000);
    for (auto& x : v) x = dist(rng);
    return v;
}

bool isSorted(const vector<int>& v) {
    for (int i = 0; i + 1 < (int)v.size(); i++)
        if (v[i] > v[i+1]) return false;
    return true;
}

void printArr(const vector<int>& v, int limit = 10) {
    for (int i = 0; i < min((int)v.size(), limit); i++)
        cout << v[i] << " ";
    if ((int)v.size() > limit) cout << "...";
    cout << endl;
}

// ────────────────────────────────────────────────────────────
// MAIN
// ────────────────────────────────────────────────────────────

int main() {
    const int BUBBLE_N = 5000;
    const int MERGE_N  = 500000;

    cout << "=================================================" << endl;
    cout << "  Parallel Sorting Algorithms using OpenMP       " << endl;
    cout << "=================================================" << endl;

    // ── Bubble Sort ──────────────────────────────────────────
    cout << "\n--- Bubble Sort (n = " << BUBBLE_N << ") ---" << endl;

    auto orig = generateRandom(BUBBLE_N);

    auto arr1 = orig;
    auto s1 = chrono::high_resolution_clock::now();
    bubbleSort_Sequential(arr1);
    auto e1 = chrono::high_resolution_clock::now();
    double t_bubble_seq = chrono::duration<double, milli>(e1-s1).count();
    cout << "Sequential sorted: " << (isSorted(arr1) ? "YES" : "NO")
         << " | Time: " << t_bubble_seq << " ms" << endl;

    auto arr2 = orig;
    auto s2 = chrono::high_resolution_clock::now();
    bubbleSort_Parallel(arr2);
    auto e2 = chrono::high_resolution_clock::now();
    double t_bubble_par = chrono::duration<double, milli>(e2-s2).count();
    cout << "Parallel   sorted: " << (isSorted(arr2) ? "YES" : "NO")
         << " | Time: " << t_bubble_par << " ms" << endl;
    cout << "Speedup (Bubble):  " << t_bubble_seq / t_bubble_par << "x" << endl;

    // ── Merge Sort ───────────────────────────────────────────
    cout << "\n--- Merge Sort (n = " << MERGE_N << ") ---" << endl;

    auto big = generateRandom(MERGE_N);

    auto arr3 = big;
    s1 = chrono::high_resolution_clock::now();
    mergeSort_Sequential(arr3, 0, (int)arr3.size()-1);
    e1 = chrono::high_resolution_clock::now();
    double t_merge_seq = chrono::duration<double, milli>(e1-s1).count();
    cout << "Sequential sorted: " << (isSorted(arr3) ? "YES" : "NO")
         << " | Time: " << t_merge_seq << " ms" << endl;

    auto arr4 = big;
    s2 = chrono::high_resolution_clock::now();
    mergeSort_Parallel(arr4, 0, (int)arr4.size()-1);
    e2 = chrono::high_resolution_clock::now();
    double t_merge_par = chrono::duration<double, milli>(e2-s2).count();
    cout << "Parallel   sorted: " << (isSorted(arr4) ? "YES" : "NO")
         << " | Time: " << t_merge_par << " ms" << endl;
    cout << "Speedup (Merge):   " << t_merge_seq / t_merge_par << "x" << endl;

    cout << "\nFirst 10 elements of sorted array: ";
    printArr(arr4);

    return 0;
}



/*

commands - 

g++ -O2 -fopenmp assignment2_HPC.cpp -o assignment2_HPC.exe

.\assignment2_HPC.exe

============================================================
 ASSIGNMENT 2: PARALLEL BUBBLE SORT & MERGE SORT (OPENMP)
============================================================

DESCRIPTION:
This program implements Bubble Sort and Merge Sort using both
sequential and parallel approaches (OpenMP) and compares their
performance.

============================================================
WHAT IS BUBBLE SORT?
============================================================
Bubble Sort is a simple comparison-based sorting algorithm
where adjacent elements are repeatedly swapped if they are in
wrong order.

- Repeated passes over array
- Largest element moves to end each pass

USED FOR:
- Teaching sorting concepts
- Small datasets only

============================================================
WHAT IS MERGE SORT?
============================================================
Merge Sort is a divide and conquer algorithm that divides the
array into halves, sorts them, and merges them.

- Recursive splitting
- Efficient merging

USED FOR:
- Large datasets
- Stable sorting requirement

============================================================
TIME COMPLEXITY:
============================================================

Bubble Sort:
Sequential: O(n^2)
Parallel:   O(n^2 / P) (theoretical, but overhead exists)

Merge Sort:
Sequential: O(n log n)
Parallel:   O((n log n)/P + overhead)

Where:
n = number of elements
P = number of threads

============================================================
SPACE COMPLEXITY:
============================================================

Bubble Sort:
O(1)

Merge Sort:
O(n)

Parallel Merge Sort:
O(n + P)

============================================================
WHAT IS SYNCHRONIZATION OVERHEAD?
============================================================
Synchronization overhead is the delay caused when multiple
threads coordinate access to shared data.

In this program:
- swap operations in Bubble Sort
- merge operations in Merge Sort

Effect:
- reduces speedup
- increases execution time

============================================================
WHAT IS TASK OVERHEAD?
============================================================
Task overhead is cost of:
- creating threads/tasks
- scheduling tasks
- switching execution context

In Merge Sort:
- recursive parallel sections create task overhead

============================================================
WHY PARALLEL MAY BE SLOWER?
============================================================
1. Small input size → overhead > computation
2. Frequent synchronization in Bubble Sort
3. Merge step is sequential bottleneck
4. Thread creation cost
5. Memory contention

============================================================
OPENMP CONCEPTS USED:
============================================================

1. #pragma omp parallel for
   → divides loop iterations among threads

2. #pragma omp parallel sections
   → executes recursive branches in parallel

3. #pragma omp section
   → defines independent parallel tasks

4. schedule(static)
   → equal division of loop work

============================================================
APPLICATIONS:
============================================================

Bubble Sort:
- Educational use only

Merge Sort:
- Large-scale sorting systems
- Databases
- External sorting

============================================================
POSSIBLE VIVA / INTERVIEW QUESTIONS:
============================================================

1. What is Bubble Sort?
Ans: Simple adjacent swapping sorting algorithm.

2. What is Merge Sort?
Ans: Divide and conquer sorting algorithm.

3. Difference between Bubble and Merge Sort?
Ans: Bubble is O(n^2), Merge is O(n log n).

4. Why Merge Sort is faster?
Ans: It divides problem into smaller subproblems.

5. What is OpenMP?
Ans: API for shared-memory parallel programming.

6. What is parallel computing?
Ans: Running multiple computations simultaneously.

7. What is synchronization overhead?
Ans: Delay due to thread coordination.

8. What is task overhead?
Ans: Cost of creating and managing threads.

9. Why Bubble Sort parallel is inefficient?
Ans: Too many swaps + synchronization cost.

10. Why Merge Sort parallel sometimes slower?
Ans: Merge step is sequential bottleneck.

11. What is race condition?
Ans: Multiple threads modifying same data incorrectly.

12. What is critical section?
Ans: Code executed by one thread at a time.

13. Why speedup is not always linear?
Ans: Due to overhead and memory limits.

14. What is divide and conquer?
Ans: Breaking problem into smaller subproblems.

15. What is scalability in HPC?
Ans: Ability to improve performance with more threads.

============================================================
KEY OBSERVATION:
============================================================

Parallel performance depends on:
- Input size
- Thread count
- Memory access pattern
- Synchronization cost

IMPORTANT:
Merge Sort scales better than Bubble Sort in parallel systems.

============================================================
*/