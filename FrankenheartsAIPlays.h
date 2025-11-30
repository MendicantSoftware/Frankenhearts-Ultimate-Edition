#include "FrankenheartsAILinker.h"
#include "FrankenheartsAISpeech.h"

//Helper functions

bool CertainLossOfTrick(Player& pBot, const Card& cTargetCard) {

    Card cBotBestCard = allCards[BotFindClosestLowestCard(pBot, cTargetCard)];

    if (cTargetCard.iValue > cBotBestCard.iValue) {
        WG::print("Bot is certain it will lose this trick");
        pBot.cCurrentCard = cBotBestCard;
        return true;
    } else {
        return false;
    }

}
 
bool AttemptLossOfTrick(Player& pBot, const Card& cTargetCard) {

    pBot.cCurrentCard = allCards[BotFindClosestLowestCard(pBot, cTargetCard)];
    return true;

}

bool CertainWinTrick(Player& pBot, const Card& cTargetCard) {

    Card cBotBestCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, cTargetCard.iSuit)];

    if (cTargetCard.iValue < cBotBestCard.iValue) {
        WG::print("Bot is certain it will win this trick");
        pBot.cCurrentCard = cBotBestCard;
        return true;
    } else {
        return false;
    }

}

bool AttemptWinTrick(Player& pBot, const Card& cTargetCard) {

    pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, cTargetCard.iSuit)];
    return true;

}

//Hard Coded Plays

//All Suits

bool AvoidUnbeatableLead(Player& pBot, const std::vector<Card>& vcCardsRemaining) {

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) == 0) {
        WG::print("Avoid unbeatable lead: No jokers detected");
        return false;
    }

    //Determine if bot is unbeatable

    int iClubsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, CLUBS_SUIT);
    int iSpadesUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, SPADES_SUIT);
    int iDiamondsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, DIAMONDS_SUIT);
    int iHeartsUnbeatability = BotIdentifyUnbeatableSuit(pBot, vcCardsRemaining, HEARTS_SUIT);

    int iUnbeatability = iClubsUnbeatability + iSpadesUnbeatability + iDiamondsUnbeatability + iHeartsUnbeatability;

    WG::printvarint("Avoid unbeatable lead: Bot is not unbeatable", iUnbeatability);

    if (iUnbeatability == 0) {

        WG::print("Bot wants to avoid lead as a consequence of being unbeatable.");
        pBot.cCurrentCard = allCards["cJk"];
        return true;

    }

    return false;



}

bool PreventMoonShootingInTrickWithPoints(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Player>& vpPlayers, const std::vector<Card>& vcCardsRemaining) {

    if (!grGameRule.bHearts || vcCardsRemaining.size() > 32 || FindVolumeOfSuitInCardVector(grGameRule.vcInternalCardsInTrick, HEARTS_SUIT) == 0 || !BotIdentifyMoonShoot(vpPlayers, pBot)) {
        return false;
    }

    if (CertainWinTrick(pBot, allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)])) {
        WG::print("Bot detected another player may be shooting the moon and has intentionally taken a point");
        return true;
    } else if (cLeadCard.iSuit == HEARTS_SUIT) {
        WG::print("Bot has detected another player may be shooting the moon, avoiding playing high hearts");
        pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, cLeadCard.iSuit)];
        return true;
    }

    WG::print("Bot wants to take a point to prevent a moonshoot but cannot do it");
    return false;

}

bool PreventMoonShootingInSuit(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Player>& vpPlayers, const std::vector<Card>& vcCardsRemaining) {

    if (!grGameRule.bHearts || !BotIdentifyMoonShoot(vpPlayers, pBot)) {
        return false;
    }


    if (BotIdentifyMoonShooterScore(vpPlayers, pBot) < 6) {
        WG::print("Bot identified only one player with points, but determined they are most likely not moonshooting");
        return false;
    }

    if (CertainWinTrick(pBot, allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)])) {
        WG::print("Bot detected another player may be shooting the moon and has intentionally taken lead");
        return true;
    } else {
        WG::print("Bot is playing its lowest card to make it harder for an opponent to moonshot");
        pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, cLeadCard.iSuit)];
        return true;
    }


    return false;

}

