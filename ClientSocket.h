#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <atomic>
#include <windows.h>
#include <cstring> 

#define DEVELOPERMODE true

#define ECHO true

#include "Hints.h"
#include "ClientPlayer.h"
#include "SIMP.h"
#include "ServerDebug.h"

#include "DeckClient.h"
#include "WolfgangTile.h"

#pragma comment(lib, "ws2_32.lib") 

#pragma once

Player pPlayer;

class ClientSocket {
public:

    void ClientSendMessage(std::string sMessage) {

        if (sMessage.substr(0, SIMP_FRANKENHEARTS_SendIndividualCard.size()) == SIMP_FRANKENHEARTS_SendIndividualCard) {
            std::string sCheater = "I AM A CHEATER. I ATTEMPTED TO CHEAT IN THIS GAME. DO NOT PLAY WITH ME";
            PrintWarn("THE SERVER HAS DETECTED YOU HAVE ATTEMPTED TO CHEAT...");
            send(socketClient, sCheater.data(), sCheater.length(), 0);
            return;
        }


        send(socketClient, sMessage.data(), sMessage.length(), 0);
    }

    void ClientSendCard(std::string sMessage) {
        if (allCards.find(sMessage) != allCards.end()) {
            std::string sPacket = SIMP_FRANKENHEARTS_SendIndividualCard + sMessage;
            send(socketClient, sPacket.data(), sPacket.length(), 0);
        } else {
            PrintWarn("This card does not exist!");
        }
    }

    void ClientSendPassData(std::vector<Card> vcCardsToPass) {

        std::string sCardsToPass = SIMP_FRANKENHEARTS_PassVectorStart;

        for (int i = 0; i < vcCardsToPass.size(); i++) {
            sCardsToPass += vcCardsToPass[i].sCardID;
        }

        sCardsToPass += SIMP_FRANKENHEARTS_PassVectorFinish;

        send(socketClient, sCardsToPass.data(), strlen(sCardsToPass.data()), 0);

    }

    std::vector<std::string> extractCardIDs(const std::string& deckString) {
        std::vector<std::string> cardIDs;
        size_t startPos = deckString.find('c');
        size_t endDeck = deckString.find_last_of('/');
    
        while (startPos != std::string::npos && startPos < endDeck) {
            size_t endPos = deckString.find('c', startPos + 1);
            if (endPos == std::string::npos || endPos > endDeck) {
                cardIDs.push_back(deckString.substr(startPos, endDeck - startPos)); 
                break;
            } else {
                cardIDs.push_back(deckString.substr(startPos, endPos - startPos));
                startPos = endPos;
            }
        }
    
        return cardIDs;
    }

    std::vector<std::string> ExtractScoreData(const std::string& deckString) {
        std::vector<std::string> cardIDs;
        size_t startPos = deckString.find('|');
        size_t endDeck = deckString.find_last_of('/');
    
        while (startPos != std::string::npos && startPos < endDeck) {
            size_t endPos = deckString.find('|', startPos + 1);
            if (endPos == std::string::npos || endPos > endDeck) {
                cardIDs.push_back(deckString.substr(startPos, endDeck - startPos)); 
                break;
            } else {
                cardIDs.push_back(deckString.substr(startPos, endPos - startPos));
                startPos = endPos;
            }
        }
    
        return cardIDs;
    }
    
    void ClientGiveHint() {

        if (vsDynamicHints.size() == 0) {
            vsDynamicHints = vsStaticHints;
        }

        int iSelection = rand() % vsDynamicHints.size();
        pPlayer.WritePlayerHint(vsDynamicHints[iSelection]);
        vsDynamicHints.erase(vsDynamicHints.begin() + iSelection);

    }

    void ClientRecieveScoreStrings(std::string sScoreStrings) {

        std::vector<std::string> vsScoreStrings = ExtractScoreData(sScoreStrings);

        for (int i = 0; i < vsScoreStrings.size(); i++) {
            if (vsScoreStrings[i].length() > 0) {
                vsScoreStrings[i].erase(0, 1);
            }
        }

        pPlayer.WritePlayerScores(vsScoreStrings);
        ClientGiveHint();

    }

    void ClientRecieveHand(std::string sNewDeck) {

        std::vector<std::string> sCardIDs = extractCardIDs(sNewDeck);

        std::vector<Card> vcNewDeck;

        for (int i = 0; i < sCardIDs.size(); i++) {
            vcNewDeck.push_back(allCards[sCardIDs[i]]);
        }

        pPlayer.ReWriteDeck(vcNewDeck);

    }

