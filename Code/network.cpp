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

#include "network.h"

// Quick and dirty check to see that the given qId is in range
#define VALID(q) (q >= 0 && q < numClients)

NetworkTask::NetworkTask()
  : GMTask("NET", 8192, 2, PRO_CPU_NUM) {

  // Maybe not required, but good practice?
  initialized = false;
  numClients = 0;
  pktsSent = 0;
  pktsReceived = 0;
  pktsDropped = 0;

  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].inUse = false;
  }
}

/*
 *  Initialize the ESP NOW network stack.
 */
void NetworkTask::setup() {

  esp_err_t err;
  char name[GM_PLAYER_TAG_LEN];

  Serial.println("Network task initializing");

  // Put ESP32 into Station mode
  WiFi.mode(WIFI_MODE_STA);

  // Init the network stack
  err = esp_now_init();
  if (err != ESP_OK) {
    Serial.printf("Error %d trying to initialize ESP NOW!\n", err);
  } else {
    Serial.println("ESP NOW initialized");
    initialized = true;
  }

  // Register the broadcast address as a peer?
  esp_now_peer_info_t all;
  memset(&all, 0, sizeof(all));         // work around bug in esp-now 2.x
  memcpy(all.peer_addr, broadcast, 6);

  err = esp_now_add_peer(&all);
  if (err != ESP_OK) {
    Serial.printf("Error %d registering the broadcast peer!\n", err);
    // return;
  }

  // Print MAC Address to Serial monitor
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // and save it to our player record
  WiFi.macAddress(me.node);

  // Open up our nvram namespace
  if (!prefs.begin(GM_NVM_KEY, false)) {
    Serial.println("Failed to open preferences!?");
  }

  // Fetch our own player info
  memset(name, 0, sizeof(name));
  if (!prefs.getString("tag", name, GM_PLAYER_TAG_LEN) || strlen(name) == 0) {
    Serial.println("Player tag not found in NVRAM, setting default");

    snprintf(name, GM_PLAYER_TAG_LEN, "Player%d", random(1, 99));
    setPlayerName(name);

    if (!prefs.putString("tag", name)) {
      Serial.printf("Failed to save player tag '%s' in NVRAM\n", name);
    }
  }
  
  Serial.printf("Player tag set: '%s'\n", getPlayerName());
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

  for (int i = 0; i < MAX_CLIENTS; i++) {

    if (!clients[i].inUse) {

      // Try to create a new queue; if that fails, bail out
      clients[i].handle = xQueueCreate(maxDepth, maxDepth * sizeof(gm_packet_t));
      if (clients[i].handle == 0) {
        Serial.println("ERROR: failed to create network queue!!");
        return -1;
      }

      // We're good!
      clients[i].inUse = true;
      clients[i].numFilters = 0;
      clients[i].numReceived = 0;
      numClients++;
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
  if (VALID(qId) && clients[qId].inUse) {
    clients[qId].inUse = false;
    vQueueDelete(clients[qId].handle);
    clients[qId].handle = NULL;
    numClients--;
    Serial.printf("--> Network client %d queue destroyed\n", qId);
  }
}

/*
 *  Add a filter for types to receive on this queue.  Returns -1 if no
 *  slots or the qId is invalid; returns >= 0 if added successfully.
 */
int NetworkTask::addFilter(int qId, uint8_t code) {
  if (!VALID(qId) || !clients[qId].inUse) return -1;

  for (int i = 0; i < MAX_FILTERS; i++) {
    if (clients[qId].filters[i] == GM_INVALID) {
      clients[qId].filters[i] == code;
      clients[qId].numFilters++;
      Serial.printf("--> Added filter for type %d to queue %d (%d active)\n", code, qId, clients[qId].numFilters);
      return i;
    }
  }

  // No more slots!
  return -1;
}

/*
 *  Stop receiving certain packet types.
 */
int NetworkTask::dropFilter(int qId, uint8_t code) {
  if (!VALID(qId) || !clients[qId].inUse) return -1;

  for (int i = 0; i < MAX_FILTERS; i++) {
    if (clients[qId].filters[i] == code) {
      clients[qId].filters[i] = GM_INVALID;
      clients[qId].numFilters--;
      Serial.printf("--> Dropped filter for type %d from queue %d (%d active)\n", code, qId, clients[qId].numFilters);
      return i;
    }
  }

  // Didn't find it...
  return -1;
}

/*
 *  Send a network broadcast packet.
 */
int NetworkTask::sendAll(gm_packet_t pkt) {

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcast, (uint8_t *) &pkt, sizeof(gm_packet_t));
   
  if (result == ESP_OK) {
    Serial.println("Send (broadcast) confirmed");
    pktsSent++;
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    Serial.println("ESP-NOW not Init.");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    Serial.println("Invalid Argument");
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    Serial.println("Internal Error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    Serial.println("Peer not found.");
  }
  else
  {
    Serial.println("Unknown error");
  }
  
  return result;
}

/*
 *  Send a network packet to a specific player.
 */
int NetworkTask::sendTo(gm_player_t player, gm_packet_t pkt) {

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(player.node, (uint8_t *) &pkt, sizeof(gm_packet_t));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
    pktsSent++;
  }
  else {
    Serial.println("Sending error");
    // count failures
  }
  return result;
}

/*
 *  Callback for ALL packets received from ESP NOW.  This should be fast,
 *  but since we don't run any really high bandwidth apps (yet?) for now 
 *  we just dispatch() the packet directly, which isn't optimal but I think
 *  the dual-240MHz ESP32 can handle it!
 */
void NetworkTask::recvCallback(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  Serial.printf("Received from %02x:%02x:%02x:%02x:%02x:%02x (%d bytes)\n",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], data_len);

}

