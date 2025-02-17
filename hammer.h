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

// i will not write this crap everytime
typedef uint8_t u8;
typedef uint16_t u16;
#define HE_DECL static inline

// datadecl
typedef struct h_Window h_Window;
typedef struct h_Model h_Model;
typedef struct h_Entity h_Entity;
typedef struct h_Config h_Config;
nn
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

// globals
#define SEP "/"

#ifdef _WIN32
#define SEP "\\"
#endif

#ifdef _WIN64
#define SEP "\\"
#endif

#define HAMMERCFG "hammercfg"
static h_Window window = { 0 };
static Camera camera = { 0 };
static h_Model models[65536];

static u16 models_count = 0;

u8 h_HammerRun(void) {

	printf("Hammer Engine Running, BattleCruiser operational.\n");

	if(h_WindowInit()) {
		perror("Window Initialization failed. Aborting.\n");
		return 1;
	}
	
	if(h_Loop()) {
		perror("Main loop error. Aborting.\n");
		return 1;
	}

	return 0;
}

u8 h_WindowInit(void) {
	if(access(HAMMERCFG, F_OK) == 0) {
		FILE *fp = fopen(HAMMERCFG, "r");
		char tmp[64];

		#define ff fscanf(fp, "%63s", tmp)
		while(ff != EOF) {
			if(strcmp(tmp, "width") == 0) {
				ff;
				window.width = atoi(tmp);
			}

			else if(strcmp(tmp, "height") == 0) {
				ff;
				window.height = atoi(tmp);
			}

			else {
				perror("Syntax error in HAMMERCFG.\n");
				return 1;
			}
		}

		fclose(fp);
	}

	else {
		perror("HAMMERCFG failed to open.\n");
		return 1;
	}

	// Raylib knows to spit out bunch of info.
	SetTraceLogLevel(LOG_WARNING);

	// Pop-up the window
	InitWindow(window.width, window.height, "Hammer Engine");

	// Setting up camera SH-like style.
	camera.position = (Vector3){ 0.0f, 5.0f, -7.0f };
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	return 0;
}

u8 h_Loop(void) {

	while(!WindowShouldClose()) {
		BeginDrawing();
			BeginMode3D(camera);

		ClearBackground(DARKBLUE);
		DrawFPS(10,10);
			
			EndMode3D();
		EndDrawing();
	}

	return 0;
}

#endif

#endif
