#include "WolfgangTile.h"
#include "DeckClient.h"
#include "WolfgangPane.h"
#include "ClientSocket.h"

#include <cstdlib>

#define FRANKENHEARTS_STANDARDFONT "Gothica-Book.ttf"
#define FRANKENHEARTS_ACCESSIBLEFONT "Unageo-Regular.ttf"

#define FRANKENHEARTS_GAMEMENU_CLOSEMENU 0
#define FRANKENHEARTS_GAMEMENU_EXITGAME 1

#define FRANKENHEARTS_MENUTYPE_ROOT 0
#define FRANKENHEARTS_MENUTYPE_HOWTOPLAY 1

#define FRANKENHEARTS_MAINMENU_CONNECT 0
#define FRANKENHEARTS_MAINMENU_EXITGAME 1

#define FRANKENHEARTS_CONNECTIONMENU_EXIT 0

#pragma once

struct CardPositionTracker {
    SDL_Rect sdlRectCurrentPosition;
    std::string sCardID;
};

void AccessibilityMode(const bool& bModeStatus, std::vector<WG::TextPane*>& vwtpAffectedTextPanes) {

std::string sFont = FRANKENHEARTS_ACCESSIBLEFONT;

    if (bModeStatus) {
        sFont = FRANKENHEARTS_ACCESSIBLEFONT;
    } else {
        sFont = FRANKENHEARTS_STANDARDFONT;
    }

    for (int i = 0; i < vwtpAffectedTextPanes.size(); i++) {

        vwtpAffectedTextPanes[i]->sFont = sFont;

    }

}

void ChangeManyTextPanesFontSize(std::vector<WG::TextPane*>& vwtpAffectedTextPanes, int iFontSize) {

    for (int i = 0; i < vwtpAffectedTextPanes.size(); i++) {
        std::string sPaneText = vwtpAffectedTextPanes[i]->vwtlTextLines[0].sContent;
        SDL_Color sdlPaneTextColour = vwtpAffectedTextPanes[i]->vwtlTextLines[0].sdlFontColor;
        vwtpAffectedTextPanes[i]->ClearTextVector();
        vwtpAffectedTextPanes[i]->AddLine(sPaneText, iFontSize, sdlPaneTextColour);
        vwtpAffectedTextPanes[i]->RefreshTextVectorTextures();
    }

}

void RenderPlayerHand(SDL_Renderer* sdlRenderer, Player pPlayer) {

    int iHandXStart = 10;
    int iHandYStart = 700;

    bool bRendered = false;

    for (int i = 0; i < pPlayer.GetSize(); i++) {

        SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, pPlayer.ReadDeckIndex(i).sCardPath.data());

        if (sdlCardTexture) {
            bRendered = true;
            SDL_Rect sdlCardRect;
            SDL_Rect sdlSourceRect;

            sdlSourceRect.x = 0;
            sdlSourceRect.y = 0;
            sdlSourceRect.w = 100;
            sdlSourceRect.h = 145;

            sdlCardRect.x = iHandXStart;
            sdlCardRect.y = iHandYStart;
            sdlCardRect.w = 100;
            sdlCardRect.h = 145;

            pPlayer.ReWriteCardPosition(i, sdlCardRect);

            iHandXStart += sdlCardRect.w;

            SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
            SDL_DestroyTexture(sdlCardTexture);
        }

    }

}

std::vector<CardPositionTracker> RenderCardsRecievedInPass(std::vector<Card> vcCardsRecievedInPass, SDL_Renderer* sdlRenderer, int iWindowHeight, int iWindowWidth, bool bRender, UISettings usUISettings) {

    if (!bRender) {
        return {};
    }

    std::vector<CardPositionTracker> vctCardsRecievedInPass = {};

    int iFontSize = iWindowHeight / 50 * usUISettings.dUIScale;

    WG::TextPane wtpCardsRecievedInPass(sdlRenderer, iWindowWidth, iWindowHeight, 0.7, 0.65, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpRenderCardRecievedTextPanes = {&wtpCardsRecievedInPass};
    AccessibilityMode(usUISettings.bAccessible, vwtpRenderCardRecievedTextPanes);

    wtpCardsRecievedInPass.AddLine("Cards recieved:", iFontSize, {255, 255, 255, 255});
    wtpCardsRecievedInPass.RefreshTextVectorTextures();

    int iCardWidth = iWindowWidth / (19.2 * 1.5);
    int iCardHeight = iWindowHeight / (7.5 * 1.5);

    double dXBuffer = 0.25;
    int iXOffset = 0;

    int iStartX = (iWindowWidth * 0.8) - ((iCardWidth * 3) + (iCardWidth * dXBuffer * 2)) / 2;
    int iStartY = (iWindowHeight * 0.70);



    for (int i = 0; i < vcCardsRecievedInPass.size(); i++) {

        SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, vcCardsRecievedInPass[i].sCardPath.data());

        if (sdlCardTexture) {

            SDL_Rect sdlCardRect;
            SDL_Rect sdlSourceRect;

            sdlSourceRect.x = 0;
            sdlSourceRect.y = 0; 
            sdlSourceRect.w = 100;
            sdlSourceRect.h = 145;

            sdlCardRect.x = iStartX + iXOffset;
            sdlCardRect.y = iStartY;
            sdlCardRect.w = iCardWidth;
            sdlCardRect.h = iCardHeight;

            iXOffset += iCardWidth;
            iXOffset += iCardWidth * dXBuffer;

            SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
            SDL_DestroyTexture(sdlCardTexture);

            vctCardsRecievedInPass.push_back({sdlCardRect, vcCardsRecievedInPass[i].sCardID});
            
        }

    }

    wtpCardsRecievedInPass.RenderPaneText();
    wtpCardsRecievedInPass.ClearTextVector(); 

    return vctCardsRecievedInPass;

}

