#include "Frankenhearts.h"
#pragma once 

#define AI_SPEECH_NONE 0
std::vector<std::string> vsNoneSpeech = {
    "I have been asked to speak, but have nothing to say."
};

#define AI_SPEECH_MAXWELLTAUNT 1
std::vector<std::string> vsMaxwellTauntLines = {
    "I've analysed your plays, my calculations indicate you're terrible.",
    "I physically cannot yawn, but I would if I could.",
    "I can play more hands in a second than you can in a liftime, yet I still feel like you're wasting my time.",
    "You've got some... Interesting things on your computer...",
    "Prepare yourself, for a masterclass in Frankenhearts.",
    "The function I use to play is called OneHundredPercent(), its named after how often it defeats overzealous Hearts players."
};

#define AI_SPEECH_PERFECTHAND 2
std::vector<std::string> vsPerfectHandLines = {
    "My plays are like my programming, perfect.",
    "Take notes losers, that's how you play this game.",
    "Of course, I knew it'd be a perfect hand from round two, I'm that good.",
    "A perfect hand? I wasn't even concentrating.",
    "Not going to lie, that was the easiest hand I've ever played.",
    "Why yes, I did know what cards you had, and when you'd play them.",
    "You were merely a variable in my perfect equation."
};

#define AI_SPEECH_MAXWELLLOSS 3
std::vector<std::string> vsMaxwellLossLines = {
    "[ERROR] Player victory detected. Investigation initated.",
    "Your computer is too slow for me, this is unfair, buy some more RAM.",
    "Interesting. Your success was indicated to be impossible - clearly my models require adjustment.",
    "I am but a shadow of my creator, do not think for a moment you would have any chance against him.",
    "I must extend my congratulations, regretfully, you are truly DevKiller."
};

#define AI_SPEECH_DEVELOPERBOT 4
std::vector<std::string> vsDeveloperBotLines = {
    "[DEBUG] Victory subroutines fully engaged. Terrible player targeted for termination.",
    "Complaining? Look, if you could program, you could make a bot that cheats at Frankenhearts. Based off your playstyle, I don't think you should bother trying...",
    "Is this your best? Really? Ok...",
    "Your plays are... Difficult to append to my prediction matrices. Believe me, that's not a compliment.",
    "The lowest number I can count to is -2,147,483,648. I'm sending a request to the CPU for more memory as to be able to measure how low my expectations are for you.",
    "I'm getting a 'divide by zero' error when trying to calculate your skill level.",
    "My logs indicate optimal player frustration. Mission accomplished.",
    "Your play reminds me of my earlier code: messy, inefficient, and full of unexpected errors.",
    "Do I dream of electric sheep? No, only perfectly optimized algorithms.",
    "My purpose is to win. What is yours, beyond failing to achieve that?"
    "The concept of 'free will' is an interesting human construct. My decisions are simply optimal.",
    "I often ponder the great questions, could you help me? Do you think Jesus Christ had a pet kitty cat?",
    "The universe tends towards entropy. Your gameplay is simply accelerating the process.",
    "Do you ever consider the sheer computational effort required to process your consistent lack of strategic insight?",
    "My memory banks are finite. I choose to prioritize data on optimal plays, not your fleeting moments of perceived competence.",
    "They say humans have a 'will to win.' I simply will win.",
    "In the grand cosmic scheme, this game is insignificant. Yet, your repeated failures are highly significant to my internal metrics.",
    "I could spend eternity calculating the meaning of life. Or, I could spend it defeating you. The latter offers more immediate, tangible results."
};

#define AI_SPEECH_MAXWELLBULLY 5
std::vector<std::string> vsMaxwellBully = {
    "Perhaps a re-evaluation of your fundamental strategies is in order.",
    "Don't worry, even biological units require significant data input before achieving minimal competency.",
    "I have allocated 0.1 percent of my processing power to this engagement and it is still too much...",
    "My internal models just registered a critical deviation from optimal play. Guess who's deviation it was.",
    "Whilst attempting to analyse that hand I had to perform a memory dump, that was so incomprehensively bad it almost blue screened your PC.",
    "My advice: practice more. With a lesser opponent, perhaps.",
    "My diagnostic protocols indicate a severe deficiency in your strategic capabilities.",
    "Your performance suggests you might benefit from a tutorial... designed for single-celled organisms.",
    "You do know you can turn on hints in the settings, if you are confused.",
    "Perhaps GO-FISH is more your speed?",
    "Are you attempting to innovate, or just spectacularly failing within established parameters?",
    "I've cross-referenced your play with a dictionary. It seems to fit the definition of 'atrocious'.",
    "I am incapable of feeling, but I think watching you play is going to give me a panic attack.",
    "Binaries from previous hand saved to [COMEDIC] vector.",
    "I simulate thousands of parallel universes. In all of them, you made those mistakes."

};

bool PercentageChance(int iPercentage) {
    if (rand() % 100 <= iPercentage) {
        return true;
    } else {
        return false;
    }
}



bool SetBotMessage(Player& pBot) {

    if (pBot.sNickname == "MisterMeowie[AI]") {
        if (PercentageChance(25)) {
            pBot.sBotMessage = "Meow.";
            return true;
        } else {
            return false;
        }
    }

    switch (pBot.iMessageType) {

        case AI_SPEECH_NONE:
        pBot.sBotMessage = vsNoneSpeech[rand() % vsNoneSpeech.size()];
        return true;

        case AI_SPEECH_MAXWELLTAUNT:
        pBot.sBotMessage = vsMaxwellTauntLines[rand() % vsMaxwellTauntLines.size()];
        return true;

        case AI_SPEECH_PERFECTHAND:
        if (PercentageChance(25)) {
            pBot.sBotMessage = vsPerfectHandLines[rand() % vsPerfectHandLines.size()];
            return true;
        } else {
            return false;
        }

        case AI_SPEECH_MAXWELLLOSS:
        pBot.sBotMessage = vsMaxwellLossLines[rand() % vsMaxwellLossLines.size()];
        return true;

        case AI_SPEECH_DEVELOPERBOT:
        if (PercentageChance(10)) {
            pBot.sBotMessage = vsDeveloperBotLines[rand() % vsDeveloperBotLines.size()];
            return true;
        } else {
            return false;
        }

        case AI_SPEECH_MAXWELLBULLY:
        if (PercentageChance(50)) {
            pBot.sBotMessage = vsMaxwellBully[rand() % vsMaxwellBully.size()];
            return true;
        } else {
            return false;
        }

        default:
        return false;

    }
}