bool AvoidTenOfClubs(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard) {

    if (!BotFindCardInVector(grGameRule.vcInternalCardsInTrick, TEN_OF_CLUBS) || pBot.bMoonShooting) {
        return false;
    }

    //Check if bot cares about getting the Ten Of Clubs

    if (BotCountPointsOnly(pBot) == 0 || !BotFindCardInVector(pBot.vcTakenCards, JACK_OF_DIAMONDS) || !BotFindCardInVector(pBot.vcCurrentDeck, JACK_OF_DIAMONDS)) {
        WG::print("Bot detected the Ten of Clubs in this hand, but determined it wasn't worth avoiding");
        return false;
    }

    Card cCardToBeat = allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)];

    if (CertainLossOfTrick(pBot, cCardToBeat)) {
        WG::print("Bot detected the Ten of Clubs, and avoided it");
        return true;
    }

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("Bot detected the Ten of Clubs and is avoiding it with a Joker");
        std::vector<Card> vcJokerSuit = IsolateSuit(pBot.vcCurrentDeck, JOKERS_SUIT);
        pBot.cCurrentCard = allCards[vcJokerSuit[0].sCardID];
        return true;
    }

    //Attempt to lose

    AttemptLossOfTrick(pBot, cCardToBeat);

    return true;


}

bool AvoidQueenOfSpades(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard) {

    if (!BotFindCardInVector(grGameRule.vcInternalCardsInTrick, QUEEN_OF_SPADES) || pBot.bMoonShooting) {
        return false;
    }

    //Check if bot has a card lower than any other card in the trick

    WG::print("Bot has noticed Queen of Spades in this trick");

    Card cCardToBeat = allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)];
    Card cBotBestCard = allCards[BotFindClosestLowestCard(pBot, cCardToBeat)];

    if (CertainLossOfTrick(pBot, cCardToBeat)) {
        return true;
    }

    //Check if bot has a joker

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("It will avoid the Queen using a Joker");
        std::vector<Card> vcJokerSuit = IsolateSuit(pBot.vcCurrentDeck, JOKERS_SUIT);
        pBot.cCurrentCard = allCards[vcJokerSuit[0].sCardID];
        return true;
    }

    //Attempt to lose

    AttemptLossOfTrick(pBot, cCardToBeat);

    return true;

}

//Spades

bool AvoidGivingSelfQueenOfSpades(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, QUEEN_OF_SPADES) || BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT) != QUEEN_OF_SPADES) {
        return false;
    }

    WG::printvarchar("The Lowest Card in The Bot's Spades Suit", BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT).data());

    if (allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT)].iValue > allCards[QUEEN_OF_SPADES].iValue) {
        WG::print("Bot is attempting to avoid giving itself the Queen of Spades by playing a higher card");
        pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT)];
        return true;
    }

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("Bot is playing a Joker in an attempt to avoid giving itself the Queen of Spades");
        pBot.cCurrentCard = allCards[JOKER];
        return true;
    }

    return false;

}

bool GiveQueenOfSpadesInSpades(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, QUEEN_OF_SPADES)) {
        return false;
    }

    //Check if there is a card in the trick highest than the Queen of Spades

    const Card cHighestSpadeInTrick = grGameRule.vcInternalCardsInTrick[FindHighestValueBySuitInCardVectorIndex(grGameRule.vcInternalCardsInTrick, SPADES_SUIT)];

    if (cHighestSpadeInTrick.iValue > allCards[QUEEN_OF_SPADES].iValue) {
        WG::print("Bot detects they can give QoS to an opponent because they played a card above QoS value");
        pBot.cCurrentCard = allCards[QUEEN_OF_SPADES];
        return true;
    }

    //Check vcCardsRemaining to see if a QoS play would force the next player to take the QoS

    if (grGameRule.vcInternalCardsInTrick.size() == 1) {

        if (allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, SPADES_SUIT)].iValue > allCards[QUEEN_OF_SPADES].iValue && FindVolumeOfSuitInCardVector(vcCardsRemaining, JOKERS_SUIT) == 0) {
            WG::print("Bot believes that if it plays the Queen of Spades an opponent will be forced to take it");
            pBot.cCurrentCard = allCards[QUEEN_OF_SPADES];
            return true;
        }

    }

    /*
    The bot's strategy is typically modular, but AvoidGivingSelfQueenOfSpades has a massive amount of overlap with this function, it makes more sense to bundle
    them here than it does to remember to call one everytime the other is called.
    */

    return AvoidGivingSelfQueenOfSpades(pBot, grGameRule, vcCardsRemaining);

}

