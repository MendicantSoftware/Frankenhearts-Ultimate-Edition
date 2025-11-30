#include "DeckServer.h"
#include "ServerPlayer.h"
#include "WolfgangDebug.h"
#include "ClientConnection.h"

#include "Hints.h"

#include "FrankenheartsAchievements.h"

#define DEBUG true

#define PASS_LEFT 0
#define PASS_RIGHT 1
#define PASS_NONE 2
#define PASS_WAIT 4


#pragma once


class GameRule {

public:

    //Gamestate
    int bHearts = false;

    bool bFirstTrick = true;
    bool bFirstTrickLead = true;

    bool bTrickLead = false;

    bool bResetTrick = true;
    int iPositionTracker = 0;
    int iLeadDynamic = 0;

    int iTurnCounter = 0;
    int iRoundCounter = 0;

    //Passing
    int iPassType = PASS_NONE;

    //Cards
    int iCurrentLead;
    std::vector<Card> vcGraphicsCardsInTrick = {};
    std::vector<Card> vcInternalCardsInTrick = {};
    std::vector<Card> vcAllCardsInRound = {};

    //Switches
    bool bCommunicateBreak = false;

};

void BreakHearts(GameRule& grGameRule) {
    grGameRule.bHearts = true;
    grGameRule.bCommunicateBreak = true;
}

void UnBreakHearts(GameRule& grGameRule) {
    grGameRule.bHearts = false;
    grGameRule.bCommunicateBreak = true;
}

/**
*@brief Appends one Card type vector to another.
*
*@param vcMutableVector The vector that will be appended to.
*@param vcImmutableVector The vector that will be appended.
*/
void AppendCardVector(std::vector<Card>& vcMutableVector, const std::vector<Card>& vcImmutableVector) {
    for (int i = 0; i < vcImmutableVector.size(); i++) {
        vcMutableVector.push_back(vcImmutableVector[i]);
    }
}

int FindHighestValueBySuitInCardVectorIndex(std::vector<Card> vcCards, int iSuitMacro) {
    int iHighest = 0;
    for (int i = 0; i < vcCards.size(); i++) {
        if (vcCards[i].iValue > vcCards[iHighest].iValue && vcCards[i].iSuit == iSuitMacro) {
            iHighest = i;
        }
    }
    return iHighest;
}

/**
*@brief Determines if a given Card vector has only hearts OR Jokers.
*
*@param vcCards The vector to be checked for only Hearts OR Jokers.
*
*@return true if the vector only has Hearts OR Jokers
*/
bool OnlyHasHearts(std::vector<Card> vcCards) {
    for (int i = 0; i < vcCards.size(); i++) {

        if (vcCards[i].iSuit != HEARTS_SUIT && vcCards[i].iSuit != JOKERS_SUIT) {
            return false;
        }
    }

    return true;
}

void IncrementDynamicLead(GameRule& grGameRule) {
    grGameRule.iLeadDynamic++;
    if (grGameRule.iLeadDynamic > 2) {
        grGameRule.iLeadDynamic = 0;
    }
}

void IncrementPositionTracker(GameRule& grGameRule) {
    grGameRule.iPositionTracker++;
    if (grGameRule.iPositionTracker > 2) {
        grGameRule.iPositionTracker = 0;
    }
}

void IncrementPassPosition(GameRule& grGameRule) {
    grGameRule.iPassType++;
    if (grGameRule.iPassType > PASS_NONE) {
        grGameRule.iPassType = PASS_LEFT;
    }
}

