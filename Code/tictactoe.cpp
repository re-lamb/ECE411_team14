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

#include <Preferences.h>

#include "GameMan.h"
#include "button.h"
#include "tictactoe.h"


TicTacToe::TicTacToe()
  : GMTask("TICTACTOE") {
}

const String TicTacToe::appName = "TicTacToe";

void TicTacToe::setup(bool rsvp) {
  Preferences prefs;

  dprint("TicTacToe: Setup called as ");
  dprintln(rsvp ? "guest" : "host");

  hosting = !rsvp;   // if launched by RSVP, we're the guest
  myTurn = hosting;  // player X goes first
  seqNum = 0;        // unsynchronized

  // Clear the board and precompute the text offsets
  for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      board[x][y].mark = ' ';
      board[x][y].xOffset = (x * GRID_SPACING) + x + GRID_LEFT + 6;  // tune for font size
      board[x][y].yOffset = (y * GRID_SPACING) + y + GRID_TOP + 2;   // account for grid lines
    }
  }

  // Load/init the saved stats!
  if (prefs.begin(TTT_NVM_KEY, true)) {
    stats[0] = prefs.getUShort("win", 0);
    stats[1] = prefs.getUShort("lose", 0);
    stats[2] = prefs.getUShort("draw", 0);
    prefs.end();
  } else {
    dprintln("ttt: Failed to open preferences!? Stats reset");
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
 *  Display the goodbye message (and save stats).
 */
void TicTacToe::showSignOff() {
  Preferences prefs;
  button_event_t press;

  display.clearDisplay();
  display.setFont();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(getCenterX("Thanks for playing"), 10);
  display.print("Thanks for playing");
  display.setTextSize(2);
  display.setCursor(getCenterX("TicTacToe"), 30);
  display.println("TicTacToe");
  display.println();
  display.setTextSize(1);
  display.println("Your stats:");
  display.println("   " + String(stats[0]) + " wins");
  display.println("   " + String(stats[1]) + " losses");
  display.println("   " + String(stats[2]) + " draws");
  display.println();
  display.println("[Press any button]");
  display.display();

  // Save updated stats to flash
  if (prefs.begin(TTT_NVM_KEY, false)) {
    prefs.putUShort("win", stats[0]);
    prefs.putUShort("lose", stats[1]);
    prefs.putUShort("draw", stats[2]);
    prefs.end();
  } else {
    dprintln("ttt: Failed to open preferences!? Stats unsaved");
  }

  // Press to exit
  for (;;) {
    if (xQueueReceive(buttonEvents, &(press), (TickType_t)1000))
      if (press.action == btnReleased)
        return;
  }
}

/*
 *  On startup, ask the user if they want to host a new game or
 *  find an existing one to join.  Run after the network is up.
 *  Returns false if no other player connects or the user quits.
 */
bool TicTacToe::hostOrJoin() {
  button_event_t press;

  // A hack until RSVP works :-(
  display.println("Press A to host");
  display.println("Press B to join");
  display.println("Press C for menu");
  display.display();

  for (;;) {

    if (xQueueReceive(buttonEvents, &(press), (TickType_t)1000)) {
      if (press.action == btnReleased) {
        if (press.id == BTN_A) {
          hosting = true;
          return true;
        } else if (press.id == BTN_B) {
          hosting = false;
          return true;
        } else if (press.id == BTN_C) {
          return false;
        }
      }
    }
  }
}

/*
 *  Clear the board and reset for a new game.
 */
void TicTacToe::resetBoard() {

  // Clear the board contents
  for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
      board[x][y].mark = ' ';

  // Set cursor in the center and enable
  curX = curY = 1;
  curOn = true;

  // Set the message strings
  p1label = String(hosting ? "X : " : "O : ") + String(me->tag);
  p2label = String(hosting ? "O : " : "X : ") + String(them != NULL ? them->tag : "[Player 2]");
  statusMsg = String("Ready!");
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

  // Draw the text fields
  drawMessage(Player1, p1label);
  drawMessage(Player2, p2label);
  drawMessage(Status, statusMsg);
}

/*
 *  Print a message string in the P1, P2 or MSG areas.
 */
void TicTacToe::drawMessage(MsgLine which, String msg, uint8_t color) {
  int y;

  if (which == Player1) y = P1_OFFSET;
  else if (which == Player2) y = P2_OFFSET;
  else if (which == Status) y = MSG_OFFSET;
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
 *  blink the highlight and return false.  If it's not our turn, just
 *  blink to remind the user to be patient. :-)
 */
bool TicTacToe::claimSquare(bool host) {

  bool blink = curOn;

  if (myTurn) {
    if (board[curX][curY].mark == ' ') {
      board[curX][curY].mark = (host ? 'x' : 'o');

      // Draw it
      display.setTextSize(2);
      display.setCursor(board[curX][curY].xOffset, board[curX][curY].yOffset);
      display.print(board[curX][curY].mark);
      display.display();
      return true;
    }
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

bool TicTacToe::askToPlayAgain() {

  drawMessage(Status, "Play again?");
  delay(1000);
  return true;
}

/*
 *  Format and send a TicTacToe packet.
 */
void TicTacToe::sendTTT(const uint8_t *mac, uint8_t code, uint8_t seq) {
  gm_packet_t pkt;
  ttt_packet_t ttt;

  // Fill in the payload
  ttt.type = code;
  ttt.sequence = seq;
  ttt.which = hosting ? 'x' : 'o';
  ttt.x = curX;
  ttt.y = curY;
  memcpy(ttt.who, me->tag, GM_PLAYER_TAG_LEN);

  // Fill in the GM wrapper
  pkt.pktType = GM_TICTAC;
  memcpy(pkt.dstAddr, mac, ADDR_LEN);
  memcpy(pkt.srcAddr, me->node, ADDR_LEN);
  pkt.length = sizeof(ttt_packet_t);
  memcpy(pkt.payload, &ttt, pkt.length);

  // Ship it!
  netTask.sendPkt(&pkt);
}

/*
 *  Send an update to the other player based on our
 *  current state.  Uses all the globals.  Bleah.
 */
void TicTacToe::sendUpdate() {

  // broken networking
  return;

  // dprintf("ttt: sendUpdate state %d, seq %d, host %c, turn %c\n", state, seqNum, hosting ? 'T' : 'F', myTurn ? 'T' : 'F');

  switch (state) {

    case Reset:
      /*
       *  If unsynchronized and we're the host, send a Sync with seq #1
       *  and set up to be player X.  Tells the other to assume player O.
       *  Wait for their ack to begin.  There should be a timeout/retry
       *  here in case they miss the initial packet...
       */
      if (seqNum == 0) {
        if (hosting) {
          drawMessage(Status, "Sending SYNC", WHITE);
          seqNum = 1;
          sendTTT(netTask.broadcast, TTT_SYNC, seqNum);
        } else {
          drawMessage(Status, "Wait for SYNC", WHITE);
        }
      } else if (seqNum == 1) {
        // ready to transition to game mode
        drawMessage(Status, "Ready!");
        state = Undecided;
      }
      break;

    case Undecided:
      /*
        Here we send PLAY packets when a move has been made, then swap
        turns.  Bump the sequence number.
      */
      myTurn = !myTurn;
      break;

    case Win:
    case Lose:
    case Draw:
      /*
        these are no-ops?
      */
      break;

    case Quit:
      /*
        tell the other side we're bailing out
      */
      break;
  }
}

Condition TicTacToe::recvUpdate() {
  gm_packet_t pkt;
  ttt_packet_t ttt;

  // No net?  Fail.
  if (netQ == 0) return Quit;

  // dprintf("ttt: recvUpdate state %d, seq %d, host %c, turn %c\n", state, seqNum, hosting ? 'T' : 'F', myTurn ? 'T' : 'F');

  // Look for a GM_TICTAC and ignore any IFF stuff for now :-(
  if (xQueueReceive(netQ, &(pkt), (TickType_t)0)) {
    if (pkt.pktType == GM_TICTAC && pkt.length == sizeof(ttt_packet_t)) {

      memcpy(&ttt, pkt.payload, pkt.length);
      dprintf("ttt: Received a %c (seq %d) from %s\n", ttt.type, ttt.sequence, ttt.who);

      switch (ttt.type) {

        case TTT_SYNC:
          if (ttt.sequence == 1 && ttt.which == 'x') {
            /*
             *  If this ARRIVES with seq #1, the other side is the host;
             *  we respond with a 1 and set which to 'o' to let 'em know we
             *  are resetting and they go first.  We also record them as
             *  player 2 and transition to Undecided to await their first play.
             */
            drawMessage(Status, "SYNC received");
            hosting = myTurn = false;
            seqNum = 1;
            strncpy(them->tag, ttt.who, GM_PLAYER_TAG_LEN);
            p2label = String(hosting ? "O : " : "X : ") + String(them != NULL ? them->tag : "[Player 2]");
            drawMessage(Player2, p2label);
            return Undecided;
          }
          break;

        case TTT_PLAY:
          // their move
          break;

        case TTT_QUIT:
          // Other player quit, so wrap it up nicely.
          return Quit;
          break;

        default:
          dprintf("ttt: Type %x not implemented or invalid", ttt.type);
          break;
      }
    }
  }

  // ack
  return state;
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

  dprintln("TicTacToe: Task starting");

  bool running = true;
  state = Reset;
  seqNum = 0;

  showGreeting();
  running = startNetwork();

  // Got net? Find someone to play with!
  if (running) {
    me = netTask.getPlayer(0);  // get our player rec
    them = NULL;                // until someone joins
    running = hostOrJoin();
  }

  // Run the ersatz state machine
  while (running) {

    if (state == Reset) {
      // Set up the initial conditions
      resetBoard();
      drawScreen();

/*
    [ networking broken / disabled ]
    
      // Start the network dance
      sendUpdate();

      while (state == Reset) {

        delay(25);

        if (xQueueReceive(buttonEvents, &(press), (TickType_t)0)) {
          if (press.action == btnReleased && press.id == BTN_C)
            state = Quit;
        }

        recvUpdate();
      }
*/
      curOn = true;
      drawHighlight(curX, curY, curOn);

      state = Undecided;
      myTurn = true;      // hosting;   [networking broken/disabled]
      // And fall straight into the main loop
    }

    while (state == Undecided) {

      if (xQueueReceive(buttonEvents, &(press), (TickType_t)25)) {
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
              if (claimSquare(true)) {
                state = updateCondition();
                sendUpdate();
              }
              break;

            case BTN_B:
              // for debugging: place an O
              if (claimSquare(false)) {
                state = updateCondition();
                sendUpdate();
              }
              break;

            case BTN_C:
              state = Quit;
              running = false;
              break;
          }
        }
      }
    }

    // Turn off the highlight
    curOn = false;
    drawHighlight(curX, curY, curOn);

    // If they quit bail out now
    if (!running) continue;

    // Otherwise, check the end condition
    switch (state) {
      case Win:
        drawMessage(Status, "You win!");
        stats[0]++;
        break;

      case Lose:
        drawMessage(Status, "You lose.");
        stats[1]++;
        break;

      case Draw:
        drawMessage(Status, "It's a draw.");
        stats[2]++;
        break;
    }

    // Go again?
    if (askToPlayAgain()) {
      state = Reset;
      hosting = !hosting;
      // todo: swap sides! reset the messages
      // tood: inform the other player
    } else {
      // Fall through to end
      running = false;
    }
  }

  // Free up the network connection
  netTask.destroyQueue(netId);

  // Save stats and exit gracefully
  showSignOff();

  dprintln("TicTacToe: Task complete");
}