void RenderHintText(std::string sHint, int iPassType, WG::TextPane& wtpRenderHintPane) {

    if (iPassType == PASS_NONE) {
        return;
    }

    int iFontSize = wtpRenderHintPane.dWindowHeight / 50;

    wtpRenderHintPane.ClearTextVector();
    wtpRenderHintPane.AddLine(sHint, iFontSize, {255, 255, 255, 255});
    wtpRenderHintPane.RefreshTextVectorTextures();


    wtpRenderHintPane.vwtlTextLines[0].sdlTextRect.x = (wtpRenderHintPane.dWindowWidth / 2) - (wtpRenderHintPane.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpRenderHintPane.vwtlTextLines[0].sdlTextRect.y = (wtpRenderHintPane.dWindowHeight * 0.6);

    wtpRenderHintPane.RenderPaneText();
    wtpRenderHintPane.ClearTextVector();

}

void RenderPassText(int iPassType, int iVectorSize, WG::TextPane& wtpPassText, const UISettings& usUISettings) {

    int iFontSize = wtpPassText.dWindowHeight / 41.5 * usUISettings.dUIScale;

    if (iVectorSize == 4 || iPassType == PASS_NONE) {
        return;
    }

    int iCardsToPass = 4 - iVectorSize;

    std::string sCardORCards = " cards ";

    if (iCardsToPass == 1) {
        sCardORCards = " card ";
    }

    std::string sDirection = "left";

    if (iPassType == PASS_RIGHT) { 
        sDirection = "right";
    }

    std::string sPassString = "Pass " + std::to_string(iCardsToPass) + sCardORCards + sDirection;

    if (iPassType == PASS_WAIT) {
        sPassString = "Waiting for other players";
    }

    wtpPassText.ClearTextVector();
    wtpPassText.AddLine(sPassString, iFontSize, {255, 215, 0, 255});
    wtpPassText.RefreshTextVectorTextures();

    wtpPassText.vwtlTextLines[0].sdlTextRect.x = (wtpPassText.dWindowWidth / 2) - (wtpPassText.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpPassText.vwtlTextLines[0].sdlTextRect.y = (wtpPassText.dWindowHeight * 0.5);

    wtpPassText.RenderPaneText();
    wtpPassText.ClearTextVector();

}

void RenderClickToPassText(int iPassVectorSize, WG::TextPane& wtpClickToPass, const UISettings& usUISettings) {

    int iFontSize = wtpClickToPass.dWindowHeight / 41.5 * usUISettings.dUIScale;

    if (iPassVectorSize == 4) {
        wtpClickToPass.ClearTextVector();
        wtpClickToPass.AddLine("Click to Pass", iFontSize, {255, 215, 0, 255});

        wtpClickToPass.RefreshTextVectorTextures();

        wtpClickToPass.vwtlTextLines[0].sdlTextRect.x = (wtpClickToPass.dWindowWidth / 2) - (wtpClickToPass.vwtlTextLines[0].sdlTextRect.w / 2);
        wtpClickToPass.vwtlTextLines[0].sdlTextRect.y = (wtpClickToPass.dWindowHeight * 0.5);

        wtpClickToPass.sdlPaneRect.x = wtpClickToPass.vwtlTextLines[0].sdlTextRect.x;
        wtpClickToPass.sdlPaneRect.y = wtpClickToPass.vwtlTextLines[0].sdlTextRect.y;
        wtpClickToPass.sdlPaneRect.w = wtpClickToPass.vwtlTextLines[0].sdlTextRect.w;
        wtpClickToPass.sdlPaneRect.h = wtpClickToPass.vwtlTextLines[0].sdlTextRect.h;

        wtpClickToPass.RenderPaneText();
        wtpClickToPass.ClearTextVector(); 
    }

}


std::vector<CardPositionTracker> RenderPreviousCardsInTrick(std::vector<Card> vcPreviousCardsInTrick, SDL_Renderer* sdlRenderer, int iWindowHeight, int iWindowWidth, bool bRender) {

    if (!bRender) {
        return {};
    }

    std::vector<CardPositionTracker> vctCardsInPreviousTrick = {};

    int iCardWidth = iWindowWidth / (19.2 * 1.5);
    int iCardHeight = iWindowHeight / (7.5 * 1.5);

    double dXBuffer = 0.25;
    int iXOffset = 0;

    int iStartX = (iWindowWidth / 2) - ((iCardWidth * 3) + (iCardWidth * dXBuffer * 2)) / 2;;
    int iStartY = (iWindowHeight * 0.025);



    for (int i = 0; i < vcPreviousCardsInTrick.size(); i++) {

        SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, vcPreviousCardsInTrick[i].sCardPath.data());

        if (sdlCardTexture) {

            SDL_Rect sdlCardRect;
            SDL_Rect sdlSourceRect;

            sdlSourceRect.x = 0;
            sdlSourceRect.y = 0; 
            sdlSourceRect.w = 100;
            sdlSourceRect.h = 145;

            sdlCardRect.x = iStartX + iXOffset;
            sdlCardRect.y = iStartY;
            sdlCardRect.w = iCardWidth;
            sdlCardRect.h = iCardHeight;

            iXOffset += iCardWidth;
            iXOffset += iCardWidth * dXBuffer;

            SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
            SDL_DestroyTexture(sdlCardTexture);

            vctCardsInPreviousTrick.push_back({sdlCardRect, vcPreviousCardsInTrick[i].sCardID});
            
        }

    }

    return vctCardsInPreviousTrick;

}

std::vector<CardPositionTracker>  RenderCardsInTrick(std::vector<Card> vcCardsInTrick, SDL_Renderer* sdlRenderer, int iWindowHeight, int iWindowWidth) {

    std::vector<CardPositionTracker> vctCardsInTrickPositions = {};

    int iCardWidth = iWindowWidth / 19.2;
    int iCardHeight = iWindowHeight / 7.5;

    for (int i = 0; i < vcCardsInTrick.size(); i++) {

        if (i == 0) {

            SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, vcCardsInTrick[i].sCardPath.data());

            if (sdlCardTexture) {
                SDL_Rect sdlCardRect;
                SDL_Rect sdlSourceRect;
    
                sdlSourceRect.x = 0;
                sdlSourceRect.y = 0; 
                sdlSourceRect.w = 100;
                sdlSourceRect.h = 145;
    
                sdlCardRect.x = (iWindowWidth / 2) - (sdlSourceRect.w / 2);
                sdlCardRect.y = ((iWindowHeight / 3) * 2) - (sdlSourceRect.h / 2);
                sdlCardRect.w = iCardWidth;
                sdlCardRect.h = iCardHeight;

                vctCardsInTrickPositions.push_back({sdlCardRect, vcCardsInTrick[i].sCardID});
    
                SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
                SDL_DestroyTexture(sdlCardTexture);
            }

        }

        if (i == 1) {


            SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, vcCardsInTrick[i].sCardPath.data());

            if (sdlCardTexture) {
                SDL_Rect sdlCardRect;
                SDL_Rect sdlSourceRect;
    
                sdlSourceRect.x = 0;
                sdlSourceRect.y = 0; 
                sdlSourceRect.w = 100;
                sdlSourceRect.h = 145;
    
                sdlCardRect.x = (iWindowWidth / 3) - (sdlSourceRect.w / 2);
                sdlCardRect.y = (iWindowHeight / 3) - (sdlSourceRect.h / 2);
                sdlCardRect.w = iCardWidth;
                sdlCardRect.h = iCardHeight;

                vctCardsInTrickPositions.push_back({sdlCardRect, vcCardsInTrick[i].sCardID});
    
                SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
                SDL_DestroyTexture(sdlCardTexture);
            }

        }

        if (i == 2) {
            SDL_Texture* sdlCardTexture = WG::ConvertSpriteToTexture(sdlRenderer, vcCardsInTrick[i].sCardPath.data());

            if (sdlCardTexture) {
                SDL_Rect sdlCardRect;
                SDL_Rect sdlSourceRect;
    
                sdlSourceRect.x = 0;
                sdlSourceRect.y = 0; 
                sdlSourceRect.w = 100;
                sdlSourceRect.h = 145;
    
                sdlCardRect.x = ((iWindowWidth / 3) * 2) - (sdlSourceRect.w / 2);
                sdlCardRect.y = (iWindowHeight / 3) - (sdlSourceRect.h / 2);
                sdlCardRect.w = iCardWidth;
                sdlCardRect.h = iCardHeight;

                vctCardsInTrickPositions.push_back({sdlCardRect, vcCardsInTrick[i].sCardID});
    
                SDL_RenderCopy(sdlRenderer, sdlCardTexture, &sdlSourceRect, &sdlCardRect);
                SDL_DestroyTexture(sdlCardTexture);
            }
        }
    }

    return vctCardsInTrickPositions;

}

SDL_Color GetPlayerScoreColour(std::string sNickname) {

    SDL_Color sdlDefaultColour{255, 215, 0, 255};

    if (sNickname.substr(0, 7) == "Maxwell") {
        return SDL_Color{255, 215, 0, 255};
    } else {
        return sdlDefaultColour;
    }

}

SDL_Color GetDefaultFontColour() {

    return SDL_Color{255, 215, 0, 255};

}

void RenderPlayerScoreStrings(WG::TextPane& tpPlayer, WG::TextPane& tpPositionOne, WG::TextPane& tpPositionTwo, std::vector<std::string> sScoreStrings, const UISettings& usUISettings) {

    const int iFontSize = tpPlayer.dWindowHeight / 41.5 * usUISettings.dUIScale;

    if (sScoreStrings.size() == 3) {

        //Deal with texture data

        tpPlayer.ClearTextVector();
        tpPlayer.AddLine(sScoreStrings[0], iFontSize, GetPlayerScoreColour(sScoreStrings[0]));
        tpPlayer.RefreshTextVectorTextures();

        //Reposition text

        if (tpPlayer.vwtlTextLines.size() > 0) {
            tpPlayer.vwtlTextLines[0].sdlTextRect.x = (tpPlayer.dWindowWidth / 3) - (tpPlayer.vwtlTextLines[0].sdlTextRect.w / 2);
            tpPlayer.vwtlTextLines[0].sdlTextRect.y = (tpPlayer.dWindowHeight * 0.70);
        }
        
        tpPositionOne.ClearTextVector();
        tpPositionOne.AddLine(sScoreStrings[1], iFontSize, GetPlayerScoreColour(sScoreStrings[1]));
        tpPositionOne.RefreshTextVectorTextures();

        if (tpPositionOne.vwtlTextLines.size() > 0) {
            tpPositionOne.vwtlTextLines[0].sdlTextRect.x = (tpPositionOne.dWindowWidth * 0.25) - (tpPositionOne.vwtlTextLines[0].sdlTextRect.w / 2);
            tpPositionOne.vwtlTextLines[0].sdlTextRect.y = (tpPositionOne.dWindowHeight * 0.20);
        }
    
        tpPositionTwo.ClearTextVector();
        tpPositionTwo.AddLine(sScoreStrings[2], iFontSize, GetPlayerScoreColour(sScoreStrings[2]));
        tpPositionTwo.RefreshTextVectorTextures();

        if (tpPositionTwo.vwtlTextLines.size() > 0) {
            tpPositionTwo.vwtlTextLines[0].sdlTextRect.x = (tpPositionTwo.dWindowWidth * 0.75) - (tpPositionTwo.vwtlTextLines[0].sdlTextRect.w / 2);
            tpPositionTwo.vwtlTextLines[0].sdlTextRect.y = (tpPositionTwo.dWindowHeight * 0.20);
        }
    } 

}

void ClearAllTextVectors(std::vector<WG::TextPane*>& vwtpPanesToBeCleared) {
    for (int i = 0; i < vwtpPanesToBeCleared.size(); i++) {
        vwtpPanesToBeCleared[i]->ClearTextVector();
    }
}

