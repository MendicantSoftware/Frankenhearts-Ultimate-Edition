#include "Graphics.h"
#include "ClientPlayer.h"

#define FRANKENHEARTS_ACHIEVEMENT_NONE 0
#define FRANKENHEARTS_ACHIEVEMENT_KILLERQUEEN 1
#define FRANKENHEARTS_ACHIEVEMENT_DEFYINGGRAVITY 2
#define FRANKENHEARTS_ACHIEVEMENT_STATISTICALANOMALY 3
#define FRANKENHEARTS_ACHIEVEMENT_MISCALCULATED 4
#define FRANKENHEARTS_ACHIEVEMENT_SMOOTHOPERATOR 5
#define FRANKENHEARTS_ACHIEVEMENT_DEDICATED 6
#define FRANKENHEARTS_ACHIEVEMENTS_THECOLLECTOR 7
#define FRANKENHEARTS_ACHIEVEMENTS_INFINITEGENEROSITY 8
#define FRANKENHEARTS_ACHIEVEMENTS_COMMUNIST 9
#define FRANKENHEARTS_ACHIEVEMENTS_THATSFRANKENHEARTSBABY 10
#define FRANKENHEARTS_ACHIEVEMENTS_ILLDOITMYSELF 11
#define FRANKENHEARTS_ACHIEVEMENT_GOAT 12

struct AchievementData {

    int iMacro;
    std::string sTitle;
    std::string sDescription;
    std::string sPNG;

};

