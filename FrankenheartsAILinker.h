#include "Frankenhearts.h"

#pragma once

/**
*@brief Returns the number of cards of a given suit within a particular card vector
*
*@param vcCards Card vector to count from
*@param iSuitMacro The target suit expressed as a number (according to suit ranking)
*/
int FindVolumeOfSuitInCardVector(const std::vector<Card> vcCards, int iSuitMacro) {

    int iCount = 0;

    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iSuit == iSuitMacro) {
            iCount++;
        }
    }

    return iCount;
}

/**
*@brief Returns the number of points a player has, EXLCUDING the Jack of Diamonds or Ten of Clubs.
*
*@param pTargetPlayer The player that will have their points counted
*/
int BotCountPointsOnly(const Player& pTargetPlayer) {

    int iScore = 0;

    for (int i = 0; i < pTargetPlayer.vcTakenCards.size(); i++) {

        if (pTargetPlayer.vcTakenCards[i].iSuit == HEARTS_SUIT) {
            iScore++;
        }

        if (pTargetPlayer.vcTakenCards[i].sCardID == QUEEN_OF_SPADES) {
            iScore += 13;
        }

    }

    return iScore;

}

/**
*@brief Returns the average value of a given suit within a vector. If the suit is empty, 999 will be returned, discouraging the AI from considering it a viable suit.
*
*@param vcCards Card vector to count from
*@param iTargetSuit The target suit expressed as a number (according to suit ranking)
*/
double BotDetermineSuitAverage(const std::vector<Card>& vcCards, const int iTargetSuit) {

    double dSum = 0;
    int iCardsOfSuit = 0;



    for (int i = 0; i < vcCards.size(); i++) {

        if (vcCards[i].iSuit == iTargetSuit) {
            dSum += vcCards[i].iValue;
            iCardsOfSuit++;
        }

    }

    if (iCardsOfSuit < 1) {
        return 999;
    } else {
        return dSum / iCardsOfSuit;
    }


}

double BotDetermineSuitRatio(const std::vector<Card>& vcCards, const std::vector<Card>& vcReferenceCards, int iSuitMacro) {
    return BotDetermineSuitAverage(vcCards, iSuitMacro) - BotDetermineSuitAverage(vcReferenceCards, iSuitMacro);
}

int RemoveIllegitimateRatio(double& dHeartsRatio, double& dDiamondsRatio, double& dSpadesRatio, double& dClubsRatio, const Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining, const bool bConsiderHearts) {

    int iIterator = 0;

    if (FindVolumeOfSuitInCardVector(vcCardsRemaining, HEARTS_SUIT) == 0 || FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, HEARTS_SUIT) == 0) {
        dHeartsRatio = 100;
        iIterator++;
    }

    if (!grGameRule.bHearts && !bConsiderHearts) {
        dHeartsRatio = 100;
        iIterator++;
    }

    if (FindVolumeOfSuitInCardVector(vcCardsRemaining, DIAMONDS_SUIT) == 0 || FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, DIAMONDS_SUIT) == 0) {
        dDiamondsRatio = 100;
        iIterator++;
    }

    if (FindVolumeOfSuitInCardVector(vcCardsRemaining, SPADES_SUIT) == 0 || FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, SPADES_SUIT) == 0) {
        dSpadesRatio = 100;
        iIterator++;
    }

    if (FindVolumeOfSuitInCardVector(vcCardsRemaining, CLUBS_SUIT) == 0 || FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, CLUBS_SUIT) == 0) {
        dClubsRatio = 100;
        iIterator++;
    }

    return iIterator;

}

/**
*@brief Returns a bots strongest relative suit for the purposes of evasion, it will have (on average), the greatest odds of the bot LOSING the trick.
*
*@param pBot The player being considered
*@param grGameRule A const reference to the current GameRule class
*@param vcCardsRemaining A const reference to the cards remaining in the current hand
*@param bConsiderHearts When false, hearts will be set to the least confident suit if hearts isn't broken, otherwise it can be returned as the best suit, even if hearts is unbroken. This should always be set to false when the bot is leading.
*/
int FindStrongestRelativeSuit(const Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining, const bool bConsiderHearts) {

    double dHeartsRatio = BotDetermineSuitRatio(pBot.vcCurrentDeck, vcCardsRemaining, HEARTS_SUIT);
    double dDiamondsRatio = BotDetermineSuitRatio(pBot.vcCurrentDeck, vcCardsRemaining, DIAMONDS_SUIT);
    double dSpadesRatio = BotDetermineSuitRatio(pBot.vcCurrentDeck, vcCardsRemaining, SPADES_SUIT);
    double dClubsRatio = BotDetermineSuitRatio(pBot.vcCurrentDeck, vcCardsRemaining, CLUBS_SUIT);
    
    RemoveIllegitimateRatio(dHeartsRatio, dDiamondsRatio, dSpadesRatio, dClubsRatio, pBot, grGameRule, vcCardsRemaining, bConsiderHearts);

    std::vector<double> vdRatios = {dClubsRatio, dSpadesRatio, dDiamondsRatio, dHeartsRatio};

    std::vector<double>::iterator vdiMaxIterator = std::min_element(vdRatios.begin(), vdRatios.end());
    int iMostConfidentSuit = std::distance(vdRatios.begin(), vdiMaxIterator) + 1;

    return iMostConfidentSuit;

}


