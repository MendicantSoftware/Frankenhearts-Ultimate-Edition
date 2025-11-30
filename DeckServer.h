#include <vector>
#include <map> 
#include <string>
#include <cstdlib>
#include <algorithm>

#include "WolfgangDebug.h"

#pragma once

#define CLUBS_SUIT 1
#define SPADES_SUIT 2
#define DIAMONDS_SUIT 3
#define HEARTS_SUIT 4
#define JOKERS_SUIT -2

#define NULLCARD "cNULLCARD"

#define QUEEN_OF_SPADES "cQS"
#define JACK_OF_DIAMONDS "cJD"
#define TEN_OF_CLUBS "c10C"

#define JOKER "cJk"

struct Card {
    std::string sCardID;
    int iValue;
    int iSuit;
};

std::map<std::string, Card> allCards = {

    {"c2C", {"c2C", 2, 1}},   
    {"c2S", {"c2S", 2, 2}},
    {"c2D", {"c2D", 2, 3}},
    {"c2H", {"c2H", 2, 4}},
    {"c3C", {"c3C", 3, 1}},
    {"c3S", {"c3S", 3, 2}},
    {"c3D", {"c3D", 3, 3}},
    {"c3H", {"c3H", 3, 4}},
    {"c4C", {"c4C", 4, 1}},
    {"c4S", {"c4S", 4, 2}},
    {"c4D", {"c4D", 4, 3}},
    {"c4H", {"c4H", 4, 4}},
    {"c5C", {"c5C", 5, 1}},
    {"c5S", {"c5S", 5, 2}},
    {"c5D", {"c5D", 5, 3}},
    {"c5H", {"c5H", 5, 4}}, 
    {"c6C", {"c6C", 6, 1}},
    {"c6S", {"c6S", 6, 2}},
    {"c6D", {"c6D", 6, 3}},
    {"c6H", {"c6H", 6, 4}},
    {"c7C", {"c7C", 7, 1}},
    {"c7S", {"c7S", 7, 2}},
    {"c7D", {"c7D", 7, 3}},
    {"c7H", {"c7H", 7, 4}},
    {"c8C", {"c8C", 8, 1}},
    {"c8S", {"c8S", 8, 2}},
    {"c8D", {"c8D", 8, 3}},
    {"c8H", {"c8H", 8, 4}},
    {"c9C", {"c9C", 9, 1}},
    {"c9S", {"c9S", 9, 2}},
    {"c9D", {"c9D", 9, 3}},
    {"c9H", {"c9H", 9, 4}},
    {"c10C", {"c10C", 10, 1}},
    {"c10S", {"c10S", 10, 2}},
    {"c10D", {"c10D", 10, 3}},
    {"c10H", {"c10H", 10, 4}},
    {"cJC", {"cJC", 11, 1}},
    {"cJS", {"cJS", 11, 2}},
    {"cJD", {"cJD", 11, 3}},
    {"cJH", {"cJH", 11, 4}},
    {"cQC", {"cQC", 12, 1}},
    {"cQS", {"cQS", 12, 2}},
    {"cQD", {"cQD", 12, 3}},
    {"cQH", {"cQH", 12, 4}},
    {"cKC", {"cKC", 13, 1}},
    {"cKS", {"cKS", 13, 2}},
    {"cKD", {"cKD", 13, 3}}, 
    {"cKH", {"cKH", 13, 4}}, 
    {"cAC", {"cAC", 14, 1}}, 
    {"cAS", {"cAS", 14, 2}}, 
    {"cAD", {"cAD", 14, 3}}, 
    {"cAH", {"cAH", 14, 4}}, 
    {"cJk", {"cJk", -2, -2}}, 
    {"cJk", {"cJk", -2, -2}},
    {"cNULLCARD", {"cNULLCARD", 0, 0}}
 
};

class Dealer {
    private:

        std::vector<Card> vcDeck = {};

        void SortSuitVectorByValue(std::vector<Card>& vcSuit) {
            std::sort(vcSuit.begin(), vcSuit.end(), [](const Card& a, const Card&b) {
                return a.iValue < b.iValue;
            });
        }

    public:

        void AppendCardVector(std::vector<Card>& vcFirstVector, std::vector<Card>& vcSecondVector) {
            for (int i = 0; i < vcSecondVector.size(); i++) {
                vcFirstVector.push_back(vcSecondVector[i]);
            }
        }