bool PlayHighSpade(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (grGameRule.vcInternalCardsInTrick.size() != 2 || BotFindCardInVector(grGameRule.vcInternalCardsInTrick, QUEEN_OF_SPADES) || FindVolumeOfSuitInCardVector(grGameRule.vcInternalCardsInTrick, HEARTS_SUIT) > 0) {
        return false;
    }

    if (allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT)].iValue > allCards[QUEEN_OF_SPADES].iValue) {
        WG::print("Bot identified opportunity to play the King or Ace of Spades");
        pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT)];
        return true;
    }

    return false;

}


bool HardCodedInSuitPlays(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Player> vpPlayers, const std::vector<Card>& vcCardsRemaining) {

    if (AvoidUnbeatableLead(pBot, vcCardsRemaining)) {
        return true;
    }

    if (AvoidQueenOfSpades(pBot, grGameRule, cLeadCard)) {
        return true;
    }

    if (PreventMoonShootingInTrickWithPoints(pBot, grGameRule, cLeadCard, vpPlayers, vcCardsRemaining)) {
        return true;
    }

    if (PreventMoonShootingInSuit(pBot, grGameRule, cLeadCard, vpPlayers, vcCardsRemaining)) {
        return true;
    }

    return false;

}

bool HardCodedSpadesPlays(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsRemaining) {

    if (cLeadCard.iSuit != SPADES_SUIT) {
        return false;
    }

    if (GiveQueenOfSpadesInSpades(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }
    if (AvoidQueenOfSpades(pBot, grGameRule, cLeadCard)) {
        return true;
    }
    if (PlayHighSpade(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }



    return false;

}

//Clubs

bool GiveTenOfClubs(Player& pBot, const GameRule& grGameRule, const std::vector<Player>& vpPlayers, const Card& cLeadCard) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, TEN_OF_CLUBS) || allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)].iValue < allCards[TEN_OF_CLUBS].iValue) {
        return false;
    }

    if (BotCountPointsOnly(BotPredictWinnerOfCurrentTrick(grGameRule, vpPlayers, cLeadCard)) > 8) {
        pBot.cCurrentCard = allCards[TEN_OF_CLUBS];
        WG::print("Bot detected an opportunity to the Ten of Clubs in suit, and found an opponent with a score worth targetting.");
        return true;
    }


    return false;
}

bool AvoidGivingSelfTenOfClubs(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, TEN_OF_CLUBS) || BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, CLUBS_SUIT) != TEN_OF_CLUBS || BotCountPointsOnly(pBot) < 2) {
        return false;
    }

    if (allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, CLUBS_SUIT)].iValue > allCards[TEN_OF_CLUBS].iValue) {
        WG::print("Bot is attempting to avoid giving itself the Ten of Clubs by playing a higher card");
        pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, CLUBS_SUIT)]; 
    }

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("Bot is playing a Joker in an attempt to avoid giving itself the Ten of Clubs");
        pBot.cCurrentCard = allCards[JOKER];
        return true;
    }

    return false;
    
}

