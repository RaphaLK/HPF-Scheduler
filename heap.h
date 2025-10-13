/*****
 * We need a priority queue, I'm putting a heap implementation here!
 *
 * - Raphael K. 10/11/2025
 */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct Heap {
    int* array;
    int size;
    int capacity;
} Heap;

// Function to create a heap
Heap* createHeap(int capacity)
{
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (int*)malloc(capacity * sizeof(int));
    return heap;
}

// Function to swap two integers
void swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to heapify the node at index i
void heapify(Heap* heap, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap->size
        && heap->array[left] > heap->array[largest])
        largest = left;

    if (right < heap->size
        && heap->array[right] > heap->array[largest])
        largest = right;

    if (largest != i) {
        swap(&heap->array[i], &heap->array[largest]);
        heapify(heap, largest);
    }
}

// Function to build a max heap from an existing array
void buildHeap(Heap* heap)
{
    int n = heap->size; // Get the number of elements in the
                        // heap

    // Start from the last non-leaf node (parent of the last
    // leaf) and heapify all levels in reverse order
    for (int i = (n - 1) / 2; i >= 0; i--)
        heapify(heap, i);
}

// Function to increase the value at a given index
void increaseKey(Heap* heap, int index, int newValue)
{
    if (index >= heap->size
        || heap->array[index] >= newValue) {
        printf(
            "Invalid index or new value is not greater\n");
        return;
    }

    heap->array[index] = newValue;
    while (index != 0
           && heap->array[(index - 1) / 2]
                  < heap->array[index]) {
        swap(&heap->array[index],
             &heap->array[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

// Function to insert a new value into the heap
void insertHeap(Heap* heap, int value)
{
    if (heap->size == heap->capacity) {
        printf("Heap overflow\n");
        return;
    }

    heap->size++;
    int i = heap->size - 1;
    heap->array[i] = value;

    // Fix the heap property if it is violated
    while (i != 0
           && heap->array[(i - 1) / 2] < heap->array[i]) {
        swap(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Function to extract the root (maximum element)
int extractMax(Heap* heap)
{
    if (heap->size <= 0)
        return INT_MIN;
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }

    // Store the maximum value, and remove it
    int root = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    heapify(heap, 0);

    return root;
}

// Function to print the heap
void printHeap(Heap* heap)
{
    for (int i = 0; i < heap->size; ++i)
        printf("%d ", heap->array[i]);
    printf("\n");
}

// Function to delete an element at a given index
void deleteKey(Heap* heap, int index)
{
    if (index >= heap->size) {
        printf("Invalid index\n");
        return;
    }

    // If the element to be deleted is the last element,
    // simply reduce size
    if (index == heap->size - 1) {
        heap->size--;
        return;
    }

    // Move the last element to the index of the element to
    // be deleted
    heap->array[index] = heap->array[heap->size - 1];
    heap->size--;

    // Heapify down to maintain heap property
    heapify(heap, index);
}