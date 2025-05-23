#include <cstdio>
#include <iostream>
struct pos {
  int x;
  int y;
};
#define BOARD_HEIGHT 6
#define BOARD_WIDTH 7
void printGrid(int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int yIndex = BOARD_HEIGHT - 1; yIndex > -1; yIndex--) {
    printf("_______________\n");
    for (int xIndex = 0; xIndex < BOARD_WIDTH; xIndex++) {
      printf("|%d", grid[yIndex][xIndex]);
    }
    printf("|\n");
    //   printf("|%d|%d|%d|%d|%d|%d|%d|\n", grid[yIndex][0], grid[yIndex][1],
    //          grid[yIndex][2], grid[yIndex][3], grid[yIndex][4],
    //          grid[yIndex][5], grid[yIndex][6]);
  }
  printf("---------------\n");
  for (int xIndex = 0; xIndex < BOARD_WIDTH; xIndex++) {
    printf("-%d", xIndex + 1);
  }
  printf("-(Row #)\n");
}
bool checkWin(int grid[BOARD_HEIGHT][BOARD_WIDTH], pos position) {
  int vertScore = 1;
  int player = grid[position.y][position.x];
  for (int y = position.y + 1; y < BOARD_HEIGHT; y++) {
    if (grid[y][position.x] == player) {
      vertScore += 1;
    } else {
      break;
    }
  }
  for (int y = position.y - 1; y >= 0; y--) {
    if (grid[y][position.x] == player) {
      vertScore += 1;
    } else {
      break;
    }
  }
  if (vertScore >= 4) {
    printf("vertical WIN '||||||' \n");

    return true;
  }
  ////////////////////////////////////////
  int horizScore = 1;
  for (int x = position.x + 1; x < BOARD_HEIGHT; x++) {
    if (grid[position.y][x] == player) {
      horizScore += 1;
    } else {
      break;
    }
  }
  for (int x = position.x - 1; x >= 0; x--) {
    if (grid[position.y][x] == player) {
      horizScore += 1;
    } else {
      break;
    }
  }
  if (horizScore >= 4) {
    printf("horizontal WIN '-----' \n");

    return true;
  }
  ///////////////////////////////////////////
  int leftDiagScore = 1;
  int tempX = position.x;
  int tempY = position.y;
  while (true) {
    tempX += 1;
    tempY += 1;
    if (tempX >= 0 && tempX < BOARD_WIDTH && tempY >= 0 &&
        tempY < BOARD_HEIGHT && grid[tempY][tempX] == player) {
      leftDiagScore += 1;
    } else {
      break;
    }
  }
  tempX = position.x;
  tempY = position.y;
  while (true) {
    tempX -= 1;
    tempY -= 1;
    if (tempX >= 0 && tempX < BOARD_WIDTH && tempY >= 0 &&
        tempY < BOARD_HEIGHT && grid[tempY][tempX] == player) {
      leftDiagScore += 1;
    } else {
      break;
    }
  }
  if (leftDiagScore >= 4) {
    printf("left diagnol WIN '//////' \n");
    return true;
  }
  //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//
  int rightDiagScore = 1;
  tempX = position.x;
  tempY = position.y;
  while (true) {
    tempX -= 1;
    tempY += 1;
    if (tempX >= 0 && tempX < BOARD_WIDTH && tempY >= 0 &&
        tempY < BOARD_HEIGHT && grid[tempY][tempX] == player) {
      rightDiagScore += 1;
    } else {
      break;
    }
  }
  tempX = position.x;
  tempY = position.y;
  while (true) {
    tempX += 1;
    tempY -= 1;
    if (tempX >= 0 && tempX < BOARD_WIDTH && tempY >= 0 &&
        tempY < BOARD_HEIGHT && grid[tempY][tempX] == player) {
      rightDiagScore += 1;
    } else {
      break;
    }
  }
  if (rightDiagScore >= 4) {
    printf("right diagnol WIN '\\\\\\' \n");

    return true;
  }
  return false;
}

/// Returns false if game over
bool insertPiece(int player, int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  // look for an open space and then place
  int input = 0;
  bool stillNeedsToPickASpot = true;
  pos positionUsed;
  while (stillNeedsToPickASpot) {
    std::cin >> input;
    if (input < 1 || input > BOARD_WIDTH) {
      printf("invalid row(1-%d) \n", BOARD_WIDTH);
      continue;
    }
    // convert input to grid space
    input -= 1;
    for (int yIndex = 0; yIndex < BOARD_HEIGHT; yIndex++) {
      if (grid[yIndex][input] == 0) {
        grid[yIndex][input] = player;
        stillNeedsToPickASpot = false;
        positionUsed.x = input;
        positionUsed.y = yIndex;
        break;
      }
    }
    if (stillNeedsToPickASpot) {
      printf("Column Filled, Please Choose Another Row \n");
    }
  }
  return checkWin(grid, positionUsed);
}
bool takePlayerTurn(int player, int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  printGrid(grid);
  printf("Player %d Choose a row(1-%d)\n", player, BOARD_WIDTH);
  return insertPiece(player, grid);
}
int main() {
  int grid[BOARD_HEIGHT][BOARD_WIDTH];
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      grid[y][x] = 0;
    }
  }

  printf("Welcome To miniConnect4\n");
  int maxGameLength = BOARD_HEIGHT * BOARD_WIDTH;
  int currentInsert = 0;
  while (true) {
    int player = (currentInsert % 2) + 1;
    if (takePlayerTurn(player, grid)) {
      printGrid(grid);
      printf("Player #%d wins\n", player);
      break;
    }
    currentInsert += 1;
    if (currentInsert == maxGameLength) {
      printf("ITS A TIE\n");
      printGrid(grid);
      break;
    }
  }

  // BOARD_WIDTHwide by 6 height grid  with 0 being empty 1 being player 1 and 2
  // being player 2
}