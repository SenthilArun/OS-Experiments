#include <iostream>
#include <fstream>

#if 0
int main() {
    // Define your data
    float vertices[] = {
        // Position           // Normal         // TexCoord
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // Vertex 0
         0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // Vertex 1
         0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  // Vertex 2
        -0.5f,  0.5f, 0.0f,    0.0f, 0.0f, 1.0f,  0.0f, 1.0f   // Vertex 3
    };

    unsigned int indices[] = {
        0, 1, 2, // First Triangle
        2, 3, 0  // Second Triangle
    };

    // Open a binary file in write mode
    std::ofstream outFile("MyModel.bin", std::ios::binary);

    // Write vertex data to the file
    outFile.write(reinterpret_cast<char*>(vertices), sizeof(vertices));

    // Write index data to the file
    outFile.write(reinterpret_cast<char*>(indices), sizeof(indices));

    // Close the file
    outFile.close();

    std::cout << "Binary file created successfully!" << std::endl;

    return 0;
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <Windows.h>

// Global flag to indicate if a signal has been caught
volatile sig_atomic_t signalCaught = 0;

// Signal handler function (simulated ISR)
void signalHandler(int signal) {
    if (signal == SIGINT) {
        signalCaught = 1;  // Set the flag to indicate the signal was caught
        printf("Signal Interrupt (SIGINT) received. ISR executed.\n");
    }
}

int main() {
    // Register the signal handler (simulated ISR)
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        printf("Error: Unable to register signal handler.\n");
        return EXIT_FAILURE;
    }

    printf("Press Ctrl+C to trigger the simulated ISR (Signal Interrupt)...\n");

    // Main loop - wait for the signal (similar to waiting for an interrupt)
    while (!signalCaught) {
        // Simulate some work in the main loop
        printf("Working...\n");
        Sleep(1000);  // Sleep for 1 second (simulating work)
    }

    // Cleanup after the signal has been caught
    printf("Exiting main program after handling the interrupt.\n");

    return EXIT_SUCCESS;
}
