/*
 *  tictactoe.cpp - Two player, networked Tic Tac Toe
 *
 *  Abstract:
 *      Lets the user host or join a game of Tic Tac Toe
 *      with another GameMan device.  Play alternates so
 *      that each player gets a turn placing Xs and Os.
 *      It's a simple turn-based proof-of-concept to use
 *      all the features of the GameMan API!
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include "GameMan.h"
#include "button.h"
#include "tictactoe.h"


TicTacToe::TicTacToe()
  : GMTask("TICTACTOE") {
}

const String TicTacToe::appName = "TicTacToe";

void TicTacToe::setup(bool rsvp) {
  Serial.print("TicTacToe: Setup called as ");
  Serial.println(rsvp ? "guest" : "host");

  // If menuTask launched us in response to an RSVP, we're the guest
  hosting = !rsvp;

  // Clear the board and precompute the text offsets
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      board[x][y].mark = ' ';
      board[x][y].xOffset = (x * GRID_SPACING) + x + GRID_LEFT + 6;  // tune for font size
      board[x][y].yOffset = (y * GRID_SPACING) + y + GRID_TOP + 2;   // account for grid lines
    }
  }
}

/*
 *  Connect to the network task and set up our packet queue.
 *  Returns false if anything goes wrong.  Could maybe run
 *  this in setup() but probably better to create it in the
 *  run()/task context?
 */
bool TicTacToe::startNetwork() {

  // Set up our packet queue
  netId = netTask.createQueue();

  // Add a filter for our packet type
  if (netTask.addFilter(netId, GM_TICTAC) < 0) {
    // If queue or filter add failed, no network; bail out
    Serial.println("TicTacToe: Failed to initialize the network!");
    display.println();
    display.println("ERROR:");
    display.println("Network failed");
    display.println("to initialize!");
    display.display();
    return false;
  }

  // Now grab our actual handle for reading packets!
  netQ = netTask.getHandle(netId);

  return true;
}

/*
 *  Display a simple greeting screen.  Make it fancy someday.
 */
void TicTacToe::showGreeting() {

  display.clearDisplay();
  display.setFont();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(getCenterX("Welcome to"), 10);
  display.print("Welcome to");
  display.setTextSize(2);
  display.setCursor(getCenterX("TicTacToe"), 30);
  display.println("TicTacToe");
  display.println();
  display.setTextSize(1);
  display.display();
}

/*
 *  On startup, ask the user if they want to host a new game or
 *  find an existing one to join.  Run after the network is up.
 *  Returns false if no other player connects or the user quits.
 */
bool TicTacToe::hostOrJoin() {

  gm_packet_t response;
  iff_packet_t responder;
  button_event_t press;

  bool waiting = true;
  int retries = 0;
  int dot = xTaskGetTickCount() - 5000;

  if (hosting) {

    Serial.println("TicTacToe: Starting as host");

    return true;  // XXX

    display.print("Sent invite");
    display.display();

    while (waiting && retries < 3) {

      if (elapsed(5000, dot)) {
        display.print(".");
        display.display();
        dot = xTaskGetTickCount();

        // Broadcast it
        netTask.sendRSVP("TicTacToe", netId);
        retries++;
      }

      // Pressing the C/home button just quits to the menu
      if (xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {
        if (press.id == BTN_C && press.action == btnReleased) {
          display.println();
          display.println("Quitting");
          return false;
        }
      }

      // Wait for an IFF_ACCEPT...
      if (xQueueReceive(netQ, &(response), (TickType_t)50)) {
        if (response.pktType == GM_IFF) {

          dprintln("UNPACK IFF PAYLOAD");
          // Pull out the IFF payload
          memcpy(&responder, response.payload, response.length);
          if (responder.type == IFF_ACCEPT) {
            dprintf("%s wants to play!\n", responder.who);
            // verify the player is known, etc
            waiting = false;
          } else if (responder.type == IFF_REJECT) {
            dprintf("%s won't play with me!\n", responder.who);
          }
        } else {
          dprintf("Disregarding type %d packet\n", response.pktType);
        }
      }
    }
  } else {
    // Guest: wait for sync packet to start game as player O
    Serial.println("TicTacToe: Starting as guest");

    // can we just jump into th emain loop here?
    //    start of game:  wait for sync packet from host
    //    play the game to win/lose/draw
    //    if they want to play again, switch roles/sides
    //
  }
  return true;
}

void TicTacToe::resetBoard() {

  // Clear the board contents
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      board[x][y].mark = ' ';

  // Set cursor in the center and enable
  curX = curY = 1;
  curOn = true;

  // Set the message strings

  us = String(netTask.getPlayerName() + hosting ? ": X" : ": O");
  them = String("[Player 2]" + hosting ? ": O" : ": X");
  msg = String("Reset");
}

/*
 *  Redraw the screen and playing field.
 */
void TicTacToe::drawScreen() {

  /*
    The screen is arranged with an info area at the top,
    the playing field, a timer bar, and a prompt area at
    the bottom:

      [ n pixels high bar for each player's info ]

         |   |        Playing field is fairly
      ---+---+---     compact.  The cursor is
         |   |        free to move around at
      ---+---+---     all times (d-pad).  Pos
         |   |        0,0 is at the top left.

      [ n pixels at the bottom for prompts or status ]
  */
  display.clearDisplay();
  display.drawFastHLine(GRID_LEFT, GRID_TOP + GRID_SPACING, GRID_SIZE, WHITE);
  display.drawFastHLine(GRID_LEFT, GRID_TOP + (GRID_SPACING * 2) + 1, GRID_SIZE, WHITE);
  display.drawFastVLine(GRID_LEFT + GRID_SPACING, GRID_TOP, GRID_SIZE, WHITE);
  display.drawFastVLine(GRID_LEFT + (GRID_SPACING * 2) + 1, GRID_TOP, GRID_SIZE, WHITE);
  display.display();

  // draw the chars to see the spacing
  display.setTextColor(WHITE);
  display.setTextSize(2);

  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      display.setCursor(board[x][y].xOffset, board[x][y].yOffset);
      display.print(board[x][y].mark);
      display.display();
    }
  }

  // place some text
  drawMessage(Player1, "Player 1:  X");
  drawMessage(Player2, "Player 2:  O");
  drawMessage(Status, "Ready!");

  button_event_t press;
  while (!xQueueReceive(buttonEvents, &(press), (TickType_t)100))
    ;
}