/**
*@brief Removes all card duplicates within two vectors
*
*@param vcImmutablevector Vector that will be compared against, but not edited.
*@param vcMutableVector Vector that will have cards removed from it.
*
*@return An iterator indicating the number of duplicates removed
*/
int RemoveDuplicatesInCardVectors(const std::vector<Card>& vcImmutablevector, std::vector<Card>& vcMutableVector) {
    int iIterator = 0;

    for (int i = 0; i < vcImmutablevector.size(); i++) {
        for (int x = 0; x < vcMutableVector.size(); x++) {
            if (vcMutableVector[x].sCardID == vcImmutablevector[i].sCardID) {
                vcMutableVector.erase(vcMutableVector.begin() + x);
                iIterator++;
            }
        }
    }

    return iIterator;

}


/**
*@brief Removes a specified card from a card vector
*
*@param vcMutableVector Vector that will have a card removed from it
*@param sTargetCardID The cardID that will be removed from the MutableVector
*
*@return Returns 0 if card is deleted, else 1
*/
int RemoveCardFromCardVector(std::vector<Card>& vcMutableVector, std::string sTargetCardID) {

    for (int i = 0; i < vcMutableVector.size(); i++) {
        if (vcMutableVector[i].sCardID == sTargetCardID) {
            vcMutableVector.erase(vcMutableVector.begin() + i);
            return 0;
        }
    }

    return 1;

}

/**
*@brief Removes every card of a given suit from a card vector
*
*@param vcMutableVector Vector that will have cards removed from it
*@param iSuitMacro The suit value to b
*@return An iterator indicating the number of cards removed
*/
int RemoveSuitFromCardVector(std::vector<Card>& vcMutableVector, int iSuitMacro) {

    int iIterator = 0;

    std::vector<Card> vcNewDeck;

    for (int i = 0; i < vcMutableVector.size(); i++) {

        if (vcMutableVector[i].iSuit != iSuitMacro) {
            vcNewDeck.push_back(vcMutableVector[i]);
            iIterator++;
        }

    }

    vcMutableVector = vcNewDeck;

    return iIterator;

}

/**
*@brief Isolates all cards of a given suit within a vector and returns this isolated vector
*
*@param vcCards A constant reference to the hand 
*@param iSuitMacro The suit value to isolated
*@return A card vector containing only cards of the given suit
*/
std::vector<Card> IsolateSuit(const std::vector<Card>& vcCards, int iSuitMacro) {

    std::vector<Card> vcIsolatedSuit;

    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iSuit == iSuitMacro) {
            vcIsolatedSuit.push_back(vcCards[i]);
        }
    }

    return vcIsolatedSuit;

}

/**
*@brief Finds the closest card to another that is also lower than the target card, typically used for evasion plays
*
*@param pBot The bot that is currently playing
*@param cTargetCard The card that the bot is attempting to evade
*
*@return The cardID of the found card
*/
std::string BotFindClosestLowestCard(const Player& pBot, const Card& cTargetCard) {
    int iClosest = 0;

    std::vector<Card> vcTargetSuit = IsolateSuit(pBot.vcCurrentDeck, cTargetCard.iSuit);

    for (int i = 0; i < vcTargetSuit.size(); i++) {

        if (vcTargetSuit[i].iValue < cTargetCard.iValue) {
            iClosest = i;
        } else {
            return vcTargetSuit[iClosest].sCardID;
        }

    }

    return vcTargetSuit[iClosest].sCardID;

}

/**
*@brief Finds the closest card to another that is also lower than the target card, typically used for evasion plays
*
*@param pBot The bot that is currently playing
*@param cTargetCard The card that the bot is attempting to evade
*
*@return The cardID of the found card
*/
std::string BotFindClosestLowestCardInVector(const std::vector<Card>& vcImmutableVector, const Card& cTargetCard) {
    int iClosest = 0;

    std::vector<Card> vcTargetSuit = IsolateSuit(vcImmutableVector, cTargetCard.iSuit);

    for (int i = 0; i < vcTargetSuit.size(); i++) {

        if (vcTargetSuit[i].iValue < cTargetCard.iValue) {
            iClosest = i;
        } else {
            return vcTargetSuit[iClosest].sCardID;
        }

    }

    return vcTargetSuit[iClosest].sCardID;

}

