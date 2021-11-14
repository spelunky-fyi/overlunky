#pragma once

// Observed ports:
// 11: to all 32 edge servers/vultr/ovh vps's (systat Active Users equivalent?)
// 57785: LAN communication
// 57529: broadcast
// 41262: to chosen edge server
// 12659: to chosen edge server, game stuff

// filter messages by port, or specify port -1 for all messages
void hook_network(int32_t port, bool dump_send, bool dump_receive);
