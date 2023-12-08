/*
 *  network.cpp - GameMan networking task
 *
 *  Abstract:
 *    Manages networking for the GameMan.  Tasks request a new
 *    connection and set up filters for the type of packets they
 *    want to receive; the network task handles all interaction
 *    with the hardware and dispatches packets into the queue for
 *    the clients while dropping unknown or unwanted traffic.  The
 *    network task also discovers and remembers other GameMan nodes
 *    by listening for and sending out "IFF" packets periodically
 *    so that players don't have to bother with pairing or setup of
 *    any kind (i.e., no security in this version :-)
 *
 *  Team 14 Project
 *  Portland State University
 *  ECE411 Fall 2023
 */

#include <Preferences.h>

#include "GameMan.h"
#include "network.h"

// Quick and dirty check to see that the given qId is in range
#define VALID(x) (x >= 0 && x < MAX_CLIENTS && clients[x].inUse)

NetworkTask::NetworkTask()
  : GMTask("NET", 8192, 2, PRO_CPU_NUM) {

  // Maybe not required, but good practice?
  initialized = false;
  lastHello = 0;
  numClients = 0;

  for (int i = 0; i <= pktDropped; i++) {
    pktStats[i] = 0;
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].inUse = false;
  }

  for (int i = 0; i < MAX_PLAYERS; i++) {
    memset(&players[i], 0, sizeof(gm_player_t));
  }
}

QueueHandle_t NetworkTask::incoming;
int NetworkTask::pktStats[6];

/*
 *  Initialize the ESP NOW network stack.
 */
void NetworkTask::setup(bool rsvp) {

  esp_err_t err;
  char name[GM_PLAYER_TAG_LEN];
  Preferences prefs;

  Serial.println("net: Task initializing");

  // Put ESP32 into Station mode
  WiFi.mode(WIFI_MODE_STA);

  // Init the network stack
  err = esp_now_init();
  if (err != ESP_OK) {
    Serial.printf("net: Error %d trying to initialize ESP NOW!\n", err);
  } else {
    Serial.println("net: ESP NOW initialized");
    initialized = true;
  }

  // Register the broadcast address as a peer
  addPeer(broadcast);

  // Print MAC Address to Serial monitor
  dprint("net: MAC Address: ");
  dprintln(WiFi.macAddress());

  // and save it to our player record
  WiFi.macAddress(players[0].node);

  // Open up our nvram namespace
  if (!prefs.begin(GM_NVM_KEY, false)) {
    Serial.println("net: Failed to open preferences!?");
  }

  // Fetch our own player info
  if (prefs.getBytes("tag", &name, GM_PLAYER_TAG_LEN) < 1) {
    Serial.println("net: Player tag not found in NVRAM, setting default");

    snprintf(name, GM_PLAYER_TAG_LEN, "Player%d", random(1, 99));

    // Save it to the nvram
    if (!prefs.putBytes("tag", name, GM_PLAYER_TAG_LEN)) {
      Serial.printf("net: Failed to save player tag '%s' in NVRAM\n", name);
    }
  }

  // Set it in the player table
  setPlayerName(name);
  dprintf("net: Player tag set: '%s'\n", getPlayerName());
  prefs.end();
}

/*
 *  Pretty print a wifi/Ethernet address quick and dirty.  mac better point to
 *  at least six bytes of the address to output.
 */
const char *NetworkTask::fmtMAC(const uint8_t *mac) {
  static char buf[18];

  sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return buf;
}

String NetworkTask::getNodeAddr() {
  String str = String(fmtMAC(players[0].node));
  return str;
}

/*
 *  True if the ESP NOW functionality was successfully set up.
 */
bool NetworkTask::isRunning() {
  return initialized;
}

/*
 *  Create a new queue structure and return the id (index into our
 *  client table), or -1 if queue creation fails or no free slots.
 */
