#if def BEGINNER
#include<stdio.h>
#include<stdint.h>
#define MAX_CALLBACKS 5
typedef void (*ISR)(void);
typedef (*func1)(void);
typedef (*func2)(void);
typedef (*func3)(void);
#include<windows.h>

HANDLE mutex;
ISR ISRCallback[MAX_CALLBACKS];

void registercallback(int id,ISR  cb) {
	
	if (id < 0 && id >= MAX_CALLBACKS) {
		return;
	}
	WaitForSingleObject(mutex, INFINITE);
	ISRCallback[id] = cb;
	ReleaseMutex(mutex);
}


void  callback1(void) {
	printf("Iam a callback1..");
	return;
}


void callback2(void) {
	printf("Iam a callback2..");
	return;
}



void callback3(void) {
	printf("Iam a callback2..");
	return;
}


int main() {
	
	mutex = CreateMutex(NULL, FALSE, NULL);
	//for (int i = 0;i < MAX_CALLBACKS;i++)
		registercallback(1,callback1);
		registercallback(2, callback2);
		registercallback(3, callback3);
			
		ISRCallback[1]();
        ISRCallback[2]();
        ISRCallback[3]();
	return 0;


}

#endif
/**
 * @file callback_manager.c
 * @brief Production-grade callback registration and management system
 * @version 1.0.1
 * @date 2025-01-19
 * @author Engineering Team
 *
 * MSVC Compatible Version - Fixed compilation errors
 */

#define _CRT_SECURE_NO_WARNINGS  /* Disable MSVC security warnings */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#define MUTEX_TYPE HANDLE
#define THREAD_LOCAL __declspec(thread)
#else
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#define MUTEX_TYPE pthread_mutex_t
#define THREAD_LOCAL __thread
#endif

 /*==============================================================================
  * CONFIGURATION AND CONSTANTS
  *============================================================================*/

#define CALLBACK_MANAGER_VERSION "1.0.1"
#define MAX_CALLBACKS 32
#define MAX_CALLBACK_NAME_LEN 64
#define MAX_ERROR_MSG_LEN 256
#define MUTEX_TIMEOUT_MS 5000
#define CALLBACK_EXECUTION_TIMEOUT_MS 1000

  /* Fixed: Define string constants for printf */
#define SEPARATOR_LINE "================================================================================"
#define HEADER_LINE    "--------------------------------------------------------------------------------"

// Callback priorities (higher number = higher priority)
typedef enum {
    CALLBACK_PRIORITY_LOW = 1,
    CALLBACK_PRIORITY_NORMAL = 5,
    CALLBACK_PRIORITY_HIGH = 10,
    CALLBACK_PRIORITY_CRITICAL = 15
} callback_priority_t;

// Error codes
typedef enum {
    CB_SUCCESS = 0,
    CB_ERROR_INVALID_PARAM = -1,
    CB_ERROR_NO_MEMORY = -2,
    CB_ERROR_TIMEOUT = -3,
    CB_ERROR_NOT_FOUND = -4,
    CB_ERROR_ALREADY_EXISTS = -5,
    CB_ERROR_SYSTEM_ERROR = -6,
    CB_ERROR_CALLBACK_FAILED = -7,
    CB_ERROR_NOT_INITIALIZED = -8
} callback_result_t;

/*==============================================================================
 * TYPE DEFINITIONS
 *============================================================================*/

 // Forward declarations
typedef struct callback_manager callback_manager_t;
typedef struct callback_entry callback_entry_t;

// Callback function pointer types
typedef void (*callback_func_t)(void* user_data);
typedef void (*error_handler_func_t)(callback_result_t error, const char* message);
typedef void (*log_func_t)(const char* level, const char* message);

// Performance statistics
typedef struct {
    uint64_t total_calls;
    uint64_t total_execution_time_us;
    uint64_t min_execution_time_us;
    uint64_t max_execution_time_us;
    uint64_t last_execution_time_us;
    uint32_t error_count;
    time_t last_called;
} callback_stats_t;

