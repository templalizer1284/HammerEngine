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
#define CFG_LOGIC "cfg.logic"
#define BASE_LEVELS "levels"
#define BASE_MEDIA "media"
#define BASE_SAVE "save"
#define TITLE "Hammer Engine"

// dev-only, to be removed once save/load is implemented
#define LOAD_FILE "aca.sav"

// i don't think anyone would want more res than this
#define MAX_MODELS 255
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

enum BUTTONS {
	NEW_GAME,
	LOAD_GAME,
	OPTIONS,
	QUIT
};

// fdecl
HE_DECL u8 h_HammerRun(void);
HE_DECL u8 h_WindowInit(void);
HE_DECL u8 h_EngineParseBase(void);
HE_DECL u8 h_EngineParseRoot(void);
HE_DECL u8 h_EngineParseLevel(const char *);

HE_DECL u8 h_HammerIntro(void); // TODO
HE_DECL u8 h_HammerMenuRun(void);
HE_DECL u8 h_HammerLevelRun(const char *);

HE_DECL h_Model h_EngineModelLoad(const char *);
HE_DECL u8 h_EngineModelDraw(h_Model *);

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

	h_Model hero,map;

	h_Model entities[MAX_MODELS];
	h_Model npcs[MAX_MODELS];
	
	u16 entities_count, npcs_count;

	// logic info
	
};

struct h_HammerMenu {
	Font button_font, text_font;
	char button_newgame[20];
	char button_loadgame[20];
	char button_options[20];
	char button_quit[20];
	char selector[10];
	Texture2D background_texture;
	u8 menu_switch;
};

struct h_EngineState {
	h_Window window;
	Camera camera;
	const u8 fps;
	h_Config config;
	char starting_level[255];
	h_Level *current_level;
	char *load_file;
	h_HammerMenu menu;
	bool debug;
};

// globals
static h_EngineState engine = { .window.title = TITLE,
				.camera = {0},
				.fps = 30,
				.config = {0},
				.starting_level = "",
				.current_level = NULL,
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

	switch(h_HammerMenuRun()) {

		case NEW_GAME:
			if(h_HammerLevelRun(engine.starting_level)) {
				LOG("Level running error.\n");
				return 1;
			};
		break;

		case LOAD_GAME:
			LOG("Load game not yet implemented.\n");
		break;

		case OPTIONS:
			LOG("Options not yet implemented.\n");
		break;

		case QUIT:
			return 0;
		break;
	
		default:
			LOG("Menu drawing error.\n");
			return 1;
		break;
	}
		
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

u8 h_HammerLevelRun(const char *level) {

	if(h_EngineParseLevel(level)) {
		LOG("Level parsing error.\n");
		return 1;
	}

	Model model = LoadModel("wotan/media/hero.glb");

	while(!WindowShouldClose()) {

		UpdateCamera(&engine.camera, CAMERA_THIRD_PERSON);
	
		BeginDrawing();
			ClearBackground(DARKBLUE);
			BeginMode3D(engine.camera);

			if(engine.debug) {
				DrawFPS(10.0f,10.0f);
				DrawGrid(10.0f, 1.0f);
			}

			// drawing models, hero, map and entities.
			DrawModel(model, (Vector3){0,0,0}, 1.0f, WHITE);

			// entities
			
			
			EndMode3D();
		EndDrawing();
	}

	return 0;
}

u8 h_EngineParseBase(void) {

	// first count the number of levels
	struct dirent *entry;
	struct stat statbuf;

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

		// if okay, attempts to read all folders
		else {
			while( (entry = readdir(dir)) != NULL ) {

				// skip . and ..
				if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

				char full_path[256];
				snprintf(full_path, sizeof(full_path),
				"%s%s%s%s%s", engine.config.base, SEP, BASE_LEVELS, SEP, entry->d_name);

				// check if file is actually a folder
				if( stat(full_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode) );
				else {
					LOG("File %s is not a folder, put only levels inside levels folder.\n", full_path);
					return 1;
				}
			}

			closedir(dir);
		}

		(void)snprintf(engine.config.root, sizeof(engine.config.root),
		"%s%s%s", engine.config.base, SEP, CFG_ROOT);

		(void)snprintf(engine.config.resources, sizeof(engine.config.resources),
		"%s%s%s", engine.config.base, SEP, BASE_MEDIA);

		(void)snprintf(engine.config.level, sizeof(engine.config.level),
		"%s%s%s", engine.config.base, SEP, BASE_LEVELS);

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

		else if(strcmp(tmp, "BACKGROUND") == 0) {
			ff;

			char full_path[256];
			(void)snprintf(full_path, sizeof(full_path),
			"%s%s%s%s%s", engine.config.base, SEP, BASE_MEDIA, SEP, tmp);
				
			if(access(full_path, F_OK) == 0) {
				engine.menu.background_texture = LoadTexture(full_path);
			}

			else {
				perror("Cannot open menu background image.");
				return 1;
			}

			continue;
		}

		if(strcmp(tmp, "FONT") == 0) {
			ff;
			char path[255];
			(void)snprintf(path, sizeof(path),
			"%s%s%s%s%s", engine.config.base, SEP, BASE_MEDIA, SEP, tmp);
			
			if(access(path, F_OK) == 0) {
				engine.menu.button_font = LoadFontEx(path, 32, 0, 250);
				engine.menu.text_font = LoadFontEx(path, 12, 0, 20);
			}

			else {
				LOG("Could not load font file %s.", tmp);
				return 1;
			}

			continue;

		}

		else if(strcmp(tmp, "SELECTOR") == 0) {
			ff;
			(void)snprintf(engine.menu.selector, sizeof(engine.menu.selector),
			"%1s", tmp);

			continue;
		}

		if(strcmp(tmp, "NEW_GAME_START") == 0) {
			ff;

			char path[255];
			(void)snprintf(path, sizeof(path),
			"%s%s%s", engine.config.level, SEP, tmp);
			
			if(access(path, F_OK) == 0) {
				(void)snprintf(engine.starting_level, sizeof(engine.starting_level),
				"%s", path);
			}

			else {
				LOG("Level %s doesn't exist.\n", tmp);
				return 1;
			}

			continue;
		}

		else {
			LOG("Syntax error in cfg.root, instruction '%s' not recognized.\n", tmp);
			return 1;
		}
	}
	
	fclose(fp);
	return 0;
}

