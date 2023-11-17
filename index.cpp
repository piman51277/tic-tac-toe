#include <cstdint>

#include <iostream>
#include <string>

/** State format
 * Since tic-tac-toe is a 3x3 board, we can us an uint32_t to store the state
 *
 * The first 4 bits are depth
 * 0000
 *
 * The next 9 bits represent whether a cell is empty or not
 * 0 0 0
 * 0 0 0
 * 0 0 0
 *
 * The next 18 bits are the board cells, 2 bits per cell
 * 00 00 00
 * 00 00 00
 * 00 00 00
 *
 * 10 = X
 * 01 = O
 * 00 = empty
 *
 * The last bit is the next player
 *
 * 1 = X
 * 0 = O
 */

// The initial state is 0x00000000

/** Win Conditions Bitmasks
 *
 * Since our entire state is stored in a single uint32_t, we can easily check win conditions
 * by using bitmasks
 *
 * if the conditions state & mask == mask is true, then the player has won
 *
 * Top row:
 *
 * X:
 * 0000  111 000 000  10 10 10 00 00 00 00 00 00  0
 * 0x0E054000
 *
 * O:
 * 0000  111 000 000  01 01 01 00 00 00 00 00 00  0
 * 0x0E02A000
 *
 * Middle row:
 *
 * X:
 * 0000  000 111 000  00 00 00 10 10 10 00 00 00  0
 * 0x01C01500
 *
 * O:
 * 0000  000 111 000  00 00 00 01 01 01 00 00 00  0
 * 0x01C00A80
 *
 * Bottom row:
 *
 * X:
 * 0000  000 000 111  00 00 00 00 00 00 10 10 10  0
 * 0x00380054
 *
 * O:
 * 0000  000 000 111  00 00 00 00 00 00 01 01 01  0
 * 0x0038002A
 *
 * Left column:
 *
 * X:
 * 0000  100 100 100  10 00 00 10 00 00 10 00 00  0
 * 0x09241040
 *
 * O:
 * 0000  100 100 100  01 00 00 01 00 00 01 00 00  0
 * 0x09220820
 *
 * Middle column:
 *
 * X:
 * 0000  010 010 010  00 10 00 00 10 00 00 10 00  0
 * 0x04910410
 *
 * O:
 * 0000  010 010 010  00 01 00 00 01 00 00 01 00  0
 * 0x04908208
 *
 * Right column:
 *
 * X:
 * 0000  001 001 001  00 00 10 00 00 10 00 00 10  0
 * 0x02482104
 *
 * O:
 * 0000  001 001 001  00 00 01 00 00 01 00 00 01  0
 * 0x02482082
 *
 * Diagonals:
 *
 * X:
 * 0000  100 010 001  10 00 00 00 10 00 00 00 10  0
 * 0x088C0404
 *
 * O:
 * 0000  100 010 001  01 00 00 00 01 00 00 00 01  0
 * 0x88A0202
 *
 * X:
 * 0000  001 010 100  00 00 10 00 10 00 10 00 00  0
 * 0x02A04440
 *
 * O:
 * 0000  001 010 100  00 00 01 00 01 00 01 00 00 0
 * 0x02A02220
 */

// create an array of masks for each win condition
const uint32_t winmasks[16] = {
    0x0E054000, // top row X
    0x0E02A000, // top row O
    0x01C01500, // middle row X
    0x01C00A80, // middle row O
    0x00380054, // bottom row X
    0x0038002A, // bottom row O
    0x09241040, // left column X
    0x09220820, // left column O
    0x04910410, // middle column X
    0x04908208, // middle column O
    0x02484104, // right column X
    0x02482082, // right column O
    0x088C0404, // diagonal X
    0x088A0202, // diagonal O
    0x02A04440, // diagonal X
    0x02A02220  // diagonal O
};

/**
 * Check if the last move won the game
 */
bool hasWon(uint32_t state)
{
  // check if the last move won the game
  for (int i = 0; i < 16; i++)
  {
    if ((state & winmasks[i]) == winmasks[i])
    {
      return true;
    }
  }
  return false;
}

/** we can check if the game is a draw by checking if the board is full
 * 0000 111 111 111  00 00 00 00 00 00 00 00 00  0
 * 0x0FF80000
 */

bool isDraw(uint32_t state)
{
  // check if the board is full
  return ((state & 0x0FF80000) == 0x0FF80000);
}

/**
 * Static evaluation function
 * Positive values are good for X
 * Negative values are good for O
 *
 * Since our search depth is max 9, we can always assume that the game is over
 *
 * This also doubles as a check for if the game is over
 */
int evaluate(uint32_t state)
{
  // get next player
  uint32_t player = state & 0x1;

  // check if someone won
  if (hasWon(state))
  {

    // since the check bit is for the *next* player,
    // these are reversed
    if (player == 1)
    {
      return 1000;
    }
    else
    {
      return -1000;
    }
  }

  // check if it's a draw
  if (isDraw(state))
  {

    // punish draws
    return -50;
  }

  // otherwise, return 0
  return 0;
}