/**
*@brief Transforms a vector of strings to reflect a player's position on the 'table'.
*
*@param iPlayerPosition Player's position on the 'table', held as an int in the Player class.
*@param vsPlayerScoreStrings All score strings, these strings include each player's nickname.
*@return The transformed vector, ready to be sent to the client.
*/
std::string PerformPlayerScoreStringsTransformation(int iPlayerPosition, std::vector<std::string> vsPlayerScoreStrings) {

    std::string sTransformedVector;

    if (iPlayerPosition == 0) {
        sTransformedVector += vsPlayerScoreStrings[0];
        sTransformedVector += vsPlayerScoreStrings[1];
        sTransformedVector += vsPlayerScoreStrings[2];
        return sTransformedVector;
    }

    if (iPlayerPosition == 1) {
        sTransformedVector += vsPlayerScoreStrings[1];
        sTransformedVector += vsPlayerScoreStrings[2];
        sTransformedVector += vsPlayerScoreStrings[0];
        return sTransformedVector;
    }

    if (iPlayerPosition == 2) {
        sTransformedVector += vsPlayerScoreStrings[2];
        sTransformedVector += vsPlayerScoreStrings[0];
        sTransformedVector += vsPlayerScoreStrings[1];
        return sTransformedVector;
    }

    return "ERROR IN: PerformPlayerScoreStringsTransformation";

}

std::vector<Card> PerformCardsInTrickTransformation(int iPlayerPosition, std::vector<Card> vcCardsInTrickGraphics) {

    if (iPlayerPosition == 0) {
        return vcCardsInTrickGraphics;
    }

    if (iPlayerPosition == 1) {
        std::vector<Card> vcTransformedCardsInTrick;
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[1]);
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[2]);
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[0]);
        return vcTransformedCardsInTrick;
    }

    if (iPlayerPosition == 2) {
        std::vector<Card> vcTransformedCardsInTrick;
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[2]);
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[0]);
        vcTransformedCardsInTrick.push_back(vcCardsInTrickGraphics[1]);
        return vcTransformedCardsInTrick;
    }

    return vcCardsInTrickGraphics;


}

void ResetGraphicsCardsInTrick(GameRule& grGameRule) {
    WG::printvarint("TURN COUNTER", grGameRule.iTurnCounter);
    Card cNullCard = allCards["cNULLCARD"];
    grGameRule.vcGraphicsCardsInTrick = {cNullCard, cNullCard, cNullCard};
}

void ResetInternalCardsInTrick(GameRule& grGameRule) {
    grGameRule.vcInternalCardsInTrick = {};
}

void PreventLoopCPUOverload() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void PreventMessageOverlap() {
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
}

void RemoveCardFromHand(Player& pPlayer, Card cTargetCard) {

    for (int i = 0; i < pPlayer.vcCurrentDeck.size(); i++) {
        if (pPlayer.vcCurrentDeck[i].sCardID == cTargetCard.sCardID) {
            pPlayer.vcCurrentDeck.erase(pPlayer.vcCurrentDeck.begin() + i);
        }
    }

}

void GiveAllPlayersNullCard(std::vector<Player>& vpPlayers) {
    for (int i = 0; i < vpPlayers.size(); i++) {
        vpPlayers[i].cCurrentCard = allCards[NULLCARD];
        if (vpPlayers[i].bHuman) {
            vpPlayers[i].ccClientConnection->WriteCurrentCard(allCards[NULLCARD]);
        }
    }
}

void PlayerPlayCard(GameRule& grGameRule, Player& pLeadPlayer) {
    grGameRule.vcInternalCardsInTrick.push_back(pLeadPlayer.cCurrentCard);
    grGameRule.vcGraphicsCardsInTrick[pLeadPlayer.iPosition] = pLeadPlayer.cCurrentCard;
    grGameRule.vcAllCardsInRound.push_back(pLeadPlayer.cCurrentCard);
    RemoveCardFromHand(pLeadPlayer, pLeadPlayer.cCurrentCard);
    if (!grGameRule.bHearts && pLeadPlayer.cCurrentCard.iSuit == HEARTS_SUIT) {
        BreakHearts(grGameRule);
    }
}

void FindTwoOfClubs(GameRule& grGameRule, const std::vector<Player>& vpPlayers) {

    for (int i = 0; i < vpPlayers.size(); i++) {
            if (vpPlayers[i].vcCurrentDeck[0].sCardID == "c2C") {
                grGameRule.iCurrentLead = vpPlayers[i].iPosition;
                return;
            }
    }

    WG::print("The Two Of Clubs Was Not Found!");

}

