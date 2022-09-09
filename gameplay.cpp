#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

extern Console out;
/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner. So it means the coordinate axes are:
----------------> x direction
|
|
|
|
|
V

y direction
*/

// support function that helps print the game board
void printToBoard(int x, int y, int feature, char featureChar, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	gameBoard[x][y] = feature;
	terminal_put(x, y, featureChar);
}

/*
Print the game world
We use a file to store the data about the walls. Each line in the file has four integer values and indicates a wall.
The first number is 0 or 1: 0 means the wall is vertical; 1 means the wall is horizontal.
The second number is the fixed line number: if the wall is vertical, then the line number
represents the column number; if horizontal, then it represents the row number.
The third and fourth numbers are the starting row (column) and the end row (column)
of the wall.
*/
void initialize(ifstream &fin, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	bool isHorizontal = 0;
	int lineNum = 0, start = 0, end = 0;
	// read in the file
	while (fin >> isHorizontal >> lineNum >> start >> end)
	{
		for (int index = start; index <= end; index++)
			// print the horizontal walls
			if (isHorizontal)
			{
				printToBoard(index, lineNum, WALL, WALL_CHAR, gameBoard);
			}
		else
		// print the vertical walls
		{
			printToBoard(lineNum, index, WALL, WALL_CHAR, gameBoard);
		}
	}

	// print the winning spot
	printToBoard(WIN_X, WIN_Y, WINNER, WIN_CHAR, gameBoard);

	// print the trap spots
	printToBoard(TRAP_X1, TRAP_Y1, TRAP, TRAP_CHAR, gameBoard);
	printToBoard(TRAP_X2, TRAP_Y2, TRAP, TRAP_CHAR, gameBoard);

	terminal_refresh();
}

/*
We use this function to create the play as well as the goose.
Both the player and the goose will have different starting position in a limited range
each time you start the game.
*/
Actor actorGenerator(char character)
{
	int startX, startY;
	/*
	When using the rand() functions, we found that the rand() functions gave the
	same number each time we ran the game. These two lines are the solution to
	that problem.
	*/
	int seed = time(0);
	srand(seed);

	// generating random location for the player within the range (0, 0) to (14, 14)
	if (character == PLAYER_CHAR)
	{
		startX = rand() % PLAYER_RANGE_X + PLAYER_START_X;
		startY = rand() % PLAYER_RANGE_Y + PLAYER_START_Y;
	}

	// generating random location for goose within the range (100, 18) to ()
	else if (character == MONSTER_CHAR)
	{
		startX = rand() % GOOSE_RANGE_X + GOOSE_START_X;
		startY = rand() % GOOSE_RANGE_Y + GOOSE_START_Y;
	}

	return Actor(character, startX, startY);
}

// check whether the goose catches the player
bool captured(Actor const &player, Actor const &monster)
{
	return (player.get_x() == monster.get_x() &&
		player.get_y() == monster.get_y());
}

/*
Actors like gooses can stand on the walls; this function is to repair the
missing symbol that was covered by the actors.
*/
void mapRepair(int x, int y, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	// based on the number saved in the gameboard, repair the earesed character
	switch (gameBoard[x][y])
	{
		case WALL:
			terminal_put(x, y, WALL_CHAR);
			break;
		case WINNER:
			terminal_put(x, y, WIN_CHAR);
			break;
		default:
			break;
	}

	terminal_refresh();
}

// check whether the player is on the trap location
void trapCheck(Actor &player, Actor &goose, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	if (gameBoard[player.get_x()][player.get_y()] == TRAP)
	{
		// STRENTHEN both player and goose by making them "ANGRY"
		player.set_statu(ANGRY);
		goose.set_statu(ANGRY);
		out.writeLine("Magic power strengens you and gooses!");
		out.writeLine("The gooses now can move faster, and you get the skill FLASH!");
		out.writeLine("FLASH allows you jump 40 units instantly with the direction of your last move!");
		out.writeLine("HOWEVER, you ONLY have one chance to use the skill by pressing 'F'. Be SMART!");
		// the trap is a one-time feature; disable it when it's actived
		gameBoard[player.get_x()][player.get_y()] == EMPTY;
	}
}

