#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console.

Console out;

int main()
{

	terminal_open();
	terminal_set(SETUP_MESSAGE);
	ifstream fin("wall.txt");
	if (!fin)
	{
		cout << "File Not Found!" << endl;
		return 1;
	}

	// make the player
	Actor player = actorGenerator(PLAYER_CHAR);
	// make the monster
	Actor monster = actorGenerator(MONSTER_CHAR);

	// Declare the array that will hold the game board "map"
	int gameBoard[NUM_BOARD_X][NUM_BOARD_Y] = { 0 };

	// Call the function to print the game board
	initialize(fin, gameBoard);

	// Printing the instructions
	out.writeLine("Escape the Goose! " + monster.get_location_string());
	out.writeLine("Use the arrow keys to move");
	out.writeLine("If the goose catches you, you lose!");
	out.writeLine("Be careful! Sometimes the goose can jump through walls!");

	/*
	This is the main game loop. It continues to let the player give input
	as long as they do not press escape or close, they are not captured by
	the goose, and they didn't reach the win tile
	*/
	/*
	All key presses start with "TK_" then the character. So "TK_A" is the "a"
	 key being pressed.
	*/
	int keyEntered = TK_Q;	// can be any valid value that is not ESCAPE or CLOSE
	int lastMove = TK_RIGHT;	// record the player's last movement
	while (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE &&
		!captured(player, monster) && gameBoard[player.get_x()][player.get_y()] != WINNER)
	{
		// get player key press
		keyEntered = terminal_read();
		if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
		{
			// move the player, check whether the player uses the skill
			if (keyEntered == TK_F && player.get_statu())
				flash(lastMove, player, gameBoard);
			else
				movePlayer(keyEntered, player, gameBoard);

			// call the goose's chase function
			moveGoose(player, monster, gameBoard);

			// check whether the player stepps on the trap 'X'
			if (!player.get_statu())
				trapCheck(player, monster, gameBoard);
		}

		// get the player's last movement for the player to use skill
		if (keyEntered == TK_UP || keyEntered == TK_DOWN || keyEntered == TK_LEFT ||
			keyEntered == TK_RIGHT)
			lastMove = keyEntered;
	}

	if (keyEntered != TK_CLOSE)
	{
		//once we're out of the loop, the game is over
		out.writeLine("Game has ended");
		if (captured(player, monster))
			out.writeLine("Escape Failed! You are captured!");
		else if (gameBoard[player.get_x()][player.get_y()] == WINNER)
			out.writeLine("Escape succeeded! Congratulation!");
		// Wait until user closes the window
		while (terminal_read() != TK_CLOSE);
	}

	terminal_close();
}