/*
 *  Print a message string in the P1, P2 or MSG areas.
 */
void TicTacToe::drawMessage(MsgLine which, String msg) {
  int y;

  if (which == Player1)       y = P1_OFFSET;
  else if (which == Player2)  y = P2_OFFSET;
  else if (which == Status)   y = MSG_OFFSET;
  else return;

  display.setTextSize(1);
  display.fillRect(0, y - 2, display.width(), MSG_SIZE, BLACK);
  display.setCursor(3, y);
  display.print(msg);
  display.display();
}

/*
 *  Draw a box around a given square to show where the next move
 *  will be placed.  Does not range check x or y.
 */
void TicTacToe::drawHighlight(uint8_t x, uint8_t y, bool on) {

  int left = GRID_LEFT + (x * GRID_SPACING) + x + 1;
  int top = GRID_TOP + (y * GRID_SPACING) + y + 1;

  display.drawRect(left, top, GRID_SPACING - 2, GRID_SPACING - 2, on ? HALF_BRIGHT : BLACK);
  display.display();
}

/*
 *  Track the cursor around the grid.  Keeps the cursor X, Y within
 *  the bounds and erases/redraws the highlight as necessary.
 */
void TicTacToe::trackCursor(uint8_t dir) {

  drawHighlight(curX, curY, false);

  switch (dir) {
    case BTN_UP:
      if (curY > 0) { curY--; };
      break;
    case BTN_DN:
      if (curY < 2) { curY++; };
      break;
    case BTN_LT:
      if (curX > 0) { curX--; };
      break;
    case BTN_RT:
      if (curX < 2) { curX++; };
      break;
  }

  drawHighlight(curX, curY, curOn);
}

/*
 *  Claim a square.  If we're hosting, try to place an X, otherwise O.
 *  Return true and update the display if the move is legal, otherwise
 *  blink the highlight and return false.
 */
bool TicTacToe::claimSquare() {

  bool blink = curOn;

  if (board[curX][curY].mark == ' ') {
      board[curX][curY].mark = (hosting ? 'x' : 'o');

    // Draw it
    display.setTextSize(2);
    display.setCursor(board[curX][curY].xOffset, board[curX][curY].yOffset);
    display.print(board[curX][curY].mark);
    display.display();
    return true;
  }

  // Blink the highlight to indicate no good
  for (int i = 0; i < 4; i++) {
    drawHighlight(curX, curY, !blink);
    delay(50);
    drawHighlight(curX, curY, blink);
    delay(50);
  }
  return false;
}

/*
 *  Determine if the game is complete or can continue.
 *  Compute using brute force and ignorance.
 */
