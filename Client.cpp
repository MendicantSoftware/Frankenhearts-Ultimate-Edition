#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <functional>
#include <vector>
#include <atomic>
#include <windows.h>
#include <cstring> 

#include "ClientSocket.h"

#include "SIMP.h"
#include "ServerDebug.h"

#include "DeckClient.h"
#include "ClientPlayer.h"

#include "WolfgangTile.h"
#include "WolfgangDeltaTime.h"

#include "Graphics.h"
#include "ClientAchievementRenderer.h"
#include "WolfgangPane.h"


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


void DisplayCard(std::atomic<bool>& bClientOnline, ClientSocket csClientSocket, SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer, UISettings& usUISettings) {

        srand(time(NULL));

        SDL_DisplayMode sdlDisplayMode;
        SDL_GetCurrentDisplayMode(0, &sdlDisplayMode);

        int iWindowWidth = sdlDisplayMode.w;
        int iWindowHeight = sdlDisplayMode.h;
    
        //Frame Enforcer

        WG::FrameEnforcer wgFrameEnforcer(90);

        //Create Render Anchors

        int iCardWidth = iWindowWidth / 19.2;
        int iCardHeight = iWindowHeight / 7.5;

        int iCardXStart = (iWindowWidth / 2) - ((iCardWidth * 18) / 2);
        int iCardYStart = (iWindowHeight * 0.8);

        //Intialise Texture Map
        std::map<std::string, SDL_Texture*> mtTextureMap = InitaliseTextureMap(sdlRenderer);

        //Create SDLRect trackers for card locations
        std::vector<CardPositionTracker> vctCardsInTrickPositions = {};
        std::vector<CardPositionTracker> vctCardsInPreviousTrick = {};
        std::vector<CardPositionTracker> vctCardsRecievedInPass = {};

        //Create Text Panes
        WG::TextPane wtpPlayer(sdlRenderer, iWindowWidth, iWindowHeight, 0.3, 0.7, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpPositionOne(sdlRenderer, iWindowWidth, iWindowHeight, 0.3, 0.3, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpPositionTwo(sdlRenderer, iWindowWidth, iWindowHeight, 0.3, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpPassPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpPassInfoPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpHintPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.5, 0.6, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpAchievementPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.05, 0.63, 0.1, 0.1, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpSubtitlesPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.0, 0.0, 0.0, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpPromptPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.0, 0.0, 0.0, FRANKENHEARTS_ACCESSIBLEFONT);

        //Create Chat Pane
        WG::ReportPane wrpChatPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.0, 0.4, 0.15, FRANKENHEARTS_ACCESSIBLEFONT);
        WG::TextPane wtpChatInputPane(sdlRenderer, iWindowWidth, iWindowHeight, 0.0, 0.15, 0.4, 0.05, FRANKENHEARTS_ACCESSIBLEFONT);
        std::string sChatInputPaneDefault = "";
        std::string sChatInputPaneError = "Illegal Character Detected";
        std::string sChatInputPaneContent = "";
        bool bChat = false;
        bool bRenderChat = true;
        int iChatPaneFontSize = iWindowWidth * 0.01;
        int iChatMaximumLength = 78;
        pPlayer.wrpChatPane = &wrpChatPane;

        std::vector<WG::TextPane*> vwtpAllTextPanes = {&wtpPlayer, &wtpPositionOne, &wtpPositionTwo, &wtpPassPane, &wtpPassInfoPane, &wtpHintPane, &wtpAchievementPane, &wtpSubtitlesPane, &wtpPromptPane};

        //Create Achievement Renderer
        AchievementRenderer arAchievementRenderer(&wtpAchievementPane);
        

        //Create Background Sprite Metadata
        SDL_Texture* sdlBackgroundTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/Background.png");
        SDL_Rect sdlBackgroundTextureSourceRect;
        SDL_Rect sdlBackgroundTextureDestinationRect;
        CreateBackgroundSpriteMetadata(sdlBackgroundTexture, sdlBackgroundTextureSourceRect, sdlBackgroundTextureDestinationRect, iWindowHeight, iWindowWidth);

        //Create HeartsBroken Sprite Data  
        SDL_Texture* sdlHeartsBrokenTexture = WG::ConvertSpriteToTexture(sdlRenderer, "Cards/HeartsBroken.png");
        SDL_Rect sdlHeartsBrokenSourceRect;
        SDL_Rect sdlHeartsBrokenDestinationRect;
        CreateHeartsBrokenSpriteMetadata(sdlHeartsBrokenTexture, sdlHeartsBrokenSourceRect, sdlHeartsBrokenDestinationRect, iWindowHeight, iWindowWidth);
        
        std::vector<Card> vcCardsToPass = {};
    
        while (bClientOnline.load()) {

            wgFrameEnforcer.CreateFrameData();
    
            std::vector<Card> vcPlayerDeck = pPlayer.ReadDeck();
            std::vector<Card> vcCardsInTrick = pPlayer.ReadCardsInTrick();

            int iPassType = pPlayer.iPassType;

            int iMouseX, iMouseY;
            SDL_GetMouseState(&iMouseX, &iMouseY);
    
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    bClientOnline.store(false);
                }

                if (bChat) {
                    if (event.type == SDL_TEXTINPUT) {
                        if (sChatInputPaneContent.size() < iChatMaximumLength) {
                            sChatInputPaneContent += event.text.text;
                            wtpChatInputPane.ClearTextVector();
                            wtpChatInputPane.AddLine(sChatInputPaneContent, iChatPaneFontSize, {255, 255, 255, 255});
                            wtpChatInputPane.RefreshTextVectorTextures();
                        }
                    }
                    
                    if (event.type == SDL_KEYDOWN) {

                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            bChat = false;
                            wtpChatInputPane.ClearTextVector();
                            wtpChatInputPane.AddLine(sChatInputPaneDefault, iChatPaneFontSize, {255, 255, 255, 255});
                            wtpChatInputPane.RefreshTextVectorTextures();
                            sChatInputPaneContent.clear();
                        }

                        if (event.key.keysym.sym == SDLK_BACKSPACE) {
                            if (sChatInputPaneContent.size() > 0) {
                                sChatInputPaneContent.pop_back();
                                wtpChatInputPane.ClearTextVector();
                                wtpChatInputPane.AddLine(sChatInputPaneContent, iChatPaneFontSize, {255, 255, 255, 255});
                                wtpChatInputPane.RefreshTextVectorTextures();
                            }
                        }

                        if (event.key.keysym.sym == SDLK_RETURN) {
                            if (sChatInputPaneContent.size() > 0 && ValidatePlayerNickname(sChatInputPaneContent)) {
                                bChat = false;
                                csClientSocket.ClientSendMessage(sChatInputPaneContent);
                                pPlayer.PushMessageToChat(sChatInputPaneContent);
                                sChatInputPaneContent.clear();
                                wtpChatInputPane.ClearTextVector();
                                wtpChatInputPane.AddLine(sChatInputPaneDefault, iChatPaneFontSize, {255, 255, 255, 255});
                                wtpChatInputPane.RefreshTextVectorTextures();
                            } else {
                                bChat = false;
                                wtpChatInputPane.ClearTextVector();
                                wtpChatInputPane.AddLine(sChatInputPaneError, iChatPaneFontSize, {255, 255, 255, 255});
                                wtpChatInputPane.RefreshTextVectorTextures();
                                sChatInputPaneContent.clear();
                            }
                        }

                    }

                } else {

                    if (event.type == SDL_KEYDOWN) {

                        if (event.key.keysym.sym == SDLK_DOWN) {
                            wrpChatPane.ScrollThroughLines(-1);
                        }

                        if (event.key.keysym.sym == SDLK_UP) {
                            wrpChatPane.ScrollThroughLines(1);
                        }

                        if (event.key.keysym.sym == SDLK_ESCAPE) {

                            int iMenuValue = RenderRootGameMenu(sdlRenderer, iWindowWidth, iWindowHeight, usUISettings);

                            switch (iMenuValue) {

                                case 0:
                                break;

                                case FRANKENHEARTS_GAMEMENU_EXITGAME:
                                send(csClientSocket.socketClient, SIMP_Client_Exit.data(), SIMP_Client_Exit.length(), 0);
                                bClientOnline.store(false);
                                return;
                                break;

                                default:
                                WG::printvarint("iMenuValue out of range", iMenuValue);
                                break;

                            }

                            AccessibilityMode(usUISettings.bAccessible, vwtpAllTextPanes);

                        }

                        if (event.key.keysym.sym == SDLK_p) {

                            usUISettings.bPreviousCardsInTrick = !usUISettings.bPreviousCardsInTrick;

                        }

                        if (event.key.keysym.sym == SDLK_a) {

                            usUISettings.bAccessible = !usUISettings.bAccessible;
                            AccessibilityMode(usUISettings.bAccessible, vwtpAllTextPanes);

                        }

                        if (event.key.keysym.sym == SDLK_c) {

                            bRenderChat = !bRenderChat;

                        }

                    }

                    if (event.type == SDL_KEYUP) {
                        if (event.key.keysym.sym == SDLK_t) {

                            bChat = true;
                            bRenderChat = true;

                        }
                    }

                }

                if (event.type == SDL_MOUSEBUTTONDOWN) {

                    if (event.button.button == SDL_BUTTON_LEFT) {

                        if (iPassType < PASS_NONE) {
                            pPlayer.WriteCardsInTrick({});
                        }

                        for (int i = 0; i < vcPlayerDeck.size(); i++) {
                            if (iMouseX > vcPlayerDeck[i].sdlPositionRect.x && iMouseX < vcPlayerDeck[i].sdlPositionRect.x + vcPlayerDeck[i].sdlPositionRect.w && iMouseY > vcPlayerDeck[i].sdlPositionRect.y && iMouseY < vcPlayerDeck[i].sdlPositionRect.y + vcPlayerDeck[i].sdlPositionRect.h) {
                                if (iPassType == PASS_NONE) {
                                    csClientSocket.ClientSendCard(vcPlayerDeck[i].sCardID);
                                } else if (iPassType < PASS_NONE) {
                                    if (vcCardsToPass.size() < 4 && !vcPlayerDeck[i].bSelected) {
                                        vcCardsToPass.push_back(allCards[vcPlayerDeck[i].sCardID]);
                                        pPlayer.ReWriteCardSelection(i, true);
                                    } else if (vcPlayerDeck[i].bSelected) {
                                        RemoveCardFromCardVector(vcCardsToPass, vcPlayerDeck[i].sCardID);
                                        vcPlayerDeck[i].bSelected = false;
                                        pPlayer.ReWriteCardSelection(i, false);
                                    } 

                                }
                            }
                            if (iMouseX > wtpPassPane.sdlPaneRect.x && iMouseX < wtpPassPane.sdlPaneRect.x + wtpPassPane.sdlPaneRect.w && iMouseY > wtpPassPane.sdlPaneRect.y && iMouseY < wtpPassPane.sdlPaneRect.y + wtpPassPane.sdlPaneRect.h && vcCardsToPass.size() == 4) {
                                csClientSocket.ClientSendPassData(vcCardsToPass);
                                pPlayer.RemoveAllCardSelections();
                                vcCardsToPass.clear();
                            }
                        }
                    }
                }
            }
    
            SDL_SetRenderDrawColor(sdlRenderer, 36, 11, 33, 255);
            SDL_RenderClear(sdlRenderer);

            //Perform Pre-determined rendering
            SDL_RenderCopy(sdlRenderer, sdlBackgroundTexture, &sdlBackgroundTextureSourceRect, &sdlBackgroundTextureDestinationRect);

            if (pPlayer.ReadHearts()) {
                SDL_RenderCopy(sdlRenderer, sdlHeartsBrokenTexture, &sdlHeartsBrokenSourceRect, &sdlHeartsBrokenDestinationRect);
            }

            RenderPassText(iPassType, vcCardsToPass.size(), wtpPassInfoPane, usUISettings);
            RenderClickToPassText(vcCardsToPass.size(), wtpPassPane, usUISettings);
            vctCardsInPreviousTrick = RenderPreviousCardsInTrick(pPlayer.ReadPreviousCardsInTrick(), sdlRenderer, iWindowHeight, iWindowWidth, usUISettings.bPreviousCardsInTrick);
            RenderHintText(pPlayer.ReadPlayerHint(), iPassType, wtpHintPane);
            vctCardsInTrickPositions = RenderCardsInTrick(vcCardsInTrick, sdlRenderer, iWindowHeight, iWindowWidth);
            RenderPlayerScoreStrings(wtpPlayer, wtpPositionOne, wtpPositionTwo, pPlayer.ReadPlayerScores(), usUISettings);
            RenderPrompt(wtpPromptPane, pPlayer.iPrompt, usUISettings);

            if (vcPlayerDeck.size() != 18) {
                pPlayer.WriteRenderCardsRecieved(false);
            }

            vctCardsRecievedInPass = RenderCardsRecievedInPass(pPlayer.ReadCardsRecievedInPass(), sdlRenderer, iWindowHeight, iWindowWidth, pPlayer.ReadRenderCardsRecieved(), usUISettings);

            wtpPlayer.RenderPaneText();
            wtpPlayer.ClearTextVector();
            wtpPositionOne.RenderPaneText();
            wtpPositionOne.ClearTextVector();
            wtpPositionTwo.RenderPaneText();
            wtpPositionTwo.ClearTextVector();

            arAchievementRenderer.RecieveAchievement(pPlayer.ReadCurrentAchievement(), pPlayer);
            arAchievementRenderer.RenderAchievement();

            CreateCardSubtitles(vcPlayerDeck, vctCardsInTrickPositions, vctCardsInPreviousTrick, vctCardsRecievedInPass, iMouseX, iMouseY, wtpSubtitlesPane, usUISettings);
    
            int iHandXStart = iCardXStart;
            int iHandYStart = iCardYStart;
    
            bool bRendered = false;
    
            for (int i = 0; i < pPlayer.GetSize(); i++) {
    
                bRendered = true;
                SDL_Rect sdlCardRect;
                SDL_Rect sdlSourceRect;
    
                sdlSourceRect.x = 0;
                sdlSourceRect.y = 0;
                sdlSourceRect.w = 100;
                sdlSourceRect.h = 145;
     
                sdlCardRect.x = iHandXStart;
                sdlCardRect.y = iHandYStart;
                sdlCardRect.w = iCardWidth;
                sdlCardRect.h = iCardHeight;

                if (pPlayer.ReadDeckIndex(i).bSelected) {
                    sdlCardRect.y -= sdlCardRect.h / 3;
                }

                pPlayer.ReWriteCardPosition(i, sdlCardRect);

                iHandXStart += sdlCardRect.w;
    
                SDL_RenderCopy(sdlRenderer, mtTextureMap[pPlayer.ReadDeckIndex(i).sCardID], &sdlSourceRect, &sdlCardRect);
    
            }   

            //Render Chat
            if (bRenderChat) {
                if (bChat) {
                    wrpChatPane.RenderAllElements();
                    wtpChatInputPane.RenderAllElements();
                } else {
                    wrpChatPane.RenderPaneText();
                    wtpChatInputPane.RenderPaneText();
                }
            }
    
            SDL_RenderPresent(sdlRenderer);
            wgFrameEnforcer.EnforceFrameRate();
    
        }

}

