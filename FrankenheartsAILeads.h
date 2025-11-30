#include "FrankenheartsAILinker.h"
#include "FrankenheartsAISpeech.h"

#pragma once

bool CheckSpadesRatio(const Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    /*
    An issue in fishing for spades is the potential for a player to force themselves into taking the QoS
    To avoid this, the bot checks if they have more spades then the maximum amount of possible spade tricks
    If this the case, they will fish for the Queen of Spades

    Of course, if they don't have the Ace or King, they will play a spade.
    */

    if (pBot.vcCurrentDeck[BotFindHighestValueBySuitInCardVectorIndex(pBot.vcCurrentDeck, SPADES_SUIT)].iValue < 12) {
        return true;
    }

    int iSpadesLeftInDeck = FindVolumeOfSuitInCardVector(vcCardsRemaining, SPADES_SUIT);
    int iSpadesLeftInHand = FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, SPADES_SUIT);
    

    if (BotFindCardInVector(pBot.vcCurrentDeck, "cKS")) {
        iSpadesLeftInHand--;
    }

    if (BotFindCardInVector(pBot.vcCurrentDeck, "cAS")) {
        iSpadesLeftInHand--;
    }

    if (BotFindCardInVector(pBot.vcCurrentDeck, QUEEN_OF_SPADES)) {
        iSpadesLeftInHand--;
    }

    if (iSpadesLeftInDeck > iSpadesLeftInHand) {
        return false;
    } else {
        return true;
    }

}

bool FishQueenOfSpades(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {
    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, SPADES_SUIT) > 0
        &&
        !pBot.bMoonShooting
        &&
        BotFindCardInVector(vcCardsRemaining, QUEEN_OF_SPADES)
        &&
        CheckSpadesRatio(pBot, vcCardsRemaining)
        ) {

            //Play their highest spade that is not the Ace of King if Hearts is not broken

            if (!grGameRule.bHearts || FindVolumeOfSuitInCardVector(vcCardsRemaining, HEARTS_SUIT == 0)) {

                std::vector<Card> vcSpadesSansKingAce = IsolateSuit(pBot.vcCurrentDeck, SPADES_SUIT);

                RemoveCardFromCardVector(vcSpadesSansKingAce, "cAS");
                RemoveCardFromCardVector(vcSpadesSansKingAce, "cKS");
    
                pBot.cCurrentCard = allCards[vcSpadesSansKingAce[vcSpadesSansKingAce.size() - 1].sCardID];

                WG::print("Bot is fishing for spades, playing its highest spade");

                return true;

            } else {

                pBot.cCurrentCard = allCards[BotFindClosestLowestCard(pBot, allCards[BotFindLowestCardIDBySuit(vcCardsRemaining, SPADES_SUIT)])];

                WG::print("Bot is fishing for spades, playing its lowest spade");

                return true;

            }

        } else {
            return false;
        }


}

bool ForceQueenOfSpades(Player& pBot, const std::vector<Card>& vcCardsRemaining) {

    if (!pBot.bMoonShooting
        &&
        BotFindCardInVector(pBot.vcCurrentDeck, QUEEN_OF_SPADES)
        &&
        vcCardsRemaining[BotFindLowestValueBySuitInCardVectorIndex(vcCardsRemaining, SPADES_SUIT)].iValue > allCards[QUEEN_OF_SPADES].iValue 
        && 
        BotCheckVoidInVector(vcCardsRemaining, JOKERS_SUIT)) {

        pBot.cCurrentCard = allCards[QUEEN_OF_SPADES];
        WG::print("Bot played the QoS, knowing another player would be forced to take it");
        return true;

    } else {

        return false;

    }

}

bool ForceJackOfDiamonds(Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    
    if (BotFindCardInVector(vcCardsRemaining, JACK_OF_DIAMONDS)
        &&
        vcCardsRemaining[BotFindHighestValueBySuitInCardVectorIndex(vcCardsRemaining, DIAMONDS_SUIT)].sCardID == JACK_OF_DIAMONDS
        &&
        pBot.vcCurrentDeck[BotFindHighestValueBySuitInCardVectorIndex(pBot.vcCurrentDeck, DIAMONDS_SUIT)].iValue > allCards[JACK_OF_DIAMONDS].iValue
        ) {

        pBot.cCurrentCard = allCards[pBot.vcCurrentDeck[BotFindHighestValueBySuitInCardVectorIndex(pBot.vcCurrentDeck, DIAMONDS_SUIT)].sCardID];
        WG::print("Bot is attempting to force Jack of Diamonds");
        return true;

    } else {

        return false;

    }
}

