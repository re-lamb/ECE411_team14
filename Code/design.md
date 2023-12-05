
GameMan software environment design/notes
-----------------------------------------


The GameMan software takes advantage of the choice of the ESP32 MCU
and is tailored for the specific configuration of the buttons and
display chosen for our implementation.  This means we can use the
built-in features and a fixed configuration to keep the memory foot-
print smaller and use the ESP32 features such as:

    ESP-NOW - peer-to-peer wifi networking with almost no
        setup, protocol overhead or heavy software layers

    ESP-IDF - access to true multitasking that can use both
        cores so the network, button polling, timers and game
        loop can run concurrently

Those features are provided through C libraries, but the Arduino IDE
is more C++ like, so some "glue" is provided.  Rather than use a
cooperative multitasking library and a strictly callback-based kind
of mechanism, the built-in FreeRTOS multitasking lets the game loop
run almost as if it is a typical Arduino sketch, assuming full control
over the whole machine.  The only difference is that the code for each
game or app is subclassed, loaded by the menu, and makes explicit task
yield calls to allow the scheduler to run other tasks.

Tasks
-----

Menu:
The main task (pinned to the "app" core) is the menuTask.  This handles
all of the startup and initialization, splash screen, menu selection,
and launching of apps/games.  When a selection is made, the menu task
instantiates the new program as a separate task, then puts itself into
a very low-overhead wait loop.  This lets it look for "home button"
events or dispatch certain network events, but otherwise lets the game
run in the foreground and use most of the CPU.  When the game completes
or the user holds down the "home"/"abort" button for n seconds, the
menu can suspend or shut down the game and take over again.

Buttons:
A periodic button poller called buttonTask (button.h, button.cpp) runs
at a fixed poll rate, configurable as BTN_POLL_RATE.  Initially this is
set to 10ms, which provides a reasonable amount of "debouncing" but can
be tuned after more testing.  All 7 buttons are sampled and events are
placed on the queue for button down, button up or button repeat events.
Multiple buttons can be pressed at once.  The state of any button (or
all buttons) can be queried at any time.

Network:
The ESP32's WiFi/Bluetooth functions are run on a dedicated CPU core.
We wrap the interface to the ESP-NOW network library with a C++ task
that handles all incoming packets, classifying them into event types
and enqueueing them for delivery to the current application.  This lets
the game code look for just the incoming packets it cares about, while
some "housekeeping" stuff (like new GameMan consoles coming into or out
of range, or players joining/leaving the network) can be handled by the
menuTask.

Games/Apps:
Each menu entry is a single object that is loaded into a new task.  A
default "AboutBox" shows some basic information (software version,
credits, etc) and lets the user do some simple customization like adding
their name/tag and possibly other tunable settings.  A "SysInfo" app/tab
gives a peek at battery life, free memory, network status, etc.  These
are helpful for debugging the basic software framework.

Each team member is invited to write a game or app of their own.  Ideas
for simple 2-player turn-based games include a Pong or tennis game,
Tic-Tac-Toe, Checkers (or Chess!), Battleship, or the like.  These are
turn based and should be fairly easy to implement.  The more ambitious
multi-player scenarios include MazeWar, the 1973 classic retro FPS that
inspired the project, or some kind of n-way Chinese checkers?  Or if
memory permits, a single-player Hangman or Space Invaders or other
classic arcade title could be included.  The only limits are time and
the available flash RAM on the module; a minimum of 4MB should hold at
least 2 or 3 games, while the 8-16MB ESP32 boards could hold a lot
more.

API
---

TBD.

The "SysInfo" app (or maybe AboutBox?) lets the user customize their
unit by entering a name or tag through the keypad.  This tag is then
associated with their GM's MAC address and included in IFF broadcasts
so that players can identify each other easily.  The tag is kept in
the ESP32's non-volatile RAM area using the Preferences functions.

Network design
--------------

Incoming network packets are filtered by the network task and only GM-
specific ones are handed off to the incoming packet queue.  This helps
simplify the event loop for applications, since any protocol the games
use can be kept separate from "housekeeping" functions.  Each task can
register a filter for which kinds of packets it is able to handle, so
any that aren't recognized are just ignored.

When first switched on, and every 'n' seconds later the GM transmits an
IFF packet.  This is broadcast to alert any other units in the vicinity
that a new player has entered range.  The network task itself handles
these broadcasts and keeps a list of active players.

    Packet type(s) handled:  IFF

The menu task can show the current list of known associates and if
they are active and in range, their current status.

Having the host/join/accept/reject stuff be common eliminates duplicate
code, since those can be set up like function calls
  if (networkTask.invite(player, this->taskName) == IFF_ACCEPT) {
    /* go into host mode and start the game loop */
  }


Preferences
-----------

Player name or tag is n chars (8-12?) long and kept in Preferences
(nvram), set by the Sys Info app (or Settings, whatever we call it)

Other personalizations can be saved too (up to 512 bytes?) which is
a nice touch