// Callback entry structure
struct callback_entry {
    uint32_t id;
    char name[MAX_CALLBACK_NAME_LEN];
    callback_func_t function;
    void* user_data;
    callback_priority_t priority;
    bool enabled;
    bool registered;
    callback_stats_t stats;
    time_t registered_time;

    // Linked list for priority ordering
    callback_entry_t* next;
};

// System statistics structure
typedef struct {
    uint64_t total_registrations;
    uint64_t total_unregistrations;
    uint64_t total_executions;
    uint64_t total_errors;
    time_t created_time;
} system_stats_t;

// Main callback manager structure
struct callback_manager {
    callback_entry_t entries[MAX_CALLBACKS];
    callback_entry_t* priority_list;  // Sorted by priority
    MUTEX_TYPE mutex;
    bool initialized;
    uint32_t next_id;
    uint32_t active_count;

    // Error handling
    error_handler_func_t error_handler;
    log_func_t log_function;
    char last_error[MAX_ERROR_MSG_LEN];

    // System statistics
    system_stats_t system_stats;
};

/*==============================================================================
 * GLOBAL INSTANCE AND THREAD-LOCAL STORAGE
 *============================================================================*/

static callback_manager_t g_callback_manager = { 0 };
static THREAD_LOCAL char g_error_buffer[MAX_ERROR_MSG_LEN];

/*==============================================================================
 * PLATFORM-SPECIFIC IMPLEMENTATIONS
 *============================================================================*/

#ifdef _WIN32
static bool mutex_init(MUTEX_TYPE* mutex) {
    *mutex = CreateMutex(NULL, FALSE, NULL);
    return (*mutex != NULL);
}

static bool mutex_lock(MUTEX_TYPE* mutex, uint32_t timeout_ms) {
    DWORD result = WaitForSingleObject(*mutex, timeout_ms);
    return (result == WAIT_OBJECT_0);
}

static bool mutex_unlock(MUTEX_TYPE* mutex) {
    return ReleaseMutex(*mutex) != 0;
}

static void mutex_destroy(MUTEX_TYPE* mutex) {
    if (*mutex) {
        CloseHandle(*mutex);
        *mutex = NULL;
    }
}

static uint64_t get_timestamp_us(void) {
    LARGE_INTEGER frequency, counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000) / frequency.QuadPart;
}

#else
static bool mutex_init(MUTEX_TYPE* mutex) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int result = pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    return (result == 0);
}

static bool mutex_lock(MUTEX_TYPE* mutex, uint32_t timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }

    int result = pthread_mutex_timedlock(mutex, &ts);
    return (result == 0);
}

static bool mutex_unlock(MUTEX_TYPE* mutex) {
    return (pthread_mutex_unlock(mutex) == 0);
}

static void mutex_destroy(MUTEX_TYPE* mutex) {
    pthread_mutex_destroy(mutex);
}

static uint64_t get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}
#endif

/*==============================================================================
 * UTILITY FUNCTIONS
 *============================================================================*/

static void default_log_function(const char* level, const char* message) {
    time_t now;
    struct tm* tm_info;
    char timestamp[32];

    time(&now);
    tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    printf("[%s] [%s] %s\n", timestamp, level, message);
    fflush(stdout);
}

static void default_error_handler(callback_result_t error, const char* message) {
    default_log_function("ERROR", message);
}

static void log_message(const char* level, const char* format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (g_callback_manager.log_function) {
        g_callback_manager.log_function(level, buffer);
    }
    else {
        default_log_function(level, buffer);
    }
}

static void set_error(callback_result_t error, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(g_callback_manager.last_error, sizeof(g_callback_manager.last_error), format, args);
    va_end(args);

    if (g_callback_manager.error_handler) {
        g_callback_manager.error_handler(error, g_callback_manager.last_error);
    }

    g_callback_manager.system_stats.total_errors++;
}

/*==============================================================================
 * CORE CALLBACK MANAGEMENT FUNCTIONS
 *============================================================================*/

 /**
  * @brief Initialize the callback management system
  * @return CB_SUCCESS on success, error code on failure
  */
