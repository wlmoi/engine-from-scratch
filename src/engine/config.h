#pragma once

#include <stdbool.h>
#include "input.h"
#include "types.h"

typedef struct config {
	u8 keybinds[5];
	bool is_hard_mode;
} Config_State;

void config_init(void);
void config_key_bind(Input_Key key, const char *key_name);