int NetworkTask::createQueue(int maxDepth) {

  if (!initialized) return -1;

  for (int i = 0; i < MAX_CLIENTS; i++) {

    if (!clients[i].inUse) {

      // Try to create a new queue; if that fails, bail out
      clients[i].handle = xQueueCreate(maxDepth, maxDepth * sizeof(gm_packet_t));
      if (clients[i].handle == 0) {
        Serial.println("net: Failed to create network queue!!");
        return -1;
      }

      // We're good!
      clients[i].inUse = true;
      clients[i].numReceived = 0;
      clients[i].numFilters = 0;
      for (int j = 0; j < MAX_FILTERS; j++) {
        clients[i].filters[j] = GM_INVALID;
      }

      dprintf("net: Client %d queue created\n", i);
      numClients++;
      return i;
    }
  }

  return -1;  // No slots left
}

/*
 *  Return the actual queue handle for retrieving packets.
 */
QueueHandle_t NetworkTask::getHandle(int qId) {
  if (!VALID(qId)) return NULL;

  return clients[qId].handle;
}

/*
 *  Shut down a network queue and mark the slot as free.  Each client
 *  should clean up when exiting or the table will eventually fill up!
 */
void NetworkTask::destroyQueue(int qId) {
  if (VALID(qId)) {
    clients[qId].inUse = false;
    vQueueDelete(clients[qId].handle);
    clients[qId].handle = NULL;
    numClients--;
    dprintf("net: Client %d queue destroyed\n", qId);
  }
}

/*
 *  Add a filter for types to receive on this queue.  Returns -1 if no
 *  slots or the qId is invalid; returns >= 0 if added successfully.
 */
int NetworkTask::addFilter(int qId, uint8_t code) {
  if (!VALID(qId)) return -1;

  for (int i = 0; i < MAX_FILTERS; i++) {
    if (clients[qId].filters[i] == GM_INVALID) {
      clients[qId].filters[i] = code;
      clients[qId].numFilters++;
      dprintf("net: Client %d added type %d filter (%d active)\n", qId, code, clients[qId].numFilters);
      return i;
    }
  }

  return -1;  // No more slots!
}

/*
 *  Stop receiving certain packet types.
 */
int NetworkTask::dropFilter(int qId, uint8_t code) {
  if (!VALID(qId)) return -1;

  for (int i = 0; i < MAX_FILTERS; i++) {
    if (clients[qId].filters[i] == code) {
      clients[qId].filters[i] = GM_INVALID;
      clients[qId].numFilters--;
      dprintf("net: Client %d dropped type %d filter (%d active)\n", qId, code, clients[qId].numFilters);
      return i;
    }
  }

  return -1;  // Didn't find it...
}

/*
 *  Send a network broadcast packet.
 */
int NetworkTask::sendPkt(gm_packet_t *pkt) {

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(pkt->dstAddr, (uint8_t *)pkt, sizeof(gm_packet_t));

  dprintf("net: Sent to %s (type %d)\n", fmtMAC(pkt->dstAddr), pkt->pktType);
  sendAccounting(result);
  return result;
}

/*
 *  For debugging, check/print error code and count stats.
 */
void NetworkTask::sendAccounting(esp_err_t err) {

  if (err == ESP_OK) {
    pktStats[pktTotalSent]++;
  } else {
    // For debugging, enumerate the error
    Serial.print("net: Send error: ");

    switch (err) {
      case ESP_ERR_ESPNOW_NOT_INIT: Serial.println("ESP-NOW not initialized"); break;
      case ESP_ERR_ESPNOW_ARG: Serial.println("Invalid argument"); break;
      case ESP_ERR_ESPNOW_INTERNAL: Serial.println("Internal error"); break;
      case ESP_ERR_ESPNOW_NO_MEM: Serial.println("No memory"); break;
      case ESP_ERR_ESPNOW_NOT_FOUND: Serial.println("Peer not found"); break;
      default:
        Serial.println("Unknown error");
    }

    pktStats[pktSendError]++;
  }
}

/*
 *  Callback for ALL packets received from ESP NOW.  To sidestep all the
 *  Arduino C++/ESP-IDF C calling nonsense, just queue everything and let
 *  the dispatch() run during the main task loop.  This is kind of horrible
 *  and inefficient but time is short and we can make it pretty later.
 */