bool HardCodedClubsPlays(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Player>& vpPlayers, const std::vector<Card>& vcCardsRemaining) {

    if (cLeadCard.iSuit != CLUBS_SUIT) {
        return false;
    }

    if (AvoidGivingSelfTenOfClubs(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (GiveTenOfClubs(pBot, grGameRule, vpPlayers, cLeadCard)) {
        return true;
    }

    return false;
}


//Diamonds

bool SnatchJackOfDiamonds(Player& pBot, const GameRule& grGameRule, const Card cLeadCard, const std::vector<Card>& vcCardsRemaining) {

    if (!BotFindCardInVector(grGameRule.vcInternalCardsInTrick, JACK_OF_DIAMONDS) || BotFindCardInVector(grGameRule.vcInternalCardsInTrick, QUEEN_OF_SPADES)) {
        return false;
    }

    WG::print("Bot has detected the Jack of Diamonds in a diamonds trick");

    if (CertainWinTrick(pBot, cLeadCard)) {
        WG::print("Bot is attempting to steal Jack of Diamonds");
        return true;
    } else {
        AttemptWinTrick(pBot, cLeadCard);
        return true;
    }

    return false;

}

bool StealJackOfDiamonds(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (BotFindCardInVector(grGameRule.vcInternalCardsInTrick, QUEEN_OF_SPADES)) {
        return false;
    }

    // Bot checks if it can force the owner of the Jack of Diamonds to play the Jack of Diamonds, OR force them to waste a Joker

    if (BotFindCardInVector(vcCardsRemaining, JACK_OF_DIAMONDS) 
        && 
        FindVolumeOfSuitInCardVector(vcCardsRemaining, DIAMONDS_SUIT) == 1
        &&
        allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, DIAMONDS_SUIT)].iValue > allCards[JACK_OF_DIAMONDS].iValue) {

            if (CertainWinTrick(pBot, allCards[JACK_OF_DIAMONDS])) {
                WG::print("Bot is certain that the next card will be the Jack of Diamonds, they have played a card higher than it in anticipation");
                return true;
            } else {
                AttemptWinTrick(pBot, allCards[JACK_OF_DIAMONDS]);
                return true;
            }

    }

    return false;

}

bool GiveSelfJackOfDiamonds(Player& pBot, const GameRule& grGameRule, const std::vector<Card>& vcCardsReamining) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, JACK_OF_DIAMONDS)) {
        return false;
    }

    //If there is only one card in the trick, check if there are any diamonds greater than the Jack of Diamonds remaining

    if (grGameRule.vcInternalCardsInTrick.size() == 1) {
        if (allCards[BotFindHighestCardIDBySuit(vcCardsReamining, DIAMONDS_SUIT)].iValue < allCards[JACK_OF_DIAMONDS].iValue
            &&
            allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, DIAMONDS_SUIT)].iValue < allCards[JACK_OF_DIAMONDS].iValue) {

                WG::print("Bot is convinced it can safely play the Jack of Diamonds in this instance.");
                pBot.cCurrentCard = allCards[JACK_OF_DIAMONDS];
                return true;
            }
    }

    //If there are two cards in the trick

    if (grGameRule.vcInternalCardsInTrick.size() == 2) {
        if (allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, DIAMONDS_SUIT)].iValue < allCards[JACK_OF_DIAMONDS].iValue) {
            WG::print("Bot has identified an opportunity to play the Jack of Diamonds");
            pBot.cCurrentCard = allCards[JACK_OF_DIAMONDS];
            return true;
        }
    }

    return false;

}

//Hearts

bool AvoidHearts(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsReamining) {

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) == 0 || pBot.bMoonShooting) {
        return false;
    }

    //Check if bot has a card lower than any other card in the trick

    Card cCardToBeat = allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)];
    Card cBotBestCard = allCards[BotFindClosestLowestCard(pBot, cCardToBeat)];

    if (CertainLossOfTrick(pBot, cCardToBeat)) {
        WG::print("Bot is avoiding hearts (HARD CODED)");
        return true;
    }

    //Check if bot has a joker

    if (FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) > 0) {
        WG::print("Bot is playing a Joker to avoid taking hearts");
        std::vector<Card> vcJokerSuit = IsolateSuit(pBot.vcCurrentDeck, JOKERS_SUIT);
        pBot.cCurrentCard = allCards[vcJokerSuit[0].sCardID];
        return true;
    }

    return false;

}

bool HardCodedHeartsPlays(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsReamining) {

    if (cLeadCard.iSuit != HEARTS_SUIT) {
        return false;
    }
    if (AvoidHearts(pBot, grGameRule, cLeadCard, vcCardsReamining)) {
        return true;
    }

    return false;

}