bool LeadJackOfDiamonds(Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    
    if (BotFindCardInVector(pBot.vcCurrentDeck, JACK_OF_DIAMONDS)
        &&
        vcCardsRemaining[BotFindHighestValueBySuitInCardVectorIndex(vcCardsRemaining, DIAMONDS_SUIT)].iValue < allCards[JACK_OF_DIAMONDS].iValue) {
    
    pBot.cCurrentCard = allCards[JACK_OF_DIAMONDS];
    WG::print("Bot is confident it can give itself the Jack of Diamonds");
    return true;

    } else {
        return false;
    } 

}

bool LeadTenOfClubs(Player& pBot, const std::vector<Card>& vcCardsRemaining) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, TEN_OF_CLUBS)) {
        return false;
    }

    if (allCards[BotFindLowestCardIDBySuit(vcCardsRemaining, CLUBS_SUIT)].iValue > allCards[TEN_OF_CLUBS].iValue && FindVolumeOfSuitInCardVector(vcCardsRemaining, CLUBS_SUIT)) {
        WG::print("Bot identified that if it played the ten of clubs another player would be forced to take it");
        pBot.cCurrentCard = allCards[TEN_OF_CLUBS];
        return true;
    }

    return false;

}

bool ClubsLeads(Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    if (LeadTenOfClubs(pBot, vcCardsRemaining)) {
        return true;
    }

    return false;
}

bool SpadesLeads(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {
    if (ForceQueenOfSpades(pBot, vcCardsRemaining)) {
        return true;
    }

    if (FishQueenOfSpades(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }


    return false;
}

bool DiamondsLeads(Player& pBot, const std::vector<Card>& vcCardsRemaining) {
    if (ForceJackOfDiamonds(pBot, vcCardsRemaining)) {
        return true;
    }

    if (LeadJackOfDiamonds(pBot, vcCardsRemaining)) {
        return true;
    }


    return false;
}

bool HardCodedLeads(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {
    if (SpadesLeads(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (DiamondsLeads(pBot, vcCardsRemaining)) {
        return true;
    }

    if (ClubsLeads(pBot, vcCardsRemaining)) {
        return true;
    }

    return false;

}


bool CalculateIfLeadLossPossible(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining, const int iSuitMacro) {

    if (iSuitMacro == SPADES_SUIT && !CheckSpadesRatio(pBot, vcCardsRemaining)) {
        return false;
    }

    if (iSuitMacro == HEARTS_SUIT && !grGameRule.bHearts) {
        return false;
    }

    if(FindVolumeOfSuitInCardVector(vcCardsRemaining, iSuitMacro) > 0 && FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, iSuitMacro) > 0) {

        Card cPotentialCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, iSuitMacro)];

        if (cPotentialCard.iValue < allCards[BotFindLowestCardIDBySuit(vcCardsRemaining, iSuitMacro)].iValue) {

            pBot.cCurrentCard = cPotentialCard;
            return true;

        } else {
            return false;
        }

    }

    return false;

}

bool LoseLead(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    //The bot has a general preference to create voids, it will attempt to lose lead in a near void suit first.

    std::vector<int> viLeadOrder = BotOrderSuitsByVolumeInVector(pBot.vcCurrentDeck);

    for (int i = 0; i < viLeadOrder.size(); i++) {
        if (viLeadOrder[i] == HEARTS_SUIT && grGameRule.bHearts) {

            if (CalculateIfLeadLossPossible(pBot, grGameRule, vcCardsRemaining, viLeadOrder[i])) {
                WG::print("Bot believes it will lose lead");
                return true;
            }

        } else if (CalculateIfLeadLossPossible(pBot, grGameRule, vcCardsRemaining, viLeadOrder[i])) {
            WG::print("Bot believes it will lose lead");
            return true;
        }
    }

    WG::print("Bot cannot see any confirmed lead losses");
    return false;

}

bool ReconcileConfidentQueenOfSpades(Player& pBot, const GameRule& grGameRule) {

    std::vector<Card> vcTemporaryDeck = pBot.vcCurrentDeck;

    RemoveCardFromCardVector(vcTemporaryDeck, QUEEN_OF_SPADES);

    if (FindVolumeOfSuitInCardVector(vcTemporaryDeck, SPADES_SUIT) > 0) {
        pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(vcTemporaryDeck, SPADES_SUIT)];
        WG::print("Bot avoided playing QoS in a confidence based lead");
        return true;
    } else {
        return false;
    }

}

bool ConfidenceBasedLead(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    //Make sure bot can't lead QoS here

    int iStrongestSuit = FindStrongestRelativeSuit(pBot, grGameRule, vcCardsRemaining, false);

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, iStrongestSuit) == 0) {
        return false;
    }

    Card cPotentialCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, iStrongestSuit)];

    if (cPotentialCard.sCardID == QUEEN_OF_SPADES) {
        return ReconcileConfidentQueenOfSpades(pBot, grGameRule);
    }

    if (cPotentialCard.iSuit == SPADES_SUIT && cPotentialCard.iValue > allCards[QUEEN_OF_SPADES].iValue && BotFindCardInVector(vcCardsRemaining, QUEEN_OF_SPADES)) {
        return false;
    }

    pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, iStrongestSuit)];
    WG::printvarint("Bot is leading in its most confident suit", iStrongestSuit);

    return true;

}

