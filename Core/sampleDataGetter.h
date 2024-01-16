#pragma once
#include "net_common.h"

static std::vector<char> buffer(1024 * 20);

void grabData(asio::ip::tcp::socket &socket);
void infoGetter();