// Move the player to a new location based on the user input.
void movePlayer(int key, Actor &player, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	int yMove = 0, xMove = 0;

	if (key == TK_UP || key == TK_W)
		yMove = -1;
	else if (key == TK_DOWN || key == TK_S)
		yMove = 1;
	else if (key == TK_LEFT || key == TK_A)
		xMove = -1;
	else if (key == TK_RIGHT || key == TK_D)
		xMove = 1;

	// record the last location of the player
	int lastX = player.get_x();
	int lastY = player.get_y();

	// the player can freely move around when he/she is on the wall or not hitting the wall
	if (gameBoard[player.get_x()][player.get_y()] == WALL ||
		gameBoard[player.get_x() + xMove][player.get_y() + yMove] != WALL)
		player.update_location(xMove, yMove);

	// if the player was on the wall, repair the wall character when the play leaves
	mapRepair(lastX, lastY, gameBoard);
}

// Move the player to a new location
void moveGoose(Actor &player, Actor &goose, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	int yMove = 0, xMove = 0;
	int maxMove = 2;

	/
	The goose will move randomly when the player is outside its "perception range",
		or it is not in "ANGRY"
	mode.*/
	if (player.distance(goose) > WARNING_RANGE && !goose.get_statu())
	{
		yMove = rand() % 5 - 2;
		xMove = rand() % 5 - 2;
	}
	else
	{
		// if the goose is ANGRY, SPEED UP!!
		if (goose.get_statu())
			maxMove = 3;
		// chase the player with the maximum movement
		if (player.get_x() < goose.get_x())
			xMove = max(-maxMove, player.get_x() - goose.get_x());
		else if (player.get_x() > goose.get_x())
			xMove = min(maxMove, player.get_x() - goose.get_x());

		if (player.get_y() < goose.get_y())
			yMove = max(-maxMove, player.get_y() - goose.get_y());
		else if (player.get_y() > goose.get_y())
			yMove = min(maxMove, player.get_y() - goose.get_y());
	}

	// record the last location of the goose
	int lastX = goose.get_x();
	int lastY = goose.get_y();

	// the goose can move freely (the walls will not block the goose)
	goose.update_location(xMove, yMove);

	// if the goose was on the wall, repair the wall character when the play leaves
	mapRepair(lastX, lastY, gameBoard);
}

/*
This is the skill that the player can use only once when he/she is "ANGRY".
Pressing 'F' to use the skill, the player will jump to a maximum of 40 units
ahead based on his/her last movement.
*/
void flash(int key, Actor &player, int gameBoard[NUM_BOARD_X][NUM_BOARD_Y])
{
	// this is a one-time skill, so set the player "!ANGRY" when this function is called
	player.set_statu(!ANGRY);

	int yMove = 0, xMove = 0;

	// determine the flash direction and distance
	if (key == TK_UP)
	{
		yMove = -FLASH;
		// modify the distance when out of the game board
		if ((player.get_y() + yMove) < MIN_BOARD_Y)
			yMove = MIN_BOARD_Y - player.get_y();
	}
	else if (key == TK_DOWN)
	{
		yMove = FLASH;
		if ((player.get_y() + yMove) > MAX_BOARD_Y)
			yMove = MAX_BOARD_Y - player.get_y();
	}
	else if (key == TK_LEFT)
	{
		xMove = -FLASH;
		if ((player.get_x() + xMove) < MIN_BOARD_X)
			xMove = MIN_BOARD_X - player.get_x();
	}
	else if (key == TK_RIGHT)
	{
		xMove = FLASH;
		if ((player.get_x() + xMove) > MAX_BOARD_X)
			xMove = MAX_BOARD_X - player.get_x();
	}

	// flash the player to the target location
	player.update_location(xMove, yMove);
}