std::map<int, AchievementData> AchievementMap {

    {FRANKENHEARTS_ACHIEVEMENT_NONE, {FRANKENHEARTS_ACHIEVEMENT_NONE, "NULL", "NULL", "NULL"}},
    {{FRANKENHEARTS_ACHIEVEMENT_KILLERQUEEN}, {FRANKENHEARTS_ACHIEVEMENT_KILLERQUEEN, "KILLER QUEEN", "Win the Queen of Spades in the second trick!", "Achievements/KillerQueen.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_DEFYINGGRAVITY}, {FRANKENHEARTS_ACHIEVEMENT_DEFYINGGRAVITY, "DEFYING GRAVITY", "Shoot Venus!", "Achievements/DefyingGravity.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_STATISTICALANOMALY}, {FRANKENHEARTS_ACHIEVEMENT_STATISTICALANOMALY, "STATISTICAL ANOMALY", "Lead the Two of Hearts, only to be given two Jokers back!", "Achievements/StatisticalAnomaly.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_MISCALCULATED}, {FRANKENHEARTS_ACHIEVEMENT_MISCALCULATED, "MISCALCULATED", "Miss shooting Venus by only one heart!", "Achievements/Miscalculated.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_SMOOTHOPERATOR}, {FRANKENHEARTS_ACHIEVEMENT_SMOOTHOPERATOR, "SMOOTH OPERATOR", "Play an entire round without winning a single trick!", "Achievements/SmoothOperator.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_DEDICATED}, {FRANKENHEARTS_ACHIEVEMENT_DEDICATED, "DEDICATED", "Play 50 rounds in one sitting!", "Achievements/Dedicated.png"}},
    {{FRANKENHEARTS_ACHIEVEMENTS_THECOLLECTOR}, {FRANKENHEARTS_ACHIEVEMENTS_THECOLLECTOR, "THE COLLECTOR", "Win the Jack of Diamonds, Queen of Spades and Ten of Clubs in one trick!", "Achievements/TheCollector.png"}},
    {{FRANKENHEARTS_ACHIEVEMENTS_INFINITEGENEROSITY}, {FRANKENHEARTS_ACHIEVEMENTS_INFINITEGENEROSITY, "INFINITE GENEROSITY", "Pass a Joker! Suspicious...", "Achievements/InfiniteGenerosity.png"}},
    {{FRANKENHEARTS_ACHIEVEMENTS_COMMUNIST}, {FRANKENHEARTS_ACHIEVEMENTS_COMMUNIST, "COMMUNIST", "Have only red cards after passing!", "Achievements/Communist.png"}},
    {{FRANKENHEARTS_ACHIEVEMENTS_THATSFRANKENHEARTSBABY}, {FRANKENHEARTS_ACHIEVEMENTS_THATSFRANKENHEARTSBABY, "THAT'S FRANKENHEARTS BABY!", "Take the most points possible in a single trick!", "Achievements/ThatsFrankenheartsBaby.png"}},
    {{FRANKENHEARTS_ACHIEVEMENTS_ILLDOITMYSELF}, {FRANKENHEARTS_ACHIEVEMENTS_ILLDOITMYSELF, "I'LL DO IT MYSELF", "Play against the developer AI! Good Luck!", "Achievements/IllDoItMyself.png"}},
    {{FRANKENHEARTS_ACHIEVEMENT_GOAT}, {FRANKENHEARTS_ACHIEVEMENT_GOAT, "GOAT", "Achieve the impossible and defeat the developer!", "Achievements/GOAT.png"}}

};

class AchievementRenderer {

    private:

    AchievementData adCurrentAchievement = AchievementMap[FRANKENHEARTS_ACHIEVEMENT_NONE]; 

    bool bRenderAchievement = false;

    int iCurrentFrame = 0;
    const int iKillFrame = 600;

    double dHeadingSize;
    double dDescriptionSize;

    WG::TextPane* wtpAchievementPane;
    SDL_Texture* sdlCurrentTexture = nullptr;
    SDL_Rect sdlSpriteSourceRect;
    SDL_Rect sdlSpriteDestinationRect;



    SDL_Rect sdlFadeRect;
    SDL_Color sdlBackgroundColour = {36, 11, 33};
    double dFadeRectFrameRatio = 0.7;
    int iFadeRectAlphaValue = 0;
    int iFadeRectAlphaValueIncrease = 0;

    public:

    AchievementRenderer(WG::TextPane* AchievementPane)  {

        wtpAchievementPane = AchievementPane;
        dHeadingSize = wtpAchievementPane->dWindowHeight / 50;
        dDescriptionSize = wtpAchievementPane->dWindowHeight / 60;

        sdlSpriteSourceRect.x = 0;
        sdlSpriteSourceRect.y = 0;
        sdlSpriteSourceRect.w = 350;
        sdlSpriteSourceRect.h = 450;

        sdlSpriteDestinationRect.x = 0;
        sdlSpriteDestinationRect.y = wtpAchievementPane->dWindowHeight * 0.60;
        sdlSpriteDestinationRect.w = wtpAchievementPane->dWindowHeight * 0.08;
        sdlSpriteDestinationRect.h = wtpAchievementPane->dWindowHeight * 0.1;
 
    }

    bool RecieveAchievement(int iAchievement, Player& pPlayer) {

        if (iAchievement == FRANKENHEARTS_ACHIEVEMENT_NONE) {
            return false;
        }

        adCurrentAchievement = AchievementMap[iAchievement];

        sdlCurrentTexture = WG::ConvertSpriteToTexture(wtpAchievementPane->sdlRenderer, adCurrentAchievement.sPNG.data());

        wtpAchievementPane->ClearTextVector();
        wtpAchievementPane->AddLine(adCurrentAchievement.sTitle, dHeadingSize, {255, 215, 0, 255});
        wtpAchievementPane->AddLine(adCurrentAchievement.sDescription, dDescriptionSize, {255, 215, 0, 255});
        wtpAchievementPane->RefreshTextVectorTextures();

        sdlFadeRect.x = sdlSpriteDestinationRect.x;
        sdlFadeRect.y = sdlSpriteDestinationRect.y;
        sdlFadeRect.w = sdlSpriteDestinationRect.w + wtpAchievementPane->vwtlTextLines[1].sdlTextRect.w + wtpAchievementPane->dPaneX - sdlSpriteDestinationRect.x;
        sdlFadeRect.h = sdlSpriteDestinationRect.h;

        iFadeRectAlphaValue = 0;

        bRenderAchievement = true;
        iCurrentFrame = 0;

        iFadeRectAlphaValueIncrease = (iKillFrame - (iKillFrame * dFadeRectFrameRatio)) / 90;

        pPlayer.WriteCurrentAchievement(FRANKENHEARTS_ACHIEVEMENT_NONE);

        return true;


    }

    bool RenderAchievement() {

        if (!bRenderAchievement) {
            return false;
        }

        wtpAchievementPane->RenderPaneText();
        SDL_RenderCopy(wtpAchievementPane->sdlRenderer, sdlCurrentTexture, &sdlSpriteSourceRect, &sdlSpriteDestinationRect);

        iCurrentFrame++;

        if (iCurrentFrame > iKillFrame * dFadeRectFrameRatio) {

            iFadeRectAlphaValue += iFadeRectAlphaValueIncrease;

            if (iFadeRectAlphaValue > 255) {
                iFadeRectAlphaValue = 255;
            }

            SDL_SetRenderDrawColor(wtpAchievementPane->sdlRenderer, sdlBackgroundColour.r, sdlBackgroundColour.g, sdlBackgroundColour.b, iFadeRectAlphaValue);
            SDL_RenderFillRect(wtpAchievementPane->sdlRenderer, &sdlFadeRect);

        }

        if (iCurrentFrame == iKillFrame) {

            bRenderAchievement = false;

        }

        return true;

    }



};