/**
*@brief Determines if a card is located within a given vector
*
*@param vcCards The card vector to be checked
*@param sCardID ID of the target card
*
*@return True if the card is found, else false
*/
bool BotFindCardInVector(const std::vector<Card> vcCards, std::string sCardID) {
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].sCardID == sCardID) {
            return true;
        }
    }

    return false;
}


int BotFindHighestValueInCardVectorIndexExcludingSuit(std::vector<Card> vcCards, int iSuitMacro) {
    int iHighest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue > vcCards[iHighest].iValue && vcCards[i].iSuit != iSuitMacro) {
            iHighest = i;
        }
    }
    return iHighest;
}


int BotFindHighestValueInCardVectorIndex(std::vector<Card> vcCards) {
    int iHighest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue > vcCards[iHighest].iValue) {
            iHighest = i;
        }
    }
    return iHighest;
}

int BotFindHighestValueBySuitInCardVectorIndex(std::vector<Card> vcCards, int iSuitMacro) {
    int iHighest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue > vcCards[iHighest].iValue && vcCards[i].iSuit == iSuitMacro) {
            iHighest = i;
        }
    }
    return iHighest;
}

//Returns NULLCARD on failure
std::string BotFindHighestCardIDBySuit(const std::vector<Card>& vcCards, int iSuitMacro) {

    std::vector<Card> vcTargetSuit = IsolateSuit(vcCards, iSuitMacro);

    if (vcTargetSuit.size() == 0) {
        return NULLCARD;
    }

    int iHighest = 0;

    for (int i = 0; i < vcTargetSuit.size(); i++) {

        if (vcTargetSuit[i].iValue > vcTargetSuit[iHighest].iValue) {
            iHighest = i;
        } 

    }

    return vcTargetSuit[iHighest].sCardID;

}

int BotFindLowestValueInCardVectorExcludingSuit(std::vector<Card> vcCards, int iSuitMacro) {
    int iLowest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue < vcCards[iLowest].iValue && vcCards[i].iSuit != iSuitMacro) {
            iLowest = i;
        }
    }
    return iLowest;
}

int BotFindLowestValueInCardVectorIndex(std::vector<Card> vcCards) {
    int iLowest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue < vcCards[iLowest].iValue) {
            iLowest = i;
        }
    }
    return iLowest;
}

//I am deprecated
int BotFindLowestValueBySuitInCardVectorIndex(std::vector<Card> vcCards, int iSuitMacro) {

    PrintWarn("Call to deprecated function 1");

    int iLowest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue < vcCards[iLowest].iValue && vcCards[i].iSuit == iSuitMacro) {
            iLowest = i;
        }
    }
    return iLowest;
}

std::string BotFindLowestCardIDBySuit(const std::vector<Card>& vcCards, int iSuitMacro) {

    std::vector<Card> vcTargetSuit = IsolateSuit(vcCards, iSuitMacro);

    if (vcTargetSuit.size() == 0) {
        return NULLCARD;
    }

    int iLowest = 0;

    for (int i = 0; i < vcTargetSuit.size(); i++) {

        if (vcTargetSuit[i].iValue < vcTargetSuit[iLowest].iValue) {
            iLowest = i;
        } 

    }

    return vcTargetSuit[iLowest].sCardID;

}

std::string BotFindLowestCardID(const std::vector<Card>& vcCards) {

    if (vcCards.size() == 0) {
        return NULLCARD;
    }

    int iLowest = 0;

    for (int i = 0; i < vcCards.size(); i++) {

        if (vcCards[i].iValue < vcCards[iLowest].iValue) {
            iLowest = i;
        } 

    }

    return vcCards[iLowest].sCardID;

}

bool BotCheckForOnlyHearts(GameRule& grGameRule, const Player& pBot) {


    for (int i = 0; i < pBot.vcCurrentDeck.size(); i++) {
        if (pBot.vcCurrentDeck[i].iSuit != HEARTS_SUIT && pBot.vcCurrentDeck[i].iSuit != JOKERS_SUIT) {
            return false;
        }
    }

    if (!grGameRule.bHearts) {
        BreakHearts(grGameRule);
    }

    return true;

}

bool BotCheckVoidInVector(const std::vector<Card> vcVector, const int iSuitMacro) {

    for (int i = 0; i < vcVector.size(); i++) {

        if (vcVector[i].iSuit == iSuitMacro) {
            return false;
        }

    }

    return true;


}

bool BotCheckVoid(const Player& pLeadPlayer, const GameRule& grGameRule) {

    for (int i = 0; i < pLeadPlayer.vcCurrentDeck.size(); i++) {

        if (pLeadPlayer.vcCurrentDeck[i].iSuit == grGameRule.vcInternalCardsInTrick[0].iSuit) {
            return false;
        }

    }

    return true;

}

