#include "Frankenhearts.h"
#include "FrankenheartsAILinker.h"
#include "FrankenheartsAILeads.h"
#include "FrankenheartsAIPlays.h"
#include "FrankenheartsAIMoonShoot.h"
#include "FrankenheartsAISpeech.h"

#include <cstdlib>
#include <thread>
#include <chrono> //The bots have lag built into them

#pragma once

void IntentionalLag(bool bLong) {
    if (DEBUG) {

    } else {
        if (bLong) {
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
        }
    }

}


void ResetBot(Player& pBot) {
    pBot.bMoonShooting = false;
    pBot.bQueenOfSpades = false;
    pBot.bJackOfDiamonds = false;
    pBot.bTenOfClubs = false;
}

int ResetManyBots(std::vector<Player>& vpPlayers) {

    int iIterator = 0;

    for (int i = 0; i < vpPlayers.size(); i++) {
        if (!vpPlayers[i].bHuman) {
            ResetBot(vpPlayers[i]);
            iIterator++;
        }
    }

    return iIterator;

}

void AnalyseHand(Player& pBot) {

    //Locate Special Cards
    for (int i = 0; i < pBot.vcCurrentDeck.size(); i++) {

        if (pBot.vcCurrentDeck[i].sCardID == QUEEN_OF_SPADES) {
            pBot.bQueenOfSpades = true;
        }
        if (pBot.vcCurrentDeck[i].sCardID == TEN_OF_CLUBS) {
            pBot.bTenOfClubs = true;
        }
        if (pBot.vcCurrentDeck[i].sCardID == JACK_OF_DIAMONDS) {
            pBot.bJackOfDiamonds = true;
        }

    }

}


std::vector<Card> CountCards(const GameRule& grGameRule, const Player& pBot) {

    Dealer dCardCounter;
    std::vector<Card> vcCardsRemaining = dCardCounter.GiveNewDeck();
    
    RemoveDuplicatesInCardVectors(grGameRule.vcAllCardsInRound, vcCardsRemaining);
    RemoveDuplicatesInCardVectors(pBot.vcCurrentDeck, vcCardsRemaining);

    return vcCardsRemaining;

}

Card DetermineLeadCard(GameRule& grGameRule) {

    int iLeadCard = 0;
    int iLeadSuit = grGameRule.vcInternalCardsInTrick[iLeadCard].iSuit;

    if (iLeadSuit == JOKERS_SUIT) {
        if (grGameRule.vcInternalCardsInTrick.size() == 1) {
            return allCards[NULLCARD];
        }
        if (grGameRule.vcInternalCardsInTrick.size() == 2) {

            if (grGameRule.vcInternalCardsInTrick[1].iSuit == JOKERS_SUIT) {
                return allCards[NULLCARD];
            } else {
                return grGameRule.vcInternalCardsInTrick[1];
            }
        }
    }

    Card cLeadCard = grGameRule.vcInternalCardsInTrick[iLeadCard];

    return cLeadCard;

}

int BotLeadTwoOfClubs(GameRule& grGameRule, Player& pBot) {

    IntentionalLag(true);

    AnalyseHand(pBot);
    
    ResetGraphicsCardsInTrick(grGameRule);
    pBot.cCurrentCard = allCards["c2C"]; 
    PlayerPlayCard(grGameRule, pBot);
    grGameRule.bFirstTrickLead = false;

    return 0;

}

std::string FirstTrickVoidClubs(std::vector<Card> vcSpades, std::vector<Card> vcDiamonds) {

    //IMPROVEMENT: An AI with few spades, and the Queen of Spades shouldn't play a spade in this instance

    if (vcSpades.size() > 0) {
        if (vcSpades[vcSpades.size() - 1].sCardID != QUEEN_OF_SPADES) {
            return vcSpades[vcSpades.size() - 1].sCardID;
        } else {
            if (vcSpades.size() > 1) {
                return vcSpades[vcSpades.size() - 2].sCardID;
            }
        }
    }

    if (vcDiamonds.size() > 0) {
        if (vcDiamonds[vcDiamonds.size() - 1].sCardID != JACK_OF_DIAMONDS) {
            return vcDiamonds[vcDiamonds.size() - 1].sCardID;
        } else {
            if (vcDiamonds.size() > 1) {
                return vcDiamonds[vcDiamonds.size() - 2].sCardID;
            }
        }
    }

    WG::print("First Trick Void Clubs has reached its end, this is not possible");

    return "cAC";

}

int BotFirstTrickPlay(GameRule& grGameRule, Player& pBot) {

    IntentionalLag(false);

    AnalyseHand(pBot);

    std::vector<Card> vcClubs = IsolateSuit(pBot.vcCurrentDeck, CLUBS_SUIT);
    std::vector<Card> vcSpades = IsolateSuit(pBot.vcCurrentDeck, SPADES_SUIT);
    std::vector<Card> vcDiamonds = IsolateSuit(pBot.vcCurrentDeck, DIAMONDS_SUIT);

    if (vcClubs.size() > 0) {
        pBot.cCurrentCard = allCards[vcClubs[FindHighestValueBySuitInCardVectorIndex(pBot.vcCurrentDeck, CLUBS_SUIT)].sCardID];
        WG::printvarchar("Bot is playing card", pBot.cCurrentCard.sCardID.data());
    } else {
        pBot.cCurrentCard = allCards[FirstTrickVoidClubs(vcSpades, vcDiamonds)];
        PlayerPlayCard(grGameRule, pBot);
        WG::printvarchar("Bot is playing card", pBot.cCurrentCard.sCardID.data());
        return 0;
    }

    if (pBot.cCurrentCard.sCardID == TEN_OF_CLUBS) {

        if (vcClubs.size() > 1) {
            int iCardIndex = FindHighestValueBySuitInCardVectorIndex(pBot.vcCurrentDeck, CLUBS_SUIT) - 1;
            pBot.cCurrentCard = allCards[vcClubs[iCardIndex].sCardID];
            PlayerPlayCard(grGameRule, pBot);
            WG::printvarchar("Bot is playing card", pBot.cCurrentCard.sCardID.data());
            return 0;
        } else {
            pBot.cCurrentCard = allCards[FirstTrickVoidClubs(vcSpades, vcDiamonds)];
            PlayerPlayCard(grGameRule, pBot);
            WG::printvarchar("Bot is playing card", pBot.cCurrentCard.sCardID.data());
            return 0;
        }

    } else {
        PlayerPlayCard(grGameRule, pBot);
        return 0;
    }

    WG::print("Bot reached end of BotFristTrickPlay");
    return 1;

}