void NetworkTask::recvCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {

  gm_packet_t pkt;

  dprintf("net: Received from %s (%d bytes)\n", fmtMAC(mac_addr), data_len);

  memcpy(&pkt, data, data_len);

  if (xQueueSend(incoming, (void *)&pkt, (TickType_t)0) != pdTRUE) {
    Serial.println("net: Incoming queue full, packet dropped!");
    pktStats[pktRecvOverflow]++;
  } else {
    pktStats[pktTotalRecv]++;
  }
}

/*
 *  Examine the packet type and place it on the appropriate client queue if
 *  anyone is filtering for it.  Otherwise just drop it.
 */
void NetworkTask::dispatch() {

  gm_packet_t pkt;

  // If our network isn't up, there's nothin' to do
  if (!initialized || !incoming) return;

  // Got packets?  Wait (block) up to 10ms...
  if (!xQueueReceive(incoming, &(pkt), (TickType_t)10)) return;

  dprintf("net: Dispatch type %d: ", pkt.pktType);

  // See if anyone wants it
  // todo: this is brute force and slow, but sufficient for now?
  for (int q = 0; q < MAX_CLIENTS; q++) {
    if (clients[q].inUse) {
      // dprintf("\n--> client %d ", q);
      for (int f = 0; f < MAX_FILTERS; f++) {
        if (clients[q].filters[f] == pkt.pktType) {
          // dprintf("match filter %d! ", f);
          if (clients[q].handle) {
            // dprintln("handle is good, about to queue:");
            if (xQueueSend(clients[q].handle, (void *)&pkt, (TickType_t)0) == pdTRUE) {
              dprintf("Sent to client %d\n", q);
              pktStats[pktDispatched]++;
              clients[q].numReceived++;
            } else {
              dprintf("Failed, client %d queue full!\n", q);
              pktStats[pktDropped]++;
            }
          } else {
            dprintf("Failed, client %d queue invalid!\n", q);
            pktStats[pktDropped]++;
          }
          // Bug out. For now, it's first come, first served...
          return;
        }
      }
    }
  }

  // Nobody's interested
  dprintln("Dropping unloved packet");
  pktStats[pktDropped]++;
}

/*
 *  (Internal) Convenience routine to construct and send an IFF_HELLO or
 *  GOODBYE broadcast packet.
 */
void NetworkTask::sendIFF(uint8_t code) {
  gm_packet_t pkt;
  iff_packet_t hello;

  // Fill in the payload
  hello.type = code;
  hello.reqId = 0;
  memcpy(hello.who, players[0].tag, GM_PLAYER_TAG_LEN);
  strncpy(hello.what, menuTask.getCurrentApp(), IFF_PAYLOAD);
  hello.timeSent = xTaskGetTickCount();

  // Fill in the GM wrapper
  pkt.pktType = GM_IFF;
  memcpy(pkt.dstAddr, broadcast, ADDR_LEN);
  memcpy(pkt.srcAddr, players[0].node, ADDR_LEN);
  pkt.length = sizeof(iff_packet_t);
  memcpy(pkt.payload, &hello, pkt.length);

  // Ship it!
  sendPkt(&pkt);
}

/*
 *  Broadcast an RSVP packet.  This is a slight tweak to the IFF format
 *  used just to poke the menuTask when trying to sync up two players
 *  to start a game.  Normal IFF receive processing uses the replyTo
 *  field as the queue ID to send responses back to the requester.
 *  Network rendezvous is hard and this is a quick hack.
 */