bool CheckVoid(const Player& pLeadPlayer, const GameRule& grGameRule) {

    for (int i = 0; i < pLeadPlayer.vcCurrentDeck.size(); i++) {

        if (pLeadPlayer.vcCurrentDeck[i].iSuit == grGameRule.vcInternalCardsInTrick[0].iSuit) {
            return false;
        }

    }

    return true;

}

bool CheckFirstHandVoid(const Player& pLeadPlayer) {

    std::vector<Card> vcTemporaryDeck = {};

    for (int i = 0; i < pLeadPlayer.vcCurrentDeck.size(); i++) {
        if (pLeadPlayer.vcCurrentDeck[i].iSuit == CLUBS_SUIT) {
            vcTemporaryDeck.push_back(pLeadPlayer.vcCurrentDeck[i]);
        }
    }

    if (vcTemporaryDeck.size() == 1 && vcTemporaryDeck[0].sCardID == TEN_OF_CLUBS) {
        return true;
    } 

    if (vcTemporaryDeck.size() == 0) {
        return true;
    }

    return false;

}

int LeadTwoOfClubs(GameRule& grGameRule, Player& pLeadPlayer) {

    pLeadPlayer.cCurrentCard = pLeadPlayer.ccClientConnection->ReadCurrentCard();

    pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_FIRSTTRICK_LEAD);

    if (pLeadPlayer.cCurrentCard.sCardID == "c2C") {

        ResetGraphicsCardsInTrick(grGameRule);
        
        grGameRule.vcInternalCardsInTrick.push_back(pLeadPlayer.cCurrentCard);
        grGameRule.vcGraphicsCardsInTrick[pLeadPlayer.iPosition] = pLeadPlayer.cCurrentCard;
        RemoveCardFromHand(pLeadPlayer, pLeadPlayer.cCurrentCard);
        pLeadPlayer.cCurrentCard = allCards[NULLCARD];
        pLeadPlayer.ccClientConnection->WriteCurrentCard(allCards[NULLCARD]);
        grGameRule.bFirstTrickLead = false;

        pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);

        return 0;
    } else {
        return 1;
    }

}

int LeadCard(GameRule& grGameRule, Player& pLeadPlayer) {

    pLeadPlayer.cCurrentCard = pLeadPlayer.ccClientConnection->ReadCurrentCard();

    if (grGameRule.bHearts) {
        pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_LEAD_HEARTSBROKEN);
    } else {
        pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_LEAD_HEARTSUNBROKEN);
    }

    if (pLeadPlayer.cCurrentCard.sCardID != NULLCARD) {

        if (pLeadPlayer.cCurrentCard.iSuit == HEARTS_SUIT && !grGameRule.bHearts) {
            if (OnlyHasHearts(pLeadPlayer.vcCurrentDeck)) {
                BreakHearts(grGameRule);
            } else {
                return 1;
            }
        }

        if (pLeadPlayer.cCurrentCard.iSuit == JOKERS_SUIT) {
            if (pLeadPlayer.vcCurrentDeck[0].iSuit != JOKERS_SUIT) {
                return 1;
            }
        }

        ResetGraphicsCardsInTrick(grGameRule);
        PlayerPlayCard(grGameRule, pLeadPlayer);
        grGameRule.bTrickLead = false;
        
        pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
        return 0;
    } else {
        return 1;
    }


}