void CreateBackgroundSpriteMetadata(SDL_Texture* sdlBackgroundTexture, SDL_Rect& sdlBackgroundTextureSourceRect, SDL_Rect& sdlBackgroundTextureDestinationRect, const int iWindowHeight, const int iWindowWidth) {
    
    SDL_QueryTexture(sdlBackgroundTexture, NULL, NULL, &sdlBackgroundTextureSourceRect.w, &sdlBackgroundTextureSourceRect.h);
    sdlBackgroundTextureSourceRect.x = 0;
    sdlBackgroundTextureSourceRect.y = 0;
    sdlBackgroundTextureDestinationRect.x = (iWindowWidth / 2)  - ((iWindowHeight / 2.2) / 2);
    sdlBackgroundTextureDestinationRect.y = (iWindowHeight * 0.33) - ((iWindowHeight / 2.2) / 2);
    sdlBackgroundTextureDestinationRect.w = iWindowHeight / 2.2;
    sdlBackgroundTextureDestinationRect.h = iWindowHeight / 2.2;

}

void CreateHeartsBrokenSpriteMetadata(SDL_Texture* sdlHeartsBrokenTexture, SDL_Rect& sdlHeartsBrokenSourceRect, SDL_Rect& sdlHeartsBrokenDestinationRect, const int iWindowHeight, const int iWindowWidth) {
    
    SDL_QueryTexture(sdlHeartsBrokenTexture, NULL, NULL, &sdlHeartsBrokenSourceRect.w, &sdlHeartsBrokenSourceRect.h);
    sdlHeartsBrokenSourceRect.x = 0;
    sdlHeartsBrokenSourceRect.y = 0;
    sdlHeartsBrokenDestinationRect.x = 0;
    sdlHeartsBrokenDestinationRect.y = 0;
    sdlHeartsBrokenDestinationRect.w = iWindowHeight / 10.8;
    sdlHeartsBrokenDestinationRect.h = iWindowHeight / 10.8;
    sdlHeartsBrokenDestinationRect.x = iWindowWidth - sdlHeartsBrokenDestinationRect.w;

}

void CreateMenuSpriteMetadata(SDL_Texture* sdlMenuTexture, SDL_Rect& sdlMenuTextureSourceRect, SDL_Rect& sdlMenuTextureDestinationRect, const int iWindowHeight, const int iWindowWidth) {
    
    SDL_QueryTexture(sdlMenuTexture, NULL, NULL, &sdlMenuTextureSourceRect.w, &sdlMenuTextureSourceRect.h);
    sdlMenuTextureSourceRect.x = 0;
    sdlMenuTextureSourceRect.y = 0;
    sdlMenuTextureDestinationRect.w = iWindowHeight / 1.5;
    sdlMenuTextureDestinationRect.h = iWindowHeight / 1.5;
    sdlMenuTextureDestinationRect.x = (iWindowWidth / 3) - (sdlMenuTextureDestinationRect.w / 2);
    sdlMenuTextureDestinationRect.y = (iWindowHeight / 2) - (sdlMenuTextureDestinationRect.h / 2);

}

