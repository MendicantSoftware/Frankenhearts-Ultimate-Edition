#include <SDL.h>
#include <SDL_image.h>

#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <functional>

#include "WolfgangVectors.h"
#include "WolfgangDebug.h"

#define TILEHEIGHT 20

#pragma once

namespace WG {

    /**
    *Returns the SpriteSheet/Sprite located at the given path as an SDL_Texture
    */
    SDL_Texture* ConvertSpriteToTexture(SDL_Renderer* sdlRenderer, const char* cSpriteSheetDirectory) {
        SDL_Surface* sdlSpriteSheetSurface = IMG_Load(cSpriteSheetDirectory);
        SDL_Texture* sdlSpriteSheet = SDL_CreateTextureFromSurface(sdlRenderer, sdlSpriteSheetSurface);
        SDL_FreeSurface(sdlSpriteSheetSurface);
        return sdlSpriteSheet;
    }



    /**
    *WG Tiles and Tile Maps (by default) accept WG::TileSprite(s) built using ConvertSpriteToTexture.
    *
    *This is the structure of a TileMap:
    *SourceImage -> ConvertSpriteToTexture -> TileSprite -> TileMap
    *
    * Whilst not required, it is recommended to use this format.
    */
    class TileSprite {

        public: 

        SDL_Texture* sdlSpriteSheet;

        SDL_Rect sdlSourceRect;
        SDL_Rect sdlDestinationRect;

        /**
        *Updates the TileSprite's sdlSourceRect member to reflect the intended sprite's location within the sprite's sheet.
        */
        void CalculateSourceRect(int iRowInSheet, int iColInSheet, int iSpriteSizeW, int iSpriteSizeH) {
            sdlSourceRect.x = iSpriteSizeW * iColInSheet;
            sdlSourceRect.y = iSpriteSizeH * iRowInSheet;
            sdlSourceRect.w = iSpriteSizeW;
            sdlSourceRect.h = iSpriteSizeH;
            sdlDestinationRect.w = sdlSourceRect.w;
            sdlDestinationRect.h = sdlSourceRect.h;
        }


    };

    /**
    *WG::TileMap(s) automatically create Tiles, generally it is not requried to declare them.
    *Tiles within a TileMap are accessed through the TileMap member: std::vector<Tile> vwtAllTiles
    */
    class Tile {

    public:

    WG::Vector wvPos;
    TileSprite tsSprite;

    TileSprite tsPreferedSprite;

    std::vector<TileSprite> vwtsSprites;

    int iCol, iRow;

    int iTerrainType = 0;

    int iTileIndex;

    bool bHighlighted = false;
    bool bSelectHighlighted = false;
    bool bMovementHighlighted = false;
    bool bScannerHighlighted = false;
    bool bTargetHighlighted = false;

    bool bOccupied = false;

    bool bTree = false;

    WG::Vector wvLeftMidPoint;
    WG::Vector wvRightMidPoint;
    WG::Vector wvTopPoint;
    WG::Vector wvBotPoint;

    //Funky Function Pointer Shit

    std::function<int(void)> fpWFunction = nullptr;
    std::function<int(void)> fpEFunction = nullptr;
    std::function<int(void)> fpRFunction = nullptr;
    std::function<int(void)> fpAFunction = nullptr;

    std::function<int(int)> fpLeftClickFunction = nullptr;
    std::function<int(int)> fpMouseHoverFunction = nullptr;
    std::function<int(WG::Tile)> fpRightClickFunction = nullptr;
    std::function<void(int, int)> fpConfirmPositionFunction = nullptr;

    //Mouse Types

    int iLeftMouseClick = -1;
    int iRightMouseClick = -1;
    int iMouseHover = -1;


    /**
    *Renders a particular Tile, if the Tile is within a TileMap, the TileMap will call this function.
    *
    *Normally, this should not be directly called.
    */
    void RenderTile(SDL_Renderer* sdlRenderer) {
        for (int i = 0; i < vwtsSprites.size(); i++) {

            int iHeightDifference = 0;

            if (vwtsSprites[i].sdlSourceRect.h > TILEHEIGHT) {
                iHeightDifference = vwtsSprites[i].sdlSourceRect.h - TILEHEIGHT;
            }

            vwtsSprites[i].sdlDestinationRect.x = int(wvPos.dx);
            vwtsSprites[i].sdlDestinationRect.y = int(wvPos.dy) - iHeightDifference; 
            SDL_RenderCopy(sdlRenderer, vwtsSprites[i].sdlSpriteSheet, &vwtsSprites[i].sdlSourceRect, &vwtsSprites[i].sdlDestinationRect);
        }
        //FIND A NEW SPOT FOR THIS
        CalculateDetectionVectors();
    }