int FirstTrickPlay(GameRule& grGameRule, Player& pLeadPlayer) {

    pLeadPlayer.cCurrentCard = pLeadPlayer.ccClientConnection->ReadCurrentCard();

    bool bVoid = CheckVoid(pLeadPlayer, grGameRule);
    bVoid = CheckFirstHandVoid(pLeadPlayer);

    if (pLeadPlayer.cCurrentCard.sCardID == NULLCARD) {

        if (!bVoid) {
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY);
        } else {
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY_VOID);            
        }


        return 1;
    }

    if (!bVoid) {

        WG::print("Player is not void in Suit on first trick");
        if (pLeadPlayer.cCurrentCard.sCardID == "c10C") {
            return 1;
        } else if (pLeadPlayer.cCurrentCard.iSuit == CLUBS_SUIT || pLeadPlayer.cCurrentCard.iSuit == JOKERS_SUIT) {
            PlayerPlayCard(grGameRule, pLeadPlayer);
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
            return 0;
        }

    } else {

        WG::print("Player is void in Suit on first trick");

        if (pLeadPlayer.cCurrentCard.iSuit == HEARTS_SUIT || pLeadPlayer.cCurrentCard.sCardID == "cQS" || pLeadPlayer.cCurrentCard.sCardID == "c10C") {
            return 1;
        } else {
            PlayerPlayCard(grGameRule, pLeadPlayer);
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
            return 0;
        }

    }

    return 1;


}

int TrickPlay(GameRule& grGameRule, Player& pLeadPlayer) {

    pLeadPlayer.cCurrentCard = pLeadPlayer.ccClientConnection->ReadCurrentCard();

    bool bVoid = CheckVoid(pLeadPlayer, grGameRule);

    //Deal with Joker as lead card

    if (pLeadPlayer.cCurrentCard.sCardID == NULLCARD) {

        if (!bVoid) {
            pLeadPlayer.SetCurrentPrompt(grGameRule.vcInternalCardsInTrick[0].iSuit);
        } else {
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_PLAY_VOID);
        }

        return 1;
    }

    if (!bVoid) {

        if (pLeadPlayer.cCurrentCard.iSuit == grGameRule.vcInternalCardsInTrick[0].iSuit || pLeadPlayer.cCurrentCard.iSuit == JOKERS_SUIT) {
            PlayerPlayCard(grGameRule, pLeadPlayer);
            pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
            return 0;
        } else {
            return 1;
        }

    } else {

        if (pLeadPlayer.cCurrentCard.iSuit == HEARTS_SUIT && !grGameRule.bHearts) {
            BreakHearts(grGameRule);
        }

        PlayerPlayCard(grGameRule, pLeadPlayer);
        pLeadPlayer.SetCurrentPrompt(FRANKENHEARTS_PROMPT_EMPTY);
        return 0;

    }

    return 1;

}

void DealNewHand(std::vector<Player>& vpPlayers, Dealer& dDealer) {

    dDealer.NewDeck();

    for (int i = 0; i < vpPlayers.size(); i++) {
        vpPlayers[i].vcCurrentDeck = dDealer.GiveHand();
        dDealer.SortHand(vpPlayers[i].vcCurrentDeck);
    }

}

void DetermineLead(GameRule& grGameRule, std::vector<Player>& vpPlayers) {

    int iLeadCard = 0;
    int iLeadSuit = grGameRule.vcInternalCardsInTrick[iLeadCard].iSuit;

    while (iLeadSuit == JOKERS_SUIT) {
        WG::print("Joker Detected in [0] Position");
        iLeadCard++;
        iLeadSuit = grGameRule.vcInternalCardsInTrick[iLeadCard].iSuit;
    }



    for (int i = 0; i < vpPlayers.size(); i++) {

        if (grGameRule.vcInternalCardsInTrick[vpPlayers[i].iDynamicPosition].iValue == grGameRule.vcInternalCardsInTrick[FindHighestValueBySuitInCardVectorIndex(grGameRule.vcInternalCardsInTrick, iLeadSuit)].iValue 
            && 
            grGameRule.vcInternalCardsInTrick[vpPlayers[i].iDynamicPosition].iSuit == iLeadSuit) {

            grGameRule.iCurrentLead = vpPlayers[i].iPosition;
            grGameRule.iLeadDynamic = vpPlayers[i].iPosition;
            grGameRule.iPositionTracker = 0;

            AchievementDetermineLeadDerived(grGameRule.vcInternalCardsInTrick, vpPlayers[i], grGameRule.iTurnCounter);

            if (vpPlayers[i].bHuman) {
                
            }

            AppendCardVector(vpPlayers[i].vcTakenCards, grGameRule.vcInternalCardsInTrick);
            grGameRule.vcInternalCardsInTrick.clear();
            return;

        }
    }

    grGameRule.iLeadDynamic = grGameRule.iCurrentLead;

}