class SuitVolume {

    public:

    int iSuit;
    int iVolume;

    SuitVolume(int Suit, const std::vector<Card>& vcCards) {
        iSuit = Suit;
        iVolume = FindVolumeOfSuitInCardVector(vcCards, iSuit);

        if (iVolume == 0) {
            iVolume = 100;
        }
        
    }
};

std::vector<int> BotOrderSuitsByVolumeInVector(const std::vector<Card>& vcCards) {

    std::vector<SuitVolume> vsvSuitVolumes = {};

    vsvSuitVolumes.push_back(SuitVolume(HEARTS_SUIT, vcCards));
    vsvSuitVolumes.push_back(SuitVolume(DIAMONDS_SUIT, vcCards));
    vsvSuitVolumes.push_back(SuitVolume(CLUBS_SUIT, vcCards));
    vsvSuitVolumes.push_back(SuitVolume(SPADES_SUIT, vcCards));


    std::sort(vsvSuitVolumes.begin(), vsvSuitVolumes.end(), [](const SuitVolume& a, const SuitVolume& b) {
        return a.iVolume < b.iVolume;
    });

    std::vector<int> viCardMacros = {};

    for (int i = 0; i < vsvSuitVolumes.size(); i++) {

        viCardMacros.push_back(vsvSuitVolumes[i].iSuit);

    }

    return viCardMacros;

}

Player BotPredictWinnerOfCurrentTrick(const GameRule& grGameRule, const std::vector<Player>& vpPlayers, const Card& cLeadCard) {

    const Card cWinningCard = allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)];

    for (int i = 0; i < grGameRule.vcInternalCardsInTrick.size(); i++) {
        if (vpPlayers[i].cCurrentCard.sCardID == cWinningCard.sCardID) {
            return vpPlayers[i];
        } 
    }

    WG::print("Bot predict winner of trick has reached end of control, which is not possible");
    return vpPlayers[0];

}

int BotIdentifyUnbeatableSuit(const Player& pBot, const std::vector<Card>& vcCardsRemaining, const int iSuitMacro) {

    std::vector<Card> vcBotCardsInSuit = IsolateSuit(pBot.vcCurrentDeck, iSuitMacro);
    std::vector<Card> vcCardsRemainingInSuit = IsolateSuit(vcCardsRemaining, iSuitMacro);

    for (int i = 0; i < vcBotCardsInSuit.size(); i++) {

        //If the bot has no cards of a given suit OR there are no cards of the suit reminaing, obviously the suit is unbeatable.
        if (vcBotCardsInSuit.size() == 0 || vcCardsRemainingInSuit.size() == 0) {
            return 0;
        }

        //Check if the bot's highest card is higher than the current highest card in the suit, if it is remove the highest card in bot hand, then lowest in the remaining cards
        //If the bot's highest card is lower, return the amount of cards remaining in the hand that AREN'T unbeatable.
        if (allCards[BotFindHighestCardIDBySuit(vcBotCardsInSuit, iSuitMacro)].iValue > allCards[BotFindHighestCardIDBySuit(vcCardsRemainingInSuit, iSuitMacro)].iValue) {

            RemoveCardFromCardVector(vcBotCardsInSuit, BotFindHighestCardIDBySuit(vcBotCardsInSuit, iSuitMacro));
            RemoveCardFromCardVector(vcCardsRemainingInSuit, BotFindLowestCardIDBySuit(vcCardsRemainingInSuit, iSuitMacro));

        } else {
            return vcBotCardsInSuit.size();
        }

    }

    return 0;

}

bool BotIdentifyMoonShoot(const std::vector<Player>& vpPlayers, const Player& pBot) {

    //A bot who is the only one with points should continue playing as if there is no moonshoot. If another bot has points, then there is no moonshoot.
    if (BotCountPointsOnly(pBot) > 0) {
        return false;
    }

    int iPlayersWithPoints = 0;

    for (int i = 0; i < vpPlayers.size(); i++) {
        int iPoints = BotCountPointsOnly(vpPlayers[i]);
        if (iPoints > 0) {
            iPlayersWithPoints++;
        }
    }

    if (iPlayersWithPoints > 1) {
        return false;
    } else {
        return true;
    }

}

int BotIdentifyMoonShooterScore(const std::vector<Player>& vpPlayers, const Player& pBot) {

    if (!BotIdentifyMoonShoot(vpPlayers, pBot)) {
        return 0;
    }

    for (int i = 0; i < vpPlayers.size(); i++) {

        int iPoints = BotCountPointsOnly(vpPlayers[i]);
        if (iPoints > 0) {
            return iPoints;
        }

    }

    return 0;

}
