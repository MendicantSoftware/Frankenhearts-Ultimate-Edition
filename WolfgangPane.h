#include <SDL.h>
#include <SDL_ttf.h> 

#include <string>

#include "WolfgangTile.h"
#include "WolfgangText.h"

#pragma once

#define WG_NOTIFICATION_DEFAULT 0
#define WG_NOTIFICATION_NEWTURN 1
#define WG_NOTIFICATION_TOOLTIP 2

namespace WG {
    /**
    *WG TileMaps should be rendered inside of a Map Pane
    */
    class MapPane {

        public:

        double dWindowWidth, dWindowHeight;
        double dPaneX, dPaneY;
        double dPaneRelativeX, dPaneRelativeY;
        double dPaneW, dPaneH;

        double dMapOffsetX = 0.025;
        double dMapOffsetY = 0.5;

        SDL_Rect sdlPaneRect;

        SDL_Renderer* sdlRenderer; 

        WG::TileMap* wtmTileMap;


        /**
        *MapPanes are responsive to the window size, they are constructed with ratios, it is possible
        *to change these ratios to ints by directly editing MapPane's attributes.
        *MapPanes should only be squares, not rectangles.
        @param WindowWidth Window Width as declared, it may also be queried using SDL.
        @param WindowHeight Window Width as declared.
        @param XPosition Intended X position of the pane as a ratio of the window width (between 0 and 1).
        @param YPosition Intended Y position of the pane as a ratio of the window width.
        @param PaneWidth Intended pane width as a ratio of the window width.
        @param PaneHeight Intended pane height as a ratio of the window width.
        */
        MapPane(double WindowWidth, double WindowHeight, double XPosition, double YPosition, double PaneSize) {

            dWindowWidth = WindowWidth;
            dWindowHeight = WindowHeight;

            dPaneX = dWindowWidth * XPosition; 
            dPaneY = dWindowHeight * YPosition;

            dPaneW = dWindowWidth * PaneSize;
            dPaneH = dWindowHeight * PaneSize;

            sdlPaneRect.x = int(dPaneX);
            sdlPaneRect.y = int(dPaneY);
            sdlPaneRect.w = int(dPaneW);
            sdlPaneRect.h = int(dPaneH);

        }

        void AttachTileMap(WG::TileMap *TileMap, int &iColsToRender, int &iRowsToRender, int &iStartCol, int &iStartRow) {

            wtmTileMap = TileMap;

            int iMaxCols = (dPaneW / wtmTileMap->vwtAllTiles.operator[](0).tsSprite.sdlDestinationRect.w) * 0.9;
            int iMaxRows = (dPaneH / wtmTileMap->vwtAllTiles.operator[](0).tsSprite.sdlDestinationRect.h) * 0.9;

            if (iMaxCols < iColsToRender) {
                iColsToRender = iMaxCols;
            }

            if (iMaxRows < iRowsToRender) {
                iRowsToRender = iMaxRows;
            }

            //OPTIMISATION: CalculatesSubMap twice because there is no other way to ensure submap is correct size

            wtmTileMap->CalculateSubMap(iColsToRender, iRowsToRender, iStartCol, iStartRow);

            WG::Vector wvMapDiamensions = wtmTileMap->MapSize(wtmTileMap->vwtSubMap);

            wtmTileMap->iXInit = (dPaneX + (dPaneW / 2)) - (wvMapDiamensions.dx / 2);
            wtmTileMap->iYInit = (dPaneY + (dPaneH / 2)) - (wvMapDiamensions.dy / 2);

            wtmTileMap->CalculateSubMap(iColsToRender, iRowsToRender, iStartCol, iStartRow);

        }

        void RenderPaneRect() {
            SDL_SetRenderDrawColor(sdlRenderer, 100, 100, 100, 100);
            SDL_RenderDrawRect(sdlRenderer, &sdlPaneRect);        
        }   

        void RenderPaneSubMap() {
            wtmTileMap->RenderTileMapSection(sdlRenderer);
        }

        void RenderAllElements() {
            RenderPaneRect();
            RenderPaneSubMap();
        }

        std::vector<int> CreateGenericIndicatorRect(int ObjectCol, int ObjectRow, int IndicatorSize) {


            std::vector<int> viIndicatedTiles;


            int iNegativeXIndicatorSize = IndicatorSize, iPositiveXIndicatorSize = IndicatorSize;
            int iNegativeYIndicatorSize = IndicatorSize, iPositiveYIndicatorSize = IndicatorSize;
            
            if (ObjectCol - iNegativeXIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectCol - iNegativeXIndicatorSize);
                iNegativeXIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveXIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol) {
                int iOverflow = ObjectCol + iPositiveXIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol;
                iPositiveXIndicatorSize -= iOverflow;
            }
            if (ObjectRow - iNegativeYIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectRow - iNegativeYIndicatorSize);
                iNegativeYIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveYIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow) {
                int iOverflow = ObjectRow + iPositiveYIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow;
                iPositiveYIndicatorSize -= iOverflow;
            }

