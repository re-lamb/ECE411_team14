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
#include <WiFi.h>
#include <esp_now.h>
#include "task.h"

/*
 *  GM protocol identifiers
 *  (Each app/game registers any custom types here)
 */
#define GM_INVALID  0x00    // unknown or uninitialized
#define GM_IFF      0x01    // network task coordination protocol
#define GM_RSVP     0x02    // used by the menu to invite players
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
#define MAX_PENDING    10         // incoming packet queue, adjust if needed
#define MAX_CLIENTS     4         // how many network queues can we manage?
#define MAX_FILTERS     5         // probably only one per app, realistically
#define MAX_PLAYERS     5         // how many GM units can we talk to? (ESP-NOW limit is ~20)

typedef struct GMpkt {
  uint8_t srcAddr[ADDR_LEN];      // MAC of this unit on transmit
  uint8_t dstAddr[ADDR_LEN];      // destination or all ff's for broadcast
  uint8_t pktType;                // 256 types oughtta be enough
  uint8_t length;                 // size of the payload portion
  uint8_t payload[MAX_PKT_LEN];   // user/game defined, max 250 bytes!
} gm_packet_t;

/*
 *  Specific protocol id bytes used by the network/menu tasks.
 *  We just use the IFF packet type and codes for RSVPs too, but
 *  use a separate filter byte so that the menuTask can request
 *  just the RSVPs and the netTask can handle HELLO/GOODBYE (and
 *  any future enhancements such as time coordination, remote
 *  detonation, etc.)
 */
#define IFF_HELLO   0x01    // status announcement
#define IFF_RSVP    0x02    // when hosting: send invite to join
#define IFF_ACCEPT  0xac    // player responds affirmatively
#define IFF_REJECT  0x86    // player responds negatively
#define IFF_GOODBYE 0xbb    // this GM is going offline

#define IFF_PAYLOAD   32    // context dependent
#define IFF_INTERVAL  5000  // how often to send HELLO or RSVP broadcasts

typedef struct IFFpkt {
  uint8_t type;                 // IFF_* code above
  uint8_t reqId;                // for RSVPs, who's asking?
  char who[GM_PLAYER_TAG_LEN];  // tag/name of sender, if set (generated from MAC if not)
  char what[IFF_PAYLOAD];       // string identifying the app, or general
  int timeSent;                 // local time of sender for timing coordination
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

enum statCount : byte { pktTotalSent, pktSendError, pktTotalRecv, pktRecvOverflow, pktDispatched, pktDropped };

class NetworkTask : public GMTask {
  public:
    NetworkTask();
    void setup(bool rsvp) override;
    bool isRunning();

    // Network stuff
    int createQueue(int maxDepth = 8);
    QueueHandle_t getHandle(int qId);
    void destroyQueue(int qId);

    int addFilter(int qId, uint8_t code);
    int dropFilter(int qId, uint8_t code);

    int sendPkt(gm_packet_t *pkt);

    void sendRSVP(const char *appRequest, uint8_t replyTo);

    const uint8_t broadcast[ADDR_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    static const char *fmtMAC(const uint8_t *mac);
    

    // Player stuff
    String getPlayerName();
    String getNodeAddr();
    void setPlayerName(char *name);
    int findPlayer(char *name);
    gm_player_t *getPlayer(int id);

  private:
    void run() override;

    // Callback to catch/filter/distribute incoming packets
    static void recvCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len);

    static QueueHandle_t incoming;
    static int pktStats[6];

    void dispatch();
    void addPeer(const uint8_t *mac);
    void sendAccounting(esp_err_t err);
    void dumpStats();

    // GM protocol routines
    void sendIFF(uint8_t code);
    void receiveIFF(QueueHandle_t q);
    
    bool initialized;
    int lastHello;

    // Preallocate a table of connections
    gm_packet_queue_t clients[MAX_CLIENTS];
    int numClients;

    // Player info
    gm_player_t players[MAX_PLAYERS];

    int addPlayer(char *name, uint8_t *node);

};

#endif