    /**
    *Used by the TileMap to establish where in the isometric grid a particular tile lies.
    */
    void EstablishPositionInMap(int Col, int Row) {
        iCol = Col;
        iRow = Row;
    }

    /**
    *Before Mouse Detection can be used, a tile's detection Vectors MUST be initialised. 
    *
    *Typically, this does not need to be explictly called.
    */
    void CalculateDetectionVectors() {
        //The click detection of a given tile uses 

        wvLeftMidPoint.dx = vwtsSprites[0].sdlDestinationRect.x;
        wvLeftMidPoint.dy = vwtsSprites[0].sdlDestinationRect.y + tsSprite.sdlDestinationRect.h / 2;

        wvRightMidPoint.dx = vwtsSprites[0].sdlDestinationRect.x + tsSprite.sdlDestinationRect.w;
        wvRightMidPoint.dy = vwtsSprites[0].sdlDestinationRect.y + tsSprite.sdlDestinationRect.h / 2;

        wvTopPoint.dx = vwtsSprites[0].sdlDestinationRect.x + tsSprite.sdlDestinationRect.w / 2;
        wvTopPoint.dy = vwtsSprites[0].sdlDestinationRect.y;

        wvBotPoint.dx = vwtsSprites[0].sdlDestinationRect.x + tsSprite.sdlDestinationRect.w / 2;
        wvBotPoint.dy = vwtsSprites[0].sdlDestinationRect.y + tsSprite.sdlDestinationRect.h;

    }

    /**
    *Determine's if the User's mouse is within a tile's ISOMETRIC projection. 
    *
    *Returns true if mouse is within projection, otherwise false. Also stores result in bHighlight.
    */
    bool MouseDetection(Vector wvMousePos, SDL_Renderer* sdlRenderer) {

        /* 
            This mouse detection uses a "Point in Polygon" algorithm, more specifically the ray casting version of it. It works thusly:
                1. Store the edges of the polygon (Calculate Detection Vector)
                2. Cast a horizontal ray from the mouse heading toward X Positive
                3. If this ray intersects the edges of the polygon an odd number of times, the point is in the polygon, else it is not.
                    Think about it like this, if the point is inside the polygon, then it can only exit, else it must enter and exit.
                4. If the ray directly intersects a Vector intersection, it counts the intersect twice, this is why an exception is hard coded.
        */

        int iIntersections = 0;

        std::vector<WG::Edge> vweAllEdges = {WG::Edge(wvLeftMidPoint, wvTopPoint), WG::Edge(wvLeftMidPoint, wvBotPoint), WG::Edge(wvRightMidPoint, wvTopPoint), WG::Edge(wvRightMidPoint, wvBotPoint)};

        SDL_Rect sdlRayCast;
        sdlRayCast.x = wvMousePos.dx;
        sdlRayCast.y = wvMousePos.dy;
        sdlRayCast.w = tsSprite.sdlDestinationRect.w * 200;
        sdlRayCast.h = 1;

        for (int i = 0; i < vweAllEdges.size(); i++) {

            int iEdge1x = vweAllEdges[i].wvV1.dx;
            int iEdge1y = vweAllEdges[i].wvV1.dy;
            int iEdge2x = vweAllEdges[i].wvV2.dx;
            int iEdge2y = vweAllEdges[i].wvV2.dy;

            if (SDL_IntersectRectAndLine(&sdlRayCast, &iEdge1x, &iEdge1y, &iEdge2x, &iEdge2y)) {
                iIntersections++;
                if (iIntersections == 2 && wvMousePos.dy == wvRightMidPoint.dy && wvMousePos.dx > wvPos.dx) {
                    iIntersections = 1;
                }
            }
        }       

        if (iIntersections % 2 != 0) {
            bHighlighted = true;
            ExecuteMouseHoverFunction(iMouseHover);
        }

        return bHighlighted;

    }

    void AttachWFunction(std::function<int(void)> WFunction) {
        fpWFunction = WFunction;
    }

    void AttachEFunction(std::function<int(void)> EFunction) {
        fpEFunction = EFunction;
    }

