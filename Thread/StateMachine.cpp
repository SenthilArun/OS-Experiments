#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Enumeration for the states
typedef enum {
    STATE_OFF,
    STATE_ON,
    STATE_BLINK
} State_t;

// Enumeration for the events
typedef enum {
    EVENT_BUTTON_PRESS,
    EVENT_TIMER_EXPIRE
} Event_t;

// Function prototypes
void handleState(State_t state);
void ledOn();
void ledOff();
void ledToggle();
State_t processEvent(State_t currentState, Event_t event);

// Global variables for the LED state (simulated)
bool ledState = false;
/**
int main() {
    State_t currentState = STATE_OFF;

    // Simulate events
    currentState = processEvent(currentState, EVENT_BUTTON_PRESS);  // OFF -> ON
    handleState(currentState);

    currentState = processEvent(currentState, EVENT_BUTTON_PRESS);  // ON -> BLINK
    handleState(currentState);

    // Simulate a timer expire to toggle LED in BLINK state
    currentState = processEvent(currentState, EVENT_TIMER_EXPIRE);  // BLINK (toggle LED)
    handleState(currentState);

    currentState = processEvent(currentState, EVENT_BUTTON_PRESS);  // BLINK -> OFF
    handleState(currentState);

    return 0;
}**/

// Handle the state by performing the appropriate action
void handleState(State_t state) {
    switch (state) {
    case STATE_OFF:
        ledOff();
        break;
    case STATE_ON:
        ledOn();
        break;
    case STATE_BLINK:
        ledToggle();
        break;
    default:
        break;
    }
}

// Turn the LED on
void ledOn() {
    ledState = true;
    printf("LED is ON\n");
}

// Turn the LED off
void ledOff() {
    ledState = false;
    printf("LED is OFF\n");
}

// Toggle the LED state
void ledToggle() {
    ledState = !ledState;
    printf("LED is %s\n", ledState ? "ON" : "OFF");
}

// Process the event and return the next state
State_t processEvent(State_t currentState, Event_t event) {
    switch (currentState) {
    case STATE_OFF:
        if (event == EVENT_BUTTON_PRESS) {
            return STATE_ON;
        }
        break;
    case STATE_ON:
        if (event == EVENT_BUTTON_PRESS) {
            return STATE_BLINK;
        }
        break;
    case STATE_BLINK:
        if (event == EVENT_BUTTON_PRESS) {
            return STATE_OFF;
        }
        else if (event == EVENT_TIMER_EXPIRE) {
            return STATE_BLINK;  // Remain in BLINK, but toggle LED
        }
        break;
    default:
        break;
    }
    return currentState;
}
