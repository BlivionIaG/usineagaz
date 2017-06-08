#ifndef NOGUI_H
#define NOGUI_H

#include <unistd.h>
#include "bot.h"
#include "map.h"

#define NOGUI_BOT 'O'

void nogui_bot_on_map_draw(bot *in, map *map);

#endif /* NOHUI_H */