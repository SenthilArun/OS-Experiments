#include <iostream>
#include <unordered_map>
#include <map>
#include <functional>
#include <ctime>
#include <vector>

// Forward declarations and type definitions
typedef void (*callback_t)(void);  // Fixed syntax: function pointer type
typedef int timer_handle;          // Handle to identify timers

class TimerEventSystem {
private:
    // Data structures to manage timers
    std::map<time_t, std::vector<std::pair<timer_handle, callback_t>>> timeToCallbacks;
    std::unordered_map<timer_handle, time_t> handleToTime;

    timer_handle nextHandle;       // Counter for unique handles
    time_t currentTime;           // Simulated current time

    // Hardware register simulation
    void setHardwareRegister(time_t timestamp) {
        std::cout << "Setting HW timer register to: " << timestamp << std::endl;
        // In real implementation, this would program actual hardware
    }

public:
    TimerEventSystem() : nextHandle(1), currentTime(0) {}

    /*
     * Function accepts time and user callback
     * Function returns the event handler (unique identifier)
     */
    timer_handle timerAddEvent(time_t time, callback_t pUserFunction) {
        // Input validation
        if (pUserFunction == nullptr) {
            std::cout << "Error: Null callback provided" << std::endl;
            return -1; // Invalid handle
        }

        if (time <= currentTime) {
            std::cout << "Error: Cannot schedule event in the past" << std::endl;
            return -1;
        }

        // Generate unique handle
        timer_handle handle = nextHandle++;

        // Register the event
        // Store callback with its handle at the specified time
        timeToCallbacks[time].push_back({ handle, pUserFunction });

        // Store reverse mapping for easy removal
        handleToTime[handle] = time;

        // Update hardware register to earliest event time
        if (!timeToCallbacks.empty()) {
            time_t earliestTime = timeToCallbacks.begin()->first;
            setHardwareRegister(earliestTime);
        }

        std::cout << "Timer event registered: Handle=" << handle
            << ", Time=" << time << std::endl;

        return handle;
    }

    /*
     * Function to remove a scheduled timer event
     */
    bool timerRemoveEvent(timer_handle handle) {
        auto it = handleToTime.find(handle);
        if (it == handleToTime.end()) {
            std::cout << "Error: Invalid timer handle " << handle << std::endl;
            return false;
        }

        time_t eventTime = it->second;
        handleToTime.erase(it);

        // Remove from time-based storage
        auto& callbacks = timeToCallbacks[eventTime];
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(),
                [handle](const std::pair<timer_handle, callback_t>& p) {
                    return p.first == handle;
                }),
            callbacks.end()
        );

        // Remove empty time slot
        if (callbacks.empty()) {
            timeToCallbacks.erase(eventTime);
        }

        std::cout << "Timer event removed: Handle=" << handle << std::endl;
        return true;
    }

    /*
     * Function that is being called by the HW module once the timer has reached clock value
     * This simulates the hardware interrupt handler
     */
    void timerModuleInterruptHandler() {
        std::cout << "\n=== TIMER INTERRUPT at time " << currentTime << " ===" << std::endl;

        // Find all events scheduled for current time
        auto it = timeToCallbacks.find(currentTime);
        if (it != timeToCallbacks.end()) {
            // Execute all callbacks for this time
            for (auto& eventPair : it->second) {
                timer_handle handle = eventPair.first;
                callback_t callback = eventPair.second;

                std::cout << "Executing callback for handle " << handle << std::endl;
                callback(); // Execute the user callback

                // Remove from reverse mapping
                handleToTime.erase(handle);
            }

            // Remove this time slot as all events are processed
            timeToCallbacks.erase(it);
        }

        // Set hardware register for next earliest event
        if (!timeToCallbacks.empty()) {
            time_t nextTime = timeToCallbacks.begin()->first;
            setHardwareRegister(nextTime);
            std::cout << "Next timer interrupt scheduled for: " << nextTime << std::endl;
        }
        else {
            std::cout << "No more timer events scheduled" << std::endl;
        }

        std::cout << "=== INTERRUPT COMPLETE ===\n" << std::endl;
    }

    /*
     * Simulate advancing time (for testing)
     */
    void advanceTime(time_t newTime) {
        while (currentTime < newTime) {
            currentTime++;

            // Check if any timer events are due
            if (timeToCallbacks.find(currentTime) != timeToCallbacks.end()) {
                timerModuleInterruptHandler();
            }
        }
    }

    /*
     * Debug function to show current state
     */
    void printTimerState() {
        std::cout << "\n--- Timer System State ---" << std::endl;
        std::cout << "Current time: " << currentTime << std::endl;
        std::cout << "Scheduled events:" << std::endl;

        for (auto& timePair : timeToCallbacks) {
            std::cout << "  Time " << timePair.first << ": "
                << timePair.second.size() << " event(s)" << std::endl;
            for (auto& event : timePair.second) {
                std::cout << "    Handle: " << event.first << std::endl;
            }
        }
        std::cout << "-------------------------\n" << std::endl;
    }
};