callback_result_t callback_manager_init(void) {
    int i;

    if (g_callback_manager.initialized) {
        log_message("WARN", "Callback manager already initialized");
        return CB_SUCCESS;
    }

    memset(&g_callback_manager, 0, sizeof(g_callback_manager));

    if (!mutex_init(&g_callback_manager.mutex)) {
        set_error(CB_ERROR_SYSTEM_ERROR, "Failed to initialize mutex");
        return CB_ERROR_SYSTEM_ERROR;
    }

    // Initialize entries
    for (i = 0; i < MAX_CALLBACKS; i++) {
        g_callback_manager.entries[i].id = UINT32_MAX;  // Invalid ID
        g_callback_manager.entries[i].registered = false;
    }

    g_callback_manager.next_id = 1;
    g_callback_manager.initialized = true;
    g_callback_manager.error_handler = default_error_handler;
    g_callback_manager.log_function = default_log_function;
    time(&g_callback_manager.system_stats.created_time);

    log_message("INFO", "Callback manager initialized (version %s)", CALLBACK_MANAGER_VERSION);
    return CB_SUCCESS;
}

/**
 * @brief Find an available slot in the callback array
 * @return Index of available slot, or -1 if none available
 */
static int find_available_slot(void) {
    int i;
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (!g_callback_manager.entries[i].registered) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Insert callback into priority-ordered linked list
 * @param entry Callback entry to insert
 */
static void insert_into_priority_list(callback_entry_t* entry) {
    callback_entry_t* current;

    entry->next = NULL;

    if (!g_callback_manager.priority_list ||
        entry->priority > g_callback_manager.priority_list->priority) {
        // Insert at head
        entry->next = g_callback_manager.priority_list;
        g_callback_manager.priority_list = entry;
        return;
    }

    // Find insertion point
    current = g_callback_manager.priority_list;
    while (current->next && current->next->priority >= entry->priority) {
        current = current->next;
    }

    entry->next = current->next;
    current->next = entry;
}

/**
 * @brief Remove callback from priority-ordered linked list
 * @param entry Callback entry to remove
 */
static void remove_from_priority_list(callback_entry_t* entry) {
    callback_entry_t* current;

    if (!g_callback_manager.priority_list) return;

    if (g_callback_manager.priority_list == entry) {
        g_callback_manager.priority_list = entry->next;
        return;
    }

    current = g_callback_manager.priority_list;
    while (current->next && current->next != entry) {
        current = current->next;
    }

    if (current->next == entry) {
        current->next = entry->next;
    }
}

/**
 * @brief Register a new callback
 * @param name Unique name for the callback
 * @param function Function pointer to call
 * @param user_data User data to pass to the callback
 * @param priority Priority level for execution order
 * @return Callback ID on success, negative error code on failure
 */
int32_t callback_register(const char* name,
    callback_func_t function,
    void* user_data,
    callback_priority_t priority) {
    int slot;
    callback_entry_t* entry;
    int i;

    if (!g_callback_manager.initialized) {
        set_error(CB_ERROR_NOT_INITIALIZED, "Callback manager not initialized");
        return CB_ERROR_NOT_INITIALIZED;
    }

    if (!name || !function) {
        set_error(CB_ERROR_INVALID_PARAM, "Invalid parameters: name and function required");
        return CB_ERROR_INVALID_PARAM;
    }

    if (strlen(name) >= MAX_CALLBACK_NAME_LEN) {
        set_error(CB_ERROR_INVALID_PARAM, "Callback name too long (max %d chars)", MAX_CALLBACK_NAME_LEN - 1);
        return CB_ERROR_INVALID_PARAM;
    }

    if (!mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        set_error(CB_ERROR_TIMEOUT, "Failed to acquire mutex for callback registration");
        return CB_ERROR_TIMEOUT;
    }

    // Check for duplicate names
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (g_callback_manager.entries[i].registered &&
            strcmp(g_callback_manager.entries[i].name, name) == 0) {
            mutex_unlock(&g_callback_manager.mutex);
            set_error(CB_ERROR_ALREADY_EXISTS, "Callback with name '%s' already exists", name);
            return CB_ERROR_ALREADY_EXISTS;
        }
    }

    slot = find_available_slot();
    if (slot < 0) {
        mutex_unlock(&g_callback_manager.mutex);
        set_error(CB_ERROR_NO_MEMORY, "No available callback slots (max %d)", MAX_CALLBACKS);
        return CB_ERROR_NO_MEMORY;
    }

    // Initialize callback entry
    entry = &g_callback_manager.entries[slot];
    memset(entry, 0, sizeof(callback_entry_t));

    entry->id = g_callback_manager.next_id++;
    strncpy(entry->name, name, MAX_CALLBACK_NAME_LEN - 1);
    entry->name[MAX_CALLBACK_NAME_LEN - 1] = '\0';
    entry->function = function;
    entry->user_data = user_data;
    entry->priority = priority;
    entry->enabled = true;
    entry->registered = true;
    time(&entry->registered_time);

    // Initialize statistics
    entry->stats.min_execution_time_us = UINT64_MAX;

    // Insert into priority list
    insert_into_priority_list(entry);

    g_callback_manager.active_count++;
    g_callback_manager.system_stats.total_registrations++;

    mutex_unlock(&g_callback_manager.mutex);

    log_message("INFO", "Registered callback '%s' (ID: %u, Priority: %d)",
        name, entry->id, (int)priority);

    return (int32_t)entry->id;
}