/** Move bitmasks
 *
 * We can also use bitmasks to add moves to the board
 *
 * First the bitmasks to checkif a position is open:
 *
 * Top left:
 * 0000  100 000 000  00 00 00 00 00 00 00 00 00  0
 * 0x08000000
 *
 * Top middle:
 * 0000  010 000 000  00 00 00 00 00 00 00 00 00  0
 * 0x04000000
 *
 * Top right:
 * 0000  001 000 000  00 00 00 00 00 00 00 00 00  0
 * 0x02000000
 *
 * Middle left:
 * 0000  000 100 000  00 00 00 00 00 00 00 00 00  0
 * 0x01000000
 *
 * Middle middle:
 * 0000  000 010 000  00 00 00 00 00 00 00 00 00  0
 * 0x00800000
 *
 * Middle right:
 * 0000  000 001 000  00 00 00 00 00 00 00 00 00  0
 * 0x00400000
 *
 * Bottom left:
 * 0000 000 000 100  00 00 00 00 00 00 00 00 00  0
 * 0x00200000
 *
 * Bottom middle:
 * 0000 000 000 010  00 00 00 00 00 00 00 00 00  0
 * 0x00100000
 *
 * Bottom right:
 * 0000 000 000 001  00 00 00 00 00 00 00 00 00  0
 * 0x00080000
 */

const uint32_t moveMasks[9] = {
    0x08000000, // top left
    0x04000000, // top middle
    0x02000000, // top right
    0x01000000, // middle left
    0x00800000, // middle middle
    0x00400000, // middle right
    0x00200000, // bottom left
    0x00100000, // bottom middle
    0x00080000  // bottom right
};

/** Moves for X
 *
 * Top left:
 * 0000  100 000 000  10 00 00 00 00 00 00 00 00 0
 * 0x08040000
 *
 * Top middle:
 * 0000  010 000 000  00 10 00 00 00 00 00 00 00 0
 * 0x04010000
 *
 * Top right:
 * 0000  001 000 000  00 00 10 00 00 00 00 00 00 0
 * 0x02004000
 *
 * Middle left:
 * 0000  000 100 000  00 00 00 10 00 00 00 00 00 0
 * 0x01001000
 *
 * Middle middle:
 * 0000  000 010 000  00 00 00 00 10 00 00 00 00 0
 * 0x00800400
 *
 * Middle right:
 * 0000  000 001 000  00 00 00 00 00 10 00 00 00 0
 * 0x00400100
 *
 * Bottom left:
 * 0000  000 000 100  00 00 00 00 00 00 10 00 00 0
 * 0x00200040
 *
 * Bottom middle:
 * 0000  000 000 010  00 00 00 00 00 00 00 10 00 0
 * 0x00100010
 *
 * Bottom right:
 * 0000  000 000 001  00 00 00 00 00 00 00 00 10 0
 * 0x00080004
 */

const uint32_t moveMasksX[9] = {
    0x08040000, // top left
    0x04010000, // top middle
    0x02004000, // top right
    0x01001000, // middle left
    0x00800400, // middle middle
    0x00400100, // middle right
    0x00200040, // bottom left
    0x00100010, // bottom middle
    0x00080004  // bottom right
};

/** Moves for O
 *
 * Top left:
 * 0000  100 000 000  01 00 00 00 00 00 00 00 00 0
 * 0x08020000
 *
 * Top middle:
 * 0000  010 000 000  00 01 00 00 00 00 00 00 00 0
 * 0x04008000
 *
 * Top right:
 * 0000  001 000 000  00 00 01 00 00 00 00 00 00 0
 * 0x02002000
 *
 * Middle left:
 * 0000  000 100 000  00 00 00 01 00 00 00 00 00 0
 * 0x01000800
 *
 * Middle middle:
 * 0000  000 010 000  00 00 00 00 01 00 00 00 00 0
 * 0x00800200
 *
 * Middle right:
 * 0000  000 001 000  00 00 00 00 00 01 00 00 00 0
 * 0x00400080
 *
 * Bottom left:
 * 0000  000 000 100  00 00 00 00 00 00 01 00 00 0
 * 0x00200020
 *
 * Bottom middle:
 * 0000  000 000 010  00 00 00 00 00 00 00 01 00 0
 * 0x00100008
 *
 * Bottom right:
 * 0000  000 000 001  00 00 00 00 00 00 00 00 01 0
 * 0x00080002
 */

const uint32_t moveMasksO[9] = {
    0x08020000, // top left
    0x04008000, // top middle
    0x02002000, // top right
    0x01000800, // middle left
    0x00800200, // middle middle
    0x00400080, // middle right
    0x00200020, // bottom left
    0x00100008, // bottom middle
    0x00080002  // bottom right
};

/**
 * Get the children of a board
 * Returns a 0-padded array of children
 */
