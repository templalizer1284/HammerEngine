#ifndef HAMMER_ENGINE_H
#define HAMMER_ENGINE_H

#ifdef HAMMER_ENGINE_IMPLEMENTATION

// Core C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// POSIX
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>

// Raylib
#include <raylib.h>
#include <raymath.h>

// macros
#define HAMMERCFG "hammercfg"
#define CFG_LEVEL "cfg.level"
#define CFG_ROOT "cfg.root"
#define CFG_RESOURCES "cfg.resources"
#define BASE_LEVELS "levels"
#define BASE_MEDIA "media"
#define BASE_SAVE "save"
#define TITLE "Hammer Engine"

// dev-only, to be removed once save/load is implemented
#define LOAD_FILE "aca.sav"

// i don't think anyone would want more res than this
#define MAX_MODELS 1024
#define MAX_LEVELS 32

#define h_Stringify(x) #x

#define SEP "/"

#if defined(_WIN32) || defined(_WIN64)
#undef SEP
#define SEP "\\"
#endif

#ifdef DEBUG
	#define LOG(fmt, ...) printf(fmt __VA_OPT__(,) __VA_ARGS__)
#else
	#define LOG(...) ((void)0)
#endif

// i will not write this crap everytime
typedef uint8_t u8;
typedef uint16_t u16;
#define HE_DECL static inline

// ddecl
typedef struct h_Window h_Window;
typedef struct h_Model h_Model;
typedef struct h_Entity h_Entity;
typedef struct h_Level h_Level;
typedef struct h_Config h_Config;
typedef struct h_HammerMenu h_HammerMenu;

typedef struct h_EngineState h_EngineState;

enum h_MENU_SWITCH {
	NEW_GAME,
	LOAD_GAME,
	OPTIONS,
	QUIT
};

// fdecl
HE_DECL u8 h_HammerRun(void);
HE_DECL u8 h_WindowInit(void);
HE_DECL u8 h_Loop(void);
HE_DECL u8 h_EngineParseBase(void);
HE_DECL u8 h_EngineParseRoot(void);
HE_DECL u8 h_EngineParseLevel(void);

HE_DECL u8 h_HammerIntro(void); // TODO
HE_DECL u8 h_HammerMenuRun(void);

HE_DECL u8 h_EngineLoadGame(const char *); // TODO
HE_DECL u8 h_EngineSaveGame(const char *); // TODO

// ddef
struct h_Window {
	u16 width;
	u16 height;
	char *title;
};

struct h_Model {
	Model model;

	ModelAnimation *restrict animation;
	int animCount;
	int currentAnim;
	bool animate;

	BoundingBox box, transformedBox;
	Vector3 center;

	Vector3 position;
	float angle;
	bool render;

	float scale;
	Color tint;
};

struct h_Config {
	char base[32];
	char root[32];
	char level[32];
	char resources[32];
	char save[32];
};

struct h_Level {
	h_Model models[MAX_MODELS];
	u16 model_count;
	char path[256];
};

struct h_HammerMenu {
	char background_path[64];
	char button_path[64], button_select_path[64];
	Texture2D background_texture;
	Texture2D button_texture, button_select_texture;
};

struct h_EngineState {
	h_Window window;
	Camera camera;
	const u8 fps;
	h_Config config;
	h_Level levels[MAX_LEVELS];
	h_Level *current_level;
	u8 levels_count;
	char *load_file;
	h_HammerMenu menu;
	bool debug;
};

// globals
static h_EngineState engine = { .window.title = TITLE,
				.camera = {0},
				.fps = 30,
				.config = {0},
				.levels = { {0} },
				.current_level = NULL,
				.levels_count = 0,
				.debug = false,
				.menu = { 0 },
				.load_file = "" };