bool EvasiveLead(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    /*
    An evasive lead is the Bot's attempt to lose lead when it cannot find a definite suit to lose in, it instead attempts to find a
    suit it could possibly lose in.
    */

    std::vector<int> viLeadOrder = BotOrderSuitsByVolumeInVector(pBot.vcCurrentDeck);
    std::vector<Card> vcMutableCardsRemaining = vcCardsRemaining;

    pBot.cCurrentCard = allCards[NULLCARD];

    int iIterator = 0;

    while (pBot.cCurrentCard.sCardID == NULLCARD) {

        for (int i = 0; i < viLeadOrder.size(); i++) {

            if (viLeadOrder[i] == HEARTS_SUIT && grGameRule.bHearts) {

                if (CalculateIfLeadLossPossible(pBot, grGameRule, vcMutableCardsRemaining, viLeadOrder[i])) {
                    return true;
                }

            } else if (CalculateIfLeadLossPossible(pBot, grGameRule, vcMutableCardsRemaining, viLeadOrder[i])) {
                return true;
            } else if (FindVolumeOfSuitInCardVector(vcMutableCardsRemaining, viLeadOrder[i]) > 0) {
                RemoveCardFromCardVector(vcMutableCardsRemaining, BotFindLowestCardIDBySuit(vcMutableCardsRemaining, viLeadOrder[i]));
            }

            if(vcMutableCardsRemaining.size() == 0 || iIterator == 54) {
                return false;
            }

            iIterator++;

        }

    }

    return false;

}

bool ReconcileQueenOfSpadesLowestCard(Player& pBot, std::vector<Card> vcTemporaryDeck) {

    if (pBot.cCurrentCard.sCardID != QUEEN_OF_SPADES) {
        return false;
    }

    if (vcTemporaryDeck.size() == 1) {
        return false;
    }

    RemoveCardFromCardVector(vcTemporaryDeck, QUEEN_OF_SPADES);

    pBot.cCurrentCard = allCards[vcTemporaryDeck[BotFindLowestValueInCardVectorIndex(vcTemporaryDeck)].sCardID];
    WG::print("Bot was going to lead Queen of Spades as its lowest card, but chose not to");

    return true;

}

bool LeadLowestCard(Player& pBot, const GameRule& grGameRule) {

    std::vector<Card> vcTemporaryDeck = pBot.vcCurrentDeck;

    RemoveSuitFromCardVector(vcTemporaryDeck, JOKERS_SUIT);

    if (!grGameRule.bHearts) {
        RemoveSuitFromCardVector(vcTemporaryDeck, HEARTS_SUIT);
    }

    if (vcTemporaryDeck.size() == 0) {
        return false;
    }

    pBot.cCurrentCard = allCards[vcTemporaryDeck[BotFindLowestValueInCardVectorIndex(vcTemporaryDeck)].sCardID];

    WG::print("Bot played lowest card in its hand");
    
    return true;
    
}

bool SoftCodedLeads(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {
    if (LoseLead(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (ConfidenceBasedLead(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (EvasiveLead(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (LeadLowestCard(pBot, grGameRule)) {
        return true;
    }

    return false;
}
