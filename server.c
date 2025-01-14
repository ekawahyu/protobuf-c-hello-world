/*
 * Copyright (c) 2013, Artem Titoulenko
 * Copyright (c) 2022, Ekawahyu Susilo
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ping.pb-c.h"
#include <protobuf-c-rpc/protobuf-c-rpc.h>

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

// this will handle all rpc calls using the reply_to_ping service
void ping__reply_to_ping(PingService_Service * service,
 const Ping * input,
 Ping_Closure closure,
 void * closure_data) {
  printf("input->message: %s\n", input->message);

  // init the message
  Ping ping_response = PING__INIT;
  ping_response.message = strdup("hi");

  // respond with the ping_response buffer
  closure(&ping_response, closure_data);
}

static PingService_Service ping_service = PING_SERVICE__INIT(ping__);

int main(int argc, char **argv) {
  ProtobufC_RPC_Server * server;
  const char *name = NULL;
  unsigned i;

  for (i = 1; i < (unsigned) argc; i++) {
    if (starts_with(argv[i], "--port=")) {
      name = strchr (argv[i], '=') + 1;
    }
  }

  if (name == NULL) {
    fprintf(stderr, "missing --tcp=HOST:PORT");
    return 1;
  }

  server = protobuf_c_rpc_server_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, (ProtobufCService *) &ping_service, NULL);

  for (;;)
    protobuf_c_rpc_dispatch_run (protobuf_c_rpc_dispatch_default ());

  protobuf_c_rpc_server_destroy(server, 1);
  return 0;
}