            int iTopX = ObjectCol - iNegativeXIndicatorSize;
            int iTopY = ObjectRow - iNegativeYIndicatorSize;


            while (iTopX < ObjectCol + iPositiveXIndicatorSize) {
                viIndicatedTiles.push_back(wtmTileMap->TileIndex(iTopX, iTopY));
                iTopX++;
            }

            int iLeftX = ObjectCol - iNegativeXIndicatorSize;
            int iLeftY = ObjectRow - iNegativeYIndicatorSize;
            
            if (iLeftX < 1) {
                iLeftX = 1;
            }

            if (iLeftY < 1) {
                iLeftY = 1;
            }

            while (iLeftY < ObjectRow + iPositiveYIndicatorSize) {
                viIndicatedTiles.push_back(wtmTileMap->TileIndex(iLeftX, iLeftY));
                iLeftY++;
            }

            int iBotX = ObjectCol - iNegativeXIndicatorSize;
            int iBotY = ObjectRow + iPositiveYIndicatorSize;

            if (iBotX < 1) {
                iBotX = 1;
            }

            if (iBotY < 1) {
                iBotY = 1;
            }

            while (iBotX < ObjectCol + iPositiveXIndicatorSize) {
                viIndicatedTiles.push_back(wtmTileMap->TileIndex(iBotX, iBotY));
                iBotX++;
            }


            int iRightX = ObjectCol + iPositiveXIndicatorSize;
            int iRightY = ObjectRow - iNegativeYIndicatorSize;

            if (iRightX < 1) {
                iRightX = 1;
            }

            if (iRightY < 1) {
                iRightY = 1;
            }

            while (iRightY < ObjectRow + iPositiveYIndicatorSize + 1) {
                viIndicatedTiles.push_back(wtmTileMap->TileIndex(iRightX, iRightY));
                iRightY++;
            }

