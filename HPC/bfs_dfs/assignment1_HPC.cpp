// ============================================================
// Parallel BFS & DFS using OpenMP (Optimized Version)
// Fixes synchronization bottlenecks in BFS
// ============================================================

// ============================================================
// Parallel BFS & DFS using OpenMP (Corrected Version)
// ============================================================

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>
#include <chrono>

using namespace std;

class Graph {
public:
    int V;
    vector<vector<int>> adj;

    Graph(int v) : V(v), adj(v) {}

    // ========================================================
    // ADD EDGE
    // ========================================================
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // ========================================================
    // SEQUENTIAL BFS
    // ========================================================
    void BFS_Sequential(int start) {

        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        cout << "\n[Sequential BFS]: ";

        while (!q.empty()) {

            int node = q.front();
            q.pop();

            cout << node << " ";

            for (int nbr : adj[node]) {

                if (!visited[nbr]) {
                    visited[nbr] = true;
                    q.push(nbr);
                }
            }
        }

        cout << endl;
    }

    // ========================================================
    // PARALLEL BFS
    // LEVEL-BASED BFS USING OPENMP
    // ========================================================
    void BFS_Parallel(int start) {

        vector<bool> visited(V, false);

        vector<int> currentLevel;
        vector<int> nextLevel;

        visited[start] = true;
        currentLevel.push_back(start);

        cout << "\n[Parallel BFS]: ";

        while (!currentLevel.empty()) {

            nextLevel.clear();

            #pragma omp parallel
            {
                vector<int> localNext;

                #pragma omp for nowait
                for (int i = 0; i < currentLevel.size(); i++) {

                    int node = currentLevel[i];

                    for (int nbr : adj[node]) {

                        bool shouldInsert = false;

                        #pragma omp critical
                        {
                            if (!visited[nbr]) {
                                visited[nbr] = true;
                                shouldInsert = true;
                            }
                        }

                        if (shouldInsert)
                            localNext.push_back(nbr);
                    }
                }

                #pragma omp critical
                {
                    nextLevel.insert(
                        nextLevel.end(),
                        localNext.begin(),
                        localNext.end()
                    );
                }
            }

            for (int node : currentLevel)
                cout << node << " ";

            currentLevel = nextLevel;
        }

        cout << endl;
    }

    // ========================================================
    // SEQUENTIAL DFS
    // ========================================================
    void DFS_Sequential(int start) {

        vector<bool> visited(V, false);

        stack<int> st;
        st.push(start);

        cout << "\n[Sequential DFS]: ";

        while (!st.empty()) {

            int node = st.top();
            st.pop();

            if (!visited[node]) {

                visited[node] = true;

                cout << node << " ";

                // Reverse order for natural traversal
                for (int i = adj[node].size() - 1; i >= 0; i--) {

                    int nbr = adj[node][i];

                    if (!visited[nbr])
                        st.push(nbr);
                }
            }
        }

        cout << endl;
    }

    // ========================================================
    // PARALLEL DFS HELPER
    // TASK-BASED DFS
    // ========================================================
    void DFS_Parallel_Helper(int node, vector<bool>& visited) {

        bool skip = false;

        // Critical section for visited check
        #pragma omp critical
        {
            if (visited[node])
                skip = true;
            else
                visited[node] = true;
        }

        if (skip)
            return;

        cout << node << " ";

        // Create parallel tasks
        for (int i = 0; i < adj[node].size(); i++) {

            int nbr = adj[node][i];

            bool alreadyVisited;

            #pragma omp critical
            {
                alreadyVisited = visited[nbr];
            }

            if (!alreadyVisited) {

                #pragma omp task
                DFS_Parallel_Helper(nbr, visited);
            }
        }

        // Wait for child tasks
        #pragma omp taskwait
    }

    // ========================================================
    // PARALLEL DFS
    // ========================================================
    void DFS_Parallel(int start) {

        vector<bool> visited(V, false);

        cout << "\n[Parallel DFS]: ";

        #pragma omp parallel
        {
            #pragma omp single
            {
                DFS_Parallel_Helper(start, visited);
            }
        }

        cout << endl;
    }
};

