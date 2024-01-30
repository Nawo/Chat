#pragma once
#include "../Core/Common/Common.h"

static std::vector<char> buffer(1024 * 20);

void grabData(asio::ip::tcp::socket &socket);
void infoGetter();