void NetworkTask::sendRSVP(const char *appRequest, uint8_t replyTo) {
  gm_packet_t pkt;
  iff_packet_t rsvp;

  // Fill in the payload
  rsvp.type = IFF_RSVP;
  rsvp.reqId = replyTo;
  memcpy(rsvp.who, players[0].tag, GM_PLAYER_TAG_LEN);
  strncpy(rsvp.what, appRequest, IFF_PAYLOAD);
  rsvp.timeSent = xTaskGetTickCount();

  // Fill in the GM wrapper
  pkt.pktType = GM_RSVP;
  memcpy(pkt.dstAddr, broadcast, ADDR_LEN);
  memcpy(pkt.srcAddr, players[0].node, ADDR_LEN);
  pkt.length = sizeof(iff_packet_t);
  memcpy(pkt.payload, &rsvp, pkt.length);

  // Send it
  sendPkt(&pkt);
}

/*
 *  Decode and act on incoming IFF packets.
 */
void NetworkTask::receiveIFF(QueueHandle_t q) {
  gm_packet_t pkt;
  iff_packet_t iff;

  if (!xQueueReceive(q, &(pkt), (TickType_t)0)) return;

  dprint("net: Received IFF: ");

  // Pull the IFF payload from the GM wrapper and see what to do with it
  memcpy(&iff, pkt.payload, pkt.length);

  switch (iff.type) {
    case IFF_HELLO:
      dprintf("HELLO from %s (running %s)\n", iff.who, iff.what);

      if (strlen(iff.who) > 0) {

        int playerNum = findPlayer(iff.who);
        if (playerNum < 0) {
          // A new friend!  Add 'em
          playerNum = addPlayer(iff.who, pkt.srcAddr);
          addPeer(pkt.srcAddr);
        } else {
          // An old friend!  Update 'em
          players[playerNum].lastSeen = xTaskGetTickCount();
        }
      } else {
        dprintln("net: Invalid player (name not set), ignored");
      }
      break;

    case IFF_ACCEPT:
    case IFF_REJECT:
      dprintf("%s from %s (running %s)\n",
              iff.type == IFF_ACCEPT ? "ACCEPT" : "REJECT",
              iff.who, iff.what);

      // Send it to the original requester!
      // Same as dispatch(), but bypassing filters; make sure the
      // queue still exists, since they may have gone away
      if (VALID(iff.reqId)) {
        if (xQueueSend(clients[iff.reqId].handle, (void *)&pkt, (TickType_t)0) == pdTRUE) {
          dprintf("net: Dispatched to client %d\n", iff.reqId);
          pktStats[pktDispatched]++;
          clients[iff.reqId].numReceived++;
        } else {
          dprintf("net: Dispatch failed, client %d queue full!\n", iff.reqId);
          pktStats[pktDropped]++;
        }
      } else {
        dprintf("net: Dispatch failed, client %d queue invalid!\n", iff.reqId);
        pktStats[pktDropped]++;
      }
      break;

    case IFF_GOODBYE:
      dprintf("GOODBYE from %s (unimplemented)\n", iff.who);
      // use this to delete a player/mac association
      // if they are changing their name, we want to flush the old
      // or they are just switching off or have timed out, battery died, ...
      break;

    default:
      dprintf("Unknown type %d!\n", iff.type);
      // Should never happen; if it does, hex/ascii dump the payload?
      break;
  }
}

/*
 *  Debugging output to serial port.
 */
void NetworkTask::dumpStats() {

  if (!initialized) {
    Serial.println("ESP NOW networking not initialized!");
  } else {
    Serial.printf("Wifi stats for %d active clients:\n", numClients);
    Serial.printf("  Sent:  %d success, %d fail\n", pktStats[pktTotalSent], pktStats[pktSendError]);
    Serial.printf("  Recv:  %d total, %d overflow\n", pktStats[pktTotalRecv], pktStats[pktRecvOverflow]);
    Serial.printf("  Queue: %d dispatched, %d dropped\n", pktStats[pktDispatched], pktStats[pktDropped]);
  }
}

/*
 *  Get or set the player's name (on this node)
 */
String NetworkTask::getPlayerName() {
  return String(players[0].tag);
}

void NetworkTask::setPlayerName(char *name) {
  dprintf("net: setPlayerName to '%s'\n", name);
  strncpy(players[0].tag, name, GM_PLAYER_TAG_LEN);
}

/*
 *  Search for a player and return their player number, -1 if not found.
 */
