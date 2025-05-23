#include <arpa/inet.h>
#include <cstdio>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "3490" // the port client will be connecting to
#define BACKLOG 5   // how many pending connections queue will hold
struct pos {
  int x;
  int y;
};
int game_socket;
#define BOARD_HEIGHT 6
#define BOARD_WIDTH 7
// reciever can be player 1(1) player 2(2) or both(0)
int sendall(int reciever, std::string stringGiven) {
  if (reciever == 0 || reciever == 1) {
    std::cout << stringGiven;
  }
  if (reciever == 0 || reciever == 2) {
    int len = stringGiven.length();
    const char *buf = stringGiven.c_str();
    int total = 0;       // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;
    while (total < len) {
      n = send(game_socket, buf + total, bytesleft, 0);
      if (n == -1) {
        break;
      }
      total += n;
      bytesleft -= n;
    }

    len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success}
  }
  return 0; // incurrect reciever used
}
void printGrid(int player, int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  for (int yIndex = BOARD_HEIGHT - 1; yIndex > -1; yIndex--) {
    sendall(player, "_______________\n");
    for (int xIndex = 0; xIndex < BOARD_WIDTH; xIndex++) {
      std::ostringstream row;
      row << "|" << grid[yIndex][xIndex];
      sendall(player, row.str());
    }
    sendall(player, "|\n");
  }
  sendall(player, "---------------\n");
  for (int xIndex = 0; xIndex < BOARD_WIDTH; xIndex++) {
    std::ostringstream labels;
    labels << "-" << (xIndex + 1);
    sendall(player, labels.str());
  }
  sendall(player, "-(Row #)\n");
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
    sendall(0, "vertical WIN '||||||' \n");

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
    sendall(0, "horizontal WIN '-----' \n");

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
    sendall(0, "left diagnol WIN '//////' \n");
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
    sendall(0, "right diagnol WIN '\\\\\\' \n");

    return true;
  }
  return false;
}

void sendInvalidInput(int reciever) {
  std::ostringstream invalidAnswerWarning;
  invalidAnswerWarning << "invalid row(1-" << BOARD_WIDTH << ") \n";
  sendall(reciever, invalidAnswerWarning.str());
}
/// Returns false if game over
bool insertPiece(int player, int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  // look for an open space and then place
  int input = 0;
  bool stillNeedsToPickASpot = true;
  char *buf;
  pos positionUsed;
  while (stillNeedsToPickASpot) {
    if (player == 2) {
      recv(game_socket, buf, 1, 0);
      input = buf[0] - 48;
    } else {
      std::cin >> input;
    }

    if (input < 1 || input > BOARD_WIDTH) {
      sendInvalidInput(player);
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
      sendall(player, "Column Filled, Please Choose Another Row \n");
    }
  }
  return checkWin(grid, positionUsed);
}
void validInputPrint(int reciever) {
  std::ostringstream valid;
  valid << "Choose a row(1-" << BOARD_WIDTH << ") \n";
  sendall(reciever, valid.str());
}
bool takePlayerTurn(int player, int grid[BOARD_HEIGHT][BOARD_WIDTH]) {
  validInputPrint(player);
  return insertPiece(player, grid);
}
void game(int game_socket) {
  // if (send(new_fd, "Hello, world!", 13, 0) == -1)
  // perror("send");
  int grid[BOARD_HEIGHT][BOARD_WIDTH];
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    for (int x = 0; x < BOARD_WIDTH; x++) {
      grid[y][x] = 0;
    }
  }
  if (sendall(0, "Welcome To miniConnect4\n") == -1)
    perror("send");
  int maxGameLength = BOARD_HEIGHT * BOARD_WIDTH;
  int currentInsert = 0;
  while (true) {
    printGrid(0, grid);
    int currentGamePlayer = (currentInsert % 2) + 1;
    if (currentGamePlayer == 1) {
      sendall(2, "Waiting for the other to play their turn\n");

    } else {
      sendall(1, "Waiting for the other to play their turn\n");
    }
    if (takePlayerTurn(currentGamePlayer, grid)) {
      printGrid(0, grid);
      if (currentGamePlayer == 1) {
        sendall(0, "Host Player wins!!");
      } else {
        sendall(0, "Remote Player wins!!");
      }
      break;
    }
    currentInsert += 1;
    if (currentInsert == maxGameLength) {
      sendall(0, "ITS A TIE\n");
      printGrid(0, grid);
      break;
    }
  }
}
void sigchld_handler(int s) {
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
int main() {
  int sockfd; // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }
  inet_ntop(servinfo->ai_family, get_in_addr((struct sockaddr *)&servinfo), s,
            sizeof s);
  printf("Server seting up on %s:%s\n", s, PORT);
  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (1) { // main accept() loop
    sin_size = sizeof their_addr;
    game_socket = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (game_socket == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);
    printf("server: got connection from %s\n", s);

    if (!fork()) {   // this is the child process
      close(sockfd); // child doesn't need the listener
      game(game_socket);
      close(game_socket);
      exit(0);
    }
    close(game_socket); // parent doesn't need this
  }

  return 0;
}