/**
 * @brief Unregister a callback by ID
 * @param callback_id ID of the callback to unregister
 * @return CB_SUCCESS on success, error code on failure
 */
callback_result_t callback_unregister(uint32_t callback_id) {
    callback_entry_t* entry = NULL;
    int i;

    if (!g_callback_manager.initialized) {
        return CB_ERROR_NOT_INITIALIZED;
    }

    if (!mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        set_error(CB_ERROR_TIMEOUT, "Failed to acquire mutex for callback unregistration");
        return CB_ERROR_TIMEOUT;
    }

    // Find callback by ID
    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (g_callback_manager.entries[i].registered &&
            g_callback_manager.entries[i].id == callback_id) {
            entry = &g_callback_manager.entries[i];
            break;
        }
    }

    if (!entry) {
        mutex_unlock(&g_callback_manager.mutex);
        set_error(CB_ERROR_NOT_FOUND, "Callback with ID %u not found", callback_id);
        return CB_ERROR_NOT_FOUND;
    }

    // Remove from priority list
    remove_from_priority_list(entry);

    log_message("INFO", "Unregistered callback '%s' (ID: %u)", entry->name, entry->id);

    // Clear entry
    memset(entry, 0, sizeof(callback_entry_t));
    entry->id = UINT32_MAX;

    g_callback_manager.active_count--;
    g_callback_manager.system_stats.total_unregistrations++;

    mutex_unlock(&g_callback_manager.mutex);

    return CB_SUCCESS;
}

/**
 * @brief Execute all registered callbacks in priority order
 * @return Number of callbacks executed, negative error code on failure
 */
int32_t callback_execute_all(void) {
    int32_t executed_count = 0;
    uint64_t start_time, callback_start, execution_time, total_time;
    callback_entry_t* current;

    if (!g_callback_manager.initialized) {
        set_error(CB_ERROR_NOT_INITIALIZED, "Callback manager not initialized");
        return CB_ERROR_NOT_INITIALIZED;
    }

    if (!mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        set_error(CB_ERROR_TIMEOUT, "Failed to acquire mutex for callback execution");
        return CB_ERROR_TIMEOUT;
    }

    start_time = get_timestamp_us();

    current = g_callback_manager.priority_list;
    while (current) {
        if (current->enabled) {
            callback_start = get_timestamp_us();

            // Execute callback with basic error handling
            current->function(current->user_data);

            execution_time = get_timestamp_us() - callback_start;

            // Update statistics
            current->stats.total_calls++;
            current->stats.total_execution_time_us += execution_time;
            current->stats.last_execution_time_us = execution_time;
            time(&current->stats.last_called);

            if (execution_time < current->stats.min_execution_time_us) {
                current->stats.min_execution_time_us = execution_time;
            }
            if (execution_time > current->stats.max_execution_time_us) {
                current->stats.max_execution_time_us = execution_time;
            }

            executed_count++;

            log_message("DEBUG", "Executed callback '%s' in %llu us",
                current->name, (unsigned long long)execution_time);
        }
        current = current->next;
    }

    g_callback_manager.system_stats.total_executions++;

    total_time = get_timestamp_us() - start_time;
    mutex_unlock(&g_callback_manager.mutex);

    log_message("INFO", "Executed %d callbacks in %llu us",
        executed_count, (unsigned long long)total_time);

    return executed_count;
}

