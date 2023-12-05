/*
 *  network.h - Definitions to support GameMan networking
 *
 *  Abstract:
 *      Defines the custom wireless network protocols available
 *      to applications written for the GameMan.  See the design
 *      notes for details and API.
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#ifndef _GM_NET_H_
#define _GM_NET_H_

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_now.h>
#include "task.h"

/*
 *  GM protocol identifiers
 *  (Each app/game registers any custom types here)
 */
#define GM_INVALID  0x00    // unknown or uninitialized
#define GM_IFF      0x01    // network task coordination protocol
#define GM_CHAT     0x0f    // a simple chat application for testing
#define GM_TICTAC   0x10    // tic-tac-toe
#define GM_BTLSHIP  0x42    // battleship game
#define GM_MAZEWAR  0xa1    // multi-player mayhem
#define GM_ANY      0xff    // a poor man's promiscuous mode

/*
 *  GM network/player constants
 */
#define GM_PLAYER_TAG_LEN 12      // choose a reasonable length
#define GM_NVM_KEY  "GMan"        // namespace for saving preferences

#define ADDR_LEN        6         // ESP_NOW_ETH_ALEN - 48-bit Ethernet-type MAC
#define MAX_PKT_LEN   236         // ESP_NOW_MAX_DATA_LEN is 250, minus some GMpkt overhead
#define MAX_FILTERS    10         // probably only one per app, realistically
#define MAX_CLIENTS     5         // how many network queues can we manage?

typedef struct GMpkt {
  uint8_t srcAddr[ADDR_LEN];      // MAC of this unit on transmit
  uint8_t dstAddr[ADDR_LEN];      // destination or all ff's for broadcast
  uint8_t pktType;                // 256 types oughtta be enough
  uint8_t length;                 // size of the payload portion
  uint8_t payload[MAX_PKT_LEN];   // user/game defined, max 250 bytes!
} gm_packet_t;

/*
 * Specific protocol id bytes used by the network task itself
 */
#define IFF_HELLO   0x01    // status announcement
#define IFF_RSVP    0x12    // when hosting: send invite to join
#define IFF_ACCEPT  0xac    // player responds affirmatively
#define IFF_REJECT  0x86    // player responds negatively
#define IFF_GOODBYE 0x10    // this GM is going offline/app shutting down

#define IFF_PAYLOAD 32
  /*
     IFF_HELLO is sent by every GM at startup and then IFF_INTERVAL millis after
     who[] is always the name of the player, and if it changes the receiver(s)
     should all update their records.  what is their version information?  the
     timeSent field can be used to synchronize clocks (without need for NTP or
     some heavy protocol?) since it can be saved as an offset on a per-host basis.
     HELLO packets are exclusively handled by the network task itself.

     IFF_RSVP can be the common "i want to host a game of ____" packet, where the
     what field is the text name of the app/task.  for a 2-player game the first
     one to send back an IFF_ACCEPT is let in (this can be game specific?)

     IFF_ACCEPT is just a reply to the RSVP in the affirmative; IFF_REJECT if the
     invitation is declined (or times out?).  the network manager can generate and
     accept these on behalf of any app/game to prevent duplication of code, i.e.
     networkTask.invite() returns true/false...

     IFF_GOODBYE could be sent if the user quits a game or runs the DND or even
     a shutdown app (if we made the EN pin a latch :-).  maybe this is sent 
     automatically if the battery level gets too low!?
  */
typedef struct IFFpkt {
  uint8_t type;                 // IFF_* code above
  int timeSent;                 // local time of sender for timing coordination
  char who[GM_PLAYER_TAG_LEN];  // tag/name of sender, if set (generated from MAC if not)
  char what[IFF_PAYLOAD];       // string identifying the app, or general
} iff_packet_t;


typedef struct player {
  char tag[GM_PLAYER_TAG_LEN];  // who
  uint8_t node[ADDR_LEN];       // where last seen
  int lastSeen;                 // when last seen
} gm_player_t;


typedef struct queue {
  bool inUse;                       // is this entry valid?
  QueueHandle_t handle;             // where dispatched
  uint8_t filters[MAX_FILTERS];     // type codes to accept
  uint8_t numFilters;               // how many active
  int numReceived;                  // total matching packets received (debug)
} gm_packet_queue_t;


class NetworkTask : public GMTask {
  public:
    NetworkTask();
    void setup() override;
    bool isRunning();

    // Client tasks request service by creating a new queue
    int createQueue(int maxDepth = 8);
    QueueHandle_t getHandle(int qId);
    void destroyQueue(int qId);

    // ... then they register the type(s) of packets they want to see 
    int addFilter(int qId, uint8_t code);
    int dropFilter(int qId, uint8_t code);

    // Send packets
    int sendAll(gm_packet_t pkt);
    int sendTo(gm_player_t player, gm_packet_t pkt);

    const uint8_t broadcast[ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    char *getPlayerName();
    void setPlayerName(char *name);

  private:
    void run() override;

    // Callback to catch/filter/distribute incoming packets
    static void recvCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len);

    void dispatch();
    void dumpStats();

    // GM protocol routines
    void sendHello();

    bool initialized;

    // Keep it simple, and preallocate a table of connections
    gm_packet_queue_t clients[MAX_CLIENTS];
    int numClients;

    int pktsSent;
    int pktsReceived;
    int pktsDropped;

    gm_player_t me;
    Preferences prefs;
};

#endif
