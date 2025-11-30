#include <string>
#include <cstdlib>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>

#include "DeckServer.h"
#include "ClientConnection.h"



#pragma once



/**
The Frankenhearts Ultimate Edition Server uses this to create Frankenhearts players
THIS CLASS IS NOT THREAD SAFE
It should only be called within the ServerSocket Frankenhearts() method!
*/
class Player {

private:

std::string GiveBotNickname() {
    std::vector<std::string> vsNicknames = {
        "Harry",
        "Craig",
        "Kim",
        "Jaz",
        "Adam",
        "Ben",
        "Angus",
        "Andy",
        "Baz",
        "Kalahan",
        "Bree",
        "Mickey",
        "Sean",
        "Jack",
        "Mister Meowie"
    };

    int iRandom = rand() % 200;

    if (iRandom == 1) {
        return "Maxwell[AI]";
    }

    return vsNicknames[rand() % vsNicknames.size()] + "[AI]";
}

public:

    //Bot Data
    bool bHuman = true;

    bool bMoonShooting = false;

    bool bQueenOfSpades = false;
    bool bTenOfClubs = false;
    bool bJackOfDiamonds = false;

    //Connection Data
    ClientConnection* ccClientConnection;

    //Game Data
    int iPosition;
    int iDynamicPosition;
    int iScore = 0;

    std::string sNickname = "WhoopsieDoopsie";

    Card cCurrentCard = allCards["cNULLCARD"];
    std::vector<Card> vcCurrentDeck;
    std::vector<Card> vcTakenCards = {};

    bool bPassed = false;
    std::vector<Card> vcCardsToPass = {};
    std::vector<Card> vcCardsRecievedInPass = {};

    int iCurrentPrompt = 10;
    bool bSendPrompt = false;

    bool bBotMessage = false;
    std::string sBotMessage = "";
    int iMessageType = 0;

    Player(int Position, ClientConnection* Client, bool Human) {
        iPosition = Position;
        ccClientConnection = Client;
        bHuman = Human;
        if (Human) {
            sNickname = Client->sNickname;
        } else {
            sNickname = GiveBotNickname();
        }
    }

    bool SetCurrentPrompt(int iPrompt) {
        if (iPrompt == iCurrentPrompt) {
            bSendPrompt = false;
            return false;
        } else {
            WG::print("Writing prompt");
            iCurrentPrompt = iPrompt;
            bSendPrompt = true;
            return true;
        }
    }

};



