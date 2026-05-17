from concurrent.futures import ThreadPoolExecutor
import threading
import time


def print_array(arr):

    print("\nArray elements are:")

    for num in arr:
        print(num, end=" ")

    print()


# Parallel Minimum
def find_min(arr):

    min_val = [arr[0]]
    lock = threading.Lock()

    start = time.time()

    def check_min(value):

        with lock:

            min_val[0] = min(min_val[0], value)

    with ThreadPoolExecutor() as executor:

        executor.map(check_min, arr)

    end = time.time()

    print("\nMinimum Value =", min_val[0])

    print("Parallel Time =", end - start, "seconds")


# Parallel Maximum
def find_max(arr):

    max_val = [arr[0]]
    lock = threading.Lock()

    start = time.time()

    def check_max(value):

        with lock:

            max_val[0] = max(max_val[0], value)

    with ThreadPoolExecutor() as executor:

        executor.map(check_max, arr)

    end = time.time()

    print("\nMaximum Value =", max_val[0])

    print("Parallel Time =", end - start, "seconds")


# Parallel Average
def find_average(arr):

    total = [0]
    lock = threading.Lock()

    start = time.time()

    def add_value(value):

        with lock:

            total[0] += value

    with ThreadPoolExecutor() as executor:

        executor.map(add_value, arr)

    end = time.time()

    avg = total[0] / len(arr)

    print("\nSum =", total[0])

    print("Average =", avg)

    print("Parallel Time =", end - start, "seconds")


# Main Program
n = int(input("Enter number of elements: "))

arr = list(map(int, input("Enter array elements:\n").split()))

print_array(arr)

while True:

    print("\n===== MENU =====")
    print("1. Find Minimum")
    print("2. Find Maximum")
    print("3. Find Average")
    print("4. Exit")

    choice = int(input("Enter your choice: "))

    if choice == 1:

        find_min(arr)

    elif choice == 2:

        find_max(arr)

    elif choice == 3:

        find_average(arr)

    elif choice == 4:

        print("Program Ended")
        break

    else:

        print("Invalid Choice")




# Enter number of elements: 6
# Enter array elements:
# 12 45 7 89 23 56