/**
 * @brief Enable or disable a specific callback
 * @param callback_id ID of the callback
 * @param enabled True to enable, false to disable
 * @return CB_SUCCESS on success, error code on failure
 */
callback_result_t callback_set_enabled(uint32_t callback_id, bool enabled) {
    int i;

    if (!g_callback_manager.initialized) {
        return CB_ERROR_NOT_INITIALIZED;
    }

    if (!mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        return CB_ERROR_TIMEOUT;
    }

    for (i = 0; i < MAX_CALLBACKS; i++) {
        if (g_callback_manager.entries[i].registered &&
            g_callback_manager.entries[i].id == callback_id) {
            g_callback_manager.entries[i].enabled = enabled;
            mutex_unlock(&g_callback_manager.mutex);

            log_message("INFO", "Callback '%s' (ID: %u) %s",
                g_callback_manager.entries[i].name, callback_id,
                enabled ? "enabled" : "disabled");
            return CB_SUCCESS;
        }
    }

    mutex_unlock(&g_callback_manager.mutex);
    return CB_ERROR_NOT_FOUND;
}

/**
 * @brief Cleanup and shutdown the callback system
 * @return CB_SUCCESS on success, error code on failure
 */
callback_result_t callback_manager_shutdown(void) {
    int i;

    if (!g_callback_manager.initialized) {
        return CB_ERROR_NOT_INITIALIZED;
    }

    log_message("INFO", "Shutting down callback manager...");

    if (mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        // Clear all callbacks
        for (i = 0; i < MAX_CALLBACKS; i++) {
            if (g_callback_manager.entries[i].registered) {
                log_message("DEBUG", "Auto-unregistering callback '%s'",
                    g_callback_manager.entries[i].name);
            }
        }

        memset(g_callback_manager.entries, 0, sizeof(g_callback_manager.entries));
        g_callback_manager.priority_list = NULL;
        g_callback_manager.active_count = 0;

        mutex_unlock(&g_callback_manager.mutex);
    }

    mutex_destroy(&g_callback_manager.mutex);
    g_callback_manager.initialized = false;

    log_message("INFO", "Callback manager shutdown complete");
    return CB_SUCCESS;
}

/*==============================================================================
 * DIAGNOSTIC AND DEBUGGING FUNCTIONS
 *============================================================================*/

 /**
  * @brief Print detailed system status and statistics
  */