uint32_t *getChildren(uint32_t board)
{

  // allocate for up to 9 children
  uint32_t *children = new uint32_t[9];

  // set to 0
  for (int i = 0; i < 8; i++)
  {
    children[i] = 0;
  }

  // get the current player
  uint32_t player = board & 0x1;

  // get the depth
  uint32_t depth = (board & 0xF0000000) >> 28;

  // make child for each empty cell
  int childIndex = 0;
  for (int i = 0; i < 9; i++)
  {
    // check if the cell is empty
    if ((moveMasks[i] & board) == 0)
    {

      // make a copy of the board
      uint32_t child = board;

      // add the move to the board
      if (player == 0)
      {
        // apply move
        child |= moveMasksX[i];
      }
      else
      {
        // apply move
        child |= moveMasksO[i];
      }

      // flip the player
      child ^= 0x1;

      // increment the depth
      child += 0x10000000;

      // add the child to the array
      children[childIndex] = child;
      childIndex++;
    }
  }

  return children;
}

struct minimaxResult
{
  int score;
  uint32_t state;
};

/**
 * Minimax algorithm
 */
minimaxResult minimaxWorker(uint32_t state)
{
  // evaluate the state
  int score = evaluate(state);

  // if the game is over, return the score
  if (score != 0)
  {

    return {score, state};
  }

  // get the children
  uint32_t *children = getChildren(state);

  // get the depth
  uint32_t depth = (state & 0xF0000000) >> 28;

  // get the current player
  uint32_t player = state & 0x1;

  // if the player is 0, then we want to maximize
  if (player == 0)
  {
    int maxScore = -100;
    uint32_t maxState = 0;

    // loop through the children
    for (int i = 0; i < 9; i++)
    {
      // if child is 0 we are done
      if (children[i] == 0)
      {
        break;
      }

      // get the child score
      minimaxResult childResult = minimaxWorker(children[i]);
      int childScore = childResult.score;
      uint32_t childState = childResult.state;

      // if the child score is better than the current max, update the max
      if (childScore > maxScore)
      {
        maxScore = childScore;
        maxState = childState;
      }
    }

    // make the max score slightly worse, so we prefer shorter games
    maxScore -= 10;

    // if top level, return the state
    if (depth == 0)
    {
      return {maxScore, maxState};
    }

    // return the max score
    return {maxScore, state};
  }

  // if player is 1, we want to minimize
  else
  {
    int minScore = 100;
    uint32_t minState = 0;

    // loop through the children
    for (int i = 0; i < 9; i++)
    {
      // if child is 0 we are done
      if (children[i] == 0)
      {
        break;
      }

      // get the child score
      minimaxResult childResult = minimaxWorker(children[i]);
      int childScore = childResult.score;
      uint32_t childState = childResult.state;

      // if the child score is better than the current max, update the max
      if (childScore < minScore)
      {
        minScore = childScore;
        minState = childState;
      }
    }

    // make the min score slightly better, so we prefer shorter games
    minScore += 10;

    // if top level, return the state
    if (depth == 0)
    {
      return {minScore, minState};
    }

    // return the min score
    return {minScore, state};
  }
}

uint32_t minimax(uint32_t state)
{
  // the is the bets possible first move
  // it also takes the longest to process
  // so we're caching it
  if (state == 0)
  {
    return 0x18040001;
  }
  else if (state == 0x00000001)
  {
    return 0x18020000;
  }

  // reset depth
  state &= 0x0FFFFFFF;

  minimaxResult result = minimaxWorker(state);
  return result.state;
}

// DEBUG

/**
 * Print the board
 */
void printBoard(uint32_t board)
{
  // get the positions
  uint32_t positions = (board & 0x7FFFE) >> 1;
  // get the player
  uint32_t player = board & 0x1;

  // print the board
  std::cout << "Player: " << player << std::endl;

  char *boardChars = new char[9];

  // make the board
  for (int i = 8; i >= 0; i--)
  {
    int pos = positions & 0x3;

    if (pos == 0)
    {
      boardChars[i] = '-';
    }
    else if (pos == 1)
    {
      boardChars[i] = 'O';
    }
    else if (pos == 2)
    {
      boardChars[i] = 'X';
    }

    positions = positions >> 2;
  }

  // print the board
  for (int i = 0; i < 3; i++)
  {
    std::cout << boardChars[i * 3] << boardChars[i * 3 + 1] << boardChars[i * 3 + 2] << std::endl;
  }

  std::cout << "\n";
}

uint32_t readBoard()
{
  uint32_t board = 0x00000000;

  // read the board
  for (int i = 0; i < 3; i++)
  {
    std::string line;
    std::cin >> line;

    for (int j = 0; j < 3; j++)
    {
      char c = line[j];

      if (c == '-')
      {
        continue;
      }

      if (c == 'X')
      {
        board |= moveMasksX[i * 3 + j];
        board |= moveMasks[i * 3 + j];
      }
      else if (c == 'O')
      {
        board |= moveMasksO[i * 3 + j];
        board |= moveMasks[i * 3 + j];
      }
    }
  }

  return board;
}

int main()
{

  uint32_t empty = readBoard();

  // flip the player
  empty ^= 0x1;

  // return best move
  empty = minimax(empty);

  printBoard(empty);
}