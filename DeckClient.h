#include <vector>
#include <map> 
#include <string>

#include <SDL.h>

#include "WolfgangTile.h"

#pragma once


struct Card {
    std::string sCardID;
    std::string sCardPath;
    int iValue;
    int iSuit;
    SDL_Texture* sdlCardTexture;
    SDL_Rect sdlPositionRect;
    bool bSelected = false;
};

std::map<std::string, Card> allCards = {

    {"c2C", {"c2C", "Cards/2_of_clubs.png", 2, 1}},   
    {"c2S", {"c2S", "Cards/2_of_spades.png", 2, 2}},
    {"c2D", {"c2D", "Cards/2_of_diamonds.png", 2, 3}},
    {"c2H", {"c2H", "Cards/2_of_hearts.png", 2, 4}},
    {"c3C", {"c3C", "Cards/3_of_clubs.png", 3, 1}},
    {"c3S", {"c3S", "Cards/3_of_spades.png", 3, 2}},
    {"c3D", {"c3D", "Cards/3_of_diamonds.png", 3, 3}},
    {"c3H", {"c3H", "Cards/3_of_hearts.png", 3, 4}},
    {"c4C", {"c4C", "Cards/4_of_clubs.png", 4, 1}},
    {"c4S", {"c4S", "Cards/4_of_spades.png", 4, 2}},
    {"c4D", {"c4D", "Cards/4_of_diamonds.png", 4, 3}},
    {"c4H", {"c4H", "Cards/4_of_hearts.png", 4, 4}},
    {"c5C", {"c5C", "Cards/5_of_clubs.png", 5, 1}},
    {"c5S", {"c5S", "Cards/5_of_spades.png", 5, 2}},
    {"c5D", {"c5D", "Cards/5_of_diamonds.png", 5, 3}},
    {"c5H", {"c5H", "Cards/5_of_hearts.png", 5, 4}}, 
    {"c6C", {"c6C", "Cards/6_of_clubs.png", 6, 1}},
    {"c6S", {"c6S", "Cards/6_of_spades.png", 6, 2}},
    {"c6D", {"c6D", "Cards/6_of_diamonds.png", 6, 3}},
    {"c6H", {"c6H", "Cards/6_of_hearts.png", 6, 4}},
    {"c7C", {"c7C", "Cards/7_of_clubs.png", 7, 1}},
    {"c7S", {"c7S", "Cards/7_of_spades.png", 7, 2}},
    {"c7D", {"c7D", "Cards/7_of_diamonds.png", 7, 3}},
    {"c7H", {"c7H", "Cards/7_of_hearts.png", 7, 4}},
    {"c8C", {"c8C", "Cards/8_of_clubs.png", 8, 1}},
    {"c8S", {"c8S", "Cards/8_of_spades.png", 8, 2}},
    {"c8D", {"c8D", "Cards/8_of_diamonds.png", 8, 3}},
    {"c8H", {"c8H", "Cards/8_of_hearts.png", 8, 4}},
    {"c9C", {"c9C", "Cards/9_of_clubs.png", 9, 1}},
    {"c9S", {"c9S", "Cards/9_of_spades.png", 9, 2}},
    {"c9D", {"c9D", "Cards/9_of_diamonds.png", 9, 3}},
    {"c9H", {"c9H", "Cards/9_of_hearts.png", 9, 4}},
    {"c10C", {"c10C", "Cards/10_of_clubs.png", 10, 1}},
    {"c10S", {"c10S", "Cards/10_of_spades.png", 10, 2}},
    {"c10D", {"c10D", "Cards/10_of_diamonds.png", 10, 3}},
    {"c10H", {"c10H", "Cards/10_of_hearts.png", 10, 4}},
    {"cJC", {"cJC", "Cards/jack_of_clubs2.png", 11, 1}},
    {"cJS", {"cJS", "Cards/jack_of_spades2.png", 11, 2}},
    {"cJD", {"cJD", "Cards/jack_of_diamonds2.png", 11, 3}},
    {"cJH", {"cJH", "Cards/jack_of_hearts2.png", 11, 4}},
    {"cQC", {"cQC", "Cards/queen_of_clubs2.png", 12, 1}},
    {"cQS", {"cQS", "Cards/queen_of_spades2.png", 12, 2}},
    {"cQD", {"cQD", "Cards/queen_of_diamonds2.png", 12, 3}},
    {"cQH", {"cQH", "Cards/queen_of_hearts2.png", 12, 4}},
    {"cKC", {"cKC", "Cards/king_of_clubs2.png", 13, 1}},
    {"cKS", {"cKS", "Cards/king_of_spades2.png", 13, 2}},
    {"cKD", {"cKD", "Cards/king_of_diamonds2.png", 13, 3}}, 
    {"cKH", {"cKH", "Cards/king_of_hearts2.png", 13, 4}}, 
    {"cAC", {"cAC", "Cards/ace_of_clubs.png", 14, 1}}, 
    {"cAS", {"cAS", "Cards/ace_of_spades.png", 14, 2}}, 
    {"cAD", {"cAD", "Cards/ace_of_diamonds.png", 14, 3}}, 
    {"cAH", {"cAH", "Cards/ace_of_hearts.png", 14, 4}}, 
    {"cJk", {"cJk", "Cards/black_joker.png", -2, -2}}, 
    {"cJk", {"cJk", "Cards/red_joker.png", -2, -2}},
    {"cNULLCARD", {"cNULLCARD", "null", 0, 0}}
 
};