    void ClientRecieveCardsInTrick(std::string sNewCardsInTrick) {

        std::vector<std::string> sCardIDs = extractCardIDs(sNewCardsInTrick);

        std::vector<Card> vcNewCardsInTrick;

        for (int i = 0; i < sCardIDs.size(); i++) {
            vcNewCardsInTrick.push_back(allCards[sCardIDs[i]]);
        }

        pPlayer.WriteCardsInTrick(vcNewCardsInTrick);

    }

    void ClientRecieveCardsPassed(std::string sCardsPassed) {

        std::vector<std::string> sCardIDs = extractCardIDs(sCardsPassed);

        std::vector<Card> vcCardsRecievedInPass;

        for (int i = 0; i < sCardIDs.size(); i++) {
            vcCardsRecievedInPass.push_back(allCards[sCardIDs[i]]);
        }
    
        pPlayer.WriteCardsRecievedInPass(vcCardsRecievedInPass);


    }

    void ClientRecievePreviousTrick(std::string sCardsInPreviousTrick) {

    std::vector<std::string> sCardIDs = extractCardIDs(sCardsInPreviousTrick);

    std::vector<Card> vcPreviousTrick;

    for (int i = 0; i < sCardIDs.size(); i++) {
        vcPreviousTrick.push_back(allCards[sCardIDs[i]]);
    }

    pPlayer.WritePreviousCardsInTrick(vcPreviousTrick);

    }

    void ClientRecieveHeartsStatus(std::string sHeartsStatus) {
        
        sHeartsStatus.erase(0, SIMP_FRANKENHEARTS_HeartsStatus.size());

        if (sHeartsStatus == "1") {
            pPlayer.WriteHearts(true);
        }

        if (sHeartsStatus == "0") {
            pPlayer.WriteHearts(false);
        }

    }

    void ClientRecievePassStatus(std::string sPassStatus) {

        sPassStatus.erase(0, SIMP_FRANKENHEARTS_PassStatus.size());
        int iPassStatus = std::stoi(sPassStatus);

        pPlayer.WritePassType(iPassStatus);

    }

    void ClientRecieveAchievement(std::string sAchievement) {

        sAchievement.erase(0, SIMP_FRANKENHEARTS_Acheivement.size());
        WG::printvarint("Client Recieved Achievement", std::stoi(sAchievement));
        pPlayer.WriteCurrentAchievement(std::stoi(sAchievement));

    }

    void ClientRecievePrompt(std::string sPrompt) {

        sPrompt.erase(0, SIMP_FRANKENHEARTS_Prompt.size());
        WG::printvarint("Client Recieved Prompt", std::stoi(sPrompt));
        pPlayer.WritePrompt(std::stoi(sPrompt));

    }

    void ClientRecieveChatMessage(std::string sMessage) {

        WG::print("Recieved Message");
        pPlayer.PushMessageToChat(sMessage);

    }


    SOCKET socketServer = INVALID_SOCKET;
    int iPort;
    SOCKET socketClient = INVALID_SOCKET;
    sockaddr_in socketAddress;
    int socketAddressLength;
    std::string sNickname = "Doug";
    bool bSocketValid = true;