void ShootVenus(std::vector<Player>& vpPlayers, int iShootingPlayer) {

    for (int i = 0; i < vpPlayers.size(); i++) {
        if (i != iShootingPlayer) {
            vpPlayers[i].iScore += 32;
    
        } else {
            AchievementDefyingGravity(vpPlayers[i]);
        } 

        vpPlayers[i].vcTakenCards.clear();
    }

}

void ShootMoon(std::vector<Player>& vpPlayers, int iShootingPlayer) {

    for (int i = 0; i < vpPlayers.size(); i++) {
        if (i != iShootingPlayer) {
            vpPlayers[i].iScore += 16;
        } 

        vpPlayers[i].vcTakenCards.clear();
    }

}

void DetermineDeveloperBotTaunt(std::vector<Player>& vpPlayers, int iScoreRecieved) {

    bool bMaxwellAI = false;
    int iMaxwellIndex = 0;
    int iHumanScore = 0;

    for (int i = 0; i < vpPlayers.size(); i++) {
        if (vpPlayers[i].sNickname == "Maxwell[AI]") {
            bMaxwellAI = true;
            iMaxwellIndex = i;
        }
    }

    if (bMaxwellAI && iScoreRecieved > 15) {
        vpPlayers[iMaxwellIndex].bBotMessage = true;
        vpPlayers[iMaxwellIndex].iMessageType = 5;
    }
 
}

void CalculateScore(std::vector<Player>& vpPlayers,  const GameRule& grGameRule) {

    int iHumanScore = 0;


    for (int i = 0; i < vpPlayers.size(); i++) {

        int iScore = 0;
        int iReduction = 0;
        bool bTenOfClubs = false;
        bool bQueenOfSpades = false;    

        int iHeartCount = 0;

        for (int x = 0; x < vpPlayers[i].vcTakenCards.size(); x++) {

            if (vpPlayers[i].vcTakenCards[x].iSuit == HEARTS_SUIT) {
                iScore++;
                iHeartCount++;
            }

            if (vpPlayers[i].vcTakenCards[x].sCardID == QUEEN_OF_SPADES) {
                iScore += 13;
                bQueenOfSpades = true;
            }

            if (vpPlayers[i].vcTakenCards[x].sCardID == JACK_OF_DIAMONDS) {
                iReduction = 10;
            }

            if (vpPlayers[i].vcTakenCards[x].sCardID == TEN_OF_CLUBS) {
                iScore = iScore * 2;
                bTenOfClubs = true;
            }

        }

        if (bTenOfClubs) {
            iReduction = iReduction / 2;
        }

        iScore -= iReduction;

        if (iScore < 0) {
            iScore = 0;
        }

        if (iHeartCount == 13 && bQueenOfSpades && bTenOfClubs) {
            WG::printvarchar("Player shot venus", vpPlayers[i].sNickname.data());
            ShootVenus(vpPlayers, i);
            return;
        }

        if (iHeartCount == 13 && bQueenOfSpades) {
            WG::printvarchar("Player shot moon", vpPlayers[i].sNickname.data());
            ShootMoon(vpPlayers, i);
            return;
        }

        vpPlayers[i].iScore += iScore;

        if (!vpPlayers[i].bHuman && iScore == 0) {
            vpPlayers[i].bBotMessage = true;
            vpPlayers[i].iMessageType = 2;
        }

        if (vpPlayers[i].bHuman) {
            iHumanScore = iScore;
        }

        AchievementScoreDerived(vpPlayers[i], vpPlayers, grGameRule.iRoundCounter, iScore);

    }

    for (int i = 0; i < vpPlayers.size(); i++) {

        /*
        Smooth operator used to be given to players, even if someone else shot the moon. Obviously, this wasn't ideal. Unfortunately, it is really difficult to create a flag that
        checks for this. It is just much easier to have the achievement be considered AFTER everyone else has had their points counted.
        */
        AchievementSmoothOperator(vpPlayers[i]);
        vpPlayers[i].vcTakenCards.clear();

    }

    DetermineDeveloperBotTaunt(vpPlayers, iHumanScore);

}