// Example callback functions for testing
void userCallback1() {
    std::cout << "  -> User Callback 1 executed!" << std::endl;
}

void userCallback2() {
    std::cout << "  -> User Callback 2 executed!" << std::endl;
}

void userCallback3() {
    std::cout << "  -> User Callback 3 executed!" << std::endl;
}

// Example usage and testing
int main() {
    TimerEventSystem timerSystem;

    std::cout << "=== TIMER EVENT SYSTEM DEMO ===" << std::endl;

    // Schedule some timer events
    timer_handle handle1 = timerSystem.timerAddEvent(5, userCallback1);
    timer_handle handle2 = timerSystem.timerAddEvent(3, userCallback2);
    timer_handle handle3 = timerSystem.timerAddEvent(7, userCallback3);
    timer_handle handle4 = timerSystem.timerAddEvent(5, userCallback1); // Same time as handle1

    timerSystem.printTimerState();

    // Simulate time advancement
    std::cout << "Advancing time to 10..." << std::endl;
    timerSystem.advanceTime(10);

    timerSystem.printTimerState();

    // Test timer removal
    std::cout << "Testing timer removal..." << std::endl;
    timer_handle handle5 = timerSystem.timerAddEvent(15, userCallback2);
    timerSystem.printTimerState();

    timerSystem.timerRemoveEvent(handle5);
    timerSystem.printTimerState();

    return 0;
}

/*
OUTPUT WILL BE:
=== TIMER EVENT SYSTEM DEMO ===
Setting HW timer register to: 3
Timer event registered: Handle=1, Time=5
Setting HW timer register to: 3
Timer event registered: Handle=2, Time=3
Setting HW timer register to: 3
Timer event registered: Handle=3, Time=7
Setting HW timer register to: 3
Timer event registered: Handle=4, Time=5

--- Timer System State ---
Current time: 0
Scheduled events:
  Time 3: 1 event(s)
    Handle: 2
  Time 5: 2 event(s)
    Handle: 1
    Handle: 4
  Time 7: 1 event(s)
    Handle: 3
-------------------------

Advancing time to 10...

=== TIMER INTERRUPT at time 3 ===
Executing callback for handle 2
  -> User Callback 2 executed!
Next timer interrupt scheduled for: 5
=== INTERRUPT COMPLETE ===

=== TIMER INTERRUPT at time 5 ===
Executing callback for handle 1
  -> User Callback 1 executed!
Executing callback for handle 4
  -> User Callback 1 executed!
Next timer interrupt scheduled for: 7
=== INTERRUPT COMPLETE ===

=== TIMER INTERRUPT at time 7 ===
Executing callback for handle 3
  -> User Callback 3 executed!
No more timer events scheduled
=== INTERRUPT COMPLETE ===

--- Timer System State ---
Current time: 10
Scheduled events:
-------------------------
*/