// fdef
u8 h_HammerRun(void) {

	LOG("Hammer Engine Running, BattleCruiser operational.\n");

	if(h_WindowInit()) {
		perror("Window Initialization failed. Aborting.");
		return 1;
	}

	if(h_EngineParseBase()) {
		perror("Engine Base folder parsing failed. Aborting.");
		return 1;
	}

	if(h_EngineParseRoot()) {
		LOG("Error occured while parsing root cfg.\n");
		return 1;
	}

	if(h_HammerMenuRun()) {
		LOG("Menu drawing error.\n");
		return 1;
	}

	/*if(h_Loop()) {
		LOG("Main loop error.\n");
		return 1;
	}*/
		
	return 0;
}

u8 h_WindowInit(void) {
	if(access(HAMMERCFG, F_OK) == 0) {
		FILE *fp = fopen(HAMMERCFG, "r");
		char tmp[64];

		if(fp == NULL) {
			LOG("HAMMERCFG failed to open, using default values.\n");
			engine.window.width = 800;
			engine.window.height = 600;
		}

		#define ff fscanf(fp, "%63s", tmp)
		while(ff == 1) {
			if(strcmp(tmp, "width") == 0) {
				ff;
				engine.window.width = atoi(tmp);
				continue;
			}

			else if(strcmp(tmp, "height") == 0) {
				ff;
				engine.window.height = atoi(tmp);
				continue;
			}

			if(strcmp(tmp, "base") == 0) {
				ff;
				(void)snprintf(engine.config.base, sizeof(engine.config.base),
				"%s", tmp);
				continue;
			}

			else {
				perror("Syntax error in HAMMERCFG.");
				return 1;
			}
		}

		fclose(fp);
	}

	else {
		LOG("HAMMERCFG failed to open, using default settings.\n");
		engine.window.width = 800;
		engine.window.height = 600;
		return 1;
	}

	// Raylib knows to spit out bunch of info.
	SetTraceLogLevel(LOG_WARNING);

	// Pop-up the window
	InitWindow(engine.window.width, engine.window.height, engine.window.title);

	SetTargetFPS(engine.fps);

	// Setting up camera SH-like style.
	engine.camera.position = (Vector3){ 0.0f, 5.0f, -7.0f };
	engine.camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	engine.camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	engine.camera.fovy = 45.0f;
	engine.camera.projection = CAMERA_PERSPECTIVE;

	return 0;
}

u8 h_Loop(void) {
	while(!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(DARKBLUE);
			BeginMode3D(engine.camera);

			if(engine.debug) {
				DrawFPS(10,10);
				DrawGrid(100,100);
			}
			
			EndMode3D();
		EndDrawing();
	}

	return 0;
}

u8 h_EngineParseBase(void) {

	// first count the number of levels
	struct dirent *entry;
	struct stat statbuf;
	u8 folder_count = 0;

	FILE *fp = NULL;

	// checking for base folder correctness
	if(access(engine.config.base, F_OK) == 0) {

		// check i posix's opendir can access base levels folder
		char levels_folder[256] = { 0 };
		(void)snprintf(levels_folder, sizeof(levels_folder),
			"%s%s%s", engine.config.base, SEP, BASE_LEVELS);
		
		DIR *dir = opendir(levels_folder);
		if(dir == NULL) {
			LOG("Unable to open base folder.\n");
			return 1;
		}

		// if okay, attempts to read all folders, this is for the sake of keeping
		// how many levels are there.
		else {
			while( (entry = readdir(dir)) != NULL ) {

				// skip . and ..
				if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

				char full_path[256];
				snprintf(full_path, sizeof(full_path),
				"%s%s%s%s%s", engine.config.base, SEP, BASE_LEVELS, SEP, entry->d_name);

				// check if file is actually a folder
				if( stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode) ) {

					// while accumulating needed info, we can extract path for levels.
					(void)snprintf(engine.levels[folder_count].path,
					sizeof(engine.levels[folder_count].path),
					"%s", full_path);
					
					folder_count++;
				}
			}

			closedir(dir);
		}

		engine.levels_count = folder_count;
	
		(void)snprintf(engine.config.root, sizeof(engine.config.root),
		"%s%s%s", engine.config.base, SEP, CFG_ROOT);

		(void)snprintf(engine.config.resources, sizeof(engine.config.resources),
		"%s%s%s", engine.config.base, SEP, CFG_RESOURCES);

		if(access(engine.config.root, F_OK) != 0) {
			LOG("Base root config not found.\n");
			return 1;
		}
	}

	else {
		perror("Base folder provided in HAMMERCFG cannot be opened. Aborting.");
		return 1;
	}

	fclose(fp);
	return 0;
}