void PositionResumeGameText(WG::TextPane& wtpResumeGame) {

    wtpResumeGame.vwtlTextLines[0].sdlTextRect.x = ((wtpResumeGame.dWindowWidth / 3) * 2) - (wtpResumeGame.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpResumeGame.vwtlTextLines[0].sdlTextRect.y = (wtpResumeGame.dWindowHeight * 0.20);

}

void PositionExitGameText(WG::TextPane& wtpExitGame) {

    wtpExitGame.vwtlTextLines[0].sdlTextRect.x = ((wtpExitGame.dWindowWidth / 3) * 2) - (wtpExitGame.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpExitGame.vwtlTextLines[0].sdlTextRect.y = (wtpExitGame.dWindowHeight * 0.80);

}

void PositionHowToPlayText(WG::TextPane& wtpExitGame) {

    wtpExitGame.vwtlTextLines[0].sdlTextRect.x = ((wtpExitGame.dWindowWidth / 3) * 2) - (wtpExitGame.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpExitGame.vwtlTextLines[0].sdlTextRect.y = (wtpExitGame.dWindowHeight * 0.30);

}

void PositionMenuText(WG::TextPane& wtpMenuText, double dYRatio) {
    wtpMenuText.vwtlTextLines[0].sdlTextRect.x = ((wtpMenuText.dWindowWidth / 3) * 2) - (wtpMenuText.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpMenuText.vwtlTextLines[0].sdlTextRect.y = (wtpMenuText.dWindowHeight * dYRatio);
}


void PositionTooltipText(WG::TextPane& wtpMenuText) {
    wtpMenuText.vwtlTextLines[0].sdlTextRect.x = ((wtpMenuText.dWindowWidth / 2)) - (wtpMenuText.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpMenuText.vwtlTextLines[0].sdlTextRect.y = (wtpMenuText.dWindowHeight * 0.9);
}

void PositionPromptText(WG::TextPane& wtpPromptText) {
    wtpPromptText.vwtlTextLines[0].sdlTextRect.x = ((wtpPromptText.dWindowWidth / 2)) - (wtpPromptText.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpPromptText.vwtlTextLines[0].sdlTextRect.y = (wtpPromptText.dWindowHeight * 0.75);
}

void PositionSubtitleText(WG::TextPane& wtpSubtitleText) {
    wtpSubtitleText.vwtlTextLines[0].sdlTextRect.x = ((wtpSubtitleText.dWindowWidth / 2)) - (wtpSubtitleText.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpSubtitleText.vwtlTextLines[0].sdlTextRect.y = (wtpSubtitleText.dWindowHeight * 0.95);
}

void PopulateToolTip(WG::TextPane& wtpToolTip, std::string sTooltip, int iFontSize) {
    wtpToolTip.AddLine(sTooltip, iFontSize * 0.75, {255, 255, 255, 255});
    wtpToolTip.RefreshTextVectorTextures();
    PositionTooltipText(wtpToolTip);
}

void PopulateHowToPlay(WG::TextPane& wtpRules, int iFontSize) {

    wtpRules.AddLine("Starting Out:", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Frankenhearts is a trick taking evasion game in which the player avoids taking points by not 'winning' hearts or the Queen of Spades. Put simply, Frankenhearts is like golf,", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     whoever has the highest points loses.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Each round somebody 'leads' a card, and this suit becomes the 'trump suit' of that round. If you have the same suit as the trump suit in your hand you are forced to play a", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     card of that suit, if your card is worth more than the lead card, you win the hand and any hearts your opponents may have played.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     If you don't have a card of the trump suit you can play any card you like, including hearts or the Queen of Spades. This is where the game gets interesting.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    
    wtpRules.AddLine("Special Cards:", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Frankenhearts has four special cards:", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          1. The Queen of Spades is worth 13 points, just like vanilla hearts.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          2. The Jack of Diamonds is worth -10 points, but can't give you a score below zero at the end of a round.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          3. The Ten of Clubs doubles whatever points you have already, and halves the effectiveness of the Jack of Diamonds.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          4. Jokers are suitless cards, they cannot win tricks, and can be played in any suit. They cannot be lead unless they are the only card in your hand.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("Passing & Play:", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Before each round, each player passes four cards to their left, right the round after, and each third round there is no pass. The cycle then repeats.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Each round starts with the 2 of Clubs, whoever has it must lead it in order for play to begin. On the first trick (trick is another word for round) you can't play any hearts,", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     OR the Ten of Clubs, OR the Queen of Spades as point cards are not allowed on the first trick - so feel free to play your highest card without worry!", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     The next trick starts when the winner of the previous one leads, if you missed what happened in a trick, you can press [P] to see the previous trick.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("Nothing Breaks Like a Heart:", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     You aren't allowed to lead hearts until hearts has been 'broken', very poetic I know. There are only two ways in which hearts can be broken:", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          1. A heart is played by a player who has voided the current trick's void suit.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("          2. A player has lead, but has nothing but hearts. They are forced to play a heart and hearts is broken.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Once hearts are broken, you are free to play them whenever you want. If you are ever unsure if they're broken, look for the indicator in the top left corner.", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine(" ", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("Fly me to the Moon:", iFontSize, {255, 215, 0, 255});

    wtpRules.AddLine("     Frankenhearts isn't always an evasion game, sometimes throwing the game is the best strategy. If you win every single heart and the Queen of Spades, you'll have shot the moon.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     If you manage to achieve this you'll take zero points and both of your opponents will take 16 points each.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     If you take every heart, the Queen of Spades and the Ten of Clubs, the points you'd give to your opponents gets doubled for a total of 32. You'll still take zero points.", iFontSize, {255, 215, 0, 255});
    wtpRules.AddLine("     The Jack of Diamonds cannot protect you from a Moon/Venus shoot and it isn't nessecary to achieve one either.", iFontSize, {255, 215, 0, 255});



}

int RenderRulesMenu(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, bool bAccessibility) {

    const int iFontSizeHeading = iWindowHeight / 30;
    const int iFontSize = iWindowHeight / 50;

    bool bRenderMenu = true;
    int iReturnValue = FRANKENHEARTS_GAMEMENU_CLOSEMENU;

    //Prepare Text

    WG::TextPane wtpHeading(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.1, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpRules(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.12, 0.9, 0.6, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpExit(sdlRenderer, iWindowWidth, iWindowHeight, 0, 0, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpRulesMenuTextPanes = {&wtpHeading, &wtpRules, &wtpExit};
    AccessibilityMode(bAccessibility, vwtpRulesMenuTextPanes);

    wtpHeading.AddLine("RULES", iFontSizeHeading, {255, 215, 0, 255});
    wtpHeading.RefreshTextVectorTextures();

    //Centre heading pane, then centre how to text on the heading pane
    wtpHeading.dPaneX = (iWindowWidth / 2) - (wtpHeading.vwtlTextLines[0].sdlTextRect.w / 2);
    wtpRules.dPaneX = wtpHeading.dPaneX + (wtpHeading.vwtlTextLines[0].sdlTextRect.w / 2) - wtpRules.dPaneW / 2;
    wtpRules.sdlPaneRect.x = wtpRules.dPaneX;
    wtpHeading.RefreshTextVectorTextures(); 
    
    PopulateHowToPlay(wtpRules, iFontSize);
    wtpRules.RefreshTextVectorTextures();

    wtpExit.AddLine("RETURN", iFontSize, {255, 215, 0, 255});
    wtpExit.RefreshTextVectorTextures();
    SDL_QueryTexture(wtpExit.vwtlTextLines[0].sdlTextTexture , NULL, NULL, &wtpExit.vwtlTextLines[0].sdlTextRect.w, &wtpExit.vwtlTextLines[0].sdlTextRect.h);


    while (bRenderMenu) {

        //Handle User Input

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_GAMEMENU_EXITGAME;
                bRenderMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {

                    bRenderMenu = false;

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                    //Clicking on "return"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpExit.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_GAMEMENU_CLOSEMENU;
                        bRenderMenu = false;
                    }

                }
            }

        }

        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Text
        wtpHeading.RenderPaneText();
        wtpRules.RenderPaneText();
        wtpExit.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    ClearAllTextVectors(vwtpRulesMenuTextPanes);
    return iReturnValue;

}

std::string ReturnSettingTogglePosition(bool bCondition) {
    if (bCondition) {
        return "ON";
    } else {
        return "OFF";
    }
}


int UISettingsMenu(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, UISettings& usUISettings) {
    int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bUIMenu = true;

    //Prepare Text

    WG::TextPane wtpAccessibleFont(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpFontScale(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpCardSubtitles(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpPreviousCardsInTrick(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpShowPrompts(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpReturnToMainMenu(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpToolTip(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.0, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);


    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpAccessibleFont, &wtpFontScale, &wtpCardSubtitles, &wtpPreviousCardsInTrick, &wtpReturnToMainMenu, &wtpToolTip, &wtpShowPrompts};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    std::string sAccessibleFont = "Stylised Font: " + ReturnSettingTogglePosition(!usUISettings.bAccessible);
    wtpAccessibleFont.AddLine(sAccessibleFont, iFontSize, {255, 215, 0, 255});
    wtpAccessibleFont.RefreshTextVectorTextures();
    PositionMenuText(wtpAccessibleFont, 0.1);

    std::string sFontScale = "Scale UI Text: " + ReturnSettingTogglePosition(usUISettings.bUIScale);
    wtpFontScale.AddLine(sFontScale, iFontSize, {255, 215, 0, 255});
    wtpFontScale.RefreshTextVectorTextures();
    PositionMenuText(wtpFontScale, 0.2);

    std::string sCardSubtitles = "Card Subtitles: " + ReturnSettingTogglePosition(usUISettings.bCardSubtitles);
    wtpCardSubtitles.AddLine(sCardSubtitles, iFontSize, {255, 215, 0, 255});
    wtpCardSubtitles.RefreshTextVectorTextures();
    PositionMenuText(wtpCardSubtitles, 0.3);

    std::string sPreviousCardsInTrick = "Show Previous Trick: " + ReturnSettingTogglePosition(usUISettings.bPreviousCardsInTrick);
    wtpPreviousCardsInTrick.AddLine(sPreviousCardsInTrick, iFontSize, {255, 215, 0, 255});
    wtpPreviousCardsInTrick.RefreshTextVectorTextures();
    PositionMenuText(wtpPreviousCardsInTrick, 0.4);

    std::string sShowprompts = "Hints: " + ReturnSettingTogglePosition(usUISettings.bPrompts);
    wtpShowPrompts.AddLine(sShowprompts, iFontSize, {255, 215, 0, 255});
    wtpShowPrompts.RefreshTextVectorTextures();
    PositionMenuText(wtpShowPrompts, 0.5);

    wtpReturnToMainMenu.AddLine("Main Menu", iFontSize, {255, 215, 0, 255});
    wtpReturnToMainMenu.RefreshTextVectorTextures();
    PositionMenuText(wtpReturnToMainMenu, 0.8);

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bUIMenu) {

        //Poll mouse position

        int iMouseX, iMouseY;
        SDL_GetMouseState(&iMouseX, &iMouseY);
        SDL_Point sdlMousePosition;
        sdlMousePosition.x = iMouseX;
        sdlMousePosition.y = iMouseY;

        //Handle Tooltip

        //Hovering "Stylised Font"
        if (SDL_PointInRect(&sdlMousePosition, &wtpAccessibleFont.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Toggle stylish font, looks better but may be difficult to read", iFontSize);
        }

        //Hovering "Font UI"
        if (SDL_PointInRect(&sdlMousePosition, &wtpFontScale.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Increase or decrease the font size of most UI elements", iFontSize);
        }

        //Hovering "Card Subtitles"
        if (SDL_PointInRect(&sdlMousePosition, &wtpCardSubtitles.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "When a card is hovered on, its name will appear at the bottom of the screen", iFontSize);
        }

        //Hovering "Cards in previous trick"
        if (SDL_PointInRect(&sdlMousePosition, &wtpPreviousCardsInTrick.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Show the cards from the previous trick at the top of the screen", iFontSize);
        }

        //Hovering "Hints"
        if (SDL_PointInRect(&sdlMousePosition, &wtpShowPrompts.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Show hints that tell you what cards you're allowed to play", iFontSize);
        }

        //Hovering "Exit Game"
        if (SDL_PointInRect(&sdlMousePosition, &wtpReturnToMainMenu.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Return to Main Menu", iFontSize);
        }

        //Handle User Input

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                bUIMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                    bUIMenu = false;

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {

                    //Clicking on "Accessible"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpAccessibleFont.vwtlTextLines[0].sdlTextRect)) {

                        usUISettings.bAccessible = !usUISettings.bAccessible;

                        AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

                        wtpAccessibleFont.ClearTextVector();
                        sAccessibleFont = "Stylised Font: " + ReturnSettingTogglePosition(!usUISettings.bAccessible);
                        wtpAccessibleFont.AddLine(sAccessibleFont, iFontSize, {255, 215, 0, 255});
                        wtpAccessibleFont.RefreshTextVectorTextures();
                        PositionMenuText(wtpAccessibleFont, 0.1);

                        wtpFontScale.RefreshTextVectorTextures();
                        wtpPreviousCardsInTrick.RefreshTextVectorTextures();
                        wtpCardSubtitles.RefreshTextVectorTextures();
                        wtpShowPrompts.RefreshTextVectorTextures();
                        wtpReturnToMainMenu.RefreshTextVectorTextures();

                        PositionMenuText(wtpAccessibleFont, 0.1);
                        PositionMenuText(wtpFontScale, 0.2);
                        PositionMenuText(wtpCardSubtitles, 0.3);
                        PositionMenuText(wtpPreviousCardsInTrick, 0.4);
                        PositionMenuText(wtpShowPrompts, 0.5);
                        PositionMenuText(wtpReturnToMainMenu, 0.8);


                    }

                    //Clicking on "Font UI"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpFontScale.vwtlTextLines[0].sdlTextRect)) {
                        usUISettings.ChangeUIScale();

                        iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;
                        ChangeManyTextPanesFontSize(vwtpSettingsMenuTextPanes, iFontSize);

                        PositionMenuText(wtpAccessibleFont, 0.1);
                        PositionMenuText(wtpFontScale, 0.2);
                        PositionMenuText(wtpCardSubtitles, 0.3);
                        PositionMenuText(wtpPreviousCardsInTrick, 0.4);
                        PositionMenuText(wtpShowPrompts, 0.5);
                        PositionMenuText(wtpReturnToMainMenu, 0.8);

                        wtpFontScale.ClearTextVector();
                        sFontScale = "Scale UI Text: " + ReturnSettingTogglePosition(usUISettings.bUIScale);
                        wtpFontScale.AddLine(sFontScale, iFontSize, {255, 215, 0, 255});
                        wtpFontScale.RefreshTextVectorTextures();
                        PositionMenuText(wtpFontScale, 0.2);

                        wtpToolTip.ClearTextVector();

                    }

                    //Clicking on Card Subtitles
                    if (SDL_PointInRect(&sdlMousePosition, &wtpCardSubtitles.vwtlTextLines[0].sdlTextRect)) {
                        usUISettings.bCardSubtitles = !usUISettings.bCardSubtitles;

                        wtpCardSubtitles.ClearTextVector();
                        sCardSubtitles = "Card Subtitles: " + ReturnSettingTogglePosition(usUISettings.bCardSubtitles);
                        wtpCardSubtitles.AddLine(sCardSubtitles, iFontSize, {255, 215, 0, 255});
                        wtpCardSubtitles.RefreshTextVectorTextures();
                        PositionMenuText(wtpCardSubtitles, 0.3);
                        
                    }

                    //Clicking on "Previous Cards in Trick"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpPreviousCardsInTrick.vwtlTextLines[0].sdlTextRect)) {
                        usUISettings.bPreviousCardsInTrick = !usUISettings.bPreviousCardsInTrick;

                        wtpPreviousCardsInTrick.ClearTextVector();
                        std::string sPreviousCardsInTrick = "Show Previous Trick: " + ReturnSettingTogglePosition(usUISettings.bPreviousCardsInTrick);
                        wtpPreviousCardsInTrick.AddLine(sPreviousCardsInTrick, iFontSize, {255, 215, 0, 255});
                        wtpPreviousCardsInTrick.RefreshTextVectorTextures();
                        PositionMenuText(wtpPreviousCardsInTrick, 0.4);
                        
                    }

                    //Clicking on "Hints"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpShowPrompts.vwtlTextLines[0].sdlTextRect)) {
                        usUISettings.bPrompts = !usUISettings.bPrompts;

                        wtpShowPrompts.ClearTextVector();
                        sShowprompts = "Hints: " + ReturnSettingTogglePosition(usUISettings.bPrompts);
                        wtpShowPrompts.AddLine(sShowprompts, iFontSize, {255, 215, 0, 255});
                        wtpShowPrompts.RefreshTextVectorTextures();
                        PositionMenuText(wtpShowPrompts, 0.5);
                    
                    }

                    //Clicking on "Exit Game"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpReturnToMainMenu.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                        bUIMenu = false;
                    }

                }
            }

        }


        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Images
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Text
        wtpAccessibleFont.RenderPaneText();
        wtpFontScale.RenderPaneText();
        wtpCardSubtitles.RenderPaneText();
        wtpPreviousCardsInTrick.RenderPaneText();
        wtpShowPrompts.RenderPaneText();
        wtpReturnToMainMenu.RenderPaneText();

        wtpToolTip.RenderPaneText();
        wtpToolTip.ClearTextVector();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }


    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;
}


int RenderRootGameMenu(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, UISettings& usUISettings) {

    int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bRootMenu = true;

    //Prepare Text

    WG::TextPane wtpResumeGame(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpExitGame(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpRules(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpUISettings(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.5, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpToolTip(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.0, 0.0, 0.0, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpResumeGame, &wtpExitGame, &wtpRules, &wtpUISettings, &wtpToolTip};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    wtpResumeGame.AddLine("RESUME GAME", iFontSize, {255, 215, 0, 255});
    wtpResumeGame.RefreshTextVectorTextures();
    PositionResumeGameText(wtpResumeGame);

    wtpExitGame.AddLine("EXIT GAME", iFontSize, {255, 215, 0, 255});
    wtpExitGame.RefreshTextVectorTextures();
    PositionExitGameText(wtpExitGame);

    wtpRules.AddLine("RULES", iFontSize, {255, 215, 0, 255});
    wtpRules.RefreshTextVectorTextures();
    PositionHowToPlayText(wtpRules);

    wtpUISettings.AddLine("UI/GRAPHICS SETTINGS", iFontSize, {255, 215, 0, 255});
    wtpUISettings.RefreshTextVectorTextures();
    PositionMenuText(wtpUISettings, 0.4);

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bRootMenu) {

        //Handle User Input

        //Poll mouse position

        int iMouseX, iMouseY;
        SDL_GetMouseState(&iMouseX, &iMouseY);
        SDL_Point sdlMousePosition;
        sdlMousePosition.x = iMouseX;
        sdlMousePosition.y = iMouseY;

        //Handle Tooltip

        //Hovering "Resume"
        if (SDL_PointInRect(&sdlMousePosition, &wtpResumeGame.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Return to game", iFontSize);
        }

        //Hovering "Exit"
        if (SDL_PointInRect(&sdlMousePosition, &wtpExitGame.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Disconnect from the current game and return to main menu", iFontSize);
        }

        //Hovering "Rules"
        if (SDL_PointInRect(&sdlMousePosition, &wtpRules.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "See an overview of the game's rules", iFontSize);
        }

        //Hovering "UI Settings"
        if (SDL_PointInRect(&sdlMousePosition, &wtpUISettings.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Change UI settings, such as font and UI elements", iFontSize);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_GAMEMENU_EXITGAME;
                bRootMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {

                    bRootMenu = false;

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {

                    //Clicking on "Resume Game"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpResumeGame.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_GAMEMENU_CLOSEMENU;
                        bRootMenu = false;
                    }

                    //Clicking on "Exit Game"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpExitGame.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_GAMEMENU_EXITGAME;
                        bRootMenu = false;
                    }

                    //Clicking on "Rules"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpRules.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = RenderRulesMenu(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings.bAccessible);
                        if (iReturnValue == FRANKENHEARTS_GAMEMENU_EXITGAME) {
                            bRootMenu = false;
                        }
                    }

                    //Click on "UI Settings"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpUISettings.vwtlTextLines[0].sdlTextRect)) {

                        UISettingsMenu(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings);
                        AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);
                        iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

                        ChangeManyTextPanesFontSize(vwtpSettingsMenuTextPanes, iFontSize);

                        PositionResumeGameText(wtpResumeGame);
                        PositionExitGameText(wtpExitGame);
                        PositionHowToPlayText(wtpRules);
                        PositionMenuText(wtpUISettings, 0.4);
                        PositionTooltipText(wtpToolTip);

                    }

                }
            }

        }


        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Images
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Text
        wtpResumeGame.RenderPaneText();
        wtpRules.RenderPaneText();
        wtpUISettings.RenderPaneText();
        wtpExitGame.RenderPaneText();

        wtpToolTip.RenderPaneText();
        wtpToolTip.ClearTextVector();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }


    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;

}

int UsernameMenu(SDL_Renderer* sdlRenderer, std::string& sCurrentUsername, int iWindowWidth, int iWindowHeight, const UISettings& usUISettings) {

    const int iCharacterLimit = 22;

    bool bInputting = true;

    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    int iReturnValue = 0;
    
    std::string sInput;

    //CreatePanes

    WG::TextPane wtpPromptPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.45, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpInputPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.5, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpTextLengthPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.52, 0.55, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpUsernameMenuTextPanes = {&wtpPromptPane, &wtpInputPane, &wtpTextLengthPane};
    AccessibilityMode(usUISettings.bAccessible, vwtpUsernameMenuTextPanes);

    //Populate Static Panes
    wtpPromptPane.AddLine("ENTER NICKNAME:", iFontSize * 1.1, {255, 255, 255, 255});
    wtpPromptPane.RefreshTextVectorTextures();

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bInputting) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_TEXTINPUT) {
                if (sInput.size() == iCharacterLimit) {
                    wtpTextLengthPane.ClearTextVector();
                    wtpTextLengthPane.AddLine("Nicknames must be 22 characters or less!", iFontSize / 2, {255, 255, 255, 255});
                    wtpTextLengthPane.RefreshTextVectorTextures();
                } else {
                    wtpTextLengthPane.ClearTextVector();
                    sInput += event.text.text;
                }
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    bInputting = false;
                    iReturnValue = 1;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (sInput.size() > 0) {
                        sInput.pop_back();
                    }
                }

                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (ValidatePlayerNickname(sInput)) {
                        sCurrentUsername = sInput;
                        bInputting = false;
                        iReturnValue = 0;
                    } else {
                        wtpTextLengthPane.ClearTextVector();
                        wtpTextLengthPane.AddLine("Invalid Username", iFontSize / 2, {255, 255, 255, 255});
                        wtpTextLengthPane.RefreshTextVectorTextures();
                    }
                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                }
            }

        }

        //Easter Egg

        if (sInput == "DevKiller") {
            wtpTextLengthPane.ClearTextVector();
            wtpTextLengthPane.AddLine("Are you sure about that?", iFontSize / 2, {255, 255, 255, 255});
            wtpTextLengthPane.RefreshTextVectorTextures();
        }

        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Image
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Prepare and Input
        wtpInputPane.ClearTextVector();
        wtpInputPane.AddLine(sInput, iFontSize, GetDefaultFontColour());
        wtpInputPane.RefreshTextVectorTextures();
        wtpInputPane.RenderPaneText();

        //Render Prompts
        wtpPromptPane.RenderPaneText();
        wtpTextLengthPane.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpUsernameMenuTextPanes);
    return iReturnValue;

}

int IPAddressMenu(SDL_Renderer* sdlRenderer, std::string& sIPAddr, int iWindowWidth, int iWindowHeight, const UISettings& usUISettings) {

    const int iCharacterLimit = 45;

    bool bInputting = true;

    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    int iReturnValue = 0;
    
    std::string sInput;

    //CreatePanes

    WG::TextPane wtpPromptPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.45, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpInputPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.5, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpTextLengthPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.52, 0.55, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpInformationPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpIPAddressMenuTextPanes = {&wtpPromptPane, &wtpInputPane, &wtpTextLengthPane, &wtpInformationPane};
    AccessibilityMode(usUISettings.bAccessible, vwtpIPAddressMenuTextPanes);

    //Populate Static Panes
    wtpPromptPane.AddLine("ENTER SERVER IP ADDRESS:", iFontSize * 1.1, {255, 255, 255, 255});
    wtpPromptPane.RefreshTextVectorTextures();

    wtpInformationPane.AddLine("Need Help?", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("The server IP address is your server host's public IP address, they look a little like this:", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("43.250.142.140", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("Your server host will need to provide you with this address.", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.RefreshTextVectorTextures();

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bInputting) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_TEXTINPUT) {
                if (sInput.size() == iCharacterLimit) {
                    wtpTextLengthPane.ClearTextVector();
                    wtpTextLengthPane.AddLine("IP Address beyond accepted maxium!", iFontSize / 2, {255, 255, 255, 255});
                    wtpTextLengthPane.RefreshTextVectorTextures();
                } else {
                    wtpTextLengthPane.ClearTextVector();
                    sInput += event.text.text;
                }
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    bInputting = false;
                    iReturnValue = 1;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (sInput.size() > 0) {
                        sInput.pop_back();
                    }
                }

                if (event.key.keysym.sym == SDLK_RETURN) {
                    bInputting = false;
                    iReturnValue = 0;
                    sIPAddr = sInput;
                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                }
            }

        }

        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Image
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Prepare and Input
        wtpInputPane.ClearTextVector();
        wtpInputPane.AddLine(sInput, iFontSize, GetDefaultFontColour());
        wtpInputPane.RefreshTextVectorTextures();
        wtpInputPane.RenderPaneText();

        //Render Prompts
        wtpPromptPane.RenderPaneText();
        wtpTextLengthPane.RenderPaneText();
        wtpInformationPane.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    ClearAllTextVectors(vwtpIPAddressMenuTextPanes);
    SDL_DestroyTexture(sdlMenuTexture);
    return iReturnValue;

}

int IPPortMenu(SDL_Renderer* sdlRenderer, std::string& sPort, int iWindowWidth, int iWindowHeight, UISettings usUISettings) {

    const int iCharacterLimit = 5;

    bool bInputting = true;

    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    int iReturnValue = 0;
    
    std::string sInput;

    //CreatePanes

    WG::TextPane wtpPromptPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.45, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpInputPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.5, 0.5, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpTextLengthPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.52, 0.55, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpInformationPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpIPPortMenuTextPanes = {&wtpPromptPane, &wtpInputPane, &wtpTextLengthPane, &wtpInformationPane};
    AccessibilityMode(usUISettings.bAccessible, vwtpIPPortMenuTextPanes);

    //Populate Static Panes
    wtpPromptPane.AddLine("ENTER SERVER PORT:", iFontSize * 1.1, {255, 255, 255, 255});
    wtpPromptPane.RefreshTextVectorTextures();

    wtpInformationPane.AddLine("Need Help?", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("The server port is the port the server host has set Frankenhearts to, it looks like this:", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("8080", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.AddLine("8080 is the default port, unless your server host can deliberately changed it, it will be 8080.", iFontSize / 2, {255, 255, 255, 255});
    wtpInformationPane.RefreshTextVectorTextures();

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bInputting) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_TEXTINPUT) {
                if (sInput.size() == iCharacterLimit) {
                    wtpTextLengthPane.ClearTextVector();
                    wtpTextLengthPane.AddLine("A port cannot be more than five numbers long!", iFontSize / 2, {255, 255, 255, 255});
                    wtpTextLengthPane.RefreshTextVectorTextures();
                } else {
                    wtpTextLengthPane.ClearTextVector();
                    sInput += event.text.text;
                }
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    bInputting = false;
                    iReturnValue = 1;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (sInput.size() > 0) {
                        sInput.pop_back();
                    }
                }

                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (ValidatePort(sInput)) {
                        bInputting = false;
                        iReturnValue = 0;
                        sPort = sInput;
                    } else {
                        wtpTextLengthPane.ClearTextVector();
                        wtpTextLengthPane.AddLine("A port must be a number!", iFontSize / 2, {255, 255, 255, 255});
                        wtpTextLengthPane.RefreshTextVectorTextures();
                    }

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                }
            }

        }

        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Image
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Prepare and Input
        wtpInputPane.ClearTextVector();
        wtpInputPane.AddLine(sInput, iFontSize, GetDefaultFontColour());
        wtpInputPane.RefreshTextVectorTextures();
        wtpInputPane.RenderPaneText();

        //Render Prompts
        wtpPromptPane.RenderPaneText();
        wtpTextLengthPane.RenderPaneText();
        wtpInformationPane.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }

    ClearAllTextVectors(vwtpIPPortMenuTextPanes);
    SDL_DestroyTexture(sdlMenuTexture);
    return iReturnValue;

}

int ConnectionMenu(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, std::string& sIPAddress, std::string& sPort, const UISettings& usUISettings) {
    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bConnectionMenu = true;

    //Prepare Text

    WG::TextPane wtpIPAddress(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpPort(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpExitGame(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpIPAddress, &wtpPort, &wtpExitGame};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    wtpIPAddress.AddLine("SET SERVER IP", iFontSize, {255, 215, 0, 255});
    wtpIPAddress.RefreshTextVectorTextures();
    PositionMenuText(wtpIPAddress, 0.1);

    wtpPort.AddLine("SET SERVER PORT", iFontSize, {255, 215, 0, 255});
    wtpPort.RefreshTextVectorTextures();
    PositionMenuText(wtpPort, 0.2);

    wtpExitGame.AddLine("MAIN MENU", iFontSize, {255, 215, 0, 255});
    wtpExitGame.RefreshTextVectorTextures();
    PositionMenuText(wtpExitGame, 0.8);

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bConnectionMenu) {

        //Handle User Input

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                bConnectionMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    iReturnValue = FRANKENHEARTS_CONNECTIONMENU_EXIT;
                    bConnectionMenu = false;
                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                    //Clicking on "Set Server IP"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpIPAddress.vwtlTextLines[0].sdlTextRect)) {
                        IPAddressMenu(sdlRenderer, sIPAddress, iWindowWidth, iWindowHeight, usUISettings);
                    }

                    //Clicking on "Set Server Port"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpPort.vwtlTextLines[0].sdlTextRect)) {
                        IPPortMenu(sdlRenderer, sPort, iWindowWidth, iWindowHeight, usUISettings);
                    }

                    //Clicking on "Main Menu"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpExitGame.vwtlTextLines[0].sdlTextRect)) {
                        bConnectionMenu = false;
                        iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                    }

                }
            }

        }


        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Images
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Text
        wtpIPAddress.RenderPaneText();
        wtpPort.RenderPaneText();
        wtpExitGame.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }


    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;
}

