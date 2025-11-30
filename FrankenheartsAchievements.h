#include <vector>

#include "DeckServer.h"
#include "ServerPlayer.h"

#define FRANKENHEARTS_ACHIEVEMENT_NONE 0
#define FRANKENHEARTS_ACHIEVEMENT_KILLERQUEEN 1
#define FRANKENHEARTS_ACHIEVEMENT_DEFYINGGRAVITY 2
#define FRANKENHEARTS_ACHIEVEMENT_STATISTICALANOMALY 3
#define FRANKENHEARTS_ACHIEVEMENT_MISCALCULATED 4
#define FRANKENHEARTS_ACHIEVEMENT_SMOOTHOPERATOR 5
#define FRANKENHEARTS_ACHIEVEMENT_DEDICATED 6
#define FRANKENHEARTS_ACHIEVEMENTS_THECOLLECTOR 7
#define FRANKENHEARTS_ACHIEVEMENTS_INFINITEGENEROSITY 8
#define FRANKENHEARTS_ACHIEVEMENTS_COMMUNIST 9
#define FRANKENHEARTS_ACHIEVEMENTS_THATSFRANKENHEARTSBABY 10
#define FRANKENHEARTS_ACHIEVEMENTS_ILLDOITMYSELF 11
#define FRANKENHEARTS_ACHIEVEMENT_GOAT 12

/**
*@brief Determines if a card is located within a given vector
*
*@param vcCards The card vector to be checked
*@param sCardID ID of the target card
*
*@return True if the card is found, else false
*/
bool AchievementFindCardInVector(const std::vector<Card> vcCards, std::string sCardID) {
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].sCardID == sCardID) {
            return true;
        }
    }

    return false;
}

/**
*@brief Returns the number of cards of a given suit within a particular card vector
*
*@param vcCards Card vector to count from
*@param iSuitMacro The target suit expressed as a number (according to suit ranking)
*/
int AchievementFindVolumeOfSuitInCardVector(const std::vector<Card> vcCards, int iSuitMacro) {

    int iCount = 0;

    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iSuit == iSuitMacro) {
            iCount++;
        }
    }

    return iCount;
}



bool AchievementKillerQueen(const std::vector<Card>& vcInternalCardsInTrick, Player& pTargetPlayer, const int iTurnCounter) {

    //Achievement given to the player who recieves the Queen of Spades on the second turn

    if (iTurnCounter == 1 && AchievementFindCardInVector(vcInternalCardsInTrick, QUEEN_OF_SPADES)) {

        WG::printvarchar("Player has recieved killer Queen achievement", pTargetPlayer.sNickname.data());
        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_KILLERQUEEN);

        return true;

    } 

    return false;

}

bool AchievementStatiscticalAnomaly(const std::vector<Card>& vcInternalCardsInTrick, Player& pTargetPlayer) {

    if (AchievementFindCardInVector(vcInternalCardsInTrick, "c2H") && AchievementFindVolumeOfSuitInCardVector(vcInternalCardsInTrick, JOKERS_SUIT) == 2) {

        WG::printvarchar("Player has recieved statistical anomaly achievement", pTargetPlayer.sNickname.data());
        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_STATISTICALANOMALY);

        return true;

    }

    return false;

}

bool AchievementTheCollector(const std::vector<Card>& vcInternalCardsInTrick, Player& pTargetPlayer) {

    if (AchievementFindCardInVector(vcInternalCardsInTrick, JACK_OF_DIAMONDS) && AchievementFindCardInVector(vcInternalCardsInTrick, TEN_OF_CLUBS) && AchievementFindCardInVector(vcInternalCardsInTrick, QUEEN_OF_SPADES)) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENTS_THECOLLECTOR);
        return true;

    }

    return false;


}

bool AchievementDetermineLeadDerived(const std::vector<Card>& vcInternalCardsInTrick, Player& pTargetPlayer, const int iTurnCounter) {

    if (!pTargetPlayer.bHuman) {
        return false;
    }

    if (AchievementKillerQueen(vcInternalCardsInTrick, pTargetPlayer, iTurnCounter)) {
        return true;
    }

    if (AchievementStatiscticalAnomaly(vcInternalCardsInTrick, pTargetPlayer)) {
        return true;
    }

    if (AchievementTheCollector(vcInternalCardsInTrick, pTargetPlayer)) {
        return true;
    }

    return false;


}


void AchievementDefyingGravity(Player& pTargetPlayer) {

    //Achievement given to the player who shoots venus

    if (!pTargetPlayer.bHuman) {
        return;
    }

    pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_DEFYINGGRAVITY);

}

