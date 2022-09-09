#ifndef GOOSE_ESCAPE_GAMEPLAY
#define GOOSE_ESCAPE_GAMEPLAY
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"

const int EMPTY = 0;
const int WALL = 1;
const int TRAP = 2;
const int WINNER = 3;
const int WIN_X = 140;
const int WIN_Y = 43;
const int TRAP_X1 = 80;
const int TRAP_Y1 = 45;
const int TRAP_X2 = 150;
const int TRAP_Y2 = 20;

const int PLAYER_START_X = 0;
const int PLAYER_START_Y = 0;
const int PLAYER_RANGE_X = 15;
const int PLAYER_RANGE_Y = 15;

const int GOOSE_START_X = 100;
const int GOOSE_START_Y = 18;
const int GOOSE_RANGE_X = 20;
const int GOOSE_RANGE_Y = 10;

const int WARNING_RANGE = 15;
const int FLASH = 40;
const bool ANGRY = true;

//display characters
const int PLAYER_CHAR = int('@');
const int MONSTER_CHAR = int('G');
const int WALL_CHAR = int('#');
const int TRAP_CHAR = int('X');
const int WIN_CHAR = int('%');	//% sign, a special character used in the ancient game "Rogue"

/*
Game play function prototypes are given below.
*/

// function that support print the game board
void printToBoard(int x, int y, int feature, char featureChar, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// print the game board function protype
void initialize(ifstream &fin, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// actor generator function
Actor actorGenerator(char character);

// check whether the player got catched
bool captured(Actor const &player, Actor const &monster);

/*
Actors like gooses can stand on the wall; this function is to repair the
missing symbol that was covered by the actors.
*/
void mapRepair(int x, int y, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// check whether the player is on the trap location
void trapCheck(Actor &player, Actor &goose, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// Move the player to a new location based on the user input.
void movePlayer(int key, Actor &player, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// move the goose to a new location
void moveGoose(Actor &player, Actor &goose, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

// the skill that the player can use
void flash(int key, Actor &player, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y]);

#
endif
