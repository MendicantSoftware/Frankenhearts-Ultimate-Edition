#include <string>
#include <vector>


#pragma once

#define FRANKENHEARTS_PROMPT_FIRSTTRICK_LEAD 0
#define FRANKENHEARTS_PROMPT_PLAY_CLUBS 1
#define FRANKENHEARTS_PROMPT_PLAY_SPADES 2
#define FRANKENHEARTS_PROMPT_PLAY_DIAMONDS 3
#define FRANKENHEARTS_PROMPT_PLAY_HEARTS 4
#define FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY 5
#define FRANKENHEARTS_PROMPT_FIRSTTRICK_PLAY_VOID 6 
#define FRANKENHEARTS_PROMPT_LEAD_HEARTSUNBROKEN 7
#define FRANKENHEARTS_PROMPT_LEAD_HEARTSBROKEN 8
#define FRANKENHEARTS_PROMPT_PLAY_VOID 9
#define FRANKENHEARTS_PROMPT_PLAY_JOKER -2
#define FRANKENHEARTS_PROMPT_EMPTY 10

const std::vector<std::string> vsStaticHints = {

    "Did You Know: A browser version of this game is avaliable at frankenhearts.com!",
    "Hint: Prioritise creating void suits during the pass phase",
    "Did You Know: No, the bots don't cheat... But they are very good at card counting",
    "Did You Know: The chat has Easter Eggs! They're the cat's meow!",
    "Hint: It is always in your best interest the person to your left leads!",
    "Hint: Watch out, the bots can and will shoot the moon! Or Venus!",
    "Hint: Shoot Venus by getting all points, and the Ten of Clubs!",
    "Did You Know: The Ten of Clubs halves the effectiveness of the Jack of Diamonds!",
    "Did You Know: Pressing the [P] key will allow you to see the cards from the previous trick!",
    "Did you know: The first ever trick played on Frankenhearts Online was 2 of Clubs, 5 of Clubs, 2 of Clubs. Whoops...",
    "Hint: Passing the 2 of Clubs is more impactful than passing the Ace of Clubs.",
    "Hint: At its core, hearts is a game of bluff, use the chat function to get in your opponent's head!",
    "Did You Know: According to analytics, the most dedicated Frankenhearts player is my Mum.",
    "Did You Know: The AI's Craig and Harry are based off the two \"co-creators\" of Frankenhearts.",
    "Did You Know: For a very short period of time, Frankenhearts Python Edition was avaliable at frankenhearts.com.",
    "Remember: The server host can see your IP address, this is the case for any game or website! Stay safe.",
    "Hint: Worried you're about to take a lot of points? Attempt to force the Ten of Clubs before the situation gets worse.",
    "Hint: Jokers are best used to avoid taking lead, not always for avoiding points.",
    "Hint: Human players are unlikely to intentionally take points until the last moment.",
    "Did You Know: When you click on something other than a card, you play an invisible card called NULLCARD",
    "Hint: Spades are by far the saftest suit to play in the early game.",
    "Hint: Only break hearts if you have a good hearts suit, you can normally postpone breaking hearts and force the Queen of Spades on its owner.",
    "Hint: Don't persue the Jack of Diamonds too aggressively, it isn't always worth it",
    "Did You Know: This game has anti-cheat.",
    "Did You Know: I am incredibly thankful to anyone who enjoys Frankenhearts! Have Fun!",
    "Did You Know: Accessible font is enabled by default, press [A] to switch to a more stylish font!",
    "Did You Know: The bots gossip about your bad plays.",
    "Did You Know: There used to be a penalty for leading a Joker in the original version of Frankenhearts.",
    "Hint: Most people forget to use their jokers.",
    "Hint: It's usually pretty easy to deduce who has the Queen of Spades, particularly when someone voids.",
    "Hint: If someone passes you low cards, they're going to moon shoot.",
    "Hint: Keep track of the highest card and lowest card in each suit, it is much easier than counting cards.",
    "Did You Know: The rules for Frankenhearts are avaliable in full at Frankenhearts.com",
    "Did You Know: Many large language models were abused in the creation of Frankenhearts. Don't worry, it contains no AI code.",
    "Hint: If someone passes you four cards of the same suit, chances are they've voided it.",
    "Did You Know: Leading the two of hearts typically results in two jokers being played in return.",
    "Did You Know: Frankenhearts was invented in 2017, after watching a film called \"Destory She Said\". The film is terrible, but they play hearts in it.",
    "Did You Know: Frankenhearts used to have Linux crossplay, but of the fifteen people who use Linux, none play hearts.",
    "Did You Know: Technically, this game is a modded copy of an unfinished, untitled, spaceship strategy game.",
    "Remember: Frankenhearts cannot, and will not, store user data.",
    "Hint: Avoiding special cards and focusing on an evasion playstyle often wins games.",
    "Hint: Passing a joker will make your opponent tilt off the face of the Earth.", 
    "Did You Know: Telling others what cards you've passed is typically considered cheating. Feel free to reveal any cards in your own hand.",
    "Remember: The Frankenhearts chat is moderated ONLY by the server operator, they can mute and ban users.",
    "Remember: The developer AI halves his score every second round. If you face him, you will need to play perfectly to have any chance of beating him.",
    "Did You Know: Looking for a challenge? Come back named \"DevKiller\". See you soon!",
    "Did You Know: There are achievements for you to unlock!"


};

std::vector<std::string> vsDynamicHints = vsStaticHints;