/*
 *  Examine the packet type and place it on the appropriate client queue if
 *  anyone is filtering for it.  Otherwise just drop it.
 */
void NetworkTask::dispatch() {
  pktsReceived++;
  pktsDropped++;
}

// debugging
void NetworkTask::dumpStats() {

  if (!initialized) {
    Serial.println("ESP NOW networking not initialized!");
  } else {
    Serial.printf("Wifi stats for %d active clients:\n", numClients);
    Serial.printf("%d packets sent, %d recvd, %d dropped\n", pktsSent, pktsReceived, pktsDropped);
  }
}

char *NetworkTask::getPlayerName() {
  return me.tag;
}

void NetworkTask::setPlayerName(char *name) {
  strncpy(me.tag, name, GM_PLAYER_TAG_LEN);
}

/*
 *  Construct and send an IFF_HELLO packet.  This lets other GM nodes
 *  in range learn about us (and us about them, as we receive them).
 */
void NetworkTask::sendHello() {
  gm_packet_t pkt;
  iff_packet_t hello;

  // Fill in the payload
  hello.type = IFF_HELLO;
  memcpy(hello.who, me.tag, GM_PLAYER_TAG_LEN);
  memcpy(hello.what, "IDLE", 5);  // hack... menuTask.getCurrentApp(); ?
  hello.timeSent = millis();

  // Fill in the GM wrapper
  memcpy(pkt.dstAddr, broadcast, ADDR_LEN);
  memcpy(pkt.srcAddr, me.node, ADDR_LEN);
  pkt.pktType = GM_IFF;
  pkt.length = sizeof(hello);
  memcpy(pkt.payload, &hello, pkt.length);

  // Ship it!
  sendAll(pkt);
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

  Serial.printf("Network task starting up on core %d\n", xPortGetCoreID());

  // set up the receive callback
  err = esp_now_register_recv_cb(recvCallback);
  if (err != ESP_OK) {
    Serial.printf("ERROR registering network receive callback!  %d\n", err);
  }

  // loop
  for (;;) {
    delay(5000);
    sendHello();
    delay(5000);
    dumpStats();
  }
}
