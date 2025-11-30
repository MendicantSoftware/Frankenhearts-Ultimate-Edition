#include <SDL_ttf.h> 

#include <string>
#include <vector>
#include <sstream>

#include "WolfgangDebug.h"


#pragma once

namespace WG {


    /**
    *Estimates the width a of font of a given font size. The character used is W. 
    *
    *Primarily used for text wrapping.
    */
    int QueryFontWidth(const char* czFontName, int iFontSize, SDL_Renderer* sdlRenderer) {
        TTF_Font* ttfFont = TTF_OpenFont(czFontName, iFontSize);

        if (ttfFont == nullptr) {
            WG::print("ERROR: QueryFontWidth ttfFont is nullptr. Returning default value.");
            WG::printvarchar("Attempted to load font", czFontName);
            return 8;
        }

        SDL_Surface* sdlTextSurface = TTF_RenderText_Solid(ttfFont, "w", SDL_Color{0, 0, 0, 0});
        SDL_Texture* sdlTextTexture = SDL_CreateTextureFromSurface(sdlRenderer, sdlTextSurface);

        int iTextWidth, iTextHeight;

        SDL_QueryTexture(sdlTextTexture, NULL, NULL, &iTextWidth, &iTextHeight);

        SDL_FreeSurface(sdlTextSurface);
        SDL_DestroyTexture(sdlTextTexture);
        TTF_CloseFont(ttfFont);

        return iTextWidth;

    }

    /**
    *Wraps a section of text within a given carriage width, compensating for insets and font width.
    *
    *SDL (and consequently Wolfgang) does not recognise new line characters in text rendering, consequently, a vector of lines must be returned as opposed to inserting new line characters. It is recommended that text is built as lines by the developer, as opposed to relying on text wrapping.
    *
    *iFontWidth can be determined by using WG::QueryFontWidth.
    */
    std::vector<std::string> TextWrapping(const std::string& sText, int iCarriageWidth, double dXInsetRatio, int iFontWidth) {
        std::vector<std::string> vsWrappedText;
        iCarriageWidth = iCarriageWidth - iCarriageWidth * dXInsetRatio;
        iCarriageWidth = iCarriageWidth / iFontWidth; 

        WG::printvarint("iCarriageWidth", iCarriageWidth);

        std::stringstream ssStringStream(sText);
        std::string sWord;
        std::string sLine;



        while (ssStringStream >> sWord) {
            if (sLine.empty()) {
                sLine = sWord;
            } else if (sLine.length() + sWord.length() + 1 <= iCarriageWidth) {
                sLine += " " + sWord;
            } else {
                vsWrappedText.push_back(sLine);
                sLine = sWord;
            }
        }

        vsWrappedText.push_back(sLine);

        return vsWrappedText;
    }

}