int ConnectionFailed(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, const UISettings& usUISettings) {

    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bConnectionFailureMenu = true;

    //Prepare Text

    WG::TextPane wtpInformationPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.4, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpReturnToMenu(sdlRenderer, iWindowWidth, iWindowHeight, 0.1, 0.1, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpInformationPane, &wtpReturnToMenu};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    int iSecondaryFontSize = iFontSize / 2;
    wtpInformationPane.AddLine("Connection Failed!", iFontSize, GetDefaultFontColour());
    wtpInformationPane.AddLine("Your Frankenhearts Client cannot connect to the specified server!", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("", iFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("Common Connection Solutions:", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     Ensure you are connected to the internet.", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     Double check the Server's IP and Port, do not include spaces or symbols other than '.'", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     The server host may have failed to correctly port forward (if not on a local network).", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     The server needs to be started BEFORE clients can connect, ensure your host is ready.", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     Ensure you have the latest version from Frankenhearts.com.", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.RefreshTextVectorTextures();

    wtpReturnToMenu.AddLine("RETURN TO MAIN MENU", iFontSize, GetDefaultFontColour()); 
    wtpReturnToMenu.RefreshTextVectorTextures();
    PositionMenuText(wtpReturnToMenu, 0.8);

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bConnectionFailureMenu) {

        //Handle User Input

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                bConnectionFailureMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                    bConnectionFailureMenu = false;

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {
                    int iMouseX, iMouseY;
                    SDL_GetMouseState(&iMouseX, &iMouseY);
                    SDL_Point sdlMousePosition;
                    sdlMousePosition.x = iMouseX;
                    sdlMousePosition.y = iMouseY;

                    //Clicking on "Exit Game"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpReturnToMenu.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                        bConnectionFailureMenu = false;
                    }

                }
            }

        }


        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Images
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Text
        wtpInformationPane.RenderPaneText();
        wtpReturnToMenu.RenderPaneText();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }


    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;

}

