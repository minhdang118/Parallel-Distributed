# Parallel Knapsack

## How to run
1. Open a terminal and navigate to the directory containing the files.
2. Compile the files using `make`.
3. Run the program using `./parallel_knapsack`.
4. Check the generated output file `averages.txt` for the results.

## Explanation of the program
The program uses dynamic programming to solve the knapsack problem. I coded the serial part again by myself, and the parallelization was done to the part where the left-over capacity is compared to the weight of the next item. This was possible because the comparisons are independent with each other.