u8 h_EngineParseLevel(const char *path) {

	char resources[255], logic[255];

	(void)snprintf(resources, sizeof(resources),
	"%s%s%s", path, SEP, CFG_RESOURCES);

	(void)snprintf(logic, sizeof(logic),
	"%s%s%s", path, SEP, CFG_LOGIC);

	FILE *fp = NULL; char tmp[64];
	#define ff fscanf(fp, "%63s", tmp)

	static h_Level level;

	if(access(resources, F_OK) == 0 && access(logic, F_OK) == 0) {
		if( (fp = fopen(resources, "r")) == NULL ) {
			LOG("Failed to open resources config for the level.\n");
			return 1;
		}

		else {
			while(ff == 1) {
				if(strcmp(tmp, "HERO") == 0) {
					ff;
					char p[255];
					(void)snprintf(p, sizeof(p),
					"%s%s%s", engine.config.resources, SEP, tmp);
					LOG("%s", p);
					
					if(access(p, F_OK) == 0) {
						// load hero
						level.hero = h_EngineModelLoad(p);
					}

					else {
						LOG("Hero model %s cannot be loaded.\n", tmp);
						return 1;
					}

					continue;
				}

				else if(strcmp(tmp, "MAP") == 0) {
					ff;

					char p[255];
					(void)snprintf(p, sizeof(p),
					"%s%s%s", engine.config.resources, SEP, tmp);
					
					if(access(p, F_OK) == 0) {
						// load map
						level.map = h_EngineModelLoad(p);
					}

					else {
						LOG("Map model %s cannot be loaded.\n", tmp);
						return 1;
					}

					continue;
				}

				else {
					LOG("Syntax error in resources config, %s unrecognized.\n", tmp);
					return 1;
				}
			}
		}

		if( (fp = fopen(logic, "r")) == NULL ) {
			LOG("Failed to open logic config for the level.\n");
			return 1;
		}

		else {
			while(ff == 1) {
				
			}
		}
	}

	else {
		LOG("Cannot open config file for resources or logic.\n");
		return 1;
	}

	engine.current_level = &level;
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
						//engine.current_level = &engine.levels[atoi(tmp)];
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

	#define AR_SIZE 20
	char buttons[][AR_SIZE] = { {"New Game"}, {"Load Game"}, {"Options"}, {"Quit"} };
	auto num_buttons = (u8)sizeof(buttons) / AR_SIZE;
	
	char text[20];

	while(!WindowShouldClose()) {
		BeginDrawing();

			if(engine.debug) DrawFPS(10.0f, 10.0f);

			// drawing background image
			DrawTexture(engine.menu.background_texture, 0, 0, DARKBLUE);

			// drawing buttons, newgame,loadgame,options and quit
			for(auto i = 0; i < num_buttons; i++) {

				if(i == engine.menu.menu_switch) {
					(void)memset(text, 0, strlen(text));
					(void)snprintf(text, sizeof(text),
					"%s %s", buttons[i], engine.menu.selector);
				}

				else {
					(void)memset(text, 0, strlen(text));
					(void)snprintf(text, sizeof(text), "%s", buttons[i]);
				}
				
				DrawTextEx(engine.menu.button_font, text,
				(Vector2){(float)engine.window.width / 4,
				((float)engine.window.height / 2) + (float)i * 50.0f},
				(float)engine.menu.button_font.baseSize, 2, MAROON);
			}

			// check for keyboard input, to change selector
			if(IsKeyPressed(KEY_DOWN)) {
				engine.menu.menu_switch++;
			}
			
			else if(IsKeyPressed(KEY_UP)) {
				engine.menu.menu_switch--;
			}

			if(IsKeyDown(KEY_ENTER)) {
				break;
			}
			
		EndDrawing();
	}

	UnloadFont(engine.menu.button_font);

	return engine.menu.menu_switch;
}

h_Model h_EngineModelLoad(const char *path) {

	h_Model model;
	model.model = LoadModel(path);
	model.animation = LoadModelAnimations(path, &model.animCount);

	return model;
}

u8 h_EngineModelDraw(h_Model *model) {
	DrawModel(model->model, model->position, 1.0f, WHITE);
	return 0;
}

#endif // HAMMER_ENGINE_IMPLEMENTATION end

#endif // HAMMER_ENGINE_H end
