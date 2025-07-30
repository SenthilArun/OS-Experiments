#include <windows.h>
#include <iostream>

// Function that will be run by the threads
DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
    int threadNumber = *(int*)lpParam;
    for (int i = 0; i < 5; ++i)
    {
        std::cout << "Thread " << threadNumber << " is running, iteration " << i << std::endl;
        Sleep(1000); // Sleep for 1 second
    }
    return 0;
}

int main()
{
    // Thread identifiers
    DWORD threadId1, threadId2;

    // Parameters to pass to the threads
    int param1 = 1;
    int param2 = 2;

    // Create two threads
    HANDLE hThread1 = CreateThread(
        NULL,       // Default security attributes
        1024*1024,          // Default stack size
        ThreadFunction, // Function to execute
        &param1,    // Parameter to pass to the function
        0,          // Default creation flags
        &threadId1  // Receive the thread identifier
    );

    HANDLE hThread2 = CreateThread(
        NULL,
        0,
        ThreadFunction,
        &param2,
        0,
        &threadId2
    );

    // Wait for both threads to complete
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);

    // Close the thread handles
    CloseHandle(hThread1);
    CloseHandle(hThread2);

    std::cout << "Both threads have completed." << std::endl;

    return 0;
}