    void AttachRFunction(std::function<int(void)> RFunction) {
        fpRFunction = RFunction;
    }

    void AttachAFunction(std::function<int(void)> AFunction) {
        fpAFunction = AFunction;
    }

    void AttachLeftClickFunction(std::function<int(int)> LeftClickFunction) {
        fpLeftClickFunction = LeftClickFunction;
    }

    void AttachRightClickFunction(std::function<int(WG::Tile)> RightClickFunction) {
        fpRightClickFunction = RightClickFunction;
    }

    void AttachMouseHoverFunction(std::function<int(int)> MouseHoverFunction) {
        fpMouseHoverFunction = MouseHoverFunction;
    }

    void AttachConfirmPositionFunction(std::function<void(int, int)> ConfirmPositionFunction) {
        fpConfirmPositionFunction = ConfirmPositionFunction;
    }

    int ExecuteWFunction() {
        if (fpWFunction) {
            fpWFunction();
            return 0;
        }
        return 1;
    }

    int ExecuteEFunction() {
        if (fpEFunction) {
            fpEFunction();
            return 0;
        }
        return 1;
    }

    int ExecuteRFunction() {
        if (fpRFunction) {
            fpRFunction();
            return 0;
        }
        return 1;
    }

    int ExecuteAFunction() {
        if (fpAFunction) {
            fpAFunction();
            return 0;
        }
        return 1;
    }

    int ExecuteLeftClickFunction(int iTextPaneIndex) {
        if (fpLeftClickFunction) {
            fpLeftClickFunction(iTextPaneIndex);
            return 0;
        }
        return 1;
    }

    int ExecuteRightClickFunction(WG::Tile TargetTile) {
        if (fpRightClickFunction) {
            fpRightClickFunction(TargetTile);
            return 0;
        }
        return 1;
    }

    int ExecuteMouseHoverFunction(int iTextPaneIndex) {
        if (fpMouseHoverFunction) {
            fpMouseHoverFunction(iTextPaneIndex);
            return 0;
        }
        return 1;
    }

    int ExecuteConfirmPositionFunction(int iUpdatedCol, int iUpdatedRow) {
        if (fpConfirmPositionFunction) {
            fpConfirmPositionFunction(iUpdatedCol, iUpdatedRow);
            return 0;
        }
        return 1;
    }


    };

    /**
    *A TileMap is WG's default method of producing an isometric TileMap. 
    * 
    * It is recommended to create Tiles through the TileMap object. Before map
    * can be fully utilised the following structure should be followed:
    * 
    *SourceImage -> ConvertSpriteToTexture -> TileSprite -> TileMap
    */
    class TileMap {
        public:

        int iRows, iCols;
        int iX, iY;
        int iXInit, iYInit;

        std::vector<Tile> vwtAllTiles = {};

        std::vector<Tile> vwtSubMap = {};

        TileSprite tsDefaultSprite;
        TileSprite tsYellowHighlightSprite;
        TileSprite tsRedHighlightSprite;
        TileSprite tsBlueHighlightSprite;
        TileSprite tsTreeSprite;

        SDL_Rect sdlMouseDetectionRect;

        /**
        *This function is going to be removed. Do not use it.
        */
        void PrepareDefaultTextures(SDL_Renderer* sdlRenderer) {
            tsDefaultSprite.sdlSpriteSheet = WG::ConvertSpriteToTexture(sdlRenderer, "isometric_demo.png");
            tsDefaultSprite.CalculateSourceRect(0, 1, 40, 20);

            tsYellowHighlightSprite.sdlSpriteSheet = tsDefaultSprite.sdlSpriteSheet;
            tsYellowHighlightSprite.CalculateSourceRect(0, 0, 40, 20);

            tsTreeSprite.sdlSpriteSheet = tsDefaultSprite.sdlSpriteSheet;
            tsTreeSprite.CalculateSourceRect(0, 2, 40, 20);

            tsRedHighlightSprite.sdlSpriteSheet = WG::ConvertSpriteToTexture(sdlRenderer, "Highlights.png");
            tsRedHighlightSprite.CalculateSourceRect(0, 1, 40, 20);

            tsBlueHighlightSprite.sdlSpriteSheet = tsRedHighlightSprite.sdlSpriteSheet;
            tsBlueHighlightSprite.CalculateSourceRect(0, 2, 40, 20);


            

        }


