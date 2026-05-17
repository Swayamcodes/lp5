from concurrent.futures import ThreadPoolExecutor
import time
import copy


def print_array(arr):
    print(*arr)


# Sequential Bubble Sort
def seq_bubble_sort(arr):

    n = len(arr)

    for i in range(n - 1):

        for j in range(n - i - 1):

            if arr[j] > arr[j + 1]:

                arr[j], arr[j + 1] = arr[j + 1], arr[j]


# Parallel Bubble Sort
def par_bubble_sort(arr):

    n = len(arr)

    for i in range(n):

        start = i % 2

        def compare_swap(j):

            if arr[j] > arr[j + 1]:

                arr[j], arr[j + 1] = arr[j + 1], arr[j]

        with ThreadPoolExecutor() as executor:

            executor.map(compare_swap,
                         [j for j in range(start, n - 1, 2)])


# Merge Function
def merge(arr, left, mid, right):

    L = arr[left:mid + 1]
    R = arr[mid + 1:right + 1]

    i = 0
    j = 0
    k = left

    while i < len(L) and j < len(R):

        if L[i] <= R[j]:

            arr[k] = L[i]
            i += 1

        else:

            arr[k] = R[j]
            j += 1

        k += 1

    while i < len(L):

        arr[k] = L[i]
        i += 1
        k += 1

    while j < len(R):

        arr[k] = R[j]
        j += 1
        k += 1


# Sequential Merge Sort
def seq_merge_sort(arr, left, right):

    if left < right:

        mid = (left + right) // 2

        seq_merge_sort(arr, left, mid)
        seq_merge_sort(arr, mid + 1, right)

        merge(arr, left, mid, right)


# Parallel Merge Sort
def par_merge_sort(arr, left, right):

    if left < right:

        mid = (left + right) // 2

        with ThreadPoolExecutor() as executor:

            future1 = executor.submit(par_merge_sort, arr, left, mid)
            future2 = executor.submit(par_merge_sort, arr, mid + 1, right)

            future1.result()
            future2.result()

        merge(arr, left, mid, right)


# Main Program
n = int(input("Enter number of elements: "))

original = list(map(int, input("Enter array elements:\n").split()))

while True:

    print("\n===== MENU =====")
    print("1. Sequential Bubble Sort")
    print("2. Parallel Bubble Sort")
    print("3. Sequential Merge Sort")
    print("4. Parallel Merge Sort")
    print("5. Exit")

    choice = int(input("Enter your choice: "))

    arr = copy.deepcopy(original)

    if choice == 1:

        start = time.time()

        seq_bubble_sort(arr)

        end = time.time()

        print("\nSorted Array:")
        print_array(arr)

        print("Sequential Bubble Sort Time:",
              end - start, "seconds")

    elif choice == 2:

        start = time.time()

        par_bubble_sort(arr)

        end = time.time()

        print("\nSorted Array:")
        print_array(arr)

        print("Parallel Bubble Sort Time:",
              end - start, "seconds")

    elif choice == 3:

        start = time.time()

        seq_merge_sort(arr, 0, n - 1)

        end = time.time()

        print("\nSorted Array:")
        print_array(arr)

        print("Sequential Merge Sort Time:",
              end - start, "seconds")

    elif choice == 4:

        start = time.time()

        par_merge_sort(arr, 0, n - 1)

        end = time.time()

        print("\nSorted Array:")
        print_array(arr)

        print("Parallel Merge Sort Time:",
              end - start, "seconds")

    elif choice == 5:

        print("Program Ended")
        break

    else:

        print("Invalid Choice")




# Enter number of elements: 8
# Enter array elements:
# 64 34 25 12 22 11 90 5