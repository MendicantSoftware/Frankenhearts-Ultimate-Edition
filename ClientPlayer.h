#include <vector>
#include <string>
#include <mutex>

#include "WolfgangDebug.h"
#include "WolfgangPane.h"
#include "DeckClient.h"

#define FRANKENHEARTS_PROMPT_FIRSTTRICK_LEAD 0
#define FRANKENHEARTS_PROMPT_PLAY_CLUBS 1
#define FRANKENHEARTS_PROMPT_PLAY_SPADES 2
#define FRANKENHEARTS_PROMPT_PLAY_DIAMONDS 3
#define FRANKENHEARTS_PROMPT_PLAY_HEARTS 4
#define FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY 5
#define FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY_VOID 6 
#define FRANKENHEARTS_PROMPT_LEAD_HEARTSUNBROKEN 7
#define FRANKENHEARTS_PROMPT_LEAD_HEARTSBROKEN 8
#define FRANKENHEARTS_PROMPT_PLAY_VOID 9
#define FRANKENHEARTS_PROMPT_PLAY_JOKER -2
#define FRANKENHEARTS_PROMPT_EMPTY 10

#define PASS_LEFT 0
#define PASS_RIGHT 1
#define PASS_NONE 2
#define PASS_WAIT 4

#pragma once

class Player {

    private:

        std::mutex mtxPlayerMutex;


    public:

        WG::ReportPane* wrpChatPane;

        std::vector<std::string> vsChat = {};

        std::vector<std::string> vsScoreStrings;

        std::vector<Card> vsDeck;

        std::vector<Card> vsCardsInTrick;

        std::vector<Card> vsPreviousCardsInTrick;

        std::vector<Card> vsCardsRecievedInPass;
        bool bRenderCardsRecieved = false;

        bool bHearts = false;

        int iPassType = PASS_NONE;
        int iAchievement = 0;

        int iPrompt = FRANKENHEARTS_PROMPT_EMPTY;

        std::string sHint = "";

        void PushMessageToChat(std::string sMessage) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            wrpChatPane->AddDefaultWrappedText(WG::TextWrapping(sMessage, wrpChatPane->dPaneW * 2, wrpChatPane->dXTextInsetRatio, wrpChatPane->iDefaultFontSize));
            wrpChatPane->RefreshAllTextVectorTextures();
            WG::printvarint("iVector Point", wrpChatPane->iVectorPoint);
        }

        void WriteNewChatMessage(std::string sMessage) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            WG::printvarchar("Pushing into vsChat", sMessage.data());
            vsChat.push_back(sMessage);
            if (vsChat.size() > 10) {
                vsChat.erase(vsChat.begin());
            }
        }

        std::vector<std::string> ReadChat() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsChat;
        }

        int ReadPrompt() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return iPrompt;
        }

        void WritePrompt(int iNewPrompt) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            iPrompt = iNewPrompt;
        }

        bool ReadRenderCardsRecieved() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return bRenderCardsRecieved;
        }

        void WriteRenderCardsRecieved(bool bNewValue) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            bRenderCardsRecieved = bNewValue;
        }

        std::vector<Card> ReadCardsRecievedInPass() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsCardsRecievedInPass;
        }

        void WriteCardsRecievedInPass(std::vector<Card> vcCardsRecievedInPass) {
            vsCardsRecievedInPass = vcCardsRecievedInPass;
            bRenderCardsRecieved = true;
        }

        std::vector<Card> ReadPreviousCardsInTrick() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsPreviousCardsInTrick;
        }

        void WritePreviousCardsInTrick(std::vector<Card> vsPreviousTrick) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsPreviousCardsInTrick = vsPreviousTrick;
        }

        int ReadPassType() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return iPassType;
        }

        void WritePassType(int PassType) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            iPassType = PassType;
        }

        bool ReadHearts() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return bHearts;
        }

        void WriteHearts(bool bBool) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            bHearts = bBool;
        }

        std::vector<std::string> ReadPlayerScores() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsScoreStrings;
        }

        void WritePlayerScores(std::vector<std::string> vsNewScoreStrings) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsScoreStrings = vsNewScoreStrings;
        }

        std::string ReadPlayerHint() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return sHint;
        }

        void WritePlayerHint(std::string sNewHint) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            sHint = sNewHint;
        }

        std::vector<Card> ReadCardsInTrick() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsCardsInTrick;
        }

        void WriteCardsInTrick(std::vector<Card> vsNewCardsInTrick) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsCardsInTrick = vsNewCardsInTrick;
        }

        std::vector<Card> ReadDeck() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsDeck;
        }

        Card ReadDeckIndex(int i) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsDeck[i];
        }

        void ReWriteDeck(std::vector<Card> vsNewDeck) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsDeck = vsNewDeck;
        }

        void ReWriteCardPosition(int iCardIndex, SDL_Rect sdlRect) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsDeck[iCardIndex].sdlPositionRect = sdlRect;
        }

        void ReWriteCardSelection(int iCardIndex, bool bSelectionStatus) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsDeck[iCardIndex].bSelected = bSelectionStatus;
        }

        void RemoveAllCardSelections() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            for (int i = 0; i < vsDeck.size(); i++) {
                vsDeck[i].bSelected = false;
            }
        }

        void WriteDeck(std::string sCardID) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsDeck.emplace_back(allCards[sCardID]);
        }

        int RemoveIndexDeck(int iIndex) {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);

            if (iIndex > vsDeck.size()) {
                WG::printvarint("RemoveIndexDeck Error, iIndex is greater than vsDeck.size(), iIndex", iIndex);
                return 1;
            } else {
                vsDeck.erase(vsDeck.begin() + iIndex);
                return 0;
            }

        }

        void WriteCurrentAchievement(int iNewAchievement) {

            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            iAchievement = iNewAchievement;

        }

        int ReadCurrentAchievement() {

            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return iAchievement;

        }

        int GetSize() {
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            return vsDeck.size();
        }

        void ResetPlayer() {
            WG::print("Calling Reset Player");
            std::lock_guard<std::mutex> lock(mtxPlayerMutex);
            vsScoreStrings.clear();
            vsDeck.clear();
            vsCardsInTrick.clear();
            vsPreviousCardsInTrick.clear();
            vsCardsRecievedInPass.clear();
            bRenderCardsRecieved = false;
            bHearts = false;
            iPassType = PASS_NONE;
            iAchievement = 0;
            iPrompt = FRANKENHEARTS_PROMPT_EMPTY;            
        }


};


bool ValidatePlayerNickname(std::string sProposedNickname) {

    std::vector<char> vcBannedSymbols = {
        '&',
        ';',
        ':',
        '[',
        ']',
        '/'
    };

    for (int i = 0; i < sProposedNickname.size(); i++) {
        for (int x = 0; x < vcBannedSymbols.size(); x++) {
            if (sProposedNickname[i] == vcBannedSymbols[x]) {
                std::cout << "Banned Symbol" << vcBannedSymbols[x] << '\n';
                std::cout << "Located at " << i << '\n';
                return false;
            }
        }
    }

    return true;

}

bool ValidatePort(std::string sPort) {

    try {
        std::stoi(sPort);
    } catch (...) {
        return false;
    }

    return true;

}
