#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"

error_code network_get(client_t client, pps_key_t key, pps_value_t *value){

    pps_key_t request = key;

    // Prepare outgoing message with htonl.
    unsigned int out_msg;
    out_msg = htonl(request);

    // Send message.
    if (sendto(client.socket, &out_msg, sizeof(out_msg), 0,
               (struct sockaddr *) &client.server, sizeof(client.server)) == -1)
      return ERR_NETWORK;

    // Receive response.
    pps_value_t in_msg;
    ssize_t in_msg_len = recvfrom(client.socket, &in_msg, sizeof(in_msg), 0, (struct sockaddr *) &client.server,(socklen_t *) sizeof(client.server));
    if (in_msg_len == sizeof(in_msg)) { 
		// Valid response.
        // Parse response with ntohl.
        pps_value_t response;
        response = ntohl(in_msg);
		*value = response;
	}	
    return ERR_NONE;
}

error_code network_put(client_t client, pps_key_t key, pps_value_t value){
	
	unsigned char packet[5];
	
	// Prepare value with htonl.
    unsigned int value_formated;
    value_formated = htonl(value);
	
	//@TODO Overlook formatting outgoing message
	packet[0] = key;
	packet[1] = value_formated >> 24;
	packet[2] = value_formated >> 16;
	packet[3] = value_formated >> 8;
	packet[4] = value_formated;
	
	if (sendto(client.socket, packet, 5, 0,
          (struct sockaddr *) &client.server, sizeof(client.server)) == -1)
	return ERR_NETWORK;
    
	return ERR_NONE;
}