// ============================================================
// MAIN FUNCTION
// ============================================================
int main() {

    // Set number of threads
    omp_set_num_threads(4);

    Graph g(8);

    // Graph construction
    g.addEdge(0,1);
    g.addEdge(0,2);

    g.addEdge(1,3);
    g.addEdge(1,4);

    g.addEdge(3,7);

    g.addEdge(4,5);
    g.addEdge(4,6);

    cout << "========================================\n";
    cout << " Parallel BFS & DFS using OpenMP\n";
    cout << "========================================\n";

    cout << "\nThreads Used: "
         << omp_get_max_threads()
         << endl;

    // ========================================================
    // SEQUENTIAL BFS
    // ========================================================
    auto t1 = chrono::high_resolution_clock::now();

    g.BFS_Sequential(0);

    auto t2 = chrono::high_resolution_clock::now();

    cout << "Sequential BFS Time: "
         << chrono::duration_cast
            <chrono::microseconds>(t2 - t1).count()
         << " microseconds\n";

    // ========================================================
    // PARALLEL BFS
    // ========================================================
    t1 = chrono::high_resolution_clock::now();

    g.BFS_Parallel(0);

    t2 = chrono::high_resolution_clock::now();

    cout << "Parallel BFS Time: "
         << chrono::duration_cast
            <chrono::microseconds>(t2 - t1).count()
         << " microseconds\n";

    // ========================================================
    // SEQUENTIAL DFS
    // ========================================================
    t1 = chrono::high_resolution_clock::now();

    g.DFS_Sequential(0);

    t2 = chrono::high_resolution_clock::now();

    cout << "Sequential DFS Time: "
         << chrono::duration_cast
            <chrono::microseconds>(t2 - t1).count()
         << " microseconds\n";

    // ========================================================
    // PARALLEL DFS
    // ========================================================
    t1 = chrono::high_resolution_clock::now();

    g.DFS_Parallel(0);

    t2 = chrono::high_resolution_clock::now();

    cout << "Parallel DFS Time: "
         << chrono::duration_cast
            <chrono::microseconds>(t2 - t1).count()
         << " microseconds\n";

    return 0;
}
/*


commands - 

g++ -O2 -fopenmp assignment1_HPC.cpp -o assignment1_HPC.exe

.\assignment1_HPC.exe

============================================================
 ASSIGNMENT 1: PARALLEL BFS & DFS USING OPENMP
============================================================

DESCRIPTION:
This program implements Breadth First Search (BFS) and Depth
First Search (DFS) on an undirected graph using both
sequential and parallel approaches with OpenMP.

============================================================
WHAT IS BFS?
============================================================
Breadth First Search (BFS) is a graph traversal algorithm
that visits nodes level by level.

- Starts from a source node
- Visits all neighbors first
- Then moves to next level neighbors

USED FOR:
- Shortest path in unweighted graphs
- Social network analysis
- Web crawling

============================================================
WHAT IS DFS?
============================================================
Depth First Search (DFS) is a graph traversal algorithm that
explores as deep as possible before backtracking.

- Goes deep into one path
- Backtracks when no unvisited nodes remain

USED FOR:
- Cycle detection
- Topological sorting
- Maze solving
- Connected components

============================================================
TIME COMPLEXITY:
============================================================

Sequential BFS: O(V + E)
Parallel BFS:   O((V + E)/P + overhead)

Sequential DFS: O(V + E)
Parallel DFS:   O((V + E)/P + overhead)

Where:
V = vertices
E = edges
P = number of threads

============================================================
SPACE COMPLEXITY:
============================================================

BFS: O(V)
DFS: O(V)

Parallel versions:
O(V + P) due to thread-local storage and task stack

============================================================
WHAT IS SYNCHRONIZATION OVERHEAD?
============================================================
Synchronization overhead is the extra time spent when threads
wait for each other to access shared resources safely.

In OpenMP, this happens due to:
- critical sections
- barriers
- locks

Example in this program:
- visited array updates in BFS/DFS

EFFECT:
- reduces parallel speedup
- increases execution time

============================================================
WHAT IS TASK OVERHEAD?
============================================================
Task overhead is the cost of:
- creating tasks
- scheduling tasks to threads
- switching between tasks

In DFS:
- each recursive call may become a task
- too many small tasks reduce performance

============================================================
WHY PARALLEL MAY BE SLOWER?
============================================================
1. Small graph size → overhead > computation
2. Too many critical sections
3. Frequent thread synchronization
4. Task creation cost in DFS

============================================================
OPENMP CONCEPTS USED:
============================================================

1. #pragma omp parallel
   Creates multiple threads

2. #pragma omp for
   Distributes loop iterations among threads

3. #pragma omp critical
   Ensures mutual exclusion for shared data

4. #pragma omp task
   Enables recursive parallel execution (DFS)

5. #pragma omp single
   Ensures one thread starts task generation

============================================================
APPLICATIONS:
============================================================

BFS:
- Shortest path (unweighted)
- Network broadcasting
- Social graphs

DFS:
- Cycle detection
- Topological ordering
- Path finding

============================================================
POSSIBLE VIVA / INTERVIEW QUESTIONS:
============================================================

1. What is BFS and DFS?
Ans: BFS is level-order traversal, DFS is depth-first traversal.

2. Difference between BFS and DFS?
Ans: BFS uses queue, DFS uses stack/recursion.

3. Why BFS is level-based traversal?
Ans: It explores nodes level by level.

4. Why DFS uses stack or recursion?
Ans: To go deep before backtracking.

5. What is OpenMP?
Ans: API for parallel programming in shared memory systems.

6. What is parallel computing?
Ans: Execution of multiple tasks simultaneously.

7. What is synchronization overhead?
Ans: Delay caused by thread coordination.

8. What is task overhead?
Ans: Cost of creating and managing tasks.

9. Why parallel BFS is difficult?
Ans: Due to level-wise synchronization requirement.

10. Why DFS is better suited for task parallelism?
Ans: Independent recursive calls can run in parallel.

11. What is a race condition?
Ans: When multiple threads access shared data incorrectly.

12. What is critical section?
Ans: Code accessed by only one thread at a time.

13. Why parallel program may be slower than sequential?
Ans: Due to overhead > computation for small inputs.

14. What is shared memory parallelism?
Ans: All threads access same memory space.

15. What is load balancing in parallel computing?
Ans: Even distribution of work among threads.

============================================================
KEY OBSERVATION:
============================================================

Parallel performance depends on:
- Graph size
- Number of threads
- Synchronization cost
- Task granularity

IMPORTANT:
Parallelism does NOT always guarantee speedup.

============================================================
*/