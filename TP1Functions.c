#include "TP1Functions.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include<stdio.h>

void generateCSV(dataSet* dsptr, int* _x, int new_b) {
    FILE* file = fopen("new_dataset.csv", "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        return;
    }

    // Count the number of remaining objects (where _x[i] == -1)
    int new_n = 0;
    for (int i = 0; i < dsptr->n; i++) {
        if (_x[i] == -1) {
            new_n++;
        }
    }

    // Write the new header with updated `n` and remaining capacity `b`
    fprintf(file, "%d,%d\n", new_n, new_b);

    // Write the remaining objects (only those with _x[i] == -1)
    for (int i = 0; i < dsptr->n; i++) {
        if (_x[i] == -1) {
            fprintf(file, "%d,%d\n", dsptr->c[i], dsptr->a[i]);
        }
    }

    fclose(file);
    printf("New dataset saved as '%s' with %d objects and remaining capacity = %d.\n", "new_dataset", new_n, new_b);
}



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

	return total_value;
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

    int p = -1 ;
    for (int i = 0; i < n && b > 0; i++) {
      		  // x[j] ← min{¯b/a[j], 1};
          if (items[i].weight <= b) {
              x[i] = 1;
              b -= items[i].weight;

          } else {
               p = i+1;
              // Otherwise, take a fraction of it
              x[i] = (float)b / items[i].weight;
              b = 0; // The knapsack is full now
          }
      }

      printf("la valeur de p critique = %d", p);

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

	return total_value;
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


//--------------------------------------------------------------- TP3 --------------------------------------------------------------------

// Fonction principale pour le prétraitement des variables du problème du sac-à-dos
void knapsack_variable_preprocessing(dataSet* dsptr) {
    int n = dsptr->n;
    int b = dsptr->b;

        struct Item* items = (struct Item*)malloc(sizeof(struct Item) * n);
        for (int i = 0; i < n; i++) {
            items[i].value = dsptr->c[i];
            items[i].weight = dsptr->a[i];
            items[i].ratio = (float)dsptr->c[i] / dsptr->a[i];
        }

        qsort(items, n, sizeof(struct Item), compare_items);

        // Finding the p value (where we exceed capacity)
        int p_value = -1, remaining_capacity = 0;
        for (int i = 0; i < n; i++) {
            remaining_capacity += items[i].weight;
            if (remaining_capacity > b) {
                p_value = i + 1;
                break;
            }
        }

        if (p_value == -1) {
            printf("No variable to preprocess.\n");
            free(items);
            return;
        }

        printf("The p value is: %d\n\n", p_value);

        // Compute z_x (Greedy) and z_bar (LP Relaxation)
        float z_x = KP_greedy(dsptr);
        float z_bar = KP_LP(dsptr);
        float threshold = z_bar - z_x;
        printf("The threshold value is: %.2f\n\n", threshold);

        float* x = (float*)malloc(sizeof(float) * n);
        int* _x = (int*)malloc(sizeof(int) * n);
        int used_capacity = 0;
        int remaining_items = 0; // Count items that remain (_x[i] == -1)

        // calculating the _cj values
        for (int i = 0; i < n; i++) {
            // storing the _cj value in the x array
            x[i] = fabs( items[i].value - items[i].weight * ( items[p_value-1].ratio ));
            printf("_c%d = |%d - %d * %.2f| = %.2f\n", i + 1, items[i].value, items[i].weight, items[p_value-1].ratio, x[i]);
        }

        // Calculate _x[i] values
        for (int i = 0; i < n; i++) {
            if (x[i] >= threshold) {
                if (i < p_value - 1) {
                    _x[i] = 1;
                    used_capacity += items[i].weight;
                } else {
                    _x[i] = 0;
                }
            } else {
                _x[i] = -1;
                remaining_items++;
            }
        }

        // Print the _x array
        printf("Updated _x values:\n");
        for (int i = 0; i < n; i++) {
            printf("_X[%d] = %d\n", i + 1, _x[i]);
        }

        // Compute new remaining capacity
        int new_b = dsptr->b - used_capacity;
        printf("Updated remaining capacity: %d\n", new_b);

        // Stop recursion if there are no more items to process OR capacity doesn't change
        if (remaining_items == 0 || new_b <= 0 || new_b == dsptr->b) {
            printf("Stopping recursion: No remaining items or knapsack is full.\n");
            free(items);
            free(x);
            free(_x);
            return;
        }

        // Generate new dataset and call recursively
        printf("Remaining capacity in knapsack: %d\n", new_b);
        generateCSV(dsptr, _x, new_b);

        FILE* newFile = fopen("new_dataset.csv", "r");
        if (newFile != NULL) {
            dataSet data;

            char instance_file[1024];
            snprintf(instance_file, 1024, "%s", "new_dataset.csv");
            //Open the instance file
            FILE* fin = fopen(instance_file,"r");
            read_TP1_instance(fin,&data);
            fclose(fin);

            knapsack_variable_preprocessing(&data); // Recursive call

            // Free allocated memory for the new dataset
            free(data.c);
            free(data.a);
        } else {
            printf("Error opening new dataset file!\n");
        }

        free(items);
        free(x);
        free(_x);
    }