void ResetPassData(std::vector<Player>& vpPlayers) {

    for (int i = 0; i < vpPlayers.size(); i++) {

        vpPlayers[i].vcCardsRecievedInPass.clear();
        vpPlayers[i].vcCardsToPass.clear();
        vpPlayers[i].bPassed = false;

        if (vpPlayers[i].bHuman) {
            vpPlayers[i].ccClientConnection->WriteCurrentPass({});
        }

    }


}

void DistributePass(std::vector<Player>& vpPlayers, int iPassType) {

    Player *pPositionZero = nullptr;
    Player *pPositionOne = nullptr;
    Player *pPositionTwo = nullptr;

    for (int i = 0; i < vpPlayers.size(); i++) {
        
        switch (vpPlayers[i].iPosition) {

            case 0:
            pPositionZero = &vpPlayers[i];
            break;

            case 1:
            pPositionOne = &vpPlayers[i];
            break;

            case 2:
            pPositionTwo = &vpPlayers[i];
            break;

            default:
            WG::printvarchar("Distribute Pass has identified a player with an invalid position", vpPlayers[i].sNickname.data());

        }

    }

    pPositionZero->vcCardsRecievedInPass.clear();
    pPositionOne->vcCardsRecievedInPass.clear();
    pPositionTwo->vcCardsRecievedInPass.clear();

    switch (iPassType) {

        case PASS_LEFT:

        AppendCardVector(pPositionOne->vcCardsRecievedInPass, pPositionZero->vcCardsToPass);
        AppendCardVector(pPositionTwo->vcCardsRecievedInPass, pPositionOne->vcCardsToPass);
        AppendCardVector(pPositionZero->vcCardsRecievedInPass, pPositionTwo->vcCardsToPass);
        
        break;

        case PASS_RIGHT:

        AppendCardVector(pPositionTwo->vcCardsRecievedInPass, pPositionZero->vcCardsToPass);
        AppendCardVector(pPositionOne->vcCardsRecievedInPass, pPositionTwo->vcCardsToPass);
        AppendCardVector(pPositionZero->vcCardsRecievedInPass, pPositionOne->vcCardsToPass);

        break;

        default:
        WG::printvarint("Distribute pass has identified an out of range pass type", iPassType);

    }

    for (int i = 0; i < vpPlayers.size(); i++) {

        for (int x = 0; x < vpPlayers[i].vcCardsToPass.size(); x++) {

            WG::printvarchar("Removing Card From vcCardsToPass", vpPlayers[i].vcCardsToPass[x].sCardID.data());
            RemoveCardFromHand(vpPlayers[i], vpPlayers[i].vcCardsToPass[x]);
            
        }

        WG::printvarint("Size of hand before appending pass", vpPlayers[i].vcCurrentDeck.size());
        AppendCardVector(vpPlayers[i].vcCurrentDeck, vpPlayers[i].vcCardsRecievedInPass);
        WG::printvarint("Size of hand after appending pass", vpPlayers[i].vcCurrentDeck.size());

    }

}

bool CheatDetection(const Player& pPlayer) {

    for (int i = 0; i < pPlayer.vcCurrentDeck.size(); i++) {
        if (pPlayer.cCurrentCard.sCardID == pPlayer.vcCurrentDeck[i].sCardID) {
            return true;
        }
    }

    return false;

}