#pragma once

/**
 * @file client.h
 * @brief Client definition and related functions
 *
 * @author Valérian Rousset
 */

#include <stddef.h> // for size_t

#include "error.h"     // for error_code type
#include "node.h"      // in week 5
#include "node_list.h" // weeks 6 to 10
#include "args.h"      // weeks 10 and after
#include "ring.h"      // weeks 11 and after

/**
 * @brief client state
 */

typedef struct{
	const char* name;
	ring_t server;
	int socket;
	args_t* args;
	} client_t;

/**
 * @brief client_init function arguments.
 *        To be defined in week 05 and THEN UPDATED in week 10.
 */
typedef struct{
  client_t* client;
	char*** argv;
	size_t required;
	size_t optionnal;
	size_t size_args;
} client_init_args_t;

/**
 * @brief does all the work to be done at the end of life of a client
 * @param client the client to end
 */
void client_end(client_t *client);

/**
 * @brief does all the work to be done at the beginning of life of a client
 * @param client the client to initialize
 * @return some error code, if any.
 */
error_code client_init(client_init_args_t);
