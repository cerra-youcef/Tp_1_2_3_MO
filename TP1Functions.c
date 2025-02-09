#include "TP1Functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include<stdio.h>

// Helper function for sorting items by value-to-weight ratio
int compare_items(const void *a, const void *b) {
    struct Item *itemA = (struct Item *)a;
    struct Item *itemB = (struct Item *)b;
    if (itemA->ratio < itemB->ratio) {
        return 1; // Sort in descending order
    } else if (itemA->ratio > itemB->ratio) {
        return -1;
    }
    return 0;
}

// Recursive function to solve the Knapsack problem with memoization
int knapsack_recursive(int capacity, int n, int* values, int* weights, int** memo) {

    // memo variable memozise the capacity of the cartable while looping into n objects

    // Base case: If no items left or capacity is 0, return 0
    if (n == 0 || capacity == 0) {
        return 0;
    }

    // If the result of the capacity untill the object n is already computed, return it from the memo table
    if (memo[n][capacity] != -1) {
        return memo[n][capacity];
    }

    // If the current item's weight is more than the remaining capacity of the cartable, skip it
    if (weights[n - 1] > capacity) {
        memo[n][capacity] = knapsack_recursive(capacity, n - 1, values, weights, memo);
    } else {
        // Otherwise, choose the maximum of including or excluding the current item
        int include = values[n - 1] + knapsack_recursive(capacity - weights[n - 1], n - 1, values, weights, memo);
        int exclude = knapsack_recursive(capacity, n - 1, values, weights, memo);
        // we take the max between the include and exclude
        memo[n][capacity] = (include > exclude) ? include : exclude;
    }

    return memo[n][capacity];
}



int read_TP1_instance(FILE*fin,dataSet* dsptr)
{
	int rval = 0;

	//capacite b
	int b;
	//Nombre d'objets
	int n;
	rval = fscanf(fin,"%d,%d\n",&n,&b);
	dsptr->b = b;
	dsptr->n = n;
	dsptr->c = (int*)malloc(sizeof(int)*n);
	dsptr->a = (int*)malloc(sizeof(int)*n);


	int i;
	for( i = 0 ; i < n ; i++)
		rval = fscanf(fin,"%d,%d\n",&(dsptr->c[i]),&(dsptr->a[i]));

	fprintf(stderr,"\nInstance file read, we have capacity %d and there is %d items of values/weights:\n",
			b,n);
	for( i = 0 ; i < n ; i++)
		fprintf(stderr,"%d,%d\n",dsptr->c[i],dsptr->a[i]);
	fprintf(stderr,"\n");


	return rval;
}

//Algorithm 2
int KP_greedy(dataSet* dsptr)
{
int rval = 0;
    int n = dsptr->n;
    int b = dsptr->b;

    // Create an array of items with value, weight, and value-to-weight ratio
    struct Item {
        int value;  // Value of the item
        int weight; // Weight of the item
        float ratio; // Value-to-weight ratio
    };

    // Create and populate the array of items
    struct Item* items = (struct Item*)malloc(sizeof(struct Item) * n);
    for (int i = 0; i < n; i++) {
        items[i].value = dsptr->c[i];
        items[i].weight = dsptr->a[i];
        items[i].ratio = (float)dsptr->c[i] / dsptr->a[i]; // Value-to-weight ratio
    }

    // Sort items by value-to-weight ratio in descending order
    qsort(items, n, sizeof(struct Item), compare_items);

    // Array to store the greedy selection (x[i] for each item)
    int* x = (int*)malloc(sizeof(int) * n);

    // Initialize x to 0 (no items taken initially)
    for (int i = 0; i < n; i++) {
        x[i] = 0;
    }

    // Greedily select items based on their sorted value-to-weight ratio
    for (int i = 0; i < n && b > 0; i++) {
        if (items[i].weight <= b) {
            // If we can take the whole item, take it
            x[i] = 1;
            b -= items[i].weight;
        }
    }

    // Output the result (fractions of each item selected)
    printf("Selected items (fractions):\n");
    for (int i = 0; i < n; i++) {
        printf("Item %d: %d\n", i + 1, x[i]);
    }

    // Calculate and print the total value of the selected items
    int total_value = 0;
    for (int i = 0; i < n; i++) {
        total_value += x[i] * items[i].value;
    }
    printf("Total value of selected items using Kp_Greedy is: %d\n", total_value);

    // Clean up memory
    free(items);
    free(x);

	return rval;
}

//Algorithm 1
int KP_LP(dataSet* dsptr)
{
    //Initialization
	int rval = 0;
    int n = dsptr->n;
    int b = dsptr->b;

    //Create and populate our array of items

    struct Item* items = (struct Item*)malloc(sizeof(struct Item) * n);
    for (int i = 0; i < n; i++) {
            items[i].value = dsptr->c[i];
            items[i].weight = dsptr->a[i];

    		// our ratio
    		items[i].ratio = (float)dsptr->c[i] / dsptr->a[i];
    		}

    // items's Descending sort (by ratio)
    // c[1]/a[1] ⩾ c[2]/a[2] ⩾ · · · ⩾ c[n]/a[n]
    qsort(items, n, sizeof(struct Item), compare_items);

    // Array to store the fractionnel selections x[i] for revery item
    float* x = (float*)malloc(sizeof(float) * n);

    // initialization of x
    for (int i = 0; i < n; i++) {
            x[i] = 0;
        }

    for (int i = 0; i < n && b > 0; i++) {
      		  // x[j] ← min{¯b/a[j], 1};
          if (items[i].weight <= b) {
              x[i] = 1;
              b -= items[i].weight;

          } else {
              // Otherwise, take a fraction of it
              x[i] = (float)b / items[i].weight;
              b = 0; // The knapsack is full now
          }
      }

    //Output the result (fraction of each item taken)
    printf("Selected items (fractions):\n");
    for (int i = 0; i < n; i++) {
        printf("Item %d: %.2f\n", i + 1, x[i]);
    }

    // Calculate and print the total value of the selected items
    float total_value = 0;
    for (int i = 0; i < n; i++) {
        total_value += x[i] * items[i].value;
    }
    printf("Total value of selected items using KP_LP is: %.2f\n", total_value);

    // Clean up memory
    free(items);
    free(x);

	return rval;
}

//------------------------------------------------------------------------------TP2-----------------------------------------------------------------------------------------------------------

// Function to solve the Knapsack problem using recursive DP
int Recursive_Dynamic_programming(dataSet* dsptr) {
    int n = dsptr->n;
    int b = dsptr->b;

    // Create a memorization table initialized to -1
    int** memo = (int**)malloc(sizeof(int*) * (n + 1));
    for (int i = 0; i <= n; i++) {
        memo[i] = (int*)malloc(sizeof(int) * (b + 1));
        memset(memo[i], -1, sizeof(int) * (b + 1));
    }

    // Call the recursive function to compute the maximum value
    int max_value = knapsack_recursive(b, n, dsptr->c, dsptr->a, memo);

    // Output the result
    printf("Maximum value of the knapsack using dynamic programmation is: %d\n", max_value);

    // Clean up memory
    for (int i = 0; i <= n; i++) {
        free(memo[i]);
    }
    free(memo);

    return max_value;
}