int ConnectingScreen(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, const UISettings& usUISettings) {

    const int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bConnectingMenu = true;

    //Prepare Text

    WG::TextPane wtpInformationPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.4, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);


    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpInformationPane};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    int iSecondaryFontSize = iFontSize / 2;
    wtpInformationPane.AddLine("Connecting . . .", iFontSize, GetDefaultFontColour());
    wtpInformationPane.AddLine("Your Frankenhearts Client is attempting to connect to the specified server!", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.AddLine("     This should take less than a minute", iSecondaryFontSize, {255, 255, 255, 255});
    wtpInformationPane.RefreshTextVectorTextures();

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);



    //Perform Rendering

    SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
    SDL_RenderClear(sdlRenderer);

    //Render Images
    SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

    //Render Text
    wtpInformationPane.RenderPaneText();

    SDL_RenderPresent(sdlRenderer);
    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;

}

void PopulateCurrentInfo(WG::TextPane& wtpCurrentInfo, std::string& sUsername, std::string& sIPAddress, std::string& sPort, int iFontSize) {
    wtpCurrentInfo.ClearTextVector();
    std::string sCurrentUsername = "Username: " + sUsername;
    std::string sCurrentIP = "Server IP: " + sIPAddress;
    std::string sCurrentPort = "Server Port: " + sPort;
    wtpCurrentInfo.AddLine(sCurrentUsername, iFontSize / 2, {255, 255, 255, 255});
    wtpCurrentInfo.AddLine(sCurrentIP, iFontSize / 2, {255, 255, 255, 255});
    wtpCurrentInfo.AddLine(sCurrentPort, iFontSize / 2, {255, 255, 255, 255});
    wtpCurrentInfo.RefreshTextVectorTextures();
}

