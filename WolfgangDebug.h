#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

#pragma once



namespace WG {

    bool BOOL_ECHO_WOLFGANG_DEBUG = true;

    void DisplayTime() {
        if (!BOOL_ECHO_WOLFGANG_DEBUG) {
            return;
        }
        std::time_t tTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << std::put_time(std::localtime(&tTime), "%H:%M.%S") << ": ";
    }

    /**
    *Prints a statement to the terminal in the following format:
    *"cDebugMessage = iDebugVariable\n" 
    */
    void printvarint(const char* cDebugMessage, int iDebugVariable) {
        if (!BOOL_ECHO_WOLFGANG_DEBUG) {
            return;
        }
        DisplayTime();
        std::cout << cDebugMessage;
        std::cout << " = ";
        std::cout << iDebugVariable;  
        std::cout << "\n";
    }

    /**
    *Prints a statement to the terminal in the following format:
    *"cDebugMessage = dDebugVariable\n" 
    */
    void printvardouble(const char* cDebugMessage, double dDebugVariable) {
        if (!BOOL_ECHO_WOLFGANG_DEBUG) {
            return;
        }
        DisplayTime();
        std::cout << cDebugMessage;
        std::cout << " = ";
        std::cout << dDebugVariable;  
        std::cout << "\n";
    }

    /**
    *Prints a statement to the terminal in the following format:
    *"cDebugMessage: czDebugVariable\n" 
    *
    *Wolfgang does not have a printvar for string, if you want to print a string pass it using the std::string object .data() method.
    */
    void printvarchar(const char* cDebugMessage, const char* czDebugVariable) {
        if (!BOOL_ECHO_WOLFGANG_DEBUG) {
            return;
        }
        DisplayTime();
        std::cout << cDebugMessage;
        std::cout << ": ";
        std::cout << czDebugVariable;  
        std::cout << "\n";
    }

    void print(const char* cPrintMessage) {
        if (!BOOL_ECHO_WOLFGANG_DEBUG) {
            return;
        }
        DisplayTime();
        std::cout << cPrintMessage;
        std::cout << "\n";
    }



}