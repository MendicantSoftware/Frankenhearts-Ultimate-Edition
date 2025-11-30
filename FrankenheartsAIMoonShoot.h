#include "FrankenheartsAILinker.h"

#pragma once

#define AI_ABANDONMOONSHOOT 0
#define AI_PRIMEMOONSHOOT 1
#define AI_MOONSHOOT 2


class UnbeatabilitySorter {

    public:

    int iSuitMacro;
    int iValue;

    UnbeatabilitySorter(int SuitMacro, int Value) {
        iSuitMacro = SuitMacro;
        iValue = Value;
    }
};


int MoonDecideIfMoonShooting(Player& pBot, const std::vector<Card>& vcCardsRemaining, const std::vector<Player> vpPlayers) {

    //Check to see if any players others than the current player have points

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("Bot detected it has a joker, abandoning moonshoot");
        return AI_ABANDONMOONSHOOT;
    }

    for (int i = 0; i < vpPlayers.size(); i++) {
        if (BotCountPointsOnly(vpPlayers[i]) > 0 && vpPlayers[i].iPosition != pBot.iPosition) {
            WG::print("Bot detected a player other than itself has a point, abandoning moonshoot");
            pBot.bMoonShooting = false;
            return AI_ABANDONMOONSHOOT;
        }
    }

    if (pBot.bMoonShooting) {
        return AI_MOONSHOOT;
    }

    int iHeartsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, HEARTS_SUIT);

    if (iHeartsUnbeatability > IsolateSuit(pBot.vcCurrentDeck, HEARTS_SUIT).size() / 4 
        && BotDetermineSuitAverage(pBot.vcCurrentDeck, HEARTS_SUIT) < 12
        && allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, HEARTS_SUIT)].iValue < 10) {
        WG::print("Bot is too beatable in hearts.");
        pBot.bMoonShooting = false;
        return 0;
    }

    int iDiamondsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, DIAMONDS_SUIT);
    int iSpadesUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, SPADES_SUIT);
    int iClubsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, CLUBS_SUIT);

    int iUnbeatability = iDiamondsUnbeatability + iSpadesUnbeatability + iClubsUnbeatability;

    WG::printvarint("Bot's unbeatability factor", iUnbeatability);
    WG::printvarint("Bot's unbeatability threshold", vcCardsRemaining.size() / 3);

    if (iUnbeatability == 0) {
        WG::print("Bot cannot lose, it will moonshoot.");
        pBot.bMoonShooting = true;
        return AI_MOONSHOOT;
    }

    if (iUnbeatability < vcCardsRemaining.size() / 3) {
        WG::print("Bot sees an opportunity to moonshoot, if it can prime it");
        return AI_PRIMEMOONSHOOT;
    }

    WG::print("Bot finds no opportunity to moonshoot");
    return AI_ABANDONMOONSHOOT;

}


void SortUnbeatabilitySorter(std::vector<UnbeatabilitySorter>& vusSorter) {

    int iIterator = 0;

    for (int i = 0; i < vusSorter.size(); i++) {
        if (vusSorter[i].iValue == 0) {
            vusSorter[i].iValue = 100;
        }
    }

    std::sort(vusSorter.begin(), vusSorter.end(), [](const UnbeatabilitySorter& a, const UnbeatabilitySorter& b) {
        return a.iValue < b.iValue;
    });

}

bool MoonPrimeInLead(Player& pBot, const std::vector<Card>& vcCardsRemaining) {

    std::vector<UnbeatabilitySorter> vusSorter;
    vusSorter.push_back(UnbeatabilitySorter(DIAMONDS_SUIT, BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, DIAMONDS_SUIT)));
    vusSorter.push_back(UnbeatabilitySorter(SPADES_SUIT, BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, SPADES_SUIT)));
    vusSorter.push_back(UnbeatabilitySorter(CLUBS_SUIT, BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, CLUBS_SUIT)));

    SortUnbeatabilitySorter(vusSorter);

    WG::print("Bot is priming a moon shoot in lead OR void");
    pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, vusSorter[0].iSuitMacro)];

    return true;

}

bool MoonPrimeInSuit(Player& pBot, const std::vector<Card>& vcCardsRemaining, const Card& cLeadCard) {

    if (BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, cLeadCard.iSuit) == 0) {
        WG::print("Bot wants to moonshot, this suit is unbeatable, reverting to typical logic.");
        return false;
    }

    pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, cLeadCard.iSuit)];

    return true;

}

bool MoonPrimeInVoid(Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    return MoonPrimeInLead(pBot, vcCardsRemaining);

}

bool MoonShootInLead(Player& pBot, const GameRule& grGameRule) {
    WG::print("This bot is moonshooting, it is leading its highest card");
    if (grGameRule.bHearts) {
        pBot.cCurrentCard = allCards[pBot.vcCurrentDeck[BotFindHighestValueInCardVectorIndex(pBot.vcCurrentDeck)].sCardID];
    } else {
        pBot.cCurrentCard = allCards[pBot.vcCurrentDeck[BotFindHighestValueInCardVectorIndexExcludingSuit(pBot.vcCurrentDeck, HEARTS_SUIT)].sCardID];
    }
    return true;
}

bool MoonShootInSuit(Player& pBot, const Card& cLeadCard) {
    WG::print("This bot is moonshooting, it is playing its highest card of the given suit");
    pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, cLeadCard.iSuit)];
    return true;
}

bool MoonShootInVoid(Player& pBot) {
    WG::print("This bot is moonshooting, it is playing its lowest card, as it in a void");
    pBot.cCurrentCard = allCards[pBot.vcCurrentDeck[BotFindLowestValueInCardVectorExcludingSuit(pBot.vcCurrentDeck, HEARTS_SUIT)].sCardID];
    return true;
}
