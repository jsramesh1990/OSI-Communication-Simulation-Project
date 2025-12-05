#include <stdio.h>
#include <string.h>
#include "layers.h"


// Utility wrapper to print transitions
void log_layer(const char *layer, const char *data) {
printf("[%s] Data: %s\n", layer, data);
}


// -------------------- TRANSMIT SIDE --------------------
void Application_Send(const char *msg) {
log_layer("Application", msg);
char buffer[256]; sprintf(buffer, "APP{%s}", msg);
Presentation_Send(buffer);
}


void Presentation_Send(const char *data) {
log_layer("Presentation", data);
char buffer[256]; sprintf(buffer, "PRS{%s}", data);
Session_Send(buffer);
}


void Session_Send(const char *data) {
log_layer("Session", data);
char buffer[256]; sprintf(buffer, "SES{%s}", data);
Transport_Send(buffer);
}


void Transport_Send(const char *data) {
log_layer("Transport", data);
char buffer[256]; sprintf(buffer, "TCP{%s}", data);
Network_Send(buffer);
}


void Network_Send(const char *data) {
log_layer("Network", data);
char buffer[256]; sprintf(buffer, "IP{%s}", data);
DataLink_Send(buffer);
}


void DataLink_Send(const char *data) {
log_layer("DataLink", data);
char buffer[256]; sprintf(buffer, "MAC{%s}", data);
Physical_Send(buffer);
}


void Physical_Send(const char *data) {
log_layer("Physical", data);
printf("\n=== SIGNAL SENT OVER WIRE ===\n\n");


// Trigger receive chain
Physical_Receive(data);
}

// -------------------- RECEIVE SIDE --------------------
void Physical_Receive(const char *data) {
log_layer("Physical (RX)", data);
DataLink_Receive(data + 4); // remove MAC{}
}


void DataLink_Receive(const char *data) {
log_layer("DataLink (RX)", data);
Network_Receive(data + 3); // remove IP{}
}


void Network_Receive(const char *data) {
log_layer("Network (RX)", data);
Transport_Receive(data + 4); // remove TCP{}
}


void Transport_Receive(const char *data) {
log_layer("Transport (RX)", data);
Session_Receive(data + 4); // remove SES{}
}


void Session_Receive(const char *data) {
log_layer("Session (RX)", data);
Presentation_Receive(data + 4);
}


void Presentation_Receive(const char *data) {
log_layer("Presentation (RX)", data);
Application_Receive(data + 4);
}


void Application_Receive(const char *data) {
log_layer("Application (RX)", data);
printf("\n=== FINAL MESSAGE DELIVERED TO APPLICATION ===\n");
}

