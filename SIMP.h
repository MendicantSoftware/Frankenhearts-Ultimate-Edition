/*

    SIMP (Simple Instant Messaging Protocol)

    This is the SIMPScript Language Library


*/

#include <string>

#pragma once

//System
std::string SIMP_Server_Exit = "EXIT";

std::string SIMP_Client_Exit = "/EXIT";

//Connection Management
std::string SIMP_Server_List = "LIST/";
std::string SIMP_Server_DCNick = "DCNICK/";
std::string SIMP_Server_DCIP = "DCIP/";
std::string SIMP_Server_DCI = "DCI/";
std::string SIMP_Server_DMIP = "DMIP/";

std::string SIMP_Client_ForceExit = "/SERVERDEMANDSBYEBYE";

 
//Communication
std::string SIMP_Client_DM_Nickname = "/DM";

std::string SIMP_Server_Mute_Index = "MI/";
std::string SIMP_Server_Unmute_Index = "UMI/";

std::string SIMP_Server_ServerMute_All = "SM/";
std::string SIMP_Server_ServerUnmute_All = "SUM/";


//Easter Egg
std::string SIMP_Server_MisterMeowie = "MisterMeowie";
std::string SIMP_Client_MisterMeowie = "/MisterMeowie";

//WOLFGANG
std::string SIMP_WOLFGANG_DEBUG_ECHO = "WE/";

//Frankenhearts
std::string SIMP_FRANKENHEARTS_GameStart = "GameStart/";

std::string SIMP_FRANKENHEARTS_DeckStart = "/DECKSTART";
std::string SIMP_FRANKENHEARTS_DeckFinish = "/DECKFINISH";

std::string SIMP_FRANKENHEARTS_PreviousTrickStart = "/PREVCARDSTART";
std::string SIMP_FRANKENHEARTS_PreviousTrickFinish = "/PREVCARDFINISH";

std::string SIMP_FRANKENHEARTS_CardsRecievedStart = "/CARDSRECIEVEDSTART";
std::string SIMP_FRANKENHEARTS_CardsRecievedEnd = "/CARDSRECIEVEDEND";

std::string SIMP_FRANKENHEARTS_CardsInTrickStart = "/CARDSINTRICKSTART";
std::string SIMP_FRANKENHEARTS_CardsInTrickFinish = "/CARDSINTRICKFINISH";

std::string SIMP_FRANKENHEARTS_PlayerScoreStringsStart = "/PLAYERSCORESTRINGSSTART";
std::string SIMP_FRANKENHEARTS_PlayerScoreStringsFinish = "/PLAYERSCORESTRINGSFINISH";

std::string SIMP_FRANKENHEARTS_SendIndividualCard = "/CARD";

std::string SIMP_FRANKENHEARTS_PassStatus = "/PASS";

std::string SIMP_FRANKENHEARTS_PassVectorStart = "/PASSDATASTART";
std::string SIMP_FRANKENHEARTS_PassVectorFinish = "/PASSDATAFINISH";

std::string SIMP_FRANKENHEARTS_HeartsStatus = "/HEARTS";

std::string SIMP_FRANKENHEARTS_Acheivement = "/ACHIEVEMENT";

std::string SIMP_FRANKENHEARTS_Prompt = "/PROMPT";