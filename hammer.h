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

// datadecl
typedef struct h_Window h_Window;
typedef struct h_Model h_Model;
typedef struct h_Entity h_Entity;
typedef struct h_Config h_Config;
typedef struct h_EngineState h_EngineState;

// fundecl
HE_DECL u8 h_HammerRun(void);
HE_DECL u8 h_WindowInit(void);
HE_DECL u8 h_Loop(void);

// datadef
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
	const char *root_config;
	const char *level_config;
};

struct h_EngineState {
	h_Window window;
	Camera camera;
	h_Model models[MAX_MODELS];
	u16 model_count;
	u8 fps;
};

// globals
static h_EngineState engine = { .window.title = TITLE,
				.camera = {0},
				.models = {0},
				.model_count = 0,
				.fps = 30 };

// fundef
u8 h_HammerRun(void) {

	LOG("Hammer Engine Running, BattleCruiser operational.\n");

	if(h_WindowInit()) {
		perror("Window Initialization failed. Aborting.");
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
			}

			else if(strcmp(tmp, "height") == 0) {
				ff;
				engine.window.height = atoi(tmp);
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

#endif

#endif
