#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>

#include "DeckServer.h"

#pragma once

class ClientConnection {

public:

    SOCKET socketID;
    std::string sNickname;

    sockaddr_in addressClient;
    int addressClientLen = sizeof(addressClient);
    std::string sIPAddress;

    std::atomic<bool> bConnected = true;
    std::atomic<bool> bMuted = false;
    std::atomic<bool> bServerMuted = false;
    std::atomic<bool> bInGame = false;

    Card cCurrentCard = allCards["cNULLCARD"];

    int iCurrentAchievement = 0;

    std::vector<Card> vcCurrentPass = {};

    std::mutex mtxClientMutex;

    ClientConnection(SOCKET Socket, std::string Nickname) {

        socketID = Socket;
        sNickname = Nickname;

        getpeername(socketID, (struct sockaddr*)&addressClient, &addressClientLen);

        sIPAddress = inet_ntoa(addressClient.sin_addr);

    }

    std::vector<Card> ReadCurrentPass() {
        std::lock_guard<std::mutex> lock(mtxClientMutex);
        return vcCurrentPass;
    }

    void WriteCurrentPass(std::vector<Card> vcPassData) {
        std::lock_guard<std::mutex> lock(mtxClientMutex);
        vcCurrentPass = vcPassData;
    }

    Card ReadCurrentCard() {

        std::lock_guard<std::mutex> lock(mtxClientMutex);
        return cCurrentCard;

    }

    void WriteCurrentCard(Card cCardToWrite) {

        std::lock_guard<std::mutex> lock(mtxClientMutex);
        cCurrentCard = cCardToWrite;

    }

    void DestoryConnection() {

        delete this;

    }

    int ReadCurrentAchievement() {
        std::lock_guard<std::mutex> lock(mtxClientMutex);
        return iCurrentAchievement;
    }

    void WriteCurrentAchievement(int iNewAchievement) {
        std::lock_guard<std::mutex> lock(mtxClientMutex);
        iCurrentAchievement = iNewAchievement;
    }

};