void callback_print_diagnostics(void) {
    time_t uptime;
    callback_entry_t* current;
    uint64_t avg_time;

    if (!g_callback_manager.initialized) {
        printf("Callback Manager: NOT INITIALIZED\n");
        return;
    }

    printf("\n%s\n", SEPARATOR_LINE);
    printf("CALLBACK MANAGER DIAGNOSTICS\n");
    printf("%s\n", SEPARATOR_LINE);
    printf("Version: %s\n", CALLBACK_MANAGER_VERSION);
    printf("Status: %s\n", g_callback_manager.initialized ? "INITIALIZED" : "NOT INITIALIZED");
    printf("Active Callbacks: %u/%d\n", g_callback_manager.active_count, MAX_CALLBACKS);

    uptime = time(NULL) - g_callback_manager.system_stats.created_time;
    printf("Uptime: %d seconds\n", (int)uptime);

    printf("\nSYSTEM STATISTICS:\n");
    printf("  Total Registrations: %llu\n", (unsigned long long)g_callback_manager.system_stats.total_registrations);
    printf("  Total Unregistrations: %llu\n", (unsigned long long)g_callback_manager.system_stats.total_unregistrations);
    printf("  Total Executions: %llu\n", (unsigned long long)g_callback_manager.system_stats.total_executions);
    printf("  Total Errors: %llu\n", (unsigned long long)g_callback_manager.system_stats.total_errors);

    if (mutex_lock(&g_callback_manager.mutex, MUTEX_TIMEOUT_MS)) {
        printf("\nREGISTERED CALLBACKS (by priority):\n");
        printf("%-4s %-20s %-8s %-10s %-12s %-12s %-8s\n",
            "ID", "Name", "Priority", "Enabled", "Calls", "Avg Time", "Errors");
        printf("%s\n", HEADER_LINE);

        current = g_callback_manager.priority_list;
        while (current) {
            avg_time = current->stats.total_calls > 0 ?
                current->stats.total_execution_time_us / current->stats.total_calls : 0;

            printf("%-4u %-20s %-8d %-10s %-12llu %-12llu %-8u\n",
                current->id,
                current->name,
                (int)current->priority,
                current->enabled ? "YES" : "NO",
                (unsigned long long)current->stats.total_calls,
                (unsigned long long)avg_time,
                current->stats.error_count);

            current = current->next;
        }

        mutex_unlock(&g_callback_manager.mutex);
    }

    printf("%s\n\n", SEPARATOR_LINE);
}

/*==============================================================================
 * EXAMPLE USAGE AND DEMONSTRATION
 *============================================================================*/

 // Example callback functions for demonstration
void system_heartbeat(void* data) {
    static uint32_t counter = 0;
    printf("System heartbeat #%u\n", ++counter);
}

void data_processor(void* data) {
    int* value = (int*)data;
    printf("Processing data: %d\n", *value);
    (*value)++;
}

void error_monitor(void* data) {
    printf("Error monitoring check\n");
}

void performance_monitor(void* data) {
    printf("Performance monitoring\n");
}
#ifdef production  
/**
 * @brief Demonstration of the callback system
 */
int main(void) {
    callback_result_t result;
    int data_value = 100;
    int32_t heartbeat_id, data_id, error_id, perf_id;
    int32_t executed;
    int i;

    printf("=== Production-Level Callback Management System Demo ===\n\n");

    // Initialize the system
    result = callback_manager_init();
    if (result != CB_SUCCESS) {
        printf("Failed to initialize callback manager: %d\n", result);
        return 1;
    }

    // Register callbacks with different priorities
    heartbeat_id = callback_register("SystemHeartbeat", system_heartbeat,
        NULL, CALLBACK_PRIORITY_HIGH);
    data_id = callback_register("DataProcessor", data_processor,
        &data_value, CALLBACK_PRIORITY_NORMAL);
    error_id = callback_register("ErrorMonitor", error_monitor,
        NULL, CALLBACK_PRIORITY_CRITICAL);
    perf_id = callback_register("PerformanceMonitor", performance_monitor,
        NULL, CALLBACK_PRIORITY_LOW);

    printf("Registered 4 callbacks\n\n");

    // Execute callbacks multiple times
    for (i = 0; i < 3; i++) {
        printf("--- Execution Round %d ---\n", i + 1);
        executed = callback_execute_all();
        printf("Executed %d callbacks\n\n", executed);
    }

    // Demonstrate enable/disable
    printf("--- Disabling DataProcessor ---\n");
    callback_set_enabled(data_id, false);
    callback_execute_all();
    printf("\n");

    // Show diagnostics
    callback_print_diagnostics();

    // Test error handling
    printf("--- Testing Error Scenarios ---\n");
    callback_register(NULL, system_heartbeat, NULL, CALLBACK_PRIORITY_NORMAL);  // Should fail

    // Cleanup
    printf("--- Cleanup ---\n");
    callback_unregister(heartbeat_id);
    callback_unregister(data_id);
    callback_unregister(error_id);
    callback_unregister(perf_id);

    callback_manager_shutdown();

    printf("Demo completed successfully!\n");
    return 0;
}





 
#endif