        /**
        *Creates each tile within the map, establishes its position within the array, and stores these tiles within a vector
        *
        *This need only be called upon the creation of a TileMap. This function will delete all tiles in a TileMap.
        * 
        */
        void CalculateTileMap(int Cols, int Rows, int iXStart, int iYStart) {

            vwtAllTiles.clear();

            iRows = Rows;
            iCols = Cols;

            iX = iXStart;
            iY = iYStart;

            iXInit = iXStart;
            iYInit = iYStart;

            int iCurrentRow = 1;
            int iCurrentCol = 1;

            int iCurrentX = iX;
            int iCurrentY = iY;

            for(int i = 0; i < (iRows * iCols); i++) {


                Tile wtCurrentTile;
                
                if (wtCurrentTile.bHighlighted) {
                    wtCurrentTile.tsSprite = tsYellowHighlightSprite;
                } else {
                    wtCurrentTile.tsSprite = tsDefaultSprite;
                }

                wtCurrentTile.EstablishPositionInMap(iCurrentCol, iCurrentRow);

                wtCurrentTile.wvPos = WG::Vector(iCurrentX, iCurrentY);
                wtCurrentTile.tsPreferedSprite = tsDefaultSprite;

                vwtAllTiles.push_back(wtCurrentTile);

                if (iCurrentCol == iCols) {
                    iCurrentRow++;
                    iCurrentCol = 0;
                    iY +=  wtCurrentTile.tsSprite.sdlDestinationRect.h / 2;
                    iX +=  wtCurrentTile.tsSprite.sdlDestinationRect.w / 2;
                    iCurrentX = iX;
                    iCurrentY = iY;
                } else {
                    iCurrentY = iY - (iCurrentCol * wtCurrentTile.tsSprite.sdlDestinationRect.h / 2);
                    iCurrentX = iX + (iCurrentCol * wtCurrentTile.tsSprite.sdlDestinationRect.w / 2);
                } 

                

                iCurrentCol++;

            }

        }

        void CalculateSubMap(int iColsToRender, int iRowsToRender, int &iStartCol, int &iStartRow) {

            if (iStartCol < 1) {
                iStartCol = 1;
            }

            if (iStartRow < 1) {
                iStartRow = 1;
            }

            if (iStartCol > iCols - iColsToRender) {
                iStartCol = iCols - iColsToRender;
            }

            if (iStartRow > iRows - iRowsToRender) {
                iStartRow = iRows - iRowsToRender;
            }

            int iVectorIndex = TileIndex(iStartCol, iStartRow);
            int iTilesToRender = iColsToRender * iRowsToRender;

            iX = iXInit;
            iY = iYInit;

            int iCurrentX = iX;
            int iCurrentY = iY;

            int iCurrentCol = 1;
            int iCurrentRow = iStartRow;


            vwtSubMap.clear();
            
            for (int i = 0; i < iTilesToRender; i++) {

                vwtSubMap.push_back(vwtAllTiles[iVectorIndex]);
                vwtSubMap[i].wvPos = WG::Vector(iCurrentX, iCurrentY);

                if (iCurrentCol == iColsToRender) {
                    iCurrentRow++;
                    iCurrentCol = 0;
                    iY += vwtSubMap[i].tsSprite.sdlDestinationRect.h / 2;
                    iX += vwtSubMap[i].tsSprite.sdlDestinationRect.w / 2;
                    iCurrentX = iX;
                    iCurrentY = iY;
                    iVectorIndex = TileIndex(iStartCol, iCurrentRow);
                } else {
                    iCurrentY = iY - (iCurrentCol * vwtSubMap[i].tsSprite.sdlDestinationRect.h / 2);
                    iCurrentX = iX + (iCurrentCol * vwtSubMap[i].tsSprite.sdlDestinationRect.w / 2);
                    iVectorIndex++;
                } 

                iCurrentCol++;

            }

        }

