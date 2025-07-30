#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EVENT_TYPES 10

// Event type constants
#define EVENT_CLICK 0
#define EVENT_KEYPRESS 1
#define EVENT_TIMER 2

// Basic structure
typedef void (*callback_t)(void* data);

typedef struct callback_node {
    int id;
    callback_t callback;
    struct callback_node* next;
} callback_node_t;

typedef struct {
    callback_node_t* callbacks[MAX_EVENT_TYPES];
    int next_id;
} event_system_t;

// Function prototypes
void init_event_system(event_system_t* sys);
int add_listener(event_system_t* sys, int event_type, callback_t cb);
int remove_listener(event_system_t* sys, int event_type, int callback_id);
void trigger_event(event_system_t* sys, int event_type, void* data);
void cleanup_event_system(event_system_t* sys);

// Initialize event system
void init_event_system(event_system_t* sys) {
    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        sys->callbacks[i] = NULL;
    }
    sys->next_id = 0;
    printf("✓ Event system initialized\n");
}

// Add event listener
int add_listener(event_system_t* sys, int event_type, callback_t cb) {
    if (!sys || !cb || event_type >= MAX_EVENT_TYPES) {
        return -1;
    }

    callback_node_t* node = malloc(sizeof(callback_node_t));
    if (!node) {
        printf("Error: Memory allocation failed\n");
        return -1;
    }

    node->id = ++sys->next_id;
    node->callback = cb;
    node->next = sys->callbacks[event_type];
    sys->callbacks[event_type] = node;

    printf("✓ Added listener (ID: %d) for event type %d\n", node->id, event_type);
    return node->id;
}

// Remove event listener
int remove_listener(event_system_t* sys, int event_type, int callback_id) {
    if (!sys || event_type >= MAX_EVENT_TYPES) {
        return 0;
    }

    callback_node_t** current = &sys->callbacks[event_type];

    while (*current) {
        if ((*current)->id == callback_id) {
            callback_node_t* to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            printf("✓ Removed listener (ID: %d) for event type %d\n", callback_id, event_type);
            return 1;
        }
        current = &(*current)->next;
    }

    printf("✗ Listener (ID: %d) not found for event type %d\n", callback_id, event_type);
    return 0;
}

// Trigger event
void trigger_event(event_system_t* sys, int event_type, void* data) {
    if (!sys || event_type >= MAX_EVENT_TYPES) {
        return;
    }

    callback_node_t* current = sys->callbacks[event_type];
    int count = 0;

    while (current) {
        current->callback(data);
        current = current->next;
        count++;
    }

    printf("🔥 Triggered event type %d - called %d callbacks\n", event_type, count);
}

// Cleanup all event listeners
void cleanup_event_system(event_system_t* sys) {
    if (!sys) return;

    for (int i = 0; i < MAX_EVENT_TYPES; i++) {
        callback_node_t* current = sys->callbacks[i];
        while (current) {
            callback_node_t* next = current->next;
            free(current);
            current = next;
        }
        sys->callbacks[i] = NULL;
    }
    printf("🧹 Event system cleaned up\n");
}

// ==================== TEST APPLICATION ====================

// Sample data structures for different events
typedef struct {
    int x, y;
    char button[10];
} click_data_t;

typedef struct {
    char key;
    int shift_pressed;
} key_data_t;

typedef struct {
    int interval_ms;
    int tick_count;
} timer_data_t;

// Sample callback functions
void on_click_handler1(void* data) {
    click_data_t* click = (click_data_t*)data;
    printf("  [Handler 1] Mouse clicked at (%d, %d) with %s button\n",
        click->x, click->y, click->button);
}

void on_click_handler2(void* data) {
    click_data_t* click = (click_data_t*)data;
    printf("  [Handler 2] Click detected! Position: (%d, %d)\n",
        click->x, click->y);
}

void on_key_handler(void* data) {
    key_data_t* key = (key_data_t*)data;
    printf("  [Key Handler] Key '%c' pressed %s\n",
        key->key, key->shift_pressed ? "(with Shift)" : "");
}

void on_timer_handler1(void* data) {
    timer_data_t* timer = (timer_data_t*)data;
    printf("  [Timer 1] Tick #%d (interval: %dms)\n",
        timer->tick_count, timer->interval_ms);
}

void on_timer_handler2(void* data) {
    timer_data_t* timer = (timer_data_t*)data;
    printf("  [Timer 2] Another timer tick: %d\n", timer->tick_count);
}

// Main test function
int main(void) {
    printf("=== Simple Event Handler Test ===\n\n");

    // Initialize event system
    event_system_t event_sys;
    init_event_system(&event_sys);

    printf("\n--- Adding Event Listeners ---\n");

    // Add multiple click handlers
    int click_id1 = add_listener(&event_sys, EVENT_CLICK, on_click_handler1);
    int click_id2 = add_listener(&event_sys, EVENT_CLICK, on_click_handler2);

    // Add key handler
    int key_id = add_listener(&event_sys, EVENT_KEYPRESS, on_key_handler);

    // Add timer handlers
    int timer_id1 = add_listener(&event_sys, EVENT_TIMER, on_timer_handler1);
    int timer_id2 = add_listener(&event_sys, EVENT_TIMER, on_timer_handler2);

    printf("\n--- Triggering Events ---\n");

    // Test click events
    click_data_t click1 = { 100, 200, "left" };
    trigger_event(&event_sys, EVENT_CLICK, &click1);

    click_data_t click2 = { 300, 400, "right" };
    trigger_event(&event_sys, EVENT_CLICK, &click2);

    // Test key events
    key_data_t key1 = { 'A', 0 };
    trigger_event(&event_sys, EVENT_KEYPRESS, &key1);

    key_data_t key2 = { 'B', 1 }; // with shift
    trigger_event(&event_sys, EVENT_KEYPRESS, &key2);

    // Test timer events
    timer_data_t timer1 = { 1000, 1 };
    trigger_event(&event_sys, EVENT_TIMER, &timer1);

    timer_data_t timer2 = { 500, 2 };
    trigger_event(&event_sys, EVENT_TIMER, &timer2);

    printf("\n--- Removing Some Listeners ---\n");

    // Remove one click handler
    remove_listener(&event_sys, EVENT_CLICK, click_id1);

    // Remove one timer handler
    remove_listener(&event_sys, EVENT_TIMER, timer_id2);

    printf("\n--- Triggering Events Again ---\n");

    // Test click event again (should only call handler2)
    click_data_t click3 = { 500, 600, "middle" };
    trigger_event(&event_sys, EVENT_CLICK, &click3);

    // Test timer event again (should only call handler1)
    timer_data_t timer3 = { 2000, 3 };
    trigger_event(&event_sys, EVENT_TIMER, &timer3);

    printf("\n--- Testing Edge Cases ---\n");

    // Try to remove non-existent listener
    remove_listener(&event_sys, EVENT_CLICK, 999);

    // Try to trigger event with no listeners
    trigger_event(&event_sys, 5, NULL); // event type 5 has no listeners

    // Cleanup
    printf("\n--- Cleanup ---\n");
    cleanup_event_system(&event_sys);

    printf("\nTest completed successfully!\n");
    return 0;
}