std::map<std::string, std::string> CardSubtitles = {

        {"c2C", "Two of Clubs"},
        {"c2S", "Two of Spades"},
        {"c2D", "Two of Diamonds"},
        {"c2H", "Two of Hearts"},
        {"c3C", "Three of Clubs"},
        {"c3S", "Three of Spades"},
        {"c3D", "Three of Diamonds"},
        {"c3H", "Three of Hearts"},
        {"c4C", "Four of Clubs"},
        {"c4S", "Four of Spades"},
        {"c4D", "Four of Diamonds"},
        {"c4H", "Four of Hearts"},
        {"c5C", "Five of Clubs"},
        {"c5S", "Five of Spades"},
        {"c5D", "Five of Diamonds"},
        {"c5H", "Five of Hearts"},
        {"c6C", "Six of Clubs"},
        {"c6S", "Six of Spades"},
        {"c6D", "Six of Diamonds"},
        {"c6H", "Six of Hearts"},
        {"c7C", "Seven of Clubs"},
        {"c7S", "Seven of Spades"},
        {"c7D", "Seven of Diamonds"},
        {"c7H", "Seven of Hearts"},
        {"c8C", "Eight of Clubs"},
        {"c8S", "Eight of Spades"},
        {"c8D", "Eight of Diamonds"},
        {"c8H", "Eight of Hearts"},
        {"c9C", "Nine of Clubs"},
        {"c9S", "Nine of Spades"},
        {"c9D", "Nine of Diamonds"},
        {"c9H", "Nine of Hearts"},
        {"c10C", "Ten of Clubs"},
        {"c10S", "Ten of Spades"},
        {"c10D", "Ten of Diamonds"},
        {"c10H", "Ten of Hearts"},
        {"cJC", "Jack of Clubs"},
        {"cJS", "Jack of Spades"},
        {"cJD", "Jack of Diamonds"},
        {"cJH", "Jack of Hearts"},
        {"cQC", "Queen of Clubs"},
        {"cQS", "Queen of Spades"},
        {"cQD", "Queen of Diamonds"},
        {"cQH", "Queen of Hearts"},
        {"cKC", "King of Clubs"},
        {"cKS", "King of Spades"},
        {"cKD", "King of Diamonds"},
        {"cKH", "King of Hearts"},
        {"cAC", "Ace of Clubs"},
        {"cAS", "Ace of Spades"},
        {"cAD", "Ace of Diamonds"},
        {"cAH", "Ace of Hearts"},
        {"cJk", "Joker"}

};

std::map<std::string, SDL_Texture*> InitaliseTextureMap(SDL_Renderer* sdlRenderer) {

    std::map<std::string, SDL_Texture*> mtTextureMap;

    for (const auto& pair : allCards) {

        const std::string& cardID = pair.first;
        const Card& cCard = pair.second;
        
        if (cardID == "cNULLCARD") {
            continue;
        }

        mtTextureMap[cardID] = WG::ConvertSpriteToTexture(sdlRenderer, cCard.sCardPath.c_str());
        

    }

    return mtTextureMap;


}