    ClientSocket(std::string sUsername, std::string sIPAddress, std::string sPort) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed.\n";
            return;
        }

        sNickname = sUsername;

        iPort = std::stoi(sPort);

        const char* czServerAddress = sIPAddress.data();

        WG::printvarchar("Server Address", czServerAddress);

        socketClient = socket(AF_INET, SOCK_STREAM, 0);

        if (socketClient == INVALID_SOCKET) {
            std::cerr << "ERROR: Socket Creation Failed.\n";
            bSocketValid = false;
            return;
        }

        socketAddressLength = sizeof(socketAddress);
        memset(&socketAddress, 0, sizeof(socketAddress));

        socketAddress.sin_family = AF_INET;
        inet_pton(AF_INET, czServerAddress, &socketAddress.sin_addr);
        socketAddress.sin_port = htons(iPort);

        socketServer = connect(socketClient, (sockaddr*)&socketAddress, sizeof(socketAddress));

        send(socketClient, sNickname.data(), sNickname.length(), 0);

        u_long iMode = 1; // Non-blocking mode
        ioctlsocket(socketClient, FIONBIO, &iMode);

        if (socketServer == SOCKET_ERROR) {
            std::cerr << "Connection Failed!\n";
            bSocketValid = false;
            closesocket(socketClient);
            WSACleanup();
            return;
        }

        std::cout << "Connection Successful\n";
        char ipBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &socketAddress.sin_addr, ipBuffer, INET_ADDRSTRLEN);
        std::cout << "Address: " << ipBuffer << "\n";
        std::cout << "Port: " << iPort << "\n";
    }

    int Destruct() {
        closesocket(socketClient);
        WSACleanup();
        std::cout << "Client Successfully Destroyed\n";
        delete this;
        return 0;
    }

    void PrintServerMessages(std::atomic<bool>& bClientOnline) {

        srand(time(NULL));

        char cBuffer[1024];
        std::string sFullMessage = "";

        while (bClientOnline.load()) {
            memset(cBuffer, 0, sizeof(cBuffer));

            int iBytesReceived = recv(socketClient, cBuffer, sizeof(cBuffer) - 1, 0);

            if (iBytesReceived > 0) {
                sFullMessage += std::string(cBuffer, iBytesReceived);

                if (ECHO) {
                    std::cout << sFullMessage << "\n";
                }

                if (sFullMessage == SIMP_Client_ForceExit) {
                    PrintWarn("SERVER HAS REQUESTED CLIENT TERMINATION, THIS CLIENT WILL NOW TERMINATE");
                    bClientOnline.store(false);
                    return;
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_DeckStart.size()) == SIMP_FRANKENHEARTS_DeckStart) {
                    ClientRecieveHand(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_CardsInTrickStart.size()) == SIMP_FRANKENHEARTS_CardsInTrickStart) {
                    ClientRecieveCardsInTrick(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_PlayerScoreStringsStart.size()) == SIMP_FRANKENHEARTS_PlayerScoreStringsStart) {
                    ClientRecieveScoreStrings(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_HeartsStatus.size()) == SIMP_FRANKENHEARTS_HeartsStatus) {
                    ClientRecieveHeartsStatus(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_PassStatus.size()) == SIMP_FRANKENHEARTS_PassStatus) {
                    ClientRecievePassStatus(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_PreviousTrickStart.size()) == SIMP_FRANKENHEARTS_PreviousTrickStart) {
                    ClientRecievePreviousTrick(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_Acheivement.size()) == SIMP_FRANKENHEARTS_Acheivement) {
                    ClientRecieveAchievement(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_CardsRecievedStart.size()) == SIMP_FRANKENHEARTS_CardsRecievedStart) {
                    ClientRecieveCardsPassed(sFullMessage);
                } else if (sFullMessage.substr(0, SIMP_FRANKENHEARTS_Prompt.size()) == SIMP_FRANKENHEARTS_Prompt) {
                    ClientRecievePrompt(sFullMessage);
                }
                else {
                    ClientRecieveChatMessage(sFullMessage);
                }

                sFullMessage.erase();
            } else if (iBytesReceived == SOCKET_ERROR) {
                if (WSAGetLastError() != WSAEWOULDBLOCK) {
                    std::cerr << "Critical Socket Error -> Automatically Disconnecting\n";
                    closesocket(socketClient);
                    bClientOnline.store(false);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void UserInput(std::atomic<bool>& bClientOnline) {
        std::string sUserInput;

        //while (bClientOnline.load()) {

        //    if (sUserInput == "help" || sUserInput == "HELP" || sUserInput == "h") {
        //    }

        //    if (sUserInput == SIMP_Client_Exit) {
        //        bClientOnline.store(false);
        //    }

        //    if (!sUserInput.empty()) {
        //        ClientSendMessage(sUserInput);
        //    }
        //}
        //std::cout << "User Input Thread Out of Scope\n";
    }

 
};

bool CreateClientSocket(ClientSocket*& csClientSocket, std::string& sUsername, std::string& sIPAddress, std::string& sPort) {

    csClientSocket = new ClientSocket(sUsername, sIPAddress, sPort); 

    if (csClientSocket->bSocketValid) {
        return true;
    } else {
        return false;
    }

}

class UISettings {

    public:
    
    bool bAccessible = true;

    bool bUIScale = false;
    double dUIScale = 1;

    bool bCardSubtitles = false;

    bool bPrompts = false;

    bool bPreviousCardsInTrick = false;

    void RestoreDefaults() {
        bAccessible = true;
        dUIScale = 1;
        bPreviousCardsInTrick = false;
    }

    void ChangeUIScale() {
        bUIScale = !bUIScale;

        if (bUIScale) {
            dUIScale = 1.25;
        } else {
            dUIScale = 1;
        }
    }

};