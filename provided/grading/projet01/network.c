#include <stdio.h>

#include "error.h"
#include "hashtable.h"
#include "client.h"
#include "config.h"
#include "system.h"
#include "network.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>



/**
 * @brief get a value from the network
 * @param client client to use
 * @param key key of what we want to find value
 * @param value value to write to, of size MAX_MSG_ELEM_SIZE + 1
 * @return an error code
 */
error_code network_get(client_t client, pps_key_t key, pps_value_t *value)
{
    M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
    int error_not_found = 0;
    for(int i=0; i<client.server.size; i++) {
        int size_to_send = strlen(key);
        error_code error = send_packet(client.socket, key, size_to_send, client.server.nodes[i]);
        if(error!=ERR_NONE) return error;
        /* CORRECTEUR: If you cannot send the message to the first server,
         * continue sending to the next. This should not exit.
         * [-0.5 correctness]
         */

        char* in_msg = malloc(MAX_MSG_ELEM_SIZE);
        ssize_t in_msg_len = recv(client.socket, in_msg, MAX_MSG_ELEM_SIZE, 0);
        /* CORRECTEUR: Consider using `recvfrom` and verify that the message
         * indeed originates from the server to which you sent the request.
         * [-0.5 robustness]
         */


        if (in_msg_len != -1) {
            if (in_msg_len==1 && in_msg[0]=='\0'){
              error_not_found++;
            }else{

            *value = in_msg;
            /* CORRECTEUR: `in_msg` is likely not properly null-terminated.
             * Consider either using `calloc` (instead of `malloc`), or
             * `memset(in_msg, 0, MAX_MSG_ELEM_SIZE)`, or at least set the last
             * byte to '\0' manually. [-1 correctness]
             */
            return ERR_NONE;
          }
        }
    }
    if(error_not_found==0) return ERR_NETWORK; else return ERR_NOT_FOUND;
    /* CORRECTEUR: `in_msg` is never freed [-1 memory leak(s)] */
}


/**
 * @brief put a value in the network
 * @param client client to use
 * @param key key to retrieve value later
 * @param value value to add
 * @return an error code
 */
error_code network_put(client_t client, pps_key_t key, pps_value_t value)
{

M_EXIT_IF_TOO_LONG(key, MAX_MSG_ELEM_SIZE, key.name);
M_EXIT_IF_TOO_LONG(value, MAX_MSG_ELEM_SIZE, value.name);
if(key==NULL||value==NULL)return ERR_BAD_PARAMETER;

    int errors = 0;
    for(int i= 0; i<client.server.size; i++) {

      char* request = format_put_request(key, value);
      size_t request_len = strlen(key)+strlen(value)+1;
      error_code error_send = send_packet(client.socket, request, request_len, client.server.nodes[i]);
      free(request);
      int error_receive = recv(client.socket, NULL,0,0);
      /* CORRECTEUR: Consider using `recvfrom` and verify that the message
       * indeed originates from the server to which you sent the request.
       * [-0.5 robustness]
       */
      if(error_send!=ERR_NONE ||error_receive==-1) errors++;
}
    if(errors>=1) {
        return ERR_NETWORK;
    } else {
        return ERR_NONE;
    }
}


/**
 * @brief send a packet to a node
 * @param message the packet to send
 * @param socket socket to use
 * @param size size of the packet
 * @param node the node we want to send the packet to
 * @return an error code
 */
error_code send_packet(int socket, const char* message, size_t size, node_t node){

if(message==NULL || size<0) return ERR_BAD_PARAMETER;

  int error = sendto(socket, message, size, 0,
             (struct sockaddr *) &node, sizeof(node));


  if(error==-1) return ERR_NETWORK;

  return ERR_NONE;
  }



  /**
   * @brief format a put request with the caracter \0 correctly placed
   * @param value value to format
   * @param key key to format
   * @return the request correctly formated
   */
  char* format_put_request(pps_key_t key, pps_value_t value){
    char *result = calloc(strlen(key)+strlen(value)+1, sizeof(char));
    for(int i=0; i<strlen(key); i++){
        result[i] = key[i];
      }
      result[strlen(key)]='\0';
      for(int i=strlen(key)+1; i<strlen(key)+1+strlen(value); i++){
      result[i] = value[i-strlen(key)-1];
      }
      /* CORRECTEUR: Consider using `memcpy` rather than manually copying bytes
       * around...
       */

      return result;

  }

  /**
   * @brief parse a put request
   * @param in_msg the packet to parse
   * @param length length of request
   * @param key pointer to the key
   * @param value pointer to the value
   * @return an error code
   */
error_code parse_put_request(char* in_msg, size_t length, char* key, char* value){
    char* ret = memchr(in_msg, '\0', length);
    size_t size_key = strlen(in_msg);
    for(int i =0; i<size_key;i++){
     key[i] = in_msg[i];
   }
   key[size_key] = '\0';
   size_t size_value = length - size_key;

 for(int i=0; i<size_value; i++){

   value[i] = ret[i+1];
}

 value[size_value] = '\0';
return ERR_NONE;
/* CORRECTEUR: Consider using `memcpy` instead of copying each byte individually
 * and by hand.
 */

}
/* CORRECTEUR: Overall very inconsistent indentation [-1 style] */