Condition TicTacToe::updateCondition() {

  int free = 0;

  // Turn off the cursor while we check...
  curOn = false;
  drawHighlight(curX, curY, curOn);

  // Check the row conditions
  for (int r = 0; r < 3; r++) {
    if (board[0][r].mark != ' ' && (board[0][r].mark == board[1][r].mark && board[1][r].mark == board[2][r].mark)) {
      dprintf("ttt: Win condition, row %d, player %c\n", r, board[0][r].mark);

      // Show it!
      display.drawFastHLine(GRID_LEFT, GRID_TOP + (r * GRID_SPACING) + (GRID_SPACING / 2) + r, GRID_SIZE, HALF_BRIGHT);
      display.display();

      return (board[0][r].mark == 'x' && hosting || board[0][r].mark == 'o' && !hosting) ? Win : Lose;
    }
  }

  // Check the columns
  for (int c = 0; c < 3; c++) {
    if (board[c][0].mark != ' ' && (board[c][0].mark == board[c][1].mark && board[c][1].mark == board[c][2].mark)) {
      dprintf("ttt: Win condition, col %d, player %c\n", c, board[c][0].mark);

      // Show it!
      display.drawFastVLine(GRID_LEFT + (c * GRID_SPACING) + (GRID_SPACING / 2) + c, GRID_TOP, GRID_SIZE, HALF_BRIGHT);
      display.display();

      return (board[c][0].mark == 'x' && hosting || board[c][0].mark == 'o' && !hosting) ? Win : Lose;
    }
  }

  // TopLeft->BotRight diagonal?
  if (board[0][0].mark != ' ' && (board[0][0].mark == board[1][1].mark && board[1][1].mark == board[2][2].mark)) {
    dprintf("ttt: Win condition, LR diag, player %c\n", board[1][1].mark);

    display.drawLine(GRID_LEFT, GRID_TOP, GRID_LEFT + GRID_SIZE, GRID_TOP + GRID_SIZE, HALF_BRIGHT);
    display.display();

    return (board[1][1].mark == 'x' && hosting || board[1][1].mark == 'o' && !hosting) ? Win : Lose;
  }

  // BotLeft->TopRight diagonal?
  if (board[0][2].mark != ' ' && (board[0][2].mark == board[1][1].mark && board[1][1].mark == board[2][0].mark)) {
    dprintf("ttt: Win condition, RL diag, player %c\n", board[1][1].mark);

    display.drawLine(GRID_LEFT, GRID_TOP + GRID_SIZE, GRID_LEFT + GRID_SIZE, GRID_TOP, HALF_BRIGHT);
    display.display();

    return (board[1][1].mark == 'x' && hosting || board[1][1].mark == 'o' && !hosting) ? Win : Lose;
  }

  // See if the board is full
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      if (board[x][y].mark == ' ')
        free++;

  if (free == 0) {
    dprint("ttt: Game is a draw");
    return Draw;
  }

  // Turn the cursor backon and keep going
  curOn = true;
  drawHighlight(curX, curY, curOn);
  return Undecided;
}


/*
 *  Tic Tac Toe main loop
 *
 *  Starts by posting the Host/Join screen to find another player
 *  to play against.  (There's no solo/computer player at this time).
 *  Once the other player joins, the game alternates moves until a
 *  win or stalemate; then the sides switch and the board is reset.
 *  Users alternate until one quits, at which point statistics are
 *  printed and the game exits to the main menu.
 */
void TicTacToe::run() {

  button_event_t press;
  bool running = true;
  Condition state = Undecided;

  Serial.println("TicTacToe: Task starting");

  showGreeting();

  running = startNetwork();

  // Got net? Find someone to play with!
  if (running) {
    running = hostOrJoin();
  }

  // temp/testing

  resetBoard();
  drawScreen();

  curOn = true;
  drawHighlight(curX, curY, curOn);

  while (running) {

    while (xQueueReceive(buttonEvents, &(press), (TickType_t)50)) {
      if (press.action == btnReleased || press.action == btnRepeat) {

        switch (press.id) {
          case BTN_UP:
          case BTN_DN:
          case BTN_LT:
          case BTN_RT:
            trackCursor(press.id);
            break;

          case BTN_A:
            // for debugging: place an X
            hosting = true;
            claimSquare();
            break;

          case BTN_B:
            // for debugging: place an O
            hosting = false;
            claimSquare();
            break;

          case BTN_C:
            running = false;
            break;
        }

        // Since something changed, check the win condition
        state = updateCondition();

        if (state != Undecided) {
          // Turn off the highlight
          curOn = false;
          drawHighlight(curX, curY, curOn);

          // Update the status line
          switch (state) {
            case Win:
              drawMessage(Status, "You win!");
              stats[Win]++;
              break;

              case Lose:
              drawMessage(Status, "You lose.");
              stats[Lose]++;
              break;

              case Draw:
              drawMessage(Status, "It's a draw.");
              stats[Draw]++;
              break;
          }
        }
      }
    }
  }

  // Free up the network connection
  netTask.destroyQueue(netId);

  // record and present stats, goodbye screen

  // Wait for press to return to menu

  Serial.println("TicTacToe: Task complete");
}