int BotLeadCard(GameRule& grGameRule, Player& pBot, const std::vector<Player>& vpPlayers) {

    IntentionalLag(true);

    WG::printvarchar("STARTING BOT LEAD", pBot.sNickname.data());

    ResetGraphicsCardsInTrick(grGameRule);
    grGameRule.bTrickLead = false;

    AnalyseHand(pBot);

    std::vector<Card> vcCardsRemaining = CountCards(grGameRule, pBot);

    BotCheckForOnlyHearts(grGameRule, pBot);

    int iMoonShoot = MoonDecideIfMoonShooting(pBot, vcCardsRemaining, vpPlayers);

    if (iMoonShoot == AI_PRIMEMOONSHOOT) {
        if (MoonPrimeInLead(pBot, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }
    }

    if (iMoonShoot == AI_MOONSHOOT) {
        if (MoonShootInLead(pBot, grGameRule)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }
    }

    if (HardCodedLeads(pBot, grGameRule, vcCardsRemaining)) {
        PlayerPlayCard(grGameRule, pBot);
        return 0;
    }

    if (SoftCodedLeads(pBot, grGameRule, vcCardsRemaining)) {
        PlayerPlayCard(grGameRule, pBot);
        return 0;
    }

    WG::print("Bot leading random card");
    pBot.cCurrentCard = pBot.vcCurrentDeck[rand() % pBot.vcCurrentDeck.size()];
    PlayerPlayCard(grGameRule, pBot);
    return 0;

}

int BotPlayCard(GameRule& grGameRule, Player& pBot, const std::vector<Player>& vpPlayers) {

    IntentionalLag(false);

    WG::printvarchar("STARTING BOT TURN", pBot.sNickname.data());

    std::vector<Card> vcCardsRemaining = CountCards(grGameRule, pBot);

    Card cLeadCard = DetermineLeadCard(grGameRule);

    //Handle the Bot having to lead as a consequence of the lead card being a joker

    if (cLeadCard.sCardID == NULLCARD) {

        if (HardCodedLeads(pBot, grGameRule, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }
    
        if (SoftCodedLeads(pBot, grGameRule, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

    }

    std::vector<Card> vcLeadSuit = IsolateSuit(pBot.vcCurrentDeck, cLeadCard.iSuit);

    int iMoonShoot = MoonDecideIfMoonShooting(pBot, vcCardsRemaining, vpPlayers);

    if (vcLeadSuit.size() > 0) {

        //Moon shooting logic

        if (iMoonShoot == AI_PRIMEMOONSHOOT) {
            if (MoonPrimeInSuit(pBot, vcCardsRemaining, cLeadCard)) {
                PlayerPlayCard(grGameRule, pBot);
                return 0;
            }
        }

        if (iMoonShoot == AI_MOONSHOOT) {
            if (MoonShootInSuit(pBot, cLeadCard)) {
                PlayerPlayCard(grGameRule, pBot);
                return 0;
            }
        }

        //All other logic

        if (HardCodedInSuitPlays(pBot, grGameRule, cLeadCard, vpPlayers, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (HardCodedSpadesPlays(pBot, grGameRule, cLeadCard, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (HardCodedClubsPlays(pBot, grGameRule, cLeadCard, vpPlayers, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (HardCodedDiamondsPlays(pBot, grGameRule, cLeadCard, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (HardCodedHeartsPlays(pBot, grGameRule, cLeadCard, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (EvasionPlay(pBot, grGameRule, cLeadCard, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }
    } else {

        //Moon shooting logic

        if (iMoonShoot == AI_PRIMEMOONSHOOT) {
            if (MoonPrimeInVoid(pBot, vcCardsRemaining)) {
                PlayerPlayCard(grGameRule, pBot);
                return 0;
            }
        }

        if (iMoonShoot == AI_MOONSHOOT) {
            if (MoonShootInVoid(pBot)) {
                PlayerPlayCard(grGameRule, pBot);
                return 0;
            }
        }

        if (HardCodedVoidPlays(pBot, grGameRule, vpPlayers, cLeadCard)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        if (SoftCodedVoidPlays(pBot, grGameRule, vcCardsRemaining)) {
            PlayerPlayCard(grGameRule, pBot);
            return 0;
        }

        pBot.cCurrentCard = pBot.vcCurrentDeck[rand() % pBot.vcCurrentDeck.size()];
    }

    //Break Hearts if Hearts is Not Broken
    if (!grGameRule.bHearts && pBot.cCurrentCard.iSuit == HEARTS_SUIT) {
        BreakHearts(grGameRule);
    }

    PlayerPlayCard(grGameRule, pBot);

    return 0;


}