bool HardCodedDiamondsPlays(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsRemaining) {

    if (cLeadCard.iSuit != DIAMONDS_SUIT) {
        return false;
    }

    if (SnatchJackOfDiamonds(pBot, grGameRule, cLeadCard, vcCardsRemaining)) {
        return true;
    }

    if (StealJackOfDiamonds(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    if (GiveSelfJackOfDiamonds(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }
    
    return false;

}

//Void

bool PreventMoonShootingInVoid(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Player>& vpPlayers) {

    if (BotIdentifyMoonShooterScore(vpPlayers, pBot) < 6) {
        return false;
    }

    std::vector<Card> vcHandWithoutHearts = pBot.vcCurrentDeck; 
    RemoveSuitFromCardVector(vcHandWithoutHearts, HEARTS_SUIT);
    RemoveSuitFromCardVector(vcHandWithoutHearts, JOKERS_SUIT);

    if (vcHandWithoutHearts.size() > 0) {
        WG::print("Bot is voiding its lowest non-heart to make it harder to moonshot");
        pBot.cCurrentCard = allCards[BotFindLowestCardID(vcHandWithoutHearts)];
        return true;
    } else {
        WG::print("Bot is voiding its lowest heart/joker to make it harder to moonshot");
        pBot.cCurrentCard = allCards[BotFindLowestCardID(pBot.vcCurrentDeck)];
        return true;
    }

    return false;

}

bool GiveQueenOfSpadesInVoid(Player& pBot) {

    if (BotFindCardInVector(pBot.vcCurrentDeck, QUEEN_OF_SPADES) && !pBot.bMoonShooting) {
        WG::print("Bot found an opportunity to give someone the Queen of Spades");
        pBot.cCurrentCard = allCards[QUEEN_OF_SPADES];
        return true;
    }

    return false;
 
}

bool GiveTenOfClubsInVoid(Player& pBot, const GameRule& grGameRule, const std::vector<Player>& vpPlayers, const Card& cLeadCard) {

    if (!BotFindCardInVector(pBot.vcCurrentDeck, TEN_OF_CLUBS)) {
        return false;
    }

    if (BotCountPointsOnly(BotPredictWinnerOfCurrentTrick(grGameRule, vpPlayers, cLeadCard)) > 18 - grGameRule.iTurnCounter) {
        pBot.cCurrentCard = allCards[TEN_OF_CLUBS];
        WG::print("Bot detected an opportunity to the Ten of Clubs out of suit, and found an opponent with a score worth targetting.");
        return true;
    }


    return false;

}

bool HardCodedVoidPlays(Player& pBot, const GameRule& grGameRule, const std::vector<Player>& vpPlayers, const Card& cLeadCard) {

    if (GiveQueenOfSpadesInVoid(pBot)) {
        return true;
    }
    if (PreventMoonShootingInVoid(pBot, grGameRule, cLeadCard, vpPlayers)) {
        return true;
    }

    if (GiveTenOfClubsInVoid(pBot, grGameRule, vpPlayers, cLeadCard)) {
        return true;
    }

    return false;

}


//Soft Coded Plays

//In Suit

bool ReconcileJackOfDiamondsInEvasion(Player& pBot, const Card& cLeadCard) {

    std::vector<Card> vcDiamondsDeck = IsolateSuit(pBot.vcCurrentDeck, DIAMONDS_SUIT);

    RemoveCardFromCardVector(vcDiamondsDeck, JACK_OF_DIAMONDS);

    if (vcDiamondsDeck.size() > 0) {
        pBot.cCurrentCard = allCards[BotFindClosestLowestCardInVector(vcDiamondsDeck, cLeadCard)];
        WG::print("Bot was about to evade using Jack of Diamonds, evading with another Card");
        return true;
    } else {
        pBot.cCurrentCard = allCards[BotFindClosestLowestCard(pBot, cLeadCard)];
        return true;
    }

    return false;

}

bool ReconcileQueenOfSpadesInEvasion(Player& pBot) {

    std::vector<Card> vcSpadesDeck = IsolateSuit(pBot.vcCurrentDeck, SPADES_SUIT); 

    RemoveCardFromCardVector(vcSpadesDeck, QUEEN_OF_SPADES);

    if (vcSpadesDeck.size() > 0) {
        pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(vcSpadesDeck, SPADES_SUIT)];
        WG::print("Failed Evasion, Avoiding QoS");
        return true;
    } else {
        pBot.cCurrentCard = allCards[BotFindLowestCardIDBySuit(pBot.vcCurrentDeck, SPADES_SUIT)];
        WG::print("Failed Evasion, No other choice");
        return true;
    }

    return false;

}

bool FailedEvasion(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsRemaining) {

    const Card cPotentialCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, cLeadCard.iSuit)];

    if (cPotentialCard.sCardID == QUEEN_OF_SPADES) {
        return ReconcileQueenOfSpadesInEvasion(pBot);
    }

    WG::print("Bot cannot lose this hand, they will lose with their highest card of this suit");
    pBot.cCurrentCard = cPotentialCard;

    return true;

}

