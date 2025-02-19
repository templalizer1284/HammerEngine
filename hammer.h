#ifndef HAMMER_ENGINE_H
#define HAMMER_ENGINE_H

#ifdef HAMMER_ENGINE_IMPLEMENTATION

// Core C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// POSIX
#include <unistd.h>
#include <libgen.h>

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
#define TITLE "Hammer Engine"
#define MAX_MODELS 1024

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
typedef struct h_EngineState h_EngineState;

// fdecl
HE_DECL u8 h_HammerRun(void);
HE_DECL u8 h_WindowInit(void);
HE_DECL u8 h_Loop(void);
HE_DECL u8 h_EngineParseBase(const char *);

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
};

struct h_Level {
	h_Config config;
	h_Model models[MAX_MODELS];
	u16 model_count;
};

struct h_EngineState {
	h_Window window;
	Camera camera;
	const u8 fps;
	h_Config config;
	h_Level levels[32];
	h_Level *current_level;
};

// globals
static h_EngineState engine = { .window.title = TITLE,
				.camera = {0},
				.fps = 30,
				.config = {0},
				.levels = {0},
				.current_level = NULL };

// fdef
u8 h_HammerRun(void) {

	LOG("Hammer Engine Running, BattleCruiser operational.\n");

	if(h_WindowInit()) {
		perror("Window Initialization failed. Aborting.");
		return 1;
	}

	if(h_EngineParseBase(engine.config.base)) {
		perror("Engine Base folder parsing failed. Aborting.");
		return 1;
	}
	
	if(h_Loop()) {
		perror("Main loop error. Aborting.");
		return 1;
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

u8 h_Loop(void) {

	while(!WindowShouldClose()) {
		BeginDrawing();
			BeginMode3D(engine.camera);

		ClearBackground(DARKBLUE);
		DrawFPS(10,10);
			
			EndMode3D();
		EndDrawing();
	}

	return 0;
}

HE_DECL u8 h_EngineParseBase(const char *path) {

	FILE *fp = NULL;

	// checking for base folder correctness
	if(access(path, F_OK) == 0) {
		(void)snprintf(engine.config.root, sizeof(engine.config.root),
		"%s%s%s", engine.config.base, SEP, CFG_ROOT);

		// Always check for first level cfg as first level is starting point
		// Load/Save logic is done elsewhere.
		//(void)snprintf(engine.config.level, sizeof(engine.config.level),
		//"%s%s%s%s%s%s%s", tmp, SEP, BASE_LEVELS, SEP, "1", SEP, CFG_LEVEL);
				
		//(void)snprintf(engine.config.resources, sizeof(engine.config.resources),
		//"%s%s%s", tmp, SEP, CFG_RESOURCES);

		if(access(engine.config.root, F_OK) != 0) {
			perror("Base root config not found.");
			return 1;
		}
	}

	else {
		perror("Base folder provided in HAMMERCFG cannot be opened. Aborting.");
		return 1;
	}

	if( (fp = fopen(engine.config.root, "r")) != NULL) {
		// do nothing for now, control definitions.
	}

	else {
		perror("Base folder failed to open. fopen error.");
		return 1;
	}

	fclose(fp);
	return 0;
}

#endif // HAMMER_ENGINE_IMPLEMENTATION end

#endif // HAMMER_ENGINE_H end
