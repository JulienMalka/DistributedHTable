#include "node.h"
#include "error.h"
#include "system.h"

error_code node_init(node_t *node, const char *ip, uint16_t port, size_t _unused node_id)
{
    return get_server_addr(ip, port, node);
}

void node_end(node_t *node)
{
    //Void for now
}