        void NewDeck() {

            vcDeck = {
        
                {"c2C", 2, 1},   
                {"c2S", 2, 2},
                {"c2D", 2, 3},
                {"c2H", 2, 4},
                {"c3C", 3, 1},
                {"c3S", 3, 2},
                {"c3D", 3, 3},
                {"c3H", 3, 4},
                {"c4C", 4, 1},
                {"c4S", 4, 2},
                {"c4D", 4, 3},
                {"c4H", 4, 4},
                {"c5C", 5, 1},
                {"c5S", 5, 2},
                {"c5D", 5, 3},
                {"c5H", 5, 4}, 
                {"c6C", 6, 1},
                {"c6S", 6, 2},
                {"c6D", 6, 3},
                {"c6H", 6, 4},
                {"c7C", 7, 1},
                {"c7S", 7, 2},
                {"c7D", 7, 3},
                {"c7H", 7, 4},
                {"c8C", 8, 1},
                {"c8S", 8, 2},
                {"c8D", 8, 3},
                {"c8H", 8, 4},
                {"c9C", 9, 1},
                {"c9S", 9, 2},
                {"c9D", 9, 3},
                {"c9H", 9, 4},
                {"c10C", 10, 1},
                {"c10S", 10, 2},
                {"c10D", 10, 3},
                {"c10H", 10, 4},
                {"cJC", 11, 1},
                {"cJS", 11, 2},
                {"cJD", 11, 3},
                {"cJH", 11, 4},
                {"cQC", 12, 1},
                {"cQS", 12, 2},
                {"cQD", 12, 3},
                {"cQH", 12, 4},
                {"cKC", 13, 1},
                {"cKS", 13, 2},
                {"cKD", 13, 3}, 
                {"cKH", 13, 4}, 
                {"cAC", 14, 1}, 
                {"cAS", 14, 2}, 
                {"cAD", 14, 3}, 
                {"cAH", 14, 4}, 
                {"cJk", -2, -2}, 
                {"cJk", -2, -2}
        
            }; 
        }

        std::vector<Card> GiveNewDeck() {
            std::vector<Card> vcNewDeck = {
                {"c2C", 2, 1},   
                {"c2S", 2, 2},
                {"c2D", 2, 3},
                {"c2H", 2, 4},
                {"c3C", 3, 1},
                {"c3S", 3, 2},
                {"c3D", 3, 3},
                {"c3H", 3, 4},
                {"c4C", 4, 1},
                {"c4S", 4, 2},
                {"c4D", 4, 3},
                {"c4H", 4, 4},
                {"c5C", 5, 1},
                {"c5S", 5, 2},
                {"c5D", 5, 3},
                {"c5H", 5, 4}, 
                {"c6C", 6, 1},
                {"c6S", 6, 2},
                {"c6D", 6, 3},
                {"c6H", 6, 4},
                {"c7C", 7, 1},
                {"c7S", 7, 2},
                {"c7D", 7, 3},
                {"c7H", 7, 4},
                {"c8C", 8, 1},
                {"c8S", 8, 2},
                {"c8D", 8, 3},
                {"c8H", 8, 4},
                {"c9C", 9, 1},
                {"c9S", 9, 2},
                {"c9D", 9, 3},
                {"c9H", 9, 4},
                {"c10C", 10, 1},
                {"c10S", 10, 2},
                {"c10D", 10, 3},
                {"c10H", 10, 4},
                {"cJC", 11, 1},
                {"cJS", 11, 2},
                {"cJD", 11, 3},
                {"cJH", 11, 4},
                {"cQC", 12, 1},
                {"cQS", 12, 2},
                {"cQD", 12, 3},
                {"cQH", 12, 4},
                {"cKC", 13, 1},
                {"cKS", 13, 2},
                {"cKD", 13, 3}, 
                {"cKH", 13, 4}, 
                {"cAC", 14, 1}, 
                {"cAS", 14, 2}, 
                {"cAD", 14, 3}, 
                {"cAH", 14, 4}, 
                {"cJk", -2, -2}, 
                {"cJk", -2, -2}
            };

            return vcNewDeck;
        }

        std::vector<Card> GiveHand() {

            std::vector<Card> vcPlayerHand = {};

            for (int i = 0; i < 18; i++) {
                int iSelection = rand() % vcDeck.size();
                vcPlayerHand.emplace_back(vcDeck[iSelection]);
                vcDeck.erase(vcDeck.begin() + iSelection);

            }

            return vcPlayerHand;

        }

        void SortHand(std::vector<Card>& vcHand) {

            std::vector<Card> vcClubs;
            std::vector<Card> vcSpades;
            std::vector<Card> vcDiamonds;
            std::vector<Card> vcHearts;
            std::vector<Card> vcJokers;

            for (int i = 0; i < vcHand.size(); i++) {
                switch (vcHand[i].iSuit) {

                    case CLUBS_SUIT:
                    vcClubs.push_back(vcHand[i]);
                    break;

                    case SPADES_SUIT:
                    vcSpades.push_back(vcHand[i]);
                    break;

                    case DIAMONDS_SUIT:
                    vcDiamonds.push_back(vcHand[i]);
                    break;

                    case HEARTS_SUIT:
                    vcHearts.push_back(vcHand[i]);
                    break;

                    case JOKERS_SUIT:
                    vcJokers.push_back(vcHand[i]);
                    break;

                    default:
                    WG::printvarchar("SortHand failed to sort card of ID", vcHand[i].sCardID.data());
                    break;

                }
            }

                SortSuitVectorByValue(vcClubs);
                SortSuitVectorByValue(vcSpades);
                SortSuitVectorByValue(vcDiamonds);
                SortSuitVectorByValue(vcHearts);

                vcHand = {};

                AppendCardVector(vcHand, vcClubs);
                AppendCardVector(vcHand, vcDiamonds);
                AppendCardVector(vcHand, vcSpades);
                AppendCardVector(vcHand, vcHearts);
                AppendCardVector(vcHand, vcJokers);

        }

};



