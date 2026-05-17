from collections import deque
from concurrent.futures import ThreadPoolExecutor
import threading
import time


class Graph:

    def __init__(self, V):
        self.V = V
        self.adj = [[] for _ in range(V)]

    def add_edge(self, u, v):
        self.adj[u].append(v)
        self.adj[v].append(u)

    def sequential_bfs(self, start):

        visited = [False] * self.V
        queue = deque()

        visited[start] = True
        queue.append(start)

        print("Sequential BFS Order:", end=" ")

        while queue:

            node = queue.popleft()
            print(node, end=" ")

            for neigh in self.adj[node]:

                if not visited[neigh]:
                    visited[neigh] = True
                    queue.append(neigh)

        print()

    def parallel_bfs(self, start):

        visited = [False] * self.V
        visited[start] = True

        frontier = [start]
        lock = threading.Lock()

        print("Parallel BFS Order:", end=" ")

        while frontier:

            next_frontier = []

            for node in frontier:
                print(node, end=" ")

            def process(node):

                local_nodes = []

                for neigh in self.adj[node]:

                    with lock:
                        if not visited[neigh]:
                            visited[neigh] = True
                            local_nodes.append(neigh)

                return local_nodes

            with ThreadPoolExecutor() as executor:
                results = executor.map(process, frontier)

            for res in results:
                next_frontier.extend(res)

            frontier = next_frontier

        print()

    def sequential_dfs(self, start):

        visited = [False] * self.V
        stack = [start]

        print("Sequential DFS Order:", end=" ")

        while stack:

            node = stack.pop()

            if visited[node]:
                continue

            visited[node] = True

            print(node, end=" ")

            neighbors = self.adj[node]

            for neigh in reversed(neighbors):

                if not visited[neigh]:
                    stack.append(neigh)

        print()

    def parallel_dfs(self, start):

        visited = [False] * self.V
        visited[start] = True

        frontier = [start]
        lock = threading.Lock()

        print("Parallel DFS Order:", end=" ")

        while frontier:

            next_frontier = []

            for node in frontier:
                print(node, end=" ")

            def process(node):

                local_nodes = []

                neighbors = self.adj[node]

                for neigh in reversed(neighbors):

                    with lock:
                        if not visited[neigh]:
                            visited[neigh] = True
                            local_nodes.append(neigh)

                return local_nodes

            with ThreadPoolExecutor() as executor:
                results = executor.map(process, frontier)

            for res in results:
                next_frontier.extend(res)

            next_frontier.reverse()

            frontier = next_frontier

        print()


V = int(input("Enter number of vertices: "))
E = int(input("Enter number of edges: "))

g = Graph(V)

print("Enter edges:")

for _ in range(E):
    u, v = map(int, input().split())
    g.add_edge(u, v)

start_node = int(input("Enter starting vertex: "))

print("\n--- BFS and DFS Traversal ---\n")

start = time.time()
g.sequential_bfs(start_node)
end = time.time()

print("Sequential BFS Time:", end - start, "seconds\n")

start = time.time()
g.parallel_bfs(start_node)
end = time.time()

print("Parallel BFS Time:", end - start, "seconds\n")

start = time.time()
g.sequential_dfs(start_node)
end = time.time()

print("Sequential DFS Time:", end - start, "seconds\n")

start = time.time()
g.parallel_dfs(start_node)
end = time.time()

print("Parallel DFS Time:", end - start, "seconds")





# Enter number of vertices: 10
# Enter number of edges: 15
# Enter edges:
# 0 1
# 0 2
# 1 3
# 1 4
# 2 5
# 2 6
# 3 7
# 4 7
# 5 8
# 6 8
# 7 9
# 8 9
# 3 4
# 5 6
# 7 8
# Enter starting vertex: 0