int main(int argc, char** argv) {

    //Initalise SDL Window and Renderer

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_DisplayMode sdlDisplayMode;
    SDL_GetCurrentDisplayMode(0, &sdlDisplayMode);

    int iWindowWidth = sdlDisplayMode.w;
    int iWindowHeight = sdlDisplayMode.h;

    SDL_Window* sdlWindow = SDL_CreateWindow(
        "Frankenhearts Ultimate Edition - Multiplayer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        iWindowWidth,
        iWindowHeight,
        0
    );

    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);

    if (!sdlWindow || !sdlRenderer) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::string sUsername = "Frankenheart's Fan";
    std::string sIPAddr = "127.0.0.1";
    std::string sPort = "8080";

    UISettings usUISettings;

    while (true) {

        ClientSocket* Client = nullptr;

        int iMainMenu = MainMenu(sdlRenderer, iWindowWidth, iWindowHeight, Client, sUsername, sIPAddr, sPort, usUISettings);

        if (iMainMenu == FRANKENHEARTS_MAINMENU_EXITGAME) {
            return 0;
            SDL_DestroyRenderer(sdlRenderer);
            SDL_DestroyWindow(sdlWindow);
            SDL_Quit();
        }

        std::atomic<bool> bClientOnline = true;

        std::thread tPrintServerMessages(std::bind(&ClientSocket::PrintServerMessages, Client, std::ref(bClientOnline)));
        std::thread tInputThread(std::bind(&ClientSocket::UserInput, Client, std::ref(bClientOnline)));

        DisplayCard(bClientOnline, *Client, sdlWindow, sdlRenderer, usUISettings);

        tInputThread.join();
        tPrintServerMessages.join();
    
        bClientOnline.store(false);

        Client->Destruct();
        pPlayer.ResetPlayer();


    }


}