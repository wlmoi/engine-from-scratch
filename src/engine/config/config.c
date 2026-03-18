#include "../global.h"
#include "../io.h"
#include "../util.h"
#include "../input.h"
#include "../config.h"

#include <ctype.h>

static const char *CONFIG_DEFAULT =
	"[controls]\n"
	"left = A\n"
	"right = D\n"
	"up = W\n"
	"shoot = E\n"
	"down = S\n"
	"escape = Escape\n"
	"\n"
	"[gameplay]\n"
	"mode = easy\n"
	"\n";

static char tmp_buffer[20] = {0};

static void config_bind_defaults(void) {
	global.config.keybinds[INPUT_KEY_LEFT] = SDL_SCANCODE_A;
	global.config.keybinds[INPUT_KEY_RIGHT] = SDL_SCANCODE_D;
	global.config.keybinds[INPUT_KEY_UP] = SDL_SCANCODE_W;
	global.config.keybinds[INPUT_KEY_SHOOT] = SDL_SCANCODE_E;
	global.config.keybinds[INPUT_KEY_ESCAPE] = SDL_SCANCODE_ESCAPE;
	global.config.is_hard_mode = false;
}

static char *config_try_get_value(const char *config_buffer, const char *value) {
	char *line = strstr(config_buffer, value);
	if (!line)
		return NULL;

	usize len = strlen(line);
	char *end = line + len;

	char *curr = line;
	char *tmp_ptr = &tmp_buffer[0];
	// Move pointer to '='.
	while (*curr != '=' && curr != end)
		++curr;
	if (curr == end)
		return NULL;

	// Consume '='.
	++curr;
	// Consume any spaces.
	while (*curr == ' ')
		++curr;
	// Get characters until end of line.
	while (*curr != '\n' && *curr != 0 && curr != end)
		*tmp_ptr++ = *curr++;

	*tmp_ptr = 0;

	// Trim trailing whitespace and CR from Windows line endings.
	while (tmp_ptr > tmp_buffer && (*(tmp_ptr - 1) == ' ' || *(tmp_ptr - 1) == '\t' || *(tmp_ptr - 1) == '\r')) {
		*(tmp_ptr - 1) = 0;
		tmp_ptr--;
	}

	return tmp_buffer;
}

static char *config_get_value(const char *config_buffer, const char *value) {
	char *result = config_try_get_value(config_buffer, value);
	if (!result)
		ERROR_EXIT("Could not find config value: %s. "
				"Try deleting config.ini and restarting.\n", value);

	return result;
}

static bool string_equals_ignore_case(const char *a, const char *b) {
	while (*a && *b) {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
			return false;
		}
		++a;
		++b;
	}

	return *a == 0 && *b == 0;
}

static void load_controls(const char *config_buffer) {
	config_key_bind(INPUT_KEY_LEFT, config_get_value(config_buffer, "left"));
	config_key_bind(INPUT_KEY_RIGHT, config_get_value(config_buffer, "right"));
	config_key_bind(INPUT_KEY_UP, config_get_value(config_buffer, "up"));
	config_key_bind(INPUT_KEY_SHOOT, config_get_value(config_buffer, "shoot"));
	config_key_bind(INPUT_KEY_ESCAPE, config_get_value(config_buffer, "escape"));
}

static void load_gameplay(const char *config_buffer) {
	char *mode = config_try_get_value(config_buffer, "mode");
	if (!mode) {
		mode = config_try_get_value(config_buffer, "hardmode");
		if (mode) {
			global.config.is_hard_mode = string_equals_ignore_case(mode, "1") || string_equals_ignore_case(mode, "true") || string_equals_ignore_case(mode, "hard");
		}
		return;
	}

	if (string_equals_ignore_case(mode, "hard")) {
		global.config.is_hard_mode = true;
	} else if (string_equals_ignore_case(mode, "easy")) {
		global.config.is_hard_mode = false;
	}
}

static int config_load(void) {
	File file_config = io_file_read("./config.ini");
	if (!file_config.is_valid)
		return 1;

	load_controls(file_config.data);
	load_gameplay(file_config.data);

	free(file_config.data);

	return 0;
}

void config_init(void) {
	config_bind_defaults();

	if (config_load() == 0)
		return;

	io_file_write((void*)CONFIG_DEFAULT, strlen(CONFIG_DEFAULT), "./config.ini");

	if (config_load() != 0)
		ERROR_EXIT("Could not create or load config file.\n");
}

void config_key_bind(Input_Key key, const char *key_name) {
	SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
	if (scan_code == SDL_SCANCODE_UNKNOWN) {
		SDL_Keycode key_code = SDL_GetKeyFromName(key_name);
		if (key_code != SDLK_UNKNOWN) {
			scan_code = SDL_GetScancodeFromKey(key_code);
		}
	}

	if (scan_code == SDL_SCANCODE_UNKNOWN) {
		ERROR_RETURN(, "Invalid scan code when binding key: %s\n", key_name);
	}

	global.config.keybinds[key] = scan_code;
}
