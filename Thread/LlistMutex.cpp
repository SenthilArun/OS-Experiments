#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Define the structure of a linked list node
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Define the structure of the linked list with a mutex for synchronization
typedef struct LinkedList {
    Node* head;
    HANDLE mutex;
} LinkedList;

// Function to initialize the linked list
void initList(LinkedList* list) {
    list->head = NULL;
    // Create a mutex for thread synchronization
    list->mutex = CreateMutex(NULL, FALSE, NULL);
    if (list->mutex == NULL) {
        printf("CreateMutex error: %d\n", GetLastError());
        exit(1);
    }
}

// Thread-safe function to insert a node at the beginning of the list
void insertAtBeginning(LinkedList* list, int data) {
    // Wait for ownership of the mutex
    WaitForSingleObject(list->mutex, INFINITE);

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        ReleaseMutex(list->mutex);
        return;
    }
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;

    // Release ownership of the mutex
    ReleaseMutex(list->mutex);
}

// Thread-safe function to insert a node at the end of the list
void insertAtEnd(LinkedList* list, int data) {
    // Wait for ownership of the mutex
    WaitForSingleObject(list->mutex, INFINITE);

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation error\n");
        ReleaseMutex(list->mutex);
        return;
    }
    newNode->data = data;
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
    }
    else {
        Node* temp = list->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    // Release ownership of the mutex
    ReleaseMutex(list->mutex);
}

// Thread-safe function to delete a node with a given key
void deleteNode(LinkedList* list, int key) {
    // Wait for ownership of the mutex
    WaitForSingleObject(list->mutex, INFINITE);

    Node* temp = list->head;
    Node* prev = NULL;

    // If the head node itself holds the key
    if (temp != NULL && temp->data == key) {
        list->head = temp->next;
        free(temp);
        ReleaseMutex(list->mutex); // Release ownership of the mutex before returning
        return;
    }

    // Search for the key to be deleted
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }

    // If the key was not present in the list
    if (temp == NULL) {
        ReleaseMutex(list->mutex); // Release ownership of the mutex before returning
        return;
    }

    // Unlink the node from the linked list
    prev->next = temp->next;
    free(temp);

    // Release ownership of the mutex
    ReleaseMutex(list->mutex);
}

// Thread-safe function to print the list
void printList(LinkedList* list) {
    // Wait for ownership of the mutex
    WaitForSingleObject(list->mutex, INFINITE);

    Node* temp = list->head;
    while (temp != NULL) {
        printf("%d -> ", temp->data);
        temp = temp->next;
    }
    printf("NULL\n");

    // Release ownership of the mutex
    ReleaseMutex(list->mutex);
}

// Function to destroy the list and free all resources
void destroyList(LinkedList* list) {
    // Destroy the mutex
    CloseHandle(list->mutex);

    // Free all nodes in the list
    Node* current = list->head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
}
/*
// Example usage of the linked list
int main() {
    LinkedList list;
    initList(&list);

    // Insert some elements
    insertAtBeginning(&list, 10);
    insertAtEnd(&list, 20);
    insertAtBeginning(&list, 5);

    // Print the list
    printList(&list);

    // Delete an element
    deleteNode(&list, 20);

    // Print the list again
    printList(&list);

    // Clean up
    destroyList(&list);

    return 0;
}
*/