int MainMenu(SDL_Renderer* sdlRenderer, int iWindowWidth, int iWindowHeight, ClientSocket*& csClientSocket, std::string& sUsername, std::string& sIPAddress, std::string& sPort, UISettings& usUISettings) {

    int iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

    //Prepare Functional Data

    int iReturnValue = 0;

    bool bMainMenu = true;

    //Prepare Text

    WG::TextPane wtpConnect(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpSetUsername(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpConnection(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpUISettings(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.5, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
    WG::TextPane wtpExitGame(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpCurrentInfo(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.0, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);

    WG::TextPane wtpToolTip(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.0, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);


    std::vector<WG::TextPane*> vwtpSettingsMenuTextPanes = {&wtpConnect, &wtpSetUsername, &wtpConnection, &wtpUISettings, &wtpExitGame, &wtpCurrentInfo, &wtpToolTip};
    AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);

    wtpConnect.AddLine("CONNECT", iFontSize, {255, 215, 0, 255});
    wtpConnect.RefreshTextVectorTextures();
    PositionMenuText(wtpConnect, 0.1);

    wtpSetUsername.AddLine("PROFILE", iFontSize, {255, 215, 0, 255});
    wtpSetUsername.RefreshTextVectorTextures();
    PositionMenuText(wtpSetUsername, 0.3);

    wtpConnection.AddLine("CONFIGURE CONNECTION", iFontSize, {255, 215, 0, 255});
    wtpConnection.RefreshTextVectorTextures();
    PositionMenuText(wtpConnection, 0.2);

    wtpUISettings.AddLine("UI/GRAPHICS SETTINGS", iFontSize, {255, 215, 0, 255});
    wtpUISettings.RefreshTextVectorTextures();
    PositionMenuText(wtpUISettings, 0.4);

    wtpExitGame.AddLine("EXIT GAME", iFontSize, {255, 215, 0, 255});
    wtpExitGame.RefreshTextVectorTextures();
    PositionMenuText(wtpExitGame, 0.8);

    PopulateCurrentInfo(wtpCurrentInfo, sUsername, sIPAddress, sPort, iFontSize);

    //Prepare Sprite

    SDL_Texture* sdlMenuTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Menu.png");
    SDL_Rect sdlMenuTextureSourceRect;
    SDL_Rect sdlMenuTextureDestinationRect;
    CreateMenuSpriteMetadata(sdlMenuTexture, sdlMenuTextureSourceRect, sdlMenuTextureDestinationRect, iWindowHeight, iWindowWidth);

    while (bMainMenu) {

        //Poll mouse position

        int iMouseX, iMouseY;
        SDL_GetMouseState(&iMouseX, &iMouseY);
        SDL_Point sdlMousePosition;
        sdlMousePosition.x = iMouseX;
        sdlMousePosition.y = iMouseY;

        //Handle Tooltip

        //Hovering "Connect"
        if (SDL_PointInRect(&sdlMousePosition, &wtpConnect.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Connect to a server and play Frankenhearts Multiplayer", iFontSize);
        }

        //Hovering "Connection Settings"
        if (SDL_PointInRect(&sdlMousePosition, &wtpConnection.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Set the IP address and Port of your host's server", iFontSize);
        }

        //Hovering "Profile"
        if (SDL_PointInRect(&sdlMousePosition, &wtpSetUsername.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Change your in game nickname", iFontSize);
        }

        //Hovering "UI Settings"
        if (SDL_PointInRect(&sdlMousePosition, &wtpUISettings.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Change UI settings, such as font and UI elements", iFontSize);
        }

        //Hovering "Exit Game"
        if (SDL_PointInRect(&sdlMousePosition, &wtpExitGame.vwtlTextLines[0].sdlTextRect)) {
            PopulateToolTip(wtpToolTip, "Return to Windows", iFontSize);
        }

        //Handle User Input

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                bMainMenu = false;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                    bMainMenu = false;

                }

            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {

                if (event.button.button == SDL_BUTTON_LEFT) {

                    //Clicking on "Connect"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpConnect.vwtlTextLines[0].sdlTextRect)) {
                        ConnectingScreen(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings);
                        if (CreateClientSocket(csClientSocket, sUsername, sIPAddress, sPort)) {
                            iReturnValue = FRANKENHEARTS_MAINMENU_CONNECT;
                            bMainMenu = false;
                        } else {
                            ConnectionFailed(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings);
                        }
                    }

                    //Clicking on "Connection Settings"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpConnection.vwtlTextLines[0].sdlTextRect)) {
                        ConnectionMenu(sdlRenderer, iWindowWidth, iWindowHeight, sIPAddress, sPort, usUISettings);
                        PopulateCurrentInfo(wtpCurrentInfo, sUsername, sIPAddress, sPort, iFontSize);
                    }

                    //Clicking on "Profile"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpSetUsername.vwtlTextLines[0].sdlTextRect)) {
                        UsernameMenu(sdlRenderer, sUsername, iWindowWidth, iWindowHeight, usUISettings);
                        PopulateCurrentInfo(wtpCurrentInfo, sUsername, sIPAddress, sPort, iFontSize);
                    }

                    //Click on "UI Settings"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpUISettings.vwtlTextLines[0].sdlTextRect)) {

                        UISettingsMenu(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings);
                        AccessibilityMode(usUISettings.bAccessible, vwtpSettingsMenuTextPanes);
                        iFontSize = iWindowHeight / 30 * usUISettings.dUIScale;

                        ChangeManyTextPanesFontSize(vwtpSettingsMenuTextPanes, iFontSize);

                        PositionMenuText(wtpConnect, 0.1);
                        PositionMenuText(wtpSetUsername, 0.3);
                        PositionMenuText(wtpConnection, 0.2);
                        PositionMenuText(wtpUISettings, 0.4);
                        PositionMenuText(wtpExitGame, 0.8);

                        PositionTooltipText(wtpToolTip);

                        PopulateCurrentInfo(wtpCurrentInfo, sUsername, sIPAddress, sPort, iFontSize);

                    }

                    //Clicking on "Exit Game"
                    if (SDL_PointInRect(&sdlMousePosition, &wtpExitGame.vwtlTextLines[0].sdlTextRect)) {
                        iReturnValue = FRANKENHEARTS_MAINMENU_EXITGAME;
                        bMainMenu = false;
                    }

                }
            }

        }


        //Perform Rendering

        SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
        SDL_RenderClear(sdlRenderer);

        //Render Images
        SDL_RenderCopy(sdlRenderer, sdlMenuTexture, &sdlMenuTextureSourceRect, &sdlMenuTextureDestinationRect);

        //Render Text
        wtpConnect.RenderPaneText();
        wtpSetUsername.RenderPaneText();
        wtpConnection.RenderPaneText();
        wtpUISettings.RenderPaneText();
        wtpExitGame.RenderPaneText();
        wtpCurrentInfo.RenderPaneText();

        wtpToolTip.RenderPaneText();
        wtpToolTip.ClearTextVector();

        SDL_RenderPresent(sdlRenderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    }


    SDL_DestroyTexture(sdlMenuTexture);
    ClearAllTextVectors(vwtpSettingsMenuTextPanes);

    return iReturnValue;

}

void ClearPositionVectors(std::vector<CardPositionTracker>& vctCardsInTrickPositions, std::vector<CardPositionTracker>& vctCardsInPreviousTrick, std::vector<CardPositionTracker>& vctCardsRecievedInPass) {
    vctCardsInPreviousTrick.clear();
    vctCardsInTrickPositions.clear();
    vctCardsRecievedInPass.clear();
}

void CreateCardSubtitles(
    std::vector<Card>& vcPlayerDeck, 
    std::vector<CardPositionTracker>& vctCardsInTrickPositions, 
    std::vector<CardPositionTracker>& vctCardsInPreviousTrick, 
    std::vector<CardPositionTracker>& vctCardsRecievedInPass,
    int iMouseX, 
    int iMouseY, 
    WG::TextPane& wtpSubtitlePane, 
    UISettings& usUISettings) {

    SDL_Point sdlMousePoint = {iMouseX, iMouseY};

    int iFontSize = wtpSubtitlePane.dWindowHeight / 35 * usUISettings.dUIScale;

    if (!usUISettings.bCardSubtitles) {
        return;
    }

    for (int i = 0; i < vcPlayerDeck.size(); i++) {
        if (iMouseX > vcPlayerDeck[i].sdlPositionRect.x && iMouseX < vcPlayerDeck[i].sdlPositionRect.x + vcPlayerDeck[i].sdlPositionRect.w && iMouseY > vcPlayerDeck[i].sdlPositionRect.y && iMouseY < vcPlayerDeck[i].sdlPositionRect.y + vcPlayerDeck[i].sdlPositionRect.h) {
            wtpSubtitlePane.AddLine(CardSubtitles[vcPlayerDeck[i].sCardID], iFontSize, {255, 255, 255, 255});
            wtpSubtitlePane.RefreshTextVectorTextures();
            PositionSubtitleText(wtpSubtitlePane);
            wtpSubtitlePane.RenderPaneText();
            wtpSubtitlePane.ClearTextVector();
            ClearPositionVectors(vctCardsInTrickPositions, vctCardsInPreviousTrick, vctCardsRecievedInPass);
            return;
        }
    }

    for (int i = 0; i < vctCardsInTrickPositions.size(); i++) {
        if (SDL_PointInRect(&sdlMousePoint, &vctCardsInTrickPositions[i].sdlRectCurrentPosition)) {
            wtpSubtitlePane.AddLine(CardSubtitles[vctCardsInTrickPositions[i].sCardID], iFontSize, {255, 255, 255, 255});
            wtpSubtitlePane.RefreshTextVectorTextures();
            PositionSubtitleText(wtpSubtitlePane);
            wtpSubtitlePane.RenderPaneText();
            wtpSubtitlePane.ClearTextVector();
            ClearPositionVectors(vctCardsInTrickPositions, vctCardsInPreviousTrick, vctCardsRecievedInPass);
            return;
        }
    }

    for (int i = 0; i < vctCardsInPreviousTrick.size(); i++) {
        if (SDL_PointInRect(&sdlMousePoint, &vctCardsInPreviousTrick[i].sdlRectCurrentPosition)) {
            wtpSubtitlePane.AddLine(CardSubtitles[vctCardsInPreviousTrick[i].sCardID], iFontSize, {255, 255, 255, 255});
            wtpSubtitlePane.RefreshTextVectorTextures();
            PositionSubtitleText(wtpSubtitlePane);
            wtpSubtitlePane.RenderPaneText();
            wtpSubtitlePane.ClearTextVector();
            ClearPositionVectors(vctCardsInTrickPositions, vctCardsInPreviousTrick, vctCardsRecievedInPass);
            return;
        }
    }

    for (int i = 0; i < vctCardsRecievedInPass.size(); i++) {
        if (SDL_PointInRect(&sdlMousePoint, &vctCardsRecievedInPass[i].sdlRectCurrentPosition)) {
            wtpSubtitlePane.AddLine(CardSubtitles[vctCardsRecievedInPass[i].sCardID], iFontSize, {255, 255, 255, 255});
            wtpSubtitlePane.RefreshTextVectorTextures();
            PositionSubtitleText(wtpSubtitlePane);
            wtpSubtitlePane.RenderPaneText();
            wtpSubtitlePane.ClearTextVector();
            ClearPositionVectors(vctCardsInTrickPositions, vctCardsInPreviousTrick, vctCardsRecievedInPass);
            return;
        }
    }

}

void RenderPrompt(WG::TextPane& wtpPromptText, int iPrompt, const UISettings& usUISettings) {

    if (!usUISettings.bPrompts) {
        return;
    }

    int iFontSize = wtpPromptText.dWindowHeight / 41.5 * usUISettings.dUIScale;

    switch (iPrompt) {

        case FRANKENHEARTS_PROMPT_FIRSTTRICK_LEAD:
        wtpPromptText.AddLine("Lead the Two of Clubs", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY:
        wtpPromptText.AddLine("Play a Club (excluding the Ten of Clubs) or a Joker", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY_VOID:
        wtpPromptText.AddLine("Play any non-point Card", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_LEAD_HEARTSBROKEN:
        wtpPromptText.AddLine("Lead any Card - You may lead a Joker if you only have Jokers", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_LEAD_HEARTSUNBROKEN:
        wtpPromptText.AddLine("Lead any Card - Except Hearts", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_CLUBS:
        wtpPromptText.AddLine("Play a Club or a Joker", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_SPADES:
        wtpPromptText.AddLine("Play a Spade or a Joker", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_DIAMONDS:
        wtpPromptText.AddLine("Play a Diamond or a Joker", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_HEARTS:
        wtpPromptText.AddLine("Play a Heart or a Joker", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_VOID:
        wtpPromptText.AddLine("You have voided this suit - Play any Card", iFontSize, {255, 255, 255, 255});
        break;

        case FRANKENHEARTS_PROMPT_PLAY_JOKER:
        wtpPromptText.AddLine("If there are only Jokers on the table - Play any Card - Otherwise follow Suit", iFontSize, {255, 255, 255, 255});
        break;

        default:
        wtpPromptText.ClearTextVector();
        return;
        break;

    }

    wtpPromptText.RefreshTextVectorTextures();
    PositionPromptText(wtpPromptText);
    wtpPromptText.RenderPaneText();
    wtpPromptText.ClearTextVector();

}

bool HandleChatInput(WG::TextPane& wtpChatInputPane, std::string& sMessage) {

    int iFontSize = wtpChatInputPane.dWindowWidth / 20;

    WG::TextPane wtpChatFeedbackPane(wtpChatInputPane.sdlRenderer, wtpChatInputPane.dWindowWidth, wtpChatInputPane.dWindowHeight, 0.7, 0.2, 0.0, 0.0, FRANKENHEARTS_ACCESSIBLEFONT);

    bool bInputting;

    std::string sInput = "";
    int iCharacterLimit = 60;

    while (bInputting) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_TEXTINPUT) {
                if (sInput.size() == iCharacterLimit) {
                    wtpChatFeedbackPane.ClearTextVector();
                    wtpChatFeedbackPane.AddLine("Nicknames must be 22 characters or less!", iFontSize / 2, {255, 255, 255, 255});
                    wtpChatFeedbackPane.RefreshTextVectorTextures();
                } else {
                    wtpChatFeedbackPane.ClearTextVector();
                    sInput += event.text.text;
                }
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    bInputting = false;
                    return false;
                }

                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    if (sInput.size() > 0) {
                        sInput.pop_back();
                    }
                }

                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (sInput.size() > 0) {
                        sMessage = sInput;
                        return true;
                    }
                }

            }

        }

    }

    return false;

}


