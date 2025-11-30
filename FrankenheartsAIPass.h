#include "FrankenheartsAILinker.h"
#include "FrankenheartsAIMoonShoot.h"

#pragma once

class SuitConfidence {

    public:
    
    int iSuit;  
    double dAverage;

    SuitConfidence(const Player& pBot, int iTargetSuit) {

        iSuit = iTargetSuit;
        dAverage = BotDetermineSuitAverage(pBot.vcCurrentDeck, iTargetSuit);

    }

};

void RemoveIllegitmateConfidentSuits(std::vector<SuitConfidence>& vscSuitConfidence) {
    for (int i = 0; i < vscSuitConfidence.size(); i++) {
        if (vscSuitConfidence[i].dAverage > 14) {
            vscSuitConfidence[i].dAverage = -1;
        }
    }
}

int FindLeastConfidentSuit(const Player& pBot) {

    std::vector<SuitConfidence> vscSuitsConfidence;

    vscSuitsConfidence.push_back(SuitConfidence(pBot, HEARTS_SUIT));
    vscSuitsConfidence.push_back(SuitConfidence(pBot, DIAMONDS_SUIT));
    vscSuitsConfidence.push_back(SuitConfidence(pBot, SPADES_SUIT));
    vscSuitsConfidence.push_back(SuitConfidence(pBot, CLUBS_SUIT));

    RemoveIllegitmateConfidentSuits(vscSuitsConfidence);

    std::sort(vscSuitsConfidence.begin(), vscSuitsConfidence.end(), [](const SuitConfidence& a, const SuitConfidence& b) {
        return a.dAverage > b.dAverage;
    });

    return vscSuitsConfidence[0].iSuit;

}

bool ConfidenceBasedPass(Player& pBot) {

    Card cCardToPass = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, FindLeastConfidentSuit(pBot))];

    pBot.vcCardsToPass.push_back(cCardToPass);
    RemoveCardFromCardVector(pBot.vcCurrentDeck, cCardToPass.sCardID);
    WG::print("Bot is passing card with lowest confidence");
    return true;

}

bool FindIfSuitVoidableInPass(Player& pBot, const int iSuitMacro, int iLeftToPass) {

    if (iLeftToPass - FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, iSuitMacro) >= 0 && FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, iSuitMacro) != 0) {
        AppendCardVector(pBot.vcCardsToPass, IsolateSuit(pBot.vcCurrentDeck, iSuitMacro));

        for (int i = 0; i < pBot.vcCardsToPass.size(); i++) {
            RemoveCardFromCardVector(pBot.vcCurrentDeck, pBot.vcCardsToPass[i].sCardID);
        }

        WG::printvarint("Bot is voiding a suit in pass", iSuitMacro);
        return true;
    }

    return false;

}

bool MoonShootPass(Player& pBot) {

    if (pBot.vcCardsToPass.size() > 0) {
        return false;
    }

    Dealer dTemporaryDealer;

    int iMoonShootPotential = MoonDecideIfMoonShooting(pBot, dTemporaryDealer.GiveNewDeck(), {});

    if (iMoonShootPotential == AI_PRIMEMOONSHOOT) {
        for (int i = 0; i < 4; i++) {
            pBot.vcCardsToPass.push_back(allCards[BotFindLowestCardID(pBot.vcCurrentDeck)]);
            RemoveCardFromCardVector(pBot.vcCurrentDeck, BotFindLowestCardID(pBot.vcCurrentDeck));
        }
        WG::print("Bot's pass is an attempt to prime a moonshoot!");
        return true;
    }

    return false;


}

bool VoidEntireSuit(Player& pBot) {

    int iLeftToPass = 4 - pBot.vcCardsToPass.size();

    if (FindIfSuitVoidableInPass(pBot, CLUBS_SUIT, iLeftToPass)) {
        return true;
    }

    if (FindIfSuitVoidableInPass(pBot, DIAMONDS_SUIT, iLeftToPass)) {
        return true;
    }

    if (FindIfSuitVoidableInPass(pBot, HEARTS_SUIT, iLeftToPass)) {
        return true;
    }

    return false;

}

bool SoftCodedPasses(Player& pBot) {
    if (MoonShootPass(pBot)) {
        return true;
    }

    if (VoidEntireSuit(pBot)) {
        return true;
    }

    if (ConfidenceBasedPass(pBot)) {
        return true;
    }

    return false;

}

bool BotPassCards(Player& pBot) {

    while (pBot.vcCardsToPass.size() != 4) {
        SoftCodedPasses(pBot);
    }

    WG::printvarchar("Bot is passing", pBot.sNickname.data());

    for (int i = 0; i < pBot.vcCardsToPass.size(); i++) {
        WG::printvarchar("", pBot.vcCardsToPass[i].sCardID.data()); 
    }

    return true;

}