bool EvasionPlay(Player& pBot, const GameRule& grGameRule, const Card& cLeadCard, const std::vector<Card>& vcCardsRemaining) {

    if (cLeadCard.sCardID == JOKER && FindVolumeOfSuitInCardVector(pBot.vcCurrentDeck, JOKERS_SUIT) == 0) {
        pBot.cCurrentCard = allCards[pBot.vcCurrentDeck[0].sCardID];
    }

    const Card cPotentialCard = allCards[BotFindClosestLowestCard(pBot, allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)])];

    if (cPotentialCard.iValue > allCards[BotFindHighestCardIDBySuit(grGameRule.vcInternalCardsInTrick, cLeadCard.iSuit)].iValue && grGameRule.vcInternalCardsInTrick.size() == 2) {
        return FailedEvasion(pBot, grGameRule, cLeadCard, vcCardsRemaining);
    } 
    
    if (cPotentialCard.sCardID == JACK_OF_DIAMONDS) {
        return ReconcileJackOfDiamondsInEvasion(pBot, cLeadCard);
    }

    pBot.cCurrentCard = cPotentialCard;

    WG::print("Bot is evading");

    return true;

}

//Void

bool CreateVoid(Player& pBot, GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {
    
    std::vector<int> viLeadOrder = BotOrderSuitsByVolumeInVector(pBot.vcCurrentDeck);

    pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, viLeadOrder[0])];

    if (pBot.cCurrentCard.sCardID == NULLCARD) {
        return false;
    }

    if (pBot.cCurrentCard.iSuit == HEARTS_SUIT && !grGameRule.bHearts) {
        BreakHearts(grGameRule);
    }

    WG::print("Bot is attempting to create a void");
    return true;

}

bool IntentionallyBreakHeartsInVoid(Player& pBot, GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (grGameRule.bHearts) {
        return false;
    }

    if (FindStrongestRelativeSuit(pBot, grGameRule, vcCardsRemaining, true) == HEARTS_SUIT) {
        WG::print("Bot broke hearts in a void because it determined it is advantageous for hearts to be broken");
        pBot.cCurrentCard = allCards[BotFindHighestCardIDBySuit(pBot.vcCurrentDeck, HEARTS_SUIT)];
        BreakHearts(grGameRule);
        return true;
    }    

    return false;

}

bool SoftCodedVoidPlays(Player& pBot, GameRule& grGameRule, const std::vector<Card>& vcCardsRemaining) {

    if (IntentionallyBreakHeartsInVoid(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }
    if (CreateVoid(pBot, grGameRule, vcCardsRemaining)) {
        return true;
    }

    return false;

}

bool DeveloperBot(std::vector<Player>& vpPlayers, const GameRule& grGameRule) {

    for (int i = 0; i < vpPlayers.size(); i++) {

        if (!vpPlayers[i].bHuman && vpPlayers[i].sNickname == "Maxwell[AI]" && vpPlayers[i].iScore > 0 && grGameRule.iRoundCounter % 2 != 0) {

            vpPlayers[i].bBotMessage = true;
            vpPlayers[i].iMessageType = AI_SPEECH_DEVELOPERBOT;

            vpPlayers[i].iScore *= 0.5;

        }

    }

    return false;

}

