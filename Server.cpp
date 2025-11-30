#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <atomic>
#include <mutex>
#include <windows.h> 

#include "ClientConnection.h"
#include "ServerDebug.h"
#include "SIMP.h"

#include "WolfgangDebug.h"

#include "DeckServer.h"
#include "ServerPlayer.h"
#include "Frankenhearts.h"
#include "FrankenheartsAI.h"
#include "FrankenheartsAIPass.h"

#include "Hints.h"

#pragma comment(lib, "ws2_32.lib") 

#define SINGLEPLAYER_MODE true

int iCheckInputInt(std::string sInput) {
    int iSafe;
        try {
            iSafe = std::stoi(sInput);
        }   catch(std::exception) {
            iSafe = -1;
    }
    return iSafe;
}

class ServerSocket {


    void HelpRequest() {

        std::cout << "\n\n";
        std::cout << "\t\tFRANKENHEARTS ULTIMATE EDITION SERVER\n";
        std::cout << "\t\tPowered By:\n";
        std::cout << "\t\tSIMPLE INSTANT MESSAGING PROTOCOL\n";
        std::cout << "\t\tWOLFGANG: GAME ENGINE\n";
        std::cout << "\n\n";
        std::cout << "\t\tCreated By\n";
        std::cout << "\t\tMENDICANT SYSTEMS\n";
        std::cout << "\t\tVERSION 0.1\n";
        std::cout << "\n\n";
        std::cout << "CURRENT COMMANDS\n";
        HelpSyntax();
        HelpSystem();
        HelpConnectionMangement();
        HelpCommunication();
        HelpFrankenhearts();

    }

    //SERVER COMMANDS

    void SendPrivateMessageIP(std::string sMessage) {

        int iSent = 0;

        sMessage += '\n';

        size_t iStringIndex = sMessage.find('/'); 
        sMessage.erase(0, iStringIndex + 1);

        iStringIndex = sMessage.find('/');

        if (iStringIndex == std::string::npos) {
            Print("Incorrect Syntax -> No / before message");
            return;
        }

        std::string sIPAddress = sMessage.substr(0, iStringIndex);
        sMessage.erase(0, iStringIndex + 1);

        std::string sFromServer = "[Server (Private Message)] ";

        sFromServer += sMessage;

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            if (vccsocketConnections[i]->sIPAddress == sIPAddress) {

                iSent++;

                send(vccsocketConnections[i]->socketID, sFromServer.data(), strlen(sFromServer.data()), 0);

            }
        }

