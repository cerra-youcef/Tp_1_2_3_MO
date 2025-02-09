#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <assert.h>




typedef struct dataSet
{
	//Attributes of the instance
	//Nombre d'objets
	int n;
	//Capacite b
	int b;
	//Tableau d'entiers de taille n contenant la valeur de chacun des objets
	int*c;
	//Tableau d'entiers de taille n contenant le poids de chacun des objets
	int*a;


} dataSet;

//Create an array of items which every item has a weight , value and ratio (to sort)
struct Item {
	int value;  // Value of the item
	int weight; // Weight of the item
	float ratio; // Value-to-weight ratio
};
//Read our dataset
int read_TP1_instance(FILE*fin,dataSet* dsptr);

//TP1
int compare_items(const void *a, const void *b);

//Algorithm 2
int KP_greedy(dataSet* dsptr);
//Algorithm 1
int KP_LP(dataSet* dsptr);


//TP2
int Recursive_Dynamic_programming(dataSet* dsptr);
int knapsack_recursive(int capacity, int n, int* values, int* weights, int** memo);