bool AchievementMiscalculated(Player& pTargetPlayer) {

    if (AchievementFindCardInVector(pTargetPlayer.vcTakenCards, TEN_OF_CLUBS) && AchievementFindCardInVector(pTargetPlayer.vcTakenCards, QUEEN_OF_SPADES) && AchievementFindVolumeOfSuitInCardVector(pTargetPlayer.vcTakenCards, HEARTS_SUIT) == 12) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_MISCALCULATED);
        return true;

    }

    return false;

}

bool AchievementSmoothOperator(Player& pTargetPlayer) {

    if (!pTargetPlayer.bHuman) {
        return false;
    }

    if (pTargetPlayer.vcTakenCards.size() == 0) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_SMOOTHOPERATOR);
        return true;

    }

    return false;

}

bool AchievementDedicated(Player& pTargetPlayer, const int iRoundNumber) {

    if (iRoundNumber == 50) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_DEDICATED);
        return true;

    }

    return false;

}

bool AchievementThatsFrankenheartsBaby(Player& pTargetPlayer, int iScoreRecieved) {

    if (iScoreRecieved == 50) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENTS_THATSFRANKENHEARTSBABY);
        return true;

    }

    return false;

}

bool AchievementGOAT(Player& pTargetPlayer, std::vector<Player>& vpPlayers) {

    for (int i = 0; i < vpPlayers.size(); i++) {

        if (!vpPlayers[i].bHuman && vpPlayers[i].sNickname == "Maxwell[AI]" && vpPlayers[i].iScore > 99 && pTargetPlayer.iScore < vpPlayers[i].iScore) {
            vpPlayers[i].bBotMessage = true;
            vpPlayers[i].iMessageType = 3;
            pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_GOAT);
            vpPlayers[i].sNickname = "Maxwell[AI] ";
            return true;

        }

    }

    return false;

}

bool AchievementGOATFindPlayers(std::vector<Player>& vpPlayers) {

    for (int i = 0; i < vpPlayers.size(); i++) {

        if (vpPlayers[i].bHuman) {

            AchievementGOAT(vpPlayers[i], vpPlayers);

        }

    }

    return false;

}

bool AchievementScoreDerived(Player& pTargetPlayer, std::vector<Player>& vpPlayers, const int iRoundNumber, const int iScoreRecieved) {

    if (!pTargetPlayer.bHuman) {
        return false;
    }

    if (AchievementThatsFrankenheartsBaby(pTargetPlayer, iScoreRecieved)) {
        return true;
    }

    if (AchievementMiscalculated(pTargetPlayer)) {
        return true;
    }

    if (AchievementDedicated(pTargetPlayer, iRoundNumber)) {
        return true;
    }

    return false;

}

bool AchievementInfiniteGenerosity(Player& pTargetPlayer) {

    if (AchievementFindCardInVector(pTargetPlayer.vcCardsToPass, "cJk")) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENTS_INFINITEGENEROSITY);
        return true;

    }

    return false;

}

bool AchievementCommunist(Player& pTargetPlayer) {

    if (AchievementFindVolumeOfSuitInCardVector(pTargetPlayer.vcCurrentDeck, CLUBS_SUIT) == 0, AchievementFindVolumeOfSuitInCardVector(pTargetPlayer.vcCurrentDeck, SPADES_SUIT) == 0) {

        pTargetPlayer.ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENTS_COMMUNIST);
        return true;

    }

    return false;

}

bool AchievementPassDerived(Player& pTargetPlayer) {

    if (!pTargetPlayer.bHuman) {
        return false;
    }

    if (AchievementInfiniteGenerosity(pTargetPlayer)) {
        return true;
    }

    if (AchievementCommunist(pTargetPlayer)) {
        return true;
    }

    return false;

}

bool AchievementIllDoItMyself(std::vector<Player>& vpPlayers) {

    bool bAchievement = false;

    for (int i = 0; i < vpPlayers.size(); i++) {

        if (!vpPlayers[i].bHuman && vpPlayers[i].sNickname == "Maxwell[AI]") {
            bAchievement = true;
        }

    }

    if (!bAchievement) {
        return false;
    }

    for (int i = 0; i < vpPlayers.size(); i++) {

        if (vpPlayers[i].bHuman) {
            vpPlayers[i].ccClientConnection->WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENTS_ILLDOITMYSELF);
        }

    }

    return true;

}

bool AchievementGameStartDerived(std::vector<Player>& vpPlayers) {

    if (AchievementIllDoItMyself(vpPlayers)) {
        return true;
    }

    return false;

}