        PrintVarInt("Messages Sent", iSent);

    }

    void ClientSendPrivateMessageNick(std::string sClientMessage, ClientConnection* ccThreadClient) {

        //flag

        if (ccThreadClient->bMuted.load()) {
            const char* czBannedUsername = "[SERVER (Automated) -> INFO] You have been muted, message not sent\n";
            send(ccThreadClient->socketID, czBannedUsername, strlen(czBannedUsername), 0);
            return;
        }

        int iSent = 0;

        sClientMessage.erase(0, 1);

        size_t iStringIndex = sClientMessage.find('/'); 
        sClientMessage.erase(0, iStringIndex + 1);

        iStringIndex = sClientMessage.find('/');

        if (iStringIndex == std::string::npos) {
            std::string sErrorMessage = "[SERVER (Automated -> ERROR)] Incorrect Syntax\n";
            send(ccThreadClient->socketID, sErrorMessage.data(), strlen(sErrorMessage.data()), 0);
            return;
        }

        std::string sNickname = sClientMessage.substr(0, iStringIndex);
        sClientMessage.erase(0, iStringIndex + 1);

        std::string sFromClient = "[" + ccThreadClient->sNickname + "(Private Message)]";

        sFromClient += sClientMessage;
        sFromClient += '\n';

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            if (vccsocketConnections[i]->sNickname.substr(0, sNickname.length()) == sNickname) {

                iSent++;

                send(vccsocketConnections[i]->socketID, sFromClient.data(), sFromClient.size(), 0);

                if (!ccThreadClient->bServerMuted.load()) {
                    std::cout << ccThreadClient->sIPAddress << "[Reciever: " << vccsocketConnections[i]->sNickname << "]" << sFromClient;
                }

            }

        }

        if (iSent == 0) {
            std::string sErrorMessage = "[SERVER (Automated -> WARN)] Nickname Not Found\n";
            send(ccThreadClient->socketID, sErrorMessage.data(), strlen(sErrorMessage.data()), 0);
            return;
        }

    }

    void MuteClientIndex(std::string sIndex) {

        size_t iStringIndex = sIndex.find('/'); 
        sIndex.erase(0, iStringIndex + 1);

        int iIndex = iCheckInputInt(sIndex);

        if (iIndex < -1) {
            PrintWarn("Invalid Index");
            return;
        } 

        if (iIndex < vccsocketConnections.size()) {
            vccsocketConnections[iIndex]->bMuted.store(true);
            PrintVarStringInfo("Muted", vccsocketConnections[iIndex]->sNickname);
        } else {
            PrintWarn("Index Outside of Range");
        }

    }

    void UnmuteClientIndex(std::string sIndex) {

        size_t iStringIndex = sIndex.find('/'); 
        sIndex.erase(0, iStringIndex + 1);

        int iIndex = iCheckInputInt(sIndex);

        if (iIndex < -1) {
            Print("Invalid Index");
            return;
        } 

        if (iIndex < vccsocketConnections.size()) {
            vccsocketConnections[iIndex]->bMuted.store(false);
            PrintVarStringInfo("Unmuted", vccsocketConnections[iIndex]->sNickname);
        } else {
            PrintWarn("Index Outside of Range");
        }

    }

    void ServerMuteAll() {

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            vccsocketConnections[i]->bServerMuted.store(true);
        
        }

        PrintInfo("Muted all Clients");

    }

    void ServerUnmuteAll() {

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            vccsocketConnections[i]->bServerMuted.store(false);
        
        }

        PrintInfo("Unmuted all Clients");

    }

    void ForceDisconnectionNickname(std::string sNickname) {

        size_t iIndex = sNickname.find('/'); 

        sNickname.erase(0, iIndex + 1);

        for (int i = 0; i < vccsocketConnections.size(); i++) {


            if (vccsocketConnections[i]->sNickname.substr(0, sNickname.length()) == sNickname) {

                Print("Attempting to disconnect Client");
                RemoveIndividualClient(vccsocketConnections[i]);

            }
        }

    }

    void ForceDisconnectionIP(std::string sIPAddress) {

        size_t iIndex = sIPAddress.find('/'); 

        sIPAddress.erase(0, iIndex + 1);

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            if (vccsocketConnections[i]->sIPAddress == sIPAddress) {

                Print("Attempting to disconnect Client");
                RemoveIndividualClient(vccsocketConnections[i]);

            }
        }

    }

    void HandleClientCard(std::string sClientCardString, ClientConnection* ccThreadClient) {

        if (allCards.find(sClientCardString) != allCards.end()) {
            ccThreadClient->WriteCurrentCard(allCards[sClientCardString]);
        } else {
            WG::print("Client sent invalid card -> note this should not be possible! Async between DeckClient.h and DeckServer.h");
            WG::print("If you see this message, please tell me"); 
        }

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

    void HandleClientPassVector(std::string sClientMessage, ClientConnection* ccThreadClient) {

        std::vector<std::string> vsPassedCards = extractCardIDs(sClientMessage);

        std::vector<Card> vcPassedCards = {};

        for (int i = 0; i < vsPassedCards.size(); i++) {
            vcPassedCards.push_back(allCards[vsPassedCards[i]]);
        }

        ccThreadClient->WriteCurrentPass(vcPassedCards);

    }
    

    void ExecuteClientCommand(std::string sClientMessage, ClientConnection* ccThreadClient) {

        //Frankenhearts

        if (sClientMessage.substr(0, SIMP_FRANKENHEARTS_SendIndividualCard.size()) == SIMP_FRANKENHEARTS_SendIndividualCard) {
            sClientMessage.erase(0, SIMP_FRANKENHEARTS_SendIndividualCard.size());
            HandleClientCard(sClientMessage, ccThreadClient);
            return;
        }

        if (sClientMessage.substr(0, SIMP_FRANKENHEARTS_PassVectorStart.size()) == SIMP_FRANKENHEARTS_PassVectorStart) {
            HandleClientPassVector(sClientMessage, ccThreadClient);
        }

        //System

        if (sClientMessage == SIMP_Client_Exit) {
            RemoveIndividualClient(ccThreadClient);
            Print("User Disconnected");
            return;
        }

        //Communcation

        if (sClientMessage.substr(0, SIMP_Client_DM_Nickname.length()) == SIMP_Client_DM_Nickname) {
        }

        //Easter Eggs

        if (sClientMessage == SIMP_Client_MisterMeowie) {
            ClientMisterMeowie(ccThreadClient);
        }

    }

    void ClientSendPassType(const Player& pPlayer, const int iPassType) {

        std::string sPassType = SIMP_FRANKENHEARTS_PassStatus + std::to_string(iPassType);
        PreventMessageOverlap();

        send(pPlayer.ccClientConnection->socketID, sPassType.data(), strlen(sPassType.data()), 0);

    }

    void ClientSendAllPassType(const std::vector<Player>& vpPlayers, const int iPassType) {

        std::string sPassType = SIMP_FRANKENHEARTS_PassStatus + std::to_string(iPassType);

        for (int i = 0; i < vpPlayers.size(); i++) {
            if (vpPlayers[i].bHuman) {
                PreventMessageOverlap();
                send(vpPlayers[i].ccClientConnection->socketID, sPassType.data(), strlen(sPassType.data()), 0);
            }
        }

    }

    void ClientSendIndividualAchievement(Player& pTargetPlayer) {

        int iAchievement = pTargetPlayer.ccClientConnection->ReadCurrentAchievement();

        if (iAchievement == FRANKENHEARTS_ACHIEVEMENT_NONE) {
            return;
        }

        std::string sAchievementMessage = SIMP_FRANKENHEARTS_Acheivement + std::to_string(iAchievement);

        PreventMessageOverlap();
        send(pTargetPlayer.ccClientConnection->socketID, sAchievementMessage.data(), strlen(sAchievementMessage.data()), 0);
        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_NONE);

    }

    void ClientSendAllClientsPrompts(std::vector<Player>& vpPlayers) {

        for (int i = 0; i < vpPlayers.size(); i++) {

            if (vpPlayers[i].bHuman && vpPlayers[i].bSendPrompt) {

                PreventMessageOverlap();

                std::string sPrompt = SIMP_FRANKENHEARTS_Prompt;
                sPrompt += std::to_string(vpPlayers[i].iCurrentPrompt);

                send(vpPlayers[i].ccClientConnection->socketID, sPrompt.data(), strlen(sPrompt.data()), 0);

                vpPlayers[i].bSendPrompt = false;

                WG::printvarchar("Send prompt to client", vpPlayers[i].sNickname.data());

            }

        }


    }

    void ClientSendAllClientsAchievements(std::vector<Player>& vpPlayers) {

        for (int i = 0; i < vpPlayers.size(); i++) {

            if (vpPlayers[i].bHuman) {

                ClientSendIndividualAchievement(vpPlayers[i]);

            }

        }

    }

    void ClientSendScoreStrings(const std::vector<Player>& vpPlayers) {

        std::vector<std::string> vsPlayerScoreStrings;
        
        for (int i = 0; i < vpPlayers.size(); i++) {
            std::string sPlayerScore;
            sPlayerScore += "|";
            sPlayerScore += vpPlayers[i].sNickname;
            sPlayerScore += ": ";
            sPlayerScore += std::to_string(vpPlayers[i].iScore);

            vsPlayerScoreStrings.push_back(sPlayerScore);
        }

        for (int i = 0; i < vpPlayers.size(); i++) {

            if (vpPlayers[i].bHuman) {

                PreventMessageOverlap();

                std::string sTranformedPlayerScoreStrings = PerformPlayerScoreStringsTransformation(vpPlayers[i].iPosition, vsPlayerScoreStrings);
                std::string sPlayerScoreStringsPacket = SIMP_FRANKENHEARTS_PlayerScoreStringsStart;
                sPlayerScoreStringsPacket += sTranformedPlayerScoreStrings;
                sPlayerScoreStringsPacket += SIMP_FRANKENHEARTS_PlayerScoreStringsFinish;
    
                send(vpPlayers[i].ccClientConnection->socketID, sPlayerScoreStringsPacket.data(), strlen(sPlayerScoreStringsPacket.data()), 0);
    
            }

        }

    }

    void ClientSendCardsInTrick(const GameRule& grGameRule, const std::vector<Player>& vpPlayers) {

        for (int i = 0; i < vpPlayers.size(); i++) {

            if (vpPlayers[i].bHuman) {

                PreventMessageOverlap();

                std::vector<Card> vcTransformedCardInTricksVector = PerformCardsInTrickTransformation(vpPlayers[i].iPosition, grGameRule.vcGraphicsCardsInTrick);

                std::string sCardsInTrick;
    
                sCardsInTrick += SIMP_FRANKENHEARTS_CardsInTrickStart;
    
    
    
                for (int x = 0; x < vcTransformedCardInTricksVector.size(); x++) {
                    sCardsInTrick += vcTransformedCardInTricksVector[x].sCardID;
                }
                
                sCardsInTrick += SIMP_FRANKENHEARTS_CardsInTrickFinish;
    
    
                send(vpPlayers[i].ccClientConnection->socketID, sCardsInTrick.data(), strlen(sCardsInTrick.data()), 0);

            }

        }

    }

    void ClientMisterMeowie(ClientConnection* ccThreadClient) {
        std::vector<std::string> sMisterMeowie = {
            "\n",
           R"( __..--''``---....___   _..._    __                   )",
           R"( /// //_.-'    .-/";  `        ``<._  ``.''_ `. / // /)",
           R"(///_.-' _..--.'_    \                    `( ) ) // // )",
           R"(/ (_..-' // (< _     ;_..__               ; `' / ///  )",
           R"( / // // //  `-._,_)' // / ``--...____..-' /// / //   )",
            "\tMeow! If I'm not sleeping, I'm biting!\n"
        };

        for (int i = 0; i < sMisterMeowie.size(); i++) {
            sMisterMeowie[i] += '\n';
            send(ccThreadClient->socketID, sMisterMeowie[i].data(), sMisterMeowie[i].size(), 0);
        }

    }

    void ForceDisconnectionIndex(std::string sIndex) {

        size_t iStringIndex = sIndex.find('/'); 
        sIndex.erase(0, iStringIndex + 1);

        int iIndex = iCheckInputInt(sIndex);

        if (iIndex < -1) {
            Print("Invalid Index");
            return;
        } 

        if (iIndex < vccsocketConnections.size()) {
            RemoveIndividualClient(vccsocketConnections[iIndex]);
        } else {
            Print("Index Outside of Range");
        }

    }

    void DisplayAllClients() {

        Print("CONNECTED CLIENTS:");

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            std::cout << i << "\t" << vccsocketConnections[i]->sIPAddress << "\t" << vccsocketConnections[i]->sNickname << "\t" << "SocketID " << vccsocketConnections[i]->socketID <<"\n"; 

        }

    }

    void ForwardClientMessage(std::string sMessage, ClientConnection* ccOutputClient) {

        if (ccOutputClient->bMuted.load()) {
            const char* czBannedUsername = "[SERVER (Automated) -> INFO] You have been muted, message not sent\n";
            send(ccOutputClient->socketID, czBannedUsername, strlen(czBannedUsername), 0);
            return;
        }

        std::string sNicknameWrapper = "[" + ccOutputClient->sNickname + "]";
        std::string sOutput = sNicknameWrapper + sMessage;
        sOutput += '\n';

        if (!ccOutputClient->bServerMuted.load()) {
            std::cout << ccOutputClient->sIPAddress << " "<< sOutput;
        }

        for (int i = 0; i < vccsocketConnections.size(); i++) {

            if (vccsocketConnections[i]->socketID != ccOutputClient->socketID) {
                send(vccsocketConnections[i]->socketID, sOutput.data(), sOutput.length(), 0);

            }
        }

    }

    void RemoveIndividualClient(ClientConnection* ccDisconnectionClient) {
        for (int i = 0; i < vccsocketConnections.size(); i++) {
            if (vccsocketConnections[i]->socketID == ccDisconnectionClient->socketID) {
                send(vccsocketConnections[i]->socketID, SIMP_Client_ForceExit.data(), SIMP_Client_ForceExit.size(), 0);
                ccDisconnectionClient->bConnected.store(false);
                closesocket(ccDisconnectionClient->socketID);
                if (!ccDisconnectionClient->bInGame.load()) {
                    delete vccsocketConnections[i];
                    Print("Removed Client");
                }
                vccsocketConnections.erase(vccsocketConnections.begin() + i);
            }
        }
    }

    bool CheckNickname(std::string sNickname, int socketClient) {

        for (int i = 0; i < SIMP_BLACKLISTEDNICKNAMES.size(); i++) {
            if (SIMP_BLACKLISTEDNICKNAMES[i] == sNickname.substr(0, SIMP_BLACKLISTEDNICKNAMES[i].length())) {
                const char* czBannedUsername = "[SERVER (Automated) -> ERROR] Banned Username\n";
                send(socketClient, czBannedUsername, strlen(czBannedUsername), 0);
                return false;
            }
        }

        for (int i = 0; i < vccsocketConnections.size(); i++) {
            if (sNickname == vccsocketConnections[i]->sNickname.substr(0, vccsocketConnections[i]->sNickname.length())) {
                const char* czBannedUsername = "[SERVER (Automated) -> ERROR] Username Taken\n";
                send(socketClient, czBannedUsername, strlen(czBannedUsername), 0);
                return false;
            }
        }

        return true;


    }

    void MisterMeowie() {
        std::vector<std::string> sMisterMeowie = {
            "\n",
           R"( __..--''``---....___   _..._    __                   )",
           R"( /// //_.-'    .-/";  `        ``<._  ``.''_ `. / // /)",
           R"(///_.-' _..--.'_    \                    `( ) ) // // )",
           R"(/ (_..-' // (< _     ;_..__               ; `' / ///  )",
           R"( / // // //  `-._,_)' // / ``--...____..-' /// / //   )",
            "\tMeow! If I'm not sleeping, I'm biting!\n"
        };

        for (int i = 0; i < sMisterMeowie.size(); i++) {
            std::cout << sMisterMeowie[i] << '\n';
        }

    }

    void ServerSendMessage(std::string sMessage) {

        for (int i = 0; i < vccsocketConnections.size(); i++) {
            sMessage += '\n';
            sMessage.insert(0, "[SERVER]");
            send(vccsocketConnections[i]->socketID, sMessage.data(), strlen(sMessage.data()), 0);
        }
    }

    void ServerSendBotMessage(Player& pBot) {

        pBot.bBotMessage = SetBotMessage(pBot);

        if (!pBot.bBotMessage) {
            return;
        } else {
            pBot.bBotMessage = false;
        }

        std::string sMessage = pBot.sBotMessage;
        sMessage.insert(0, pBot.sNickname);

        for (int i = 0; i < vccsocketConnections.size(); i++) {
            PreventMessageOverlap();
            send(vccsocketConnections[i]->socketID, sMessage.data(), strlen(sMessage.data()), 0);
        }
    }

    void PrintSimpASCII() {
        std::vector<const char*> vczFuckText = {

            "░▒▓███████▓▒░▒▓█▓▒░▒▓██████████████▓▒░░▒▓███████▓▒░  \n",
            "░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░\n", 
            "░▒▓█▓▒░      ░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░\n", 
            " ░▒▓██████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓███████▓▒░ \n", 
            "       ░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░       \n", 
            "       ░▒▓█▓▒░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░       \n", 
            "░▒▓███████▓▒░░▒▓█▓▒░▒▓█▓▒░░▒▓█▓▒░░▒▓█▓▒░▒▓█▓▒░       \n",
            "          SIMPLE INSTANT MESSAGING PROTOCOL\n" 
        };

        for (int i = 0; i < vczFuckText.size(); i++) {
            std::cout << vczFuckText[i];
        }
    }

    void SetSocketNonBlocking(SOCKET socket) {
        u_long iMode = 1; // 1 for non-blocking, 0 for blocking
        if (ioctlsocket(socket, FIONBIO, &iMode) != 0) {
            std::cerr << "Error setting socket to non-blocking\n";
        }
    }

    void HandleClient(SOCKET clientSocket, std::atomic<bool>& bServerOnline) {        
        char cBuffer[1024];
        memset(cBuffer, 0, sizeof(cBuffer));

        bool bNicknameAccepted = false;
        std::string sNickname;
        int iNicknameRecieved;

        sNickname.clear();
        iNicknameRecieved = recv(clientSocket, cBuffer, sizeof(cBuffer), 0);
        sNickname.append(cBuffer, iNicknameRecieved);

        SetSocketNonBlocking(clientSocket);
    
        ClientConnection* ccThreadClient = new ClientConnection(clientSocket, sNickname);
        vccsocketConnections.push_back(ccThreadClient);

        Print("Client Connected\n");
        PrintVarString("\tNickname  ", ccThreadClient->sNickname);
        PrintVarString("\tIP Address", ccThreadClient->sIPAddress);
        PrintVarInt   ("\tSocket ID ", ccThreadClient->socketID);
        PrintVarInt   ("\tIndex     ", vccsocketConnections.size() - 1);
    
        //REMOVE EVENTUALLY
        if (SINGLEPLAYER_MODE) {
            WG::print("Starting New Frankenhearts Game!");
            std::thread tGameThread(&ServerSocket::Frankenhearts, this, std::ref(bServerOnline));
            tGameThread.detach();
        }

        std::string sFullMessage = "";  
    
        while(ccThreadClient->bConnected.load()) {
            memset(cBuffer, 0, sizeof(cBuffer));  
    

            int iBytesReceived = recv(clientSocket, cBuffer, sizeof(cBuffer) - 1, 0);
    
            if (iBytesReceived > 0) {

                sFullMessage += std::string(cBuffer, iBytesReceived);
    
                if (sFullMessage[0] != '/') {
                    ForwardClientMessage(sFullMessage, ccThreadClient);
                } else {
                    ExecuteClientCommand(sFullMessage, ccThreadClient);
                }

                sFullMessage.clear();

            } else if (iBytesReceived == 0) {

                PrintInfo("Client closed the connection.");
                RemoveIndividualClient(ccThreadClient);
                break;
            }
            else if (iBytesReceived == -1) {
                if (WSAGetLastError() != WSAEWOULDBLOCK) {
                    PrintInfo("Recieve Error Occurred -> Likely Disconnection");
                    RemoveIndividualClient(ccThreadClient);
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    public:

        int iPort = 8080;

        SOCKET socketServer = -1;
        struct sockaddr_in socketAddress;
        socklen_t socketAddressLength;

        std::vector<ClientConnection*> vccsocketConnections;

        ServerSocket(int Port) {
            iPort = Port;

            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                Print("WSAStartup failed");
                return;
            }

            socketServer = socket(AF_INET, SOCK_STREAM, 0);

            if (socketServer == INVALID_SOCKET) {
                Print("Socket Creation Failed");
                return;
            }

            int iOption = 1;
            setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, (const char*)&iOption, sizeof(iOption));

            socketAddressLength = sizeof(socketAddress);
            memset(&socketAddress, 0, sizeof(socketAddress));

            socketAddress.sin_family = AF_INET;
            
            socketAddress.sin_addr.s_addr = INADDR_ANY;


            socketAddress.sin_port = htons(iPort);

            if (bind(socketServer, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR) {
                Print("Server Failed to Bind");
                closesocket(socketServer);
                WSACleanup();
                return;
            }

            if (listen(socketServer, 3) == SOCKET_ERROR) {
                Print("Server Could Not Begin Listening");
                closesocket(socketServer);
                WSACleanup();
                return;
            }

            SetSocketNonBlocking(socketServer);

            PrintSimpASCII();
            std::cout << "Address: " << inet_ntoa(socketAddress.sin_addr) << "\n";
            PrintVarInt("Port", iPort);
        }

        int Destruct() {
            for (int i = 0; i < vccsocketConnections.size(); i++) {
                RemoveIndividualClient(vccsocketConnections[i]);
            }

            closesocket(socketServer);
            Print("Closed Server");
            WSACleanup();
            return 0;
        }

        void UserInput(std::atomic<bool>& bServerOnline) {
            std::string sUserInput;

            while (bServerOnline.load()) {

                bool bLegitimateCommand = false;

                std::getline(std::cin, sUserInput);

                if (sUserInput == "help" || sUserInput == "HELP" || sUserInput == "h") {
                    HelpRequest();
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_FRANKENHEARTS_GameStart) {
                    WG::print("Starting New Frankenhearts Game!");
                    std::thread tGameThread(&ServerSocket::Frankenhearts, this, std::ref(bServerOnline));
                    tGameThread.detach();
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_Server_Exit) {
                    bServerOnline.store(false);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_List.size()) == SIMP_Server_List) {
                    DisplayAllClients();
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_DCNick.size()) == SIMP_Server_DCNick) {
                    ForceDisconnectionNickname(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_DCIP.size()) == SIMP_Server_DCIP) {
                    ForceDisconnectionIP(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_DCI.size()) == SIMP_Server_DCI) {
                    ForceDisconnectionIndex(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_DMIP.size()) == SIMP_Server_DMIP) {
                    SendPrivateMessageIP(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_Mute_Index.size()) == SIMP_Server_Mute_Index) {
                    MuteClientIndex(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput.substr(0, SIMP_Server_Unmute_Index.size()) == SIMP_Server_Unmute_Index) {
                    UnmuteClientIndex(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_Server_ServerMute_All) {
                    ServerMuteAll();
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_Server_ServerUnmute_All) {
                    ServerUnmuteAll();
                    bLegitimateCommand = true;
                }

                if (sUserInput[0] == '*') {
                    sUserInput.erase(sUserInput.begin() + 0);
                    ServerSendMessage(sUserInput);
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_Server_MisterMeowie) {
                    MisterMeowie();
                    bLegitimateCommand = true;
                }

                if (sUserInput == SIMP_WOLFGANG_DEBUG_ECHO) {
                    WG::BOOL_ECHO_WOLFGANG_DEBUG = !WG::BOOL_ECHO_WOLFGANG_DEBUG;
                    bLegitimateCommand = true;
                }

                if (!bLegitimateCommand) {
                    PrintInfo("Command not recognised -> Type \"help\" for command syntaxs");
                }


            }

            Print("Server Input Thread Out of Scope");



        }


        void AcceptConnections(std::atomic<bool>& bServerOnline) {

            while (bServerOnline.load()) {
                sockaddr_in addressClient;
                socklen_t addressClientLen = sizeof(addressClient);
                
                // Non-blocking accept
                int socketClient = accept(socketServer, (sockaddr*)&addressClient, &addressClientLen);
                
                if (socketClient != -1) {
                    Print("Client Connecting . . .");
                    std::thread clientThread(&ServerSocket::HandleClient, this, socketClient, std::ref(bServerOnline));
                    clientThread.detach();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
    
            Print("No Longer Accepting Connections");
        }

        void CheckPlayersConnectionStatus(std::vector<Player>& vpPlayers) {

            for (int i = 0; i < vpPlayers.size(); i++) {

                if (vpPlayers[i].bHuman) {
                    if (!vpPlayers[i].ccClientConnection->bConnected.load()) {
                        vpPlayers[i].bHuman = false;
                        vpPlayers[i].ccClientConnection->DestoryConnection();
                    }
                }

            }

        }

        bool CheckHumanPlayer(std::vector<Player>& vpPlayers) {
            for (int i = 0; i < vpPlayers.size(); i++) {
                if (vpPlayers[i].bHuman) {
                    return true;
                } 
            }
            PrintWarn("No human players detected, game automatically closed");
            return false;
        }


        //GAME FUNCTIONS

        void GiveAllClientsPreviousTrick(std::vector<Player>& vpPlayers, const GameRule& grGameRule) {

            WG::print("Giving all players previous cards in trick");

            std::string sPreviousTrick = SIMP_FRANKENHEARTS_PreviousTrickStart;

            for (int i = 0; i < grGameRule.vcInternalCardsInTrick.size(); i++) {
                sPreviousTrick += grGameRule.vcInternalCardsInTrick[i].sCardID;
            }

            sPreviousTrick += SIMP_FRANKENHEARTS_PreviousTrickFinish;

            for (int i = 0; i < vpPlayers.size(); i++) {
                if (vpPlayers[i].bHuman) {
                    PreventMessageOverlap();
                    send(vpPlayers[i].ccClientConnection->socketID, sPreviousTrick.data(), strlen(sPreviousTrick.data()), 0);
                }
            }
        }

        void GiveAllClientsHeartsStatus(std::vector<Player>& vpPlayers, GameRule& grGameRule) {
            WG::print("Giving Clients Hearts Status");
            for (int i = 0; i < vpPlayers.size(); i++) {
                GiveClientHeartsStatus(vpPlayers[i], grGameRule);
            }
            grGameRule.bCommunicateBreak = false;
        }

        void GiveClientHeartsStatus(Player& pPlayer, const GameRule& grGameRule) {

            if (!pPlayer.bHuman) {
                return;
            }

            std::string sHeartsStatus = SIMP_FRANKENHEARTS_HeartsStatus;

            if (grGameRule.bHearts) {
                sHeartsStatus += "1";
            } else {
                sHeartsStatus += "0";
            }

            send(pPlayer.ccClientConnection->socketID, sHeartsStatus.data(), strlen(sHeartsStatus.data()), 0);

        }

        void GiveAllClientsCardsRecieved(std::vector<Player>& vpPlayers) {
            for (int i = 0; i < vpPlayers.size(); i++) {
                if (vpPlayers[i].bHuman) {

                    PreventMessageOverlap();

                    std::string sCardsRecievedString = "";

                    sCardsRecievedString += SIMP_FRANKENHEARTS_CardsRecievedStart;

                    for (int x = 0; x < vpPlayers[i].vcCardsRecievedInPass.size(); x++) {
                        sCardsRecievedString += vpPlayers[i].vcCardsRecievedInPass[x].sCardID;
                    }

                    sCardsRecievedString += SIMP_FRANKENHEARTS_CardsRecievedEnd;

                    WG::printvarchar("Cards recieved in pass sent to", vpPlayers[i].sNickname.data());

                    send(vpPlayers[i].ccClientConnection->socketID, sCardsRecievedString.data(), strlen(sCardsRecievedString.data()), 0);


                }
            }
        }

        void GiveAllClientsHand(std::vector<Player>& vpPlayers) {
            for (int i = 0; i < vpPlayers.size(); i++) {
                GiveClientHand(vpPlayers[i]);
            }
        }

        void GiveClientHand(Player& pPlayer) {

            if (!pPlayer.bHuman) {
                return;
            }

            PreventMessageOverlap();

            std::string sNewDeckString;

            sNewDeckString += SIMP_FRANKENHEARTS_DeckStart;

            for (int i = 0; i < pPlayer.vcCurrentDeck.size(); i++) {
                sNewDeckString += pPlayer.vcCurrentDeck[i].sCardID;
            }

            sNewDeckString += SIMP_FRANKENHEARTS_DeckFinish;

            send(pPlayer.ccClientConnection->socketID, sNewDeckString.data(), strlen(sNewDeckString.data()), 0);

        }

        //GAME FUNCTIONS

        bool PassRound(std::vector<Player>& vpPlayers, int iPassType, Dealer dDealer) {

            ResetPassData(vpPlayers);

            if (iPassType == PASS_NONE) {
                return true;
            }
        
            int iSuccessfulPasses = 0;
        
            while (iSuccessfulPasses != 3) {

                CheckPlayersConnectionStatus(vpPlayers);
        
                for (int i = 0; i < vpPlayers.size(); i++) {
        
                    if (vpPlayers[i].bHuman) {
                        if (vpPlayers[i].ccClientConnection->ReadCurrentPass().size() != 0 && !vpPlayers[i].bPassed) {
                            vpPlayers[i].vcCardsToPass = vpPlayers[i].ccClientConnection->ReadCurrentPass();
                            ClientSendPassType(vpPlayers[i], PASS_WAIT);
                            vpPlayers[i].bPassed = true;
                            iSuccessfulPasses++;

                            AchievementPassDerived(vpPlayers[i]);
                        }
                    } else {
                        if (vpPlayers[i].vcCardsToPass.size() == 0) {
                            BotPassCards(vpPlayers[i]);
                            iSuccessfulPasses++;
                        }
                    }
                }

                PreventLoopCPUOverload();

            }

            DistributePass(vpPlayers, iPassType);

            for (int i = 0; i < vpPlayers.size(); i++) {
                dDealer.SortHand(vpPlayers[i].vcCurrentDeck);
            }

            GiveAllClientsHand(vpPlayers);
            ClientSendAllPassType(vpPlayers, PASS_NONE);
            GiveAllClientsCardsRecieved(vpPlayers);



            return true;

        }

        void Frankenhearts(std::atomic<bool>& bServerOnline) {

            srand(time(NULL));

            //Create Players:

            std::vector<Player> vpPlayers;

            int iIterator = vccsocketConnections.size();

            if (iIterator > 3) {
                iIterator = 3;
            } 

            for (int i = 0; i < iIterator; i++) {
                vpPlayers.push_back(Player(i, vccsocketConnections[i], true));
                vccsocketConnections[i]->bInGame.store(true);
            }

            //Check for developer bot challengers

            if (vpPlayers.size() == 1 && vpPlayers[0].sNickname == "DevKiller") {
                vpPlayers.push_back(Player((vpPlayers.size()), nullptr, false));
                vpPlayers[1].sNickname = "Maxwell[AI]";
                vpPlayers[1].iMessageType = AI_SPEECH_MAXWELLTAUNT;
                vpPlayers[1].bBotMessage = true;
            }

            //Create Bots:

            while (vpPlayers.size() != 3) {
                vpPlayers.push_back(Player((vpPlayers.size()), nullptr, false));
            }

            //Create Dealer

            Dealer dDealer;

            //Create Gamerule 

            GameRule grGameRule;
            ResetGraphicsCardsInTrick(grGameRule);

            //Start Game Loop

            AchievementGameStartDerived(vpPlayers);
            ClientSendAllClientsAchievements(vpPlayers);

            while (bServerOnline.load()) {

                CheckPlayersConnectionStatus(vpPlayers);
                if (!CheckHumanPlayer(vpPlayers)) {
                    return;
                }


                if (grGameRule.bResetTrick) {

                    ClientSendScoreStrings(vpPlayers);

                    ResetManyBots(vpPlayers);

                    DealNewHand(vpPlayers, dDealer);
                    PreventMessageOverlap();
                    GiveAllClientsHand(vpPlayers);

                    grGameRule.vcAllCardsInRound.clear();

                    IncrementPassPosition(grGameRule);
                    ClientSendAllPassType(vpPlayers, grGameRule.iPassType);
                    PassRound(vpPlayers, grGameRule.iPassType, dDealer);

                    grGameRule.iRoundCounter++;
                    PrintVarIntInfo("Round", grGameRule.iRoundCounter);

                    grGameRule.bResetTrick = false;
                    grGameRule.bFirstTrick = true;
                    grGameRule.bFirstTrickLead = true;

                    UnBreakHearts(grGameRule);

                    FindTwoOfClubs(grGameRule, vpPlayers);

                    grGameRule.iLeadDynamic = grGameRule.iCurrentLead;

                } else {

                    if (grGameRule.bFirstTrick) {

                        while(grGameRule.bFirstTrickLead) {

                            int iCardPlayed;

                            if (vpPlayers[grGameRule.iCurrentLead].bHuman) {
                                iCardPlayed = LeadTwoOfClubs(grGameRule, vpPlayers[grGameRule.iCurrentLead]);
                            } else {
                                iCardPlayed = BotLeadTwoOfClubs(grGameRule, vpPlayers[grGameRule.iCurrentLead]);
                            }


                            if (iCardPlayed == 0) {

                                vpPlayers[grGameRule.iCurrentLead].iDynamicPosition = 0;
                                GiveClientHand(vpPlayers[grGameRule.iCurrentLead]);
                                PreventMessageOverlap();
                                ClientSendCardsInTrick(grGameRule, vpPlayers);

                                IncrementDynamicLead(grGameRule);
                                IncrementPositionTracker(grGameRule);


                                GiveAllPlayersNullCard(vpPlayers);

                            }
                            
                            ClientSendAllClientsPrompts(vpPlayers);
                            PreventLoopCPUOverload();

                        }


                        while(!grGameRule.bFirstTrickLead && grGameRule.vcInternalCardsInTrick.size() != 3) {

                            CheckPlayersConnectionStatus(vpPlayers);

                            int iCardPlayed;

                            if (vpPlayers[grGameRule.iLeadDynamic].bHuman) {
                                iCardPlayed = FirstTrickPlay(grGameRule, vpPlayers[grGameRule.iLeadDynamic]);
                            } else {
                                WG::print("");
                                iCardPlayed = BotFirstTrickPlay(grGameRule, vpPlayers[grGameRule.iLeadDynamic]);
                                WG::print("");
                            }

                            if (iCardPlayed == 0) {

                                GiveClientHand(vpPlayers[grGameRule.iLeadDynamic]);
                                vpPlayers[grGameRule.iLeadDynamic].iDynamicPosition = grGameRule.iPositionTracker;
                                PreventMessageOverlap();
                                ClientSendCardsInTrick(grGameRule, vpPlayers);

                                IncrementDynamicLead(grGameRule);
                                IncrementPositionTracker(grGameRule);

                                GiveAllPlayersNullCard(vpPlayers);


                            }

                            ClientSendAllClientsPrompts(vpPlayers);
                            PreventLoopCPUOverload();

                        }

                        if (grGameRule.vcInternalCardsInTrick.size() == 3) {

                            GiveAllClientsPreviousTrick(vpPlayers, grGameRule);

                            for (int i = 0; i < vpPlayers.size(); i++) {
                                vpPlayers[i].SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
                            }


                            DetermineLead(grGameRule, vpPlayers);
                            grGameRule.bFirstTrick = false;
                            grGameRule.bTrickLead = true;
                            grGameRule.iTurnCounter++;

                        }

                    } else {

                        while (grGameRule.bTrickLead) {

                            CheckPlayersConnectionStatus(vpPlayers);

                            int iCardPlayed;

                            if (vpPlayers[grGameRule.iLeadDynamic].bHuman) {
                                iCardPlayed = LeadCard(grGameRule, vpPlayers[grGameRule.iLeadDynamic]);
                            } else {
                                WG::print("");
                                iCardPlayed = BotLeadCard(grGameRule, vpPlayers[grGameRule.iLeadDynamic], vpPlayers);
                                WG::print("");
                            }

                            if (iCardPlayed == 0) {

                                GiveClientHand(vpPlayers[grGameRule.iLeadDynamic]);
                                vpPlayers[grGameRule.iLeadDynamic].iDynamicPosition = grGameRule.iPositionTracker;
                                PreventMessageOverlap();
                                ClientSendCardsInTrick(grGameRule, vpPlayers);

                                IncrementDynamicLead(grGameRule);
                                IncrementPositionTracker(grGameRule);

                                GiveAllPlayersNullCard(vpPlayers);

                            }

                            ClientSendAllClientsPrompts(vpPlayers);
                            PreventLoopCPUOverload();

                        }

                        while (!grGameRule.bTrickLead && grGameRule.vcInternalCardsInTrick.size() != 3) {

                            CheckPlayersConnectionStatus(vpPlayers);

                            int iCardPlayed;

                            if (vpPlayers[grGameRule.iLeadDynamic].bHuman) {
                                iCardPlayed = TrickPlay(grGameRule, vpPlayers[grGameRule.iLeadDynamic]);
                            } else {
                                WG::print("");
                                iCardPlayed = BotPlayCard(grGameRule, vpPlayers[grGameRule.iLeadDynamic], vpPlayers);
                                WG::print("");
                            }

                            if (iCardPlayed == 0) {

                                GiveClientHand(vpPlayers[grGameRule.iLeadDynamic]);
                                vpPlayers[grGameRule.iLeadDynamic].iDynamicPosition = grGameRule.iPositionTracker;
                                PreventMessageOverlap();
                                ClientSendCardsInTrick(grGameRule, vpPlayers);

                                IncrementDynamicLead(grGameRule);
                                IncrementPositionTracker(grGameRule);

                                GiveAllPlayersNullCard(vpPlayers);

                            }

                            ClientSendAllClientsPrompts(vpPlayers);
                            PreventLoopCPUOverload();

                        }

                        if(grGameRule.vcInternalCardsInTrick.size() == 3) {

                            for (int i = 0; i < grGameRule.vcInternalCardsInTrick.size(); i++) {
                                WG::printvarchar("Card in Trick", grGameRule.vcInternalCardsInTrick[i].sCardID.data());
                            }

                            GiveAllClientsPreviousTrick(vpPlayers, grGameRule);
                            
                            for (int i = 0; i < vpPlayers.size(); i++) {
                                vpPlayers[i].SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
                            }

                            DetermineLead(grGameRule, vpPlayers);
                            grGameRule.bTrickLead = true;
                            grGameRule.iTurnCounter++;

                        }


                        

                    }

                    if (grGameRule.iTurnCounter == 18) {

                        grGameRule.bResetTrick = true;
                        grGameRule.iTurnCounter = 0;

                        CalculateScore(vpPlayers, grGameRule);
                        
                        AchievementGOATFindPlayers(vpPlayers);
                        DeveloperBot(vpPlayers, grGameRule);

                    }


                }

                if (grGameRule.bCommunicateBreak) {
                    PreventMessageOverlap();
                    GiveAllClientsHeartsStatus(vpPlayers, grGameRule);
                    PreventMessageOverlap();
                }

                ClientSendAllClientsAchievements(vpPlayers);
                ClientSendAllClientsPrompts(vpPlayers);

                for (int i = 0; i < vpPlayers.size(); i++) {
                    if (vpPlayers[i].bBotMessage) {
                        ServerSendBotMessage(vpPlayers[i]);
                    }
                }

            }
    
        }

};


int main() {

    if (DEBUG) {
        for (int i = 0; i < 10; i++) {
            WG::print("THIS IS A DEBUG BUILD");
        }
    }

    if (SINGLEPLAYER_MODE) {
        for (int i = 0; i < 10; i++) {
            WG::print("SINGLEPLAYER BUILD");
        }
    }

    std::atomic<bool> bServerOnline = true;

    ServerSocket Server(8080);
    std::thread tConnectionThread(std::bind(&ServerSocket::AcceptConnections, &Server, std::ref(bServerOnline)));
    std::thread tInputThread(std::bind(&ServerSocket::UserInput, &Server, std::ref(bServerOnline)));

    tInputThread.join();
    tConnectionThread.join();

    Server.Destruct();

    return 0;
}