// minimal lines version of index.cpp
#include <cstdint>
#include <iostream>
const uint32_t winmasks[16] = {
    0x0E054000, 0x0E02A000, 0x01C01500, 0x01C00A80,
    0x00380054, 0x0038002A, 0x09241040, 0x09220820,
    0x04910410, 0x04908208, 0x02484104, 0x02482082,
    0x088C0404, 0x088A0202, 0x02A04440, 0x02A02220};
bool hasWon(uint32_t state)
{
  for (int i = 0; i < 16; i++)
  {
    if ((state & winmasks[i]) == winmasks[i])
      return true;
  }
  return false;
}
int evaluate(uint32_t state)
{
  uint32_t player = state & 0x1;
  if (hasWon(state))
    return -1000 + player * 2000;
  if (((state & 0x0FF80000) == 0x0FF80000))
    return -50;
  return 0;
}
const uint32_t moveMasks[9] = {
    0x08000000, 0x04000000, 0x02000000,
    0x01000000, 0x00800000, 0x00400000,
    0x00200000, 0x00100000, 0x00080000};
const uint32_t moveMasksX[9] = {
    0x08040000, 0x04010000, 0x02004000,
    0x01001000, 0x00800400, 0x00400100,
    0x00200040, 0x00100010, 0x00080004};
const uint32_t moveMasksO[9] = {
    0x08020000, 0x04008000, 0x02002000,
    0x01000800, 0x00800200, 0x00400080,
    0x00200020, 0x00100008, 0x00080002};
uint32_t *getChildren(uint32_t board)
{
  uint32_t *children = new uint32_t[9];
  for (int i = 0; i < 8; i++)
    children[i] = 0;
  uint32_t player = board & 0x1;
  uint32_t depth = (board & 0xF0000000) >> 28;
  int childIndex = 0;
  for (int i = 0; i < 9; i++)
  {
    if ((moveMasks[i] & board) == 0)
    {
      uint32_t child = board;
      if (player == 0)
      {
        child |= moveMasksX[i];
      }
      else
      {
        child |= moveMasksO[i];
      }
      child ^= 0x1;
      child += 0x10000000;
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
minimaxResult minimaxWorker(uint32_t state)
{
  int score = evaluate(state);
  if (score != 0)
    return {score, state};
  uint32_t *children = getChildren(state);
  uint32_t depth = (state & 0xF0000000) >> 28;
  uint32_t player = state & 0x1;
  if (player == 0)
  {
    int maxScore = -100;
    uint32_t maxState = 0;
    for (int i = 0; i < 9; i++)
    {
      if (children[i] == 0)
        break;
      minimaxResult childResult = minimaxWorker(children[i]);
      int childScore = childResult.score;
      uint32_t childState = childResult.state;
      if (childScore > maxScore)
      {
        maxScore = childScore;
        maxState = childState;
      }
    }
    maxScore -= 10;
    if (depth == 0)
      return {maxScore, maxState};
    return {maxScore, state};
  }
  else
  {
    int minScore = 100;
    uint32_t minState = 0;
    for (int i = 0; i < 9; i++)
    {
      if (children[i] == 0)
        break;
      minimaxResult childResult = minimaxWorker(children[i]);
      int childScore = childResult.score;
      uint32_t childState = childResult.state;
      if (childScore < minScore)
      {
        minScore = childScore;
        minState = childState;
      }
    }
    minScore += 10;
    if (depth == 0)
      return {minScore, minState};
    return {minScore, state};
  }
}
uint32_t minimax(uint32_t state)
{
  if (state == 0)
    return 0x18040001;
  else if (state == 0x00000001)
    return 0x18020000;
  state &= 0x0FFFFFFF;
  minimaxResult result = minimaxWorker(state);
  return result.state;
}
int main()
{
  uint32_t empty = 18040001;
  empty = minimax(empty);
  std::cout << std::hex << empty << std::endl;
}