u8 h_EngineParseRoot(void) {

	FILE *fp = fopen(engine.config.root, "r");

	char tmp[64];
	#define ff fscanf(fp, "%63s", tmp)

	while(ff == 1) {
		if(strcmp(tmp, "DEBUG") == 0) {
			engine.debug = true;
			continue;
		}

		else if(strcmp(tmp, "MENU") == 0) {
			ff;
			if(strcmp(tmp, "BACKGROUND") == 0) {
				ff;

				char full_path[256];
				(void)snprintf(full_path, sizeof(full_path),
				"%s%s%s%s%s", engine.config.base, SEP, BASE_MEDIA, SEP, tmp);
				
				if(access(full_path, F_OK) == 0) {
					(void)snprintf(engine.menu.background_path,
					sizeof(engine.menu.background_path),
					"%s", full_path);
				}

				else {
					perror("Cannot open menu background image.");
					return 1;
				}
			}

			continue;
		}

		if(strcmp(tmp, "BUTTON") == 0) {
			ff;
			if(strcmp(tmp, "TEXTURE") == 0) {
				ff;
				if(strcmp(tmp, "SELECT") == 0) {
					ff;
					if(access(tmp, F_OK) == 0) {
						(void)snprintf(engine.menu.button_select_path,
						sizeof(engine.menu.button_select_path),
						"%s%s%s%s%s",
						engine.config.base, SEP,
						BASE_MEDIA, SEP, tmp);
					}

					else {
						LOG("Button texture not found.\n");
						return 1;
					}
				}

				else {
					ff;
					if(access(tmp, F_OK) == 0) {
						
					}

					else {
						LOG("Button texture not found.\n");
						return 1;
					}
				}

				continue;
			}

			else {
				LOG("Syntax error in cfg.root regarding buttons.\n");
				return 1;
			}
		}

		else {
			LOG("Syntax error in cfg.root, instruction '%s' not recognized.\n", tmp);
			return 1;
		}
	}
	
	fclose(fp);
	return 0;
}

u8 h_EngineParseLevel(void) {
	return 0;
}

u8 h_EngineLoadGame(const char *file) {

	FILE *fp = NULL;

	if(access(file, F_OK) == 0) {

		fp = fopen(file, "r");
		if(fp == NULL) {
			perror("Failed to open file.");
			return 1;
		}

		else {
			char tmp[64];
			while(fscanf(fp, "%63s", tmp) == 1) {
				if(strcmp(tmp, "LEVEL") == 0) {
					fscanf(fp, "%63s", tmp);
					if(atoi(tmp) == 0) {
						LOG("Loading saved file failed.\n");
						return 1;
					}

					else {
						engine.current_level = &engine.levels[atoi(tmp)];
					}
				}

				else {
					perror("Syntax error while reading save file.");
					return 1;
				}
			}
		}
	}

	else {
		LOG("Cannot not open file '%s'\n", file);
		return 1;
	}

	fclose(fp);
	return 0;
}

u8 h_HammerMenuRun(void) {

	// loading menu resources
	engine.menu.background_texture = LoadTexture(engine.menu.background_path);

	while(!WindowShouldClose()) {
		BeginDrawing();
			DrawTexture(engine.menu.background_texture, 0,0, WHITE);
		EndDrawing();
	}

	return 0;
}

#endif // HAMMER_ENGINE_IMPLEMENTATION end

#endif // HAMMER_ENGINE_H end