int NetworkTask::findPlayer(char *name) {

  for (int p = 0; p < MAX_PLAYERS; p++) {
    if (strncmp(players[p].tag, name, GM_PLAYER_TAG_LEN) == 0) return p;
  }

  return -1;  // nope
}

gm_player_t *NetworkTask::getPlayer(int id) {
  if (id >= 0 && id < MAX_PLAYERS) {
    return &players[id];
  }
  return NULL;
}
/*
 *  Add a new network player if there's room.  Otherwise, flail.
 */
int NetworkTask::addPlayer(char *name, uint8_t *node) {

  // Find the first slot where the name is null.  Player 0 is
  // this node, by convention/laziness
  for (int p = 1; p < MAX_PLAYERS; p++) {
    if (strlen(players[p].tag) == 0) {
      dprintf("net: Adding player '%s' (node %s) at id %d\n", name, fmtMAC(node), p);
      strncpy(players[p].tag, name, GM_PLAYER_TAG_LEN);
      memcpy(players[p].node, node, ADDR_LEN);
      players[p].lastSeen = xTaskGetTickCount();

      // Add them as a network peer if they aren't there already
      addPeer(players[p].node);
      return p;
    }
  }

  dprintf("net: Failed to add player '%s', no empty slots!\n", name);
  return -1;  // fix this if more GM hardware ever gets made :-)
}

/*
 *  Add a node address to the ESP-NOW peer list (so we can send
 *  packets to them directly, not just broadcasts).
 */
void NetworkTask::addPeer(const uint8_t *mac) {
  esp_now_peer_info_t p;
  esp_err_t err;

  if (!esp_now_is_peer_exist(mac)) {

    memset(&p, 0, sizeof(esp_now_peer_info_t));  // work around bug in esp-now 2.x
    memcpy(p.peer_addr, mac, ADDR_LEN);

    err = esp_now_add_peer(&p);
    if (err == ESP_OK) {
      dprintf("net: Added peer %s\n", fmtMAC(mac));
    } else {
      Serial.printf("net: Error %d registering peer!\n", err);
    }
  }
}

/*
 *  NetworkTask main loop
 *
 *  Here we simply wait for the callback to fire to indicate that
 *  a packet has been received.  We scan through the list of clients
 *  to see if anyone is interested in the packet, and if so push it
 *  onto their queue.  IFF packets are handled by us directly, time
 *  offsets calculated, and statistics kept.
 *  
 */
void NetworkTask::run() {

  esp_err_t err;
  int lastHello = xTaskGetTickCount();
  int lastStats = lastHello;

  Serial.printf("net: Task starting up on core %d\n", xPortGetCoreID());

  // Create the queue for incoming ESP-NOW packets
  incoming = xQueueCreate(MAX_PENDING, sizeof(gm_packet_t));
  if (!incoming) {
    Serial.println("net: Failed to create incoming packet queue!");
    initialized = false;
  }

  // Set up the global receive callback to actually catch them
  err = esp_now_register_recv_cb(recvCallback);
  if (err != ESP_OK) {
    Serial.printf("net: ERROR %d registering network receive callback!\n", err);
    initialized = false;
  }

  // Set up a queue for IFF packets (handled here)
  int iffQId = createQueue();

  if (addFilter(iffQId, GM_IFF) < 0) {
    Serial.println("net: ERROR adding IFF filter!?");
  }

  // And grab the actual queue handle
  QueueHandle_t iffQueue = getHandle(iffQId);

  // Main loop
  for (;;) {

    // Deal with pending packets (with blocking to avoid watchdogs)
    dispatch();

    // Check our queue for local processing
    receiveIFF(iffQueue);

    // n seconds since last hello packet sent?
    if (elapsed(IFF_INTERVAL, lastHello)) {
      sendIFF(IFF_HELLO);
      lastHello = xTaskGetTickCount();
    }

    // Debug: dump stats (less frequently)
    if (elapsed(30000, lastStats)) {
      dumpStats();
      lastStats = xTaskGetTickCount();
    }
  }
}