        /**
        *Determines if a tile ought have the highlighted sprite attached to it.
        */
        void UpdateTexture(std::vector<Tile> *vwtTiles) {
            for(int i = 0; i < vwtTiles->size(); i++) {
                vwtTiles->operator[](i).vwtsSprites.clear();

                vwtTiles->operator[](i).vwtsSprites.push_back(vwtTiles->operator[](i).tsSprite = vwtTiles->operator[](i).tsPreferedSprite);

                if (vwtTiles->operator[](i).bSelectHighlighted) {
                    vwtTiles->operator[](i).vwtsSprites.push_back(tsYellowHighlightSprite);
                }

                if (vwtTiles->operator[](i).bMovementHighlighted) {
                    vwtTiles->operator[](i).vwtsSprites.push_back(tsYellowHighlightSprite);
                }

                if (vwtTiles->operator[](i).bTargetHighlighted) {
                    vwtTiles->operator[](i).vwtsSprites.push_back(tsRedHighlightSprite);
                }

                if (vwtTiles->operator[](i).bScannerHighlighted) {
                    vwtTiles->operator[](i).vwtsSprites.push_back(tsBlueHighlightSprite);
                }

                if (vwtTiles->operator[](i).bHighlighted) {
                    vwtTiles->operator[](i).vwtsSprites.push_back(tsYellowHighlightSprite);
                }
            }
        }


        /**
        *Makes bSelectHighlighted false for all tiles within a tilemap.
        */
        void ClearAllStaticHighlights() {
            for (int i = 0; i < vwtAllTiles.size(); i++) {
                vwtAllTiles[i].bSelectHighlighted = false;
            }
        }

        void ClearAllScannerHighlights() {
            for (int i = 0; i < vwtAllTiles.size(); i++) {
                vwtAllTiles[i].bScannerHighlighted = false;
            }
        }

        /**
        *Makes bMovementHighlighted false for all tiles within a tilemap.
        */
        void ClearAllMovementHighlights() {
            for (int i = 0; i < vwtAllTiles.size(); i++) {
                vwtAllTiles[i].bMovementHighlighted = false;
            }
        }

        /**
        *Makes bTargetHighlighted false for all tiles within a tilemap.
        */
       void ClearAllTargetHighlights() {
            for (int i = 0; i < vwtAllTiles.size(); i++) {
                vwtAllTiles[i].bTargetHighlighted = false;
            }
        }

        /**
        *Renders all tiles in the tile map.
        */
        void RenderTileMap(SDL_Renderer* sdlRenderer) {

            UpdateTexture(&vwtAllTiles);


            for (int i = 0; i < vwtAllTiles.size(); i++) {
                vwtAllTiles[i].RenderTile(sdlRenderer);
            }
        }

        /**
        *Render Section
        */
        void RenderTileMapSection(SDL_Renderer* sdlRenderer) {

            UpdateTexture(&vwtSubMap);

            for(int i = 0; i < vwtSubMap.size(); i++) {
                vwtSubMap[i].RenderTile(sdlRenderer);
            }

        }

        /**
        *Identifies if the mouse is hovering over a tile, marks that tile as Highlighted if it exists.
        * This checks every tile within the map, but only one tile may be highlighted, the function will
        * return as soon as it identifies a single detection.
        */
        void MouseDetection(Vector wvMousePosition, std::vector<Tile> *vwtTiles, SDL_Renderer* sdlRenderer) {
            for (int i = 0; i < vwtTiles->size(); i++) {
                vwtTiles->operator[](i).bHighlighted = false;
            }

            for (int i = 0; i < vwtTiles->size(); i++) {
                vwtTiles->operator[](i).bHighlighted = false;
                bool bDetectionDetected = vwtTiles->operator[](i).MouseDetection(wvMousePosition, sdlRenderer);

                if (bDetectionDetected) {
                    break;
                }

            }

        }

        /**
        *Used for locating a Tile within a TileMap vector
        */
        int TileIndex(int iCol, int iRow) {

            /*
            BIG TIME IMPORTANT:
            This is possibly one of the most fundamentally important lines of code in the whole engine.
            Do not change it lightly, many other systems instantly break.
            iCols is not a typo, it is to step up through the vector.

            It intentionally has no error checking, it is called too often for this.

            */

            return iCols * (iRow - 1) + iCol - 1;

        }

        /**
        *Returns a Wolfgang vector where dx is width and dy is height
        */
        WG::Vector MapSize(std::vector<Tile> vwtMap)  {
            WG::Vector MapDiamensions;
            int iVectorSize = vwtMap.size() - 1;

            MapDiamensions.dx = vwtMap[iVectorSize].wvPos.dx + vwtSubMap[iVectorSize].tsSprite.sdlDestinationRect.w - vwtMap[0].wvPos.dx;
            MapDiamensions.dy = vwtMap[iVectorSize].wvPos.dy + vwtSubMap[iVectorSize].tsSprite.sdlDestinationRect.w - vwtMap[0].wvPos.dy;

            return MapDiamensions;
        }
       


    };


}

