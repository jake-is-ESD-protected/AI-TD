#include "percentileCalculator/percentileCalculator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Function to sort an array using qsort
int compare_floats(const void *a, const void *b)
{
    float da = *(const float *)a;
    float db = *(const float *)b;
    return (da > db) - (da < db);
}

// Function to calculate the 98th percentile
float findPercentile(float *values, int num_values, float percentile)
{
    // Handle edge cases
    if (num_values == 0) {
        return 0.0; // Or handle the error as you see fit
    }
    if (percentile <= 0.0 || percentile >= 100.0) {
        return 0.0; // Or handle invalid percentile
    }
    float values_copy[num_values];
    memcpy(values_copy, values, num_values * sizeof(float)); 
    // Sort the array (necessary for percentile calculation)
    qsort(values_copy, num_values, sizeof(float), compare_floats);

    // Calculate the index of the percentile
    int index = (int)ceil((percentile / 100.0) * num_values) - 1;

    // Return the value at that index
    return values_copy[index];
}