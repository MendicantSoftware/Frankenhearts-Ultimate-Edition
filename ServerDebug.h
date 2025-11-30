#include <iostream>
#include <string>
#include <vector>

#pragma once

//Implement Warn, Info, Error

void Print(const char* czMessage) {
    std::cout << "[INTERNAL]" << czMessage << "\n";
}

void PrintError(const char* czMessage) {
    std::cout << "[INTERNAL -> ERROR]" << czMessage << "\n";
}

void PrintWarn(const char* czMessage) {
    std::cout << "[INTERNAL -> WARN]" << czMessage << "\n";
}

void PrintInfo(const char* czMessage) {
    std::cout << "[INTERNAL -> INFO]" << czMessage << "\n";
}


void PrintVarInt(const char* czMessage, int iVar) {
    std::cout << "[INTERNAL]" << czMessage << ": " << iVar << "\n"; 
}

void PrintVarIntError(const char* czMessage, int iVar) {
    std::cout << "[INTERNAL -> ERROR]" << czMessage << ": " << iVar << "\n"; 
}

void PrintVarIntWarn(const char* czMessage, int iVar) {
    std::cout << "[INTERNAL -> WARN]" << czMessage << ": " << iVar << "\n"; 
}

void PrintVarIntInfo(const char* czMessage, int iVar) {
    std::cout << "[INTERNAL -> INFO]" << czMessage << ": " << iVar << "\n"; 
}

void PrintVarString(const char* czMessage, std::string sString) {
    std::cout << "[INTERNAL]" << czMessage << ": " << sString << "\n"; 
}

void PrintVarStringError(const char* czMessage, std::string sString) {
    std::cout << "[INTERNAL -> ERROR]" << czMessage << ": " << sString << "\n"; 
}

void PrintVarStringWarn(const char* czMessage, std::string sString) {
    std::cout << "[INTERNAL -> WARN]" << czMessage << ": " << sString << "\n"; 
}

void PrintVarStringInfo(const char* czMessage, std::string sString) {
    std::cout << "[INTERNAL -> INFO]" << czMessage << ": " << sString << "\n"; 
}

void HelpSyntax() {
    std::cout << "\tAll arguments are seperated by /\n";
}

void HelpSystem() {
    std::cout << "SYSTEM:\n";
    std::cout << "\tEXIT\n";
    std::cout << "\t\tTriggers shutdown, is currently intended method of closing the server.\n";
    std::cout << "\t\tIf not shutdown in this way, it can be a bit of a bitch to get started again\n";
}

void HelpConnectionMangement() {
    std::cout << "CONNECTION MANAGEMENT:\n";
    std::cout << "\tLIST/\n";
    std::cout << "\t\tLists all current users, and their respective metadata.\n";
    std::cout << "\t\tARGUMENTS:\n";

    std::cout << "\tDCNICK/\n";
    std::cout << "\t\tDisconnects a client of a given username.\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target Client Username\n";

    std::cout << "\tDCIP/\n";
    std::cout << "\t\tDisconnects a client of a given IP.\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target Client IP\n";

    std::cout << "\tDCI/\n";
    std::cout << "\t\tDisconnects a client of a given index.\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target Client index\n";
}

void HelpCommunication() {
    std::cout << "COMMUNICATION:\n";
    std::cout << "\t*\n";
    std::cout << "\t\tSends a given message to all clients.\n";
    std::cout << "\t\tARGUMENTS [EXCLUDED FROM SLASH RULE -> '/' NOT REQUIRED]:\n";
    std::cout << "\t\t\t1: Intended Message\n";

    std::cout << "\tSM/\n";
    std::cout << "\t\tPrevents all client's messages from appearing in the server console, this is refered to as 'server mute'. They are still sent to other clients.\n";
    std::cout << "\t\tARGUMENTS:\n";

    std::cout << "\tSUM/\n";
    std::cout << "\t\tRemoves the server mute flag from all clients.\n";
    std::cout << "\t\tARGUMENTS:\n";

    std::cout << "\tMI/\n";
    std::cout << "\t\tMutes a client by index. They cannot send messages, public or private.\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target Client Index\n";

    std::cout << "\tUMI/\n";
    std::cout << "\t\tUnmutes a client by index\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target Client Index\n";

    std::cout << "\tDMIP/\n";
    std::cout << "\t\tSend a direct message to all clients on a given IP address.\n";
    std::cout << "\t\tARGUMENTS:\n";
    std::cout << "\t\t\t1: Target IP address\n";
    std::cout << "\t\t\t2: Intended Message\n";

}

void HelpFrankenhearts() {
    std::cout << "FRANKENHEARTS:\n";
    std::cout << "\tGameStart/\n";
    std::cout << "\t\tStarts a game of Frankenhearts including the first three clients connected. If there are not three clients, bots will be generated\n";
    std::cout << "\t\tARGUMENTS\n";

    std::cout << "\tWE/\n";
    std::cout << "\t\tToggles the Wolfgang engine debug messages.\n";
    std::cout << "\t\tARGUMENTS\n";
}

std::vector<std::string> SIMP_BLACKLISTEDNICKNAMES = {
    "Server",
    "SERVER",
    "server"
};