            return viIndicatedTiles;

        }


        void SetMovementIndicatorRect(int ObjectCol, int ObjectRow, int IndicatorSize) {

            wtmTileMap->ClearAllMovementHighlights();
            
            int iNegativeXIndicatorSize = IndicatorSize, iPositiveXIndicatorSize = IndicatorSize;
            int iNegativeYIndicatorSize = IndicatorSize, iPositiveYIndicatorSize = IndicatorSize;
            
            if (ObjectCol - iNegativeXIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectCol - iNegativeXIndicatorSize);
                iNegativeXIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveXIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol) {
                int iOverflow = ObjectCol + iPositiveXIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol;
                iPositiveXIndicatorSize -= iOverflow;
            }
            if (ObjectRow - iNegativeYIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectRow - iNegativeYIndicatorSize);
                iNegativeYIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveYIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow) {
                int iOverflow = ObjectRow + iPositiveYIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow;
                iPositiveYIndicatorSize -= iOverflow;
            }

            int iTopX = ObjectCol - iNegativeXIndicatorSize;
            int iTopY = ObjectRow - iNegativeYIndicatorSize;


            while (iTopX < ObjectCol + iPositiveXIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iTopX, iTopY)].bMovementHighlighted = true;
                iTopX++;
            }

            int iLeftX = ObjectCol - iNegativeXIndicatorSize;
            int iLeftY = ObjectRow - iNegativeYIndicatorSize;
            
            if (iLeftX < 1) {
                iLeftX = 1;
            }

            if (iLeftY < 1) {
                iLeftY = 1;
            }

            while (iLeftY < ObjectRow + iPositiveYIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iLeftX, iLeftY)].bMovementHighlighted = true;
                iLeftY++;
            }

            int iBotX = ObjectCol - iNegativeXIndicatorSize;
            int iBotY = ObjectRow + iPositiveYIndicatorSize;

            if (iBotX < 1) {
                iBotX = 1;
            }

            if (iBotY < 1) {
                iBotY = 1;
            }

            while (iBotX < ObjectCol + iPositiveXIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iBotX, iBotY)].bMovementHighlighted = true;
                iBotX++;
            }


            int iRightX = ObjectCol + iPositiveXIndicatorSize;
            int iRightY = ObjectRow - iNegativeYIndicatorSize;

            if (iRightX < 1) {
                iRightX = 1;
            }

            if (iRightY < 1) {
                iRightY = 1;
            }

            while (iRightY < ObjectRow + iPositiveYIndicatorSize + 1) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iRightX, iRightY)].bMovementHighlighted = true;
                iRightY++;
            }

        }

        void SetTargetIndicatorRect(int ObjectCol, int ObjectRow, int IndicatorSize) {

            wtmTileMap->ClearAllTargetHighlights();
            
            int iNegativeXIndicatorSize = IndicatorSize, iPositiveXIndicatorSize = IndicatorSize;
            int iNegativeYIndicatorSize = IndicatorSize, iPositiveYIndicatorSize = IndicatorSize;
            
            if (ObjectCol - iNegativeXIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectCol - iNegativeXIndicatorSize);
                iNegativeXIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveXIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol) {
                int iOverflow = ObjectCol + iPositiveXIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iCol;
                iPositiveXIndicatorSize -= iOverflow;
            }
            if (ObjectRow - iNegativeYIndicatorSize < 1) {
                int iUnderflow = 1 - (ObjectRow - iNegativeYIndicatorSize);
                iNegativeYIndicatorSize -= iUnderflow;
            }
            if (ObjectCol + iPositiveYIndicatorSize > wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow) {
                int iOverflow = ObjectRow + iPositiveYIndicatorSize - wtmTileMap->vwtSubMap[wtmTileMap->vwtSubMap.size() - 1].iRow;
                iPositiveYIndicatorSize -= iOverflow;
            }

            int iTopX = ObjectCol - iNegativeXIndicatorSize;
            int iTopY = ObjectRow - iNegativeYIndicatorSize;


            while (iTopX < ObjectCol + iPositiveXIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iTopX, iTopY)].bTargetHighlighted = true;
                iTopX++;
            }

            int iLeftX = ObjectCol - iNegativeXIndicatorSize;
            int iLeftY = ObjectRow - iNegativeYIndicatorSize;
            
            if (iLeftX < 1) {
                iLeftX = 1;
            }

            if (iLeftY < 1) {
                iLeftY = 1;
            }

            while (iLeftY < ObjectRow + iPositiveYIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iLeftX, iLeftY)].bTargetHighlighted = true;
                iLeftY++;
            }

            int iBotX = ObjectCol - iNegativeXIndicatorSize;
            int iBotY = ObjectRow + iPositiveYIndicatorSize;

            if (iBotX < 1) {
                iBotX = 1;
            }

            if (iBotY < 1) {
                iBotY = 1;
            }

            while (iBotX < ObjectCol + iPositiveXIndicatorSize) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iBotX, iBotY)].bTargetHighlighted = true;
                iBotX++;
            }


            int iRightX = ObjectCol + iPositiveXIndicatorSize;
            int iRightY = ObjectRow - iNegativeYIndicatorSize;

            if (iRightX < 1) {
                iRightX = 1;
            }

            if (iRightY < 1) {
                iRightY = 1;
            }

            while (iRightY < ObjectRow + iPositiveYIndicatorSize + 1) {
                wtmTileMap->vwtAllTiles[wtmTileMap->TileIndex(iRightX, iRightY)].bTargetHighlighted = true;
                iRightY++;
            }

        }

    };

    class TextLine {

        public:

        std::string sContent; 
        std::vector<std::string> vsContent;
        int iSize;
        SDL_Color sdlFontColor; 
        SDL_Texture* sdlTextTexture;
        SDL_Rect sdlTextRect;
        std::vector<WG::TextLine> vwtlToolTipLines = {};

        TextLine(std::string Text, int FontSize, SDL_Color FontColor) {
            sContent = Text;
            iSize = FontSize;
            sdlFontColor = FontColor;
        }

    };


    /**
    *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
    *Text panes may be rectangles.
    */
    class TextPane {

        public:

        double dWindowWidth, dWindowHeight;
        double dPaneX, dPaneY;
        double dPaneRelativeX, dPaneRelativeY;
        double dPaneW, dPaneH;

        SDL_Renderer* sdlRenderer;

        std::vector<WG::TextLine> vwtlTextLines;
        std::vector<SDL_Texture*> vsdlBodyTextTextures; 

        SDL_Color sdlDefaultTextColor = {0, 0, 0, 255};
        int iDefaultFontSize = 18;
        double dXTextInsetRatio = 0.025;
        double dYTextInsetRatio = 0.025;

        SDL_Rect sdlPaneRect;

        std::string sFont; 

        /**
        *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
        *Text panes may be rectangles.
        @param WindowWidth Window Width as declared, it may also be queried using SDL.
        @param WindowHeight Window Width as declared.
        @param XPosition Intended X position of the pane as a ratio of the window width (between 0 and 1).
        @param YPosition Intended Y position of the pane as a ratio of the window width.
        @param PaneWidth Intended pane width as a ratio of the window width.
        @param PaneHeight Intended pane height as a ratio of the window width.
        */
        TextPane(SDL_Renderer* Renderer, double WindowWidth, double WindowHeight, double XPosition, double YPosition, double PaneWidth, double PaneHeight, std::string Font) {
            
            sdlRenderer = Renderer;

            dWindowWidth = WindowWidth;
            dWindowHeight = WindowHeight;

            dPaneX = dWindowWidth * XPosition; 
            dPaneY = dWindowHeight * YPosition;

            dPaneW = dWindowWidth * PaneWidth;
            dPaneH = dWindowHeight * PaneHeight;

            sdlPaneRect.x = int(dPaneX);
            sdlPaneRect.y = int(dPaneY);
            sdlPaneRect.w = int(dPaneW);
            sdlPaneRect.h = int(dPaneH);

            sFont = Font;

        }

        /**
        *Renders the debug rect attached to every pane.
        */
        void RenderPaneRect() {
            SDL_SetRenderDrawColor(sdlRenderer, 100, 100, 100, 100);
            SDL_RenderDrawRect(sdlRenderer, &sdlPaneRect);        
        }

        /**
        *Renders all text attached to the TextPane. 
        */
        void RenderPaneText() {
            for (int i = 0; i < vwtlTextLines.size(); i++) {
                SDL_RenderCopy(sdlRenderer, vwtlTextLines[i].sdlTextTexture, NULL, &vwtlTextLines[i].sdlTextRect);
            }
        }

        void RenderAllElements() {
            RenderPaneRect();
            RenderPaneText();
        }

        /**
        *Pushes a new line into the TextPane's TextLine std::vector with the prescribed attributes.
        */
        void AddLine(std::string Text, int FontSize, SDL_Color FontColor) {
            vwtlTextLines.push_back(WG::TextLine(Text, FontSize, FontColor));
        }

        /**
        *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
        */
        void AddDefaultLine(std::string Text) {
            vwtlTextLines.push_back(WG::TextLine(Text, iDefaultFontSize, sdlDefaultTextColor));
        }

                /**
    *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
        */
        void AddDefaultTextVector(std::vector<std::string> vsText) {
            for (int i = 0; i < vsText.size(); i++) {
                AddDefaultLine(vsText[i]);
            }
        }

        /**
        *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
        */
        void EditTextInsets(double XTextInset, double YTextInset) {
            dXTextInsetRatio = XTextInset;
            dYTextInsetRatio = YTextInset;
        }

        /**
        *Refreshes the texture of each line of text. Must be called for changes in text to manifest during rendering.
        */
        void RefreshTextVectorTextures() {

            double dYInset = dPaneY + dYTextInsetRatio * dPaneH;
            double dInitX = dPaneX + dPaneW * dXTextInsetRatio;

            for (int i = 0; i < vwtlTextLines.size(); i++) {
                //Change so doesn't reopen it every time
                TTF_Font* ttfFont = TTF_OpenFont(sFont.data(), vwtlTextLines[i].iSize);
                SDL_Surface* sdlTextSurface = TTF_RenderText_Blended(ttfFont, vwtlTextLines[i].sContent.data(), vwtlTextLines[i].sdlFontColor);
                vwtlTextLines[i].sdlTextTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlTextSurface);

                int iTextHeight, iTextWidth;
                SDL_QueryTexture(vwtlTextLines[i].sdlTextTexture, nullptr, nullptr, &iTextWidth, &iTextHeight);

                vwtlTextLines[i].sdlTextRect.w = iTextWidth;
                vwtlTextLines[i].sdlTextRect.h = iTextHeight;
                vwtlTextLines[i].sdlTextRect.x = dInitX;
                vwtlTextLines[i].sdlTextRect.y = dYInset;

                dYInset += iTextHeight;

                SDL_FreeSurface(sdlTextSurface);
                TTF_CloseFont(ttfFont);


            }

        }

        /**
        *Clears all text assigned to a given pane. Wolfgang will typically call this for you.
        */
        void ClearTextVector() {
            for (int i = 0; i < vwtlTextLines.size(); i++) {
                SDL_DestroyTexture(vwtlTextLines[i].sdlTextTexture);
            }
            vwtlTextLines.clear();
        }

    };




    /**
    *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
    *Text panes may be rectangles.
    */
   class NotifcationPane {

    public:

    int iPaneType;

    bool bCentered;

    double dWindowWidth, dWindowHeight;
    double dPaneX, dPaneY;
    double dPaneRelativeX, dPaneRelativeY;
    double dPaneW, dPaneH;

    SDL_Renderer* sdlRenderer;

    std::vector<WG::TextLine> vwtlTextLines;
    std::vector<SDL_Texture*> vsdlBodyTextTextures; 

    SDL_Color sdlDefaultTextColor = {0, 0, 0, 255};
    int iDefaultFontSize = 18;
    double dXTextInsetRatio = 0.025;
    double dYTextInsetRatio = 0.025;

    SDL_Rect sdlPaneRect;

    /**
    *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
    *Text panes may be rectangles.
    @param WindowWidth Window Width as declared, it may also be queried using SDL.
    @param WindowHeight Window Width as declared.
    @param XPosition Intended X position of the pane as a ratio of the window width (between 0 and 1).
    @param YPosition Intended Y position of the pane as a ratio of the window width.
    @param PaneWidth Intended pane width as a ratio of the window width.
    @param PaneHeight Intended pane height as a ratio of the window width.
    */
    NotifcationPane(SDL_Renderer* Renderer, double WindowWidth, double WindowHeight, int PaneType) {
        
        sdlRenderer = Renderer;

        dWindowWidth = WindowWidth;
        dWindowHeight = WindowHeight;

        iPaneType = PaneType;

        double XPosition, YPosition, PaneWidth, PaneHeight;

        switch (iPaneType) {
            case WG_NOTIFICATION_DEFAULT:
            XPosition = 0.5;
            YPosition = 0.5; 
            PaneWidth = 0.2;
            PaneHeight = 0.3;
            bCentered = true;
            break;

            case WG_NOTIFICATION_NEWTURN:
            XPosition = 0.5;
            YPosition = 0.5; 
            PaneWidth = 0.2;
            PaneHeight = 0.025;
            bCentered = true;
            break;

            case WG_NOTIFICATION_TOOLTIP:
            XPosition = 0.5;
            YPosition = 0.5;
            PaneWidth = 0.5;
            PaneHeight = 0.5;
            bCentered = true;
            break;


            default:
            XPosition = 0.5;
            YPosition = 0.5; 
            PaneWidth = 0.2;
            PaneHeight = 0.3;
            bCentered = true;
            break;
        }

        dPaneW = dWindowWidth * PaneWidth;
        dPaneH = dWindowHeight * PaneHeight;

        dPaneX = dWindowWidth * XPosition; 
        dPaneY = dWindowHeight * YPosition;

        if (bCentered) {
            dPaneX -= dPaneW / 2;
            dPaneY -= dPaneH / 2;
        }

        sdlPaneRect.x = int(dPaneX);
        sdlPaneRect.y = int(dPaneY);
        sdlPaneRect.w = int(dPaneW);
        sdlPaneRect.h = int(dPaneH);

    }

    /**
    *Renders the debug rect attached to every pane.
    */
    void RenderPaneRect() {
        SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 255, 255);
        SDL_RenderFillRect(sdlRenderer, &sdlPaneRect);
        SDL_SetRenderDrawColor(sdlRenderer, 100, 100, 100, 100);
        SDL_RenderDrawRect(sdlRenderer, &sdlPaneRect);        
    }

    /**
    *Renders all text attached to the TextPane. 
    */
    void RenderPaneText() {
        for (int i = 0; i < vwtlTextLines.size(); i++) {
            SDL_RenderCopy(sdlRenderer, vwtlTextLines[i].sdlTextTexture, NULL, &vwtlTextLines[i].sdlTextRect);
        }
    }

    void RenderAllElements() {
        RenderPaneRect();
        RenderPaneText();
    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector with the prescribed attributes.
    */
    void AddLine(std::string Text, int iFontSize, SDL_Color FontColor) {
        vwtlTextLines.push_back(WG::TextLine(Text, iFontSize, FontColor));
    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
    */
    void AddDefaultLine(std::string Text) {
        vwtlTextLines.push_back(WG::TextLine(Text, iDefaultFontSize, sdlDefaultTextColor));
    }

    void AddDefaultTextVector(std::vector<std::string> vsText) {
        for (int i = 0; i < vsText.size(); i++) {
            AddDefaultLine(vsText[i]);
        }
    }

    void AddToolTipVector(std::vector<WG::TextLine> TextLines) {

        for (int i = 0; i < TextLines.size(); i++) {

            TextLines[i].vsContent = WG::TextWrapping(TextLines[i].sContent, dPaneW, 0.025, QueryFontWidth("FreeMonoBold.ttf", TextLines[i].iSize, sdlRenderer));

            for (int x = 0; x < TextLines[i].vsContent.size(); x++) {
                AddLine(TextLines[i].vsContent[x], TextLines[i].iSize, TextLines[i].sdlFontColor);
            }

        }

    }

    void AddTextVector(std::vector<WG::TextLine> TextLines) {
        for (int i = 0; i < TextLines.size(); i++) {

            for (int x = 0; x < TextLines[i].vsContent.size(); x++) {
                AddLine(TextLines[i].vsContent[x], TextLines[i].iSize, TextLines[i].sdlFontColor);
            }

        }
    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
    */
    void EditTextInsets(double XTextInset, double YTextInset) {
        dXTextInsetRatio = XTextInset;
        dYTextInsetRatio = YTextInset;
    }

    /**
    *Refreshes the texture of each line of text. Must be called for changes in text to manifest during rendering.
    */
    void RefreshTextVectorTextures() {

        double dYInset = dPaneY + dYTextInsetRatio * dPaneH;
        double dInitX;

        double dLargestLine = 0;
        double dTexturesHeight = 0;

        for (int i = 0; i < vwtlTextLines.size(); i++) {
            //Change so doesn't reopen it every time
            TTF_Font* ttfFont = TTF_OpenFont("FreeMonoBold.ttf", vwtlTextLines[i].iSize);
            SDL_Surface* sdlTextSurface = TTF_RenderText_Solid(ttfFont, vwtlTextLines[i].sContent.data(), vwtlTextLines[i].sdlFontColor);
            vwtlTextLines[i].sdlTextTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlTextSurface);

            int iTextHeight, iTextWidth;
            SDL_QueryTexture(vwtlTextLines[i].sdlTextTexture, nullptr, nullptr, &iTextWidth, &iTextHeight);

            if (iTextWidth > dLargestLine) {
                dLargestLine = iTextWidth;
            }

            dTexturesHeight += iTextHeight;

            dInitX = dPaneX + (dPaneW / 2) - (iTextWidth / 2);

            vwtlTextLines[i].sdlTextRect.w = iTextWidth;
            vwtlTextLines[i].sdlTextRect.h = iTextHeight;
            vwtlTextLines[i].sdlTextRect.x = dInitX;
            vwtlTextLines[i].sdlTextRect.y = dYInset;

            dYInset += iTextHeight;

            SDL_FreeSurface(sdlTextSurface);
            TTF_CloseFont(ttfFont);

        }

        //Resize Pane (Depending on Type)

        ResizePaneByText(dLargestLine, dTexturesHeight);


    }

    void ResizePaneByText(int dLargestLine, int dTexturesHeight) {
        dPaneW = dLargestLine + (dLargestLine * 0.1);
        dPaneH = dTexturesHeight + (dTexturesHeight * 0.1);

        dPaneX = dWindowWidth / 2;
        dPaneY = dWindowHeight / 2;

        dPaneX -= dPaneW / 2;
        dPaneY -= dPaneH / 2;

        sdlPaneRect.w = dPaneW;
        sdlPaneRect.h = dPaneH;
        sdlPaneRect.x = dPaneX;
        sdlPaneRect.y = dPaneY;

        double dYInset = dPaneY + dYTextInsetRatio * dPaneH;

        for (int i = 0; i < vwtlTextLines.size(); i++) {

            int iTextHeight, iTextWidth;
            SDL_QueryTexture(vwtlTextLines[i].sdlTextTexture, nullptr, nullptr, nullptr, &iTextHeight);

            vwtlTextLines[i].sdlTextRect.y = dYInset;

            dYInset += iTextHeight;

        }
    }

    /**
    *Clears all text assigned to a given pane. Wolfgang will typically call this for you.
    */
    void ClearTextVector() {
        for (int i = 0; i < vwtlTextLines.size(); i++) {
            SDL_DestroyTexture(vwtlTextLines[i].sdlTextTexture);
        }
        vwtlTextLines.clear();
    }

};

    int DeleteNotificationPane(std::vector<WG::NotifcationPane*>* vwnpPanes) {
        if (vwnpPanes->size() > 0) {
            delete vwnpPanes->operator[](vwnpPanes->size() - 1);
            vwnpPanes->pop_back();
            return 0;
        }

        return 1;
    }

    WG::Vector ReturnMapCoordinates(WG::TileMap* TileMap) {

        for (int i = 0; i < TileMap->vwtSubMap.size(); i++) {
            if (TileMap->vwtSubMap[i].bHighlighted) {
                return WG::Vector(TileMap->vwtSubMap[i].iCol, TileMap->vwtSubMap[i].iRow);
            }
        }

        return WG::Vector(-1, -1);
    }

    int DisplayMapCoordinates(WG::TextPane* TargetPane, WG::TileMap* TileMap) {
        
        WG::Vector Position = ReturnMapCoordinates(TileMap);
        TargetPane->ClearTextVector();

        if (Position.dx > -1) {
            TargetPane->ClearTextVector();
            TargetPane->AddDefaultLine(std::to_string(int(Position.dx)) + ", " + std::to_string(int(Position.dy)));
            TargetPane->RefreshTextVectorTextures();
            return 0;
        } else {
            return 1;
        }



    }

    /**
    *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
    *Text panes may be rectangles.
    */
   class ReportPane {

    public:

    double dWindowWidth, dWindowHeight;
    double dPaneX, dPaneY;
    double dPaneRelativeX, dPaneRelativeY;
    double dPaneW, dPaneH;

    int iVectorPoint = 0; 
    const int iMaxLines = 6;



    SDL_Renderer* sdlRenderer;

    std::vector<WG::TextLine> vwtlTextLines;
    std::vector<WG::TextLine> vwtlTextLinesToRender;
    std::vector<SDL_Texture*> vsdlBodyTextTextures; 

    SDL_Color sdlDefaultTextColor = {255, 255, 255, 255};
    int iDefaultFontSize = 18;
    TTF_Font* ttfFont;

    std::string sFont;

    double dXTextInsetRatio = 0.025;
    double dYTextInsetRatio = 0.025;

    SDL_Rect sdlPaneRect;

    /**
    *TextPanes are responsive to the window size, they are constructed with ratios, it is possible to change these ratios to ints by directly editing MapPane's attributes.
    *Text panes may be rectangles.
    @param WindowWidth Window Width as declared, it may also be queried using SDL.
    @param WindowHeight Window Width as declared.
    @param XPosition Intended X position of the pane as a ratio of the window width (between 0 and 1).
    @param YPosition Intended Y position of the pane as a ratio of the window width.
    @param PaneWidth Intended pane width as a ratio of the window width.
    @param PaneHeight Intended pane height as a ratio of the window width.
    */
    ReportPane(SDL_Renderer* Renderer, double WindowWidth, double WindowHeight, double XPosition, double YPosition, double PaneWidth, double PaneHeight, std::string Font) {
        
        sdlRenderer = Renderer;

        dWindowWidth = WindowWidth;
        dWindowHeight = WindowHeight;

        dPaneX = dWindowWidth * XPosition; 
        dPaneY = dWindowHeight * YPosition;

        dPaneW = dWindowWidth * PaneWidth;
        dPaneH = dWindowHeight * PaneHeight;

        sdlPaneRect.x = int(dPaneX);
        sdlPaneRect.y = int(dPaneY);
        sdlPaneRect.w = int(dPaneW);
        sdlPaneRect.h = int(dPaneH);

        sFont = Font;

        iDefaultFontSize = dWindowWidth * 0.01;

        ttfFont = TTF_OpenFont(sFont.data(), iDefaultFontSize);

    }

    void UpdateFont() {
        ttfFont = TTF_OpenFont(sFont.data(), iDefaultFontSize);
    }

    /**
    *Renders the debug rect attached to every pane.
    */
    void RenderPaneRect() {
        SDL_SetRenderDrawColor(sdlRenderer, 100, 100, 100, 100);
        SDL_RenderDrawRect(sdlRenderer, &sdlPaneRect);        
    }

    /**
    *Renders all text attached to the TextPane. 
    */
    void RenderPaneText() {
        for (int i = 0; i < vwtlTextLinesToRender.size(); i++) {
            SDL_RenderCopy(sdlRenderer, vwtlTextLinesToRender[i].sdlTextTexture, NULL, &vwtlTextLinesToRender[i].sdlTextRect);
        }
    }

    void RenderAllElements() {
        RenderPaneRect();
        RenderPaneText();
    }

    int ScrollThroughLines(int iDirection) {
        if (iDirection == 1 && iVectorPoint > 0) {
            iVectorPoint--;
            FindLinesToRender();
            return 0;
        }
        if (iDirection == -1 && iVectorPoint < vwtlTextLines.size() - 1) {
            iVectorPoint++;
            FindLinesToRender();
            return 0;
        }
        return 1;
    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector with the prescribed attributes.
    */
    void AddLine(std::string Text, SDL_Color FontColor) {
        WG::TextLine wtlCurrentLine = WG::TextLine(Text, iDefaultFontSize, FontColor);
        GiveLineTexture(&wtlCurrentLine);
        vwtlTextLines.push_back(wtlCurrentLine);
        if (vwtlTextLines.size() > iMaxLines) {
            iVectorPoint++;
        }
    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
    */
    void AddDefaultLine(std::string Text) {
        WG::TextLine wtlCurrentLine = WG::TextLine(Text, iDefaultFontSize, sdlDefaultTextColor);
        GiveLineTexture(&wtlCurrentLine);
        vwtlTextLines.push_back(wtlCurrentLine);
        if (vwtlTextLines.size() > iMaxLines) {
            iVectorPoint++;
        }
    }


    void AddDefaultTextVector(std::vector<std::string> vsText) {

        WG::Logint("vsText size", vsText.size());
        for (int i = 0; i < vsText.size(); i++) {
            WG::Logchar("adding to report pane", vsText[i].data());
            AddDefaultLine(vsText[i]);
        }
    }

    void AddDefaultWrappedText(std::vector<std::string> vsText) {

        for (int i = 0; i < vsText.size(); i++) {
            AddDefaultTextVector(WG::TextWrapping(vsText[i], this->dPaneW * 2, this->dXTextInsetRatio, this->iDefaultFontSize));
        }

    }

    /**
    *Pushes a new line into the TextPane's TextLine std::vector using the TextPane's default attributes.
    */
    void EditTextInsets(double XTextInset, double YTextInset) {
        dXTextInsetRatio = XTextInset;
        dYTextInsetRatio = YTextInset;
    }

    /**
    *Refreshes the texture of each line of text. Must be called for changes in text to manifest during rendering.
    */
    void RefreshAllTextVectorTextures() {

        double dYInset = dPaneY + dYTextInsetRatio * dPaneH;
        double dInitX = dPaneX + dPaneW * dXTextInsetRatio;

        int iIterations = 0;

        vwtlTextLinesToRender.clear();

        for (int i = iVectorPoint; i < vwtlTextLines.size(); i++) {

            TTF_SetFontSize(ttfFont, vwtlTextLines[i].iSize);
            SDL_Surface* sdlTextSurface = TTF_RenderText_Blended(ttfFont, vwtlTextLines[i].sContent.data(), vwtlTextLines[i].sdlFontColor);
            vwtlTextLines[i].sdlTextTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlTextSurface);

            int iTextHeight, iTextWidth;
            SDL_QueryTexture(vwtlTextLines[i].sdlTextTexture, nullptr, nullptr, &iTextWidth, &iTextHeight);

            vwtlTextLines[i].sdlTextRect.w = iTextWidth;
            vwtlTextLines[i].sdlTextRect.h = iTextHeight;
            vwtlTextLines[i].sdlTextRect.x = dInitX;
            vwtlTextLines[i].sdlTextRect.y = dYInset;

            dYInset += iTextHeight;

            vwtlTextLinesToRender.push_back(vwtlTextLines[i]);

            iIterations++;

            if (iIterations == iMaxLines) {
                SDL_FreeSurface(sdlTextSurface);
                return;
            }

            SDL_FreeSurface(sdlTextSurface);

        }


    }
    
    /**
    *Refreshes the texture of each line of text. Must be called for changes in text to manifest during rendering.
    */
   void GiveLineTexture(TextLine* wtlCurrentLine) {

        double dYInset = dPaneY + dYTextInsetRatio * dPaneH;
        double dInitX = dPaneX + dPaneW * dXTextInsetRatio;

        TTF_SetFontSize(ttfFont, wtlCurrentLine->iSize);
        SDL_Surface* sdlTextSurface = TTF_RenderText_Solid(ttfFont, wtlCurrentLine->sContent.data(), wtlCurrentLine->sdlFontColor);
        wtlCurrentLine->sdlTextTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlTextSurface);

        int iTextHeight, iTextWidth;
        SDL_QueryTexture(wtlCurrentLine->sdlTextTexture, nullptr, nullptr, &iTextWidth, &iTextHeight);

        wtlCurrentLine->sdlTextRect.w = iTextWidth;
        wtlCurrentLine->sdlTextRect.h = iTextHeight;

        SDL_FreeSurface(sdlTextSurface);

    }

    void FindLinesToRender() {
        
        double dYInset = dPaneY + dYTextInsetRatio * dPaneH;
        double dInitX = dPaneX + dPaneW * dXTextInsetRatio;

        int iIterations = 0;

        vwtlTextLinesToRender.clear();

        for (int i = iVectorPoint; i < vwtlTextLines.size(); i++) {


            vwtlTextLines[i].sdlTextRect.x = dInitX;
            vwtlTextLines[i].sdlTextRect.y = dYInset;

            dYInset += vwtlTextLines[i].sdlTextRect.h;

            vwtlTextLinesToRender.push_back(vwtlTextLines[i]);

            iIterations++;

            if (iIterations == iMaxLines) { 
                break;
            }

        }

    }



    /**
    *Clears all text assigned to a given pane. Wolfgang will typically call this for you.
    */
    void ClearTextVector() {
        for (int i = 0; i < vwtlTextLines.size(); i++) {
            SDL_DestroyTexture(vwtlTextLines[i].sdlTextTexture);
        }
        vwtlTextLines.clear();
    }

    /**
    *Clears all text from a given vector. Wolfgang will typically call this for you.
    */
   void ClearGivenTextVector(std::vector<WG::TextLine>* vwtlText) {
    for (int i = 0; i < vwtlText->size(); i++) {
        SDL_DestroyTexture(vwtlText->operator[](i).sdlTextTexture);
    }
    vwtlText->clear();
}

};

    class NotificationPaneRenderer {

        public:

        std::vector<WG::NotifcationPane*>* vwnpNotificationPanes;

        NotificationPaneRenderer(std::vector<WG::NotifcationPane*>* NotificationPanes) {

            vwnpNotificationPanes = NotificationPanes;

        }

        int RenderAllNotificationPanes() {

            int iNotificationPanesRendered = 0;

            if (vwnpNotificationPanes->size() > 0) {
                for (int i = 0; i < vwnpNotificationPanes->size(); i++) {
                    vwnpNotificationPanes->operator[](i)->RenderAllElements();
                    iNotificationPanesRendered++;
                }
            }

            return iNotificationPanesRendered;

        }

    };

}


