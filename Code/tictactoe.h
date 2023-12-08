/*
 *  tictactoe.h - Definitions for Tic Tac Toe
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_TTT_H_
#define _GM_TTT_H_

#include "task.h"
#include "network.h"
#include "graphics.h"

#define TTT_VERSION 0.1

#define GRID_TOP      36
#define GRID_LEFT     30
#define GRID_SPACING  22
#define GRID_SIZE     ((GRID_SPACING * 3) + 2)

#define P1_OFFSET   4
#define P2_OFFSET   16
#define MSG_OFFSET  112
#define MSG_SIZE    14

//  Preferences key for storing stats in NVRAM
#define TTT_NVM_KEY "ttt-stats"

/*
 *  TicTacToe network protocol is super simple:  one format
 *  and the type code determines what the player intends.
 *      PLAY  - current player places an X or O at [x, y]
 *      DRAW  - sender offers a draw (not implemented)
 *      QUIT  - sender is done, no response expected
 *      SYNC  - start a game/play again?
 */
#define TTT_SYNC 0x53
#define TTT_PLAY 0x50
#define TTT_DRAW 0x44
#define TTT_QUIT 0x51

typedef struct TTTpkt {
  uint8_t type;                 // keep it brutally simple
  int sequence;                 // a sequence number for coordination
  char who[GM_PLAYER_TAG_LEN];  // tag/name of sender
  char which;                   // literally 'X' or 'O'
  uint8_t x;                    // x position of this play
  uint8_t y;                    // y position of this play
} ttt_packet_t;

typedef struct position {
  char mark;        // literally ' ', 'X' or 'O' :-)
  int16_t xOffset;  // precomputed x coord for text placement
  int16_t yOffset;  // precomputed y coord
} Square;

enum MsgLine : byte { Player1, Player2, Status };
enum Condition : byte { Reset, Undecided, Win, Lose, Draw, Quit };

class TicTacToe : public GMTask {
public:
  TicTacToe();
  void setup(bool rsvp) override;

  static const String appName;

private:
  void run() override;

  void showGreeting();
  bool startNetwork();
  bool hostOrJoin();  // move to menuTask?
  void resetBoard();
  void drawScreen();
  void drawMessage(MsgLine which, String msg, uint8_t color = WHITE);
  void drawHighlight(uint8_t x, uint8_t y, bool on);
  void trackCursor(uint8_t dir);
  bool claimSquare(bool host);
  void sendTTT(const uint8_t *mac, uint8_t code, uint8_t seq);
  void sendUpdate();
  Condition recvUpdate();
  Condition updateCondition();
  bool askToPlayAgain();
  void showSignOff();

  // Connection to the network
  int netId;
  QueueHandle_t netQ;

  // Player X is the host, player O is the guest
  bool hosting;
  gm_player_t *me;
  gm_player_t *them;
  String p1label;
  String p2label;
  String statusMsg;

  // The simple playing field
  Square board[3][3];

  Condition state;
  uint8_t seqNum;
  uint8_t curX;
  uint8_t curY;
  bool curOn;
  bool myTurn;

  // Wins, losses, draws
  uint16_t stats[3];
};

#endif