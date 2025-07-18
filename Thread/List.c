
#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef struct Node {
    int data;
    struct Node* next;
} Node;


// Helper function to create a node
Node* create_node(int data) {
    Node* res=(Node*)malloc(sizeof(Node));
    res->data = data;
    res->next = NULL;
    return res;

}

// Helper function to print the list
void print_list(Node* head) {
    while (head!=NULL) {
        printf("%d-->\n", head->data);
        head = head->next;
    }
}

// Helper function to free the entire list
void free_list(Node** head) 
    {
        Node* Nodedelete;
        Node* temp = *head;
        while (temp) {
           Nodedelete = temp;
           temp = temp->next;
            free(Nodedelete);
        }

    }
void delete_node(Node** head, int value) {

    Node* temp = *head;
    //special case
    Node* NodeToDelete = NULL;
    if ((*head)->data == value) {
        temp = *head;
        (*head) = (*head)->next;
        free(temp);
        return;
    }

    while (temp->next!=NULL) {

        if (temp->next->data == value) {
            NodeToDelete = temp->next;
            temp->next = temp->next->next;
            free(NodeToDelete);
            return;
        }

        temp = temp->next;
    }

}

int main() {
    Node* head = NULL;

    // Create test list: 1 -> 2 -> 3 -> 4 -> 5
    printf("Creating list: 1 -> 2 -> 3 -> 4 -> 5\n");

    // Build list backwards to get 1->2->3->4->5
    head = create_node(5);
    head->next = create_node(4);
    head->next->next = create_node(3);
    head->next->next->next = create_node(2);
    head->next->next->next->next = create_node(1);

    // Actually, let's build it forward properly
    free_list(&head);
    head = create_node(1);
    head->next = create_node(2);
    head->next->next = create_node(3);
    head->next->next->next = create_node(4);
    head->next->next->next->next = create_node(5);

    print_list(head);

    // Test 1: Delete middle node (3)
    printf("\nTest 1: Delete node with value 3\n");
    delete_node(&head, 3);
    print_list(head);

    // Test 2: Delete head node (1)
    printf("\nTest 2: Delete head node with value 1\n");
    delete_node(&head, 1);
    print_list(head);

    // Test 3: Delete tail node (5)
    printf("\nTest 3: Delete tail node with value 5\n");
    delete_node(&head, 5);
    print_list(head);

    // Test 4: Delete non-existent node (99)
    printf("\nTest 4: Try to delete non-existent node with value 99\n");
    delete_node(&head, 99);
    print_list(head);

    // Test 5: Delete remaining nodes
    printf("\nTest 5: Delete remaining nodes (4, then 2)\n");
    delete_node(&head, 4);
    print_list(head);
    delete_node(&head, 2);
    print_list(head);

    // Test 6: Delete from empty list
    printf("\nTest 6: Try to delete from empty list\n");
    delete_node(&head, 1);
    print_list(head);

    printf("\nAll tests completed!\n");

    return 0;
}