#ifndef HAMMER_ENGINE_H
#define HAMMER_ENGINE_H

#ifdef HAMMER_ENGINE_IMPLEMENTATION

// Core C
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
#define h_Vec3Mod(dst,x,y,z) \
	dst.x = x; \
	dst.y = y; \
	dst.z = y

#define SEP "/"

#if defined(_WIN32) || defined(_WIN64)
#undef SEP
#define SEP "\\"
#endif

#ifdef DEBUG
	#define LOG printf
#else
	#define LOG(...) ((void)0)
#endif

// i will not write this crap everytime
typedef uint8_t u8;
typedef uint16_t u16;

#define U8 255
#define U6 63
#define U2 4

// psx-style
#define FPS 30

#define HE_DECL static inline

// ddecl
typedef struct h_Window h_Window;
typedef struct h_Model h_Model;
typedef struct h_Entity h_Entity;
typedef struct h_Level h_Level;
typedef struct h_Config h_Config;
typedef struct h_HammerMenu h_HammerMenu;
typedef struct h_EngineControl h_EngineControl;

typedef struct h_EngineState h_EngineState;

// fdecl
HE_DECL u8 		h_HammerRun(void);
HE_DECL u8 		h_WindowInit(void);
HE_DECL u8 		h_EngineParseBase(void);
HE_DECL u8 		h_EngineParseRoot(void);
HE_DECL u8 		h_EngineParseLevel(const char *);

HE_DECL u8 		h_HammerIntro(void); // TODO
HE_DECL u8 		h_HammerMenuRun(void);
HE_DECL u8 		h_HammerLevelRun(const char *);

HE_DECL h_Model 	h_EngineModelLoad(const char *);
HE_DECL	u8 		h_EngineModelDraw(h_Model *);
HE_DECL u8		h_EngineModelExists(const char *);

HE_DECL BoundingBox	CombineBoundingBoxes(BoundingBox, BoundingBox);
HE_DECL void		UpdateTransformedBoundingBox(h_Model *);

HE_DECL	u8 		h_EngineLoadGame(const char *); // TODO
HE_DECL u8 		h_EngineSaveGame(const char *); // TODO

HE_DECL void		h_Processor(int, ...);
HE_DECL u8		h_EngineUnload(void);

// non-posix, stack-only, memory-safe getline, not the fastest but its OK
HE_DECL void		h_EngineGetline(FILE *, char *, size_t);
HE_DECL u8		h_KeywordExists(const char *);

// ddef
struct h_Window {
	u16 width;
	u16 height;
	char *title;
};

struct h_Model {
	Model model;

	ModelAnimation *animations;
	int animCount;
	int currentAnimation;
	int currentFrame;
	bool animate;

	BoundingBox box, transformedBox;
	Vector3 center;

	Vector3 position;
	float angle;
	bool render;

	char name[U8];
	u8 type;

	Vector3 scale;
	Color tint;
};

struct h_Config {
	char base[U6];
	char root[U6];
	char level[U6];
	char resources[U6];
	char save[U6];
};

struct h_Level {
	h_Model hero,map;
	h_Model entities[MAX_MODELS];
	u16 entities_count;
	char name[U8];

	// logic info
	
	// collision
	BoundingBox *col_one[U8], *col_two[U8];
	u16 col_count;

	// some collision functions take one or more args
	u8 col_action_instruction[U8];
	
	char col_action_arg1[U8][U8];
	char col_action_arg2[U8][U8];
	char col_action_arg3[U8][U8];
	char col_action_arg4[U8][U8];
	char col_action_arg5[U8][U8];
};

struct h_HammerMenu {
	Font button_font, text_font;
	char button_newgame[U6];
	char button_loadgame[U6];
	char button_options[U6];
	char button_quit[U6];
	char selector[U2];
	Texture2D background_texture;
	u8 menu_switch;
};

struct h_EngineState {
	h_Window window;
	Camera camera;
	const u8 fps;
	h_Config config;
	char starting_level[U8];
	h_Level *current_level;
	char *load_file;
	h_HammerMenu menu;
	bool debug;

	void *cvars[];
};

struct h_EngineControl {

	// hero controls
	int forward;
	int backward;
	int strafe_left;
	int strafe_right;
	int turn_left;
	int turn_right;
	int action;
	int toggle_run;

	// ui control TODO
	int pause;

	// trackers
	bool light;
	float velocity;
	float run_factor;
};

enum BUTTONS {
	NEW_GAME,
	LOAD_GAME,
	OPTIONS,
	QUIT,
	NUM_OF_BUTTONS
};

enum MODEL_TYPE {
	HERO,
	MAP,
	ENTITY
};

enum ENTITY_TYPE {
	ENTITY_HERO,
	ENTITY_MAP,
	ENTITY_STATIC
};

enum PROCESSOR_INSTRUCTIONS {
	PRINT, // one arg, const char *
	NUM_PROCESSOR_KEYWORDS
};

// for use within this engine every model must have index of its animations like this
// starting from 0 for IDLE animation
// they are in this order because not all entites attack or die, what is common for all
// entities is that they all have idle and run animations, optionally turning animations.
enum ANIMATION_POSITIONS {
	IDLE, // = 0
	RUN,
	TURN,
	DEATH,
	HIT, // when got hit by an entity
	ATTACK,
	COLLISION,
	STOP, // anim used for stopping after walk/run
	MISC_ONE, // miscellaneous extra animations, defined in logic
	MISC_TWO,
	MISC_THREE,
	MISC_FOUR,
	MISC_FIVE
};

// globals
static h_EngineState engine = { .window.title = TITLE,
				.fps = FPS,
				.starting_level = "",
				.current_level = NULL,
				.debug = false,
				.menu = { 0 },
				.load_file = "" };

static h_EngineControl controls = { .forward = KEY_W,
				    .backward = KEY_S,
				    .strafe_left = KEY_A,
				    .strafe_right = KEY_D,
				    .turn_left = KEY_LEFT,
				    .turn_right = KEY_RIGHT,
				    .action = KEY_SPACE,
				    .toggle_run = KEY_LEFT_SHIFT,

				    .pause = KEY_P,

				    .light = false,

				    // there is possibility to change this in cfg.root, see README.root
				    .velocity = 0.035f,
				    .run_factor = 0.05f };

static char *Processor_Keywords[NUM_PROCESSOR_KEYWORDS][U8] = {
	{ "PRINT" }
};

// fdef
u8
h_HammerRun(void) {

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

u8
h_WindowInit(void) {

	if(access(HAMMERCFG, F_OK) == 0) {
		FILE *fp = fopen(HAMMERCFG, "r");
		char tmp[U6];

		if(fp == NULL) {
			LOG("HAMMERCFG failed to open, using default values.\n");
			engine.window.width = 800;
			engine.window.height = 600;
		}

		#define ff fscanf(fp, "%60s", tmp)
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

u8
h_HammerLevelRun(const char *level) {

	if(h_EngineParseLevel(level)) {
		LOG("Level parsing error.\n");
		return 1;
	}

	while(!WindowShouldClose()) {

		BeginDrawing();
			ClearBackground(DARKBLUE);
			BeginMode3D(engine.camera);

			if(engine.debug) {
				DrawGrid(10.0f, 1.0f);
			}

			// drawing models, hero, map and entities.
			h_EngineModelDraw(&engine.current_level->hero);
			h_EngineModelDraw(&engine.current_level->map);

			// entities, TODO, compare entity types render accordingly
			for(int i = 0; i < engine.current_level->entities_count; i++) {
				h_EngineModelDraw(&engine.current_level->entities[i]);
			}

			// input check

			float speed;
			
			if(IsKeyDown(controls.toggle_run)) {
				speed = controls.velocity + controls.run_factor;
			}

			else {
				speed = controls.velocity;
			}
			
			if(IsKeyDown(controls.forward)) {
				engine.current_level->hero.position.z += speed * cos(DEG2RAD * engine.current_level->hero.angle);
				engine.current_level->hero.position.x += speed * sin(DEG2RAD * engine.current_level->hero.angle);
			}

			else if(IsKeyDown(controls.backward)) {
				engine.current_level->hero.position.z -= speed * cos(DEG2RAD * engine.current_level->hero.angle);
				engine.current_level->hero.position.x -= speed * sin(DEG2RAD * engine.current_level->hero.angle);
			}

			if(IsKeyDown(controls.turn_left)) {
				engine.current_level->hero.angle += 5.0f;
			}

			else if(IsKeyDown(controls.turn_right)) {
				engine.current_level->hero.angle -= 5.0f;
			}

			if(IsKeyDown(controls.strafe_left)) {
				engine.current_level->hero.position.x += speed;
			}

			else if(IsKeyDown(controls.strafe_right)) {
				engine.current_level->hero.position.x -= speed;
			}

			// check collisions
			for(int i = 0; i < engine.current_level->col_count; i++) {
				if(CheckCollisionBoxes(*engine.current_level->col_one[i], *engine.current_level->col_two[i])) {
					switch(engine.current_level->col_action_instruction[i]) {
						case PRINT:
							h_Processor(2, PRINT, (void *)engine.current_level->col_action_arg1[i]);
						break; //xx
					};
				}
			}
			
			EndMode3D();

			if(engine.debug) {
				DrawFPS(10.0f,10.0f);
			}
			
		EndDrawing();
	}

	UnloadModel(engine.current_level->hero.model);
	UnloadModel(engine.current_level->map.model);

	return 0;
}

u8
h_EngineParseBase(void) {

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

u8
h_EngineParseRoot(void) {

	FILE *fp = fopen(engine.config.root, "r");

	char tmp[U8];
	#define ff fscanf(fp, "%60s", tmp)

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

u8
h_EngineParseLevel(const char *path) {

	char resources[255], logic[255];

	(void)snprintf(resources, sizeof(resources),
	"%s%s%s", path, SEP, CFG_RESOURCES);

	(void)snprintf(logic, sizeof(logic),
	"%s%s%s", path, SEP, CFG_LOGIC);

	FILE *fp = NULL; char tmp[U8];
	#define ff fscanf(fp, "%60s", tmp)

	static h_Level level;

	(void)snprintf(level.name, sizeof(level.name),
	"%s", path);

	engine.current_level = &level;

	// parsing resources
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
					
					if(access(p, F_OK) == 0) {
						// load hero
						level.hero = h_EngineModelLoad(p);
						
						(void)snprintf(level.hero.name, sizeof(level.hero.name),
						"%s", tmp);

						level.hero.type = HERO;
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

						(void)snprintf(level.map.name, sizeof(level.map.name),
						"%s", tmp);

						level.map.type = MAP;
					}

					else {
						LOG("Map model %s cannot be loaded.\n", tmp);
						return 1;
					}

					continue;
				}

				if(strcmp(tmp, "ENTITY") == 0) {
					ff;
					
					if(strcmp(tmp, "STATIC") == 0) {
						ff;

						char full_path[255];
						(void)snprintf(full_path, sizeof(full_path),
						"%s%s%s%s%s", engine.config.base, SEP, BASE_MEDIA, SEP, tmp);
						
						if(access(full_path, F_OK) == 0) {
							FILE *fpp = fopen(full_path, "r");

							if(fpp == NULL) {
								LOG("Cannot open %s entity.\n", tmp);
								return 1;
							}

							else {
								engine.current_level->entities[engine.current_level->entities_count] = h_EngineModelLoad(full_path);
								engine.current_level->entities_count++;
							}
						}

						else {
							LOG("Cannot access %s entity.\n", tmp);
							return 1;
						}
					}

					else {
						LOG("Unknown entity type '%s' in level config.\n", tmp);
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
	}

	else {
		LOG("Cannot open config file for resources or logic.\n");
		return 1;
	}

	// parsing logic
	if( (fp = fopen(logic, "r")) == NULL) {
		LOG("Cannot open logic for current level.\n");
		return 1;
	}

	else {
		while(ff == 1) {
			if(strcmp(tmp, "POSITION") == 0) {

				// get model name
				ff;
				
				// check if model exists in array
				int counter = h_EngineModelExists(tmp);

				if(counter < 0) {
					LOG("Syntax error in level logic, model doesn't exist.\n");
					return 1;
				}

				else {
					float x,y,z;
					fscanf(fp, "%f %f %f", &x, &y, &z);

					if(counter == HERO) {
						h_Vec3Mod(engine.current_level->hero.position, x,y,z);
					}

					else if(counter == MAP) {
						h_Vec3Mod(engine.current_level->map.position, x,y,z);
					}

					else {
						h_Vec3Mod(engine.current_level->entities[counter-ENTITY].position,
						x,y,z);
					}
				}

				continue;
			}

			else if(strcmp(tmp, "COLLISION") == 0) {
				char first_model[U8], second_model[U8];

				// getting first model
				ff;
				(void)snprintf(first_model, sizeof(first_model),
				"%s", tmp);

				// getting second model
				ff;
				(void)snprintf(second_model, sizeof(second_model),
				"%s", tmp);

				// checking if not the same, you can never know
				if(strcmp(first_model, second_model) == 0) {
					LOG("Syntax error, collision detection on same model is not possible.\n");
					return 1;
				}

				// checking if both models are loaded in program
				int counter = h_EngineModelExists(first_model);
				
				if(counter < 0) {
					LOG("Error, model named %s doesn't exists", tmp);
					return 1;
				}

				else {
					if(counter == HERO) {
						engine.current_level->col_one[engine.current_level->col_count] =
						&engine.current_level->hero.transformedBox;
					}

					else if(counter == MAP) {
						engine.current_level->col_one[engine.current_level->col_count] =
						&engine.current_level->map.transformedBox;
					}

					else {
						engine.current_level->col_one[engine.current_level->col_count] =
						&engine.current_level->entities[counter-ENTITY].transformedBox;
					}
				}

				counter = h_EngineModelExists(second_model);

				// repeating this crap because i am stupid
				if(counter < 0) {
					LOG("Error, model named %s doesn't exists", tmp);
					return 1;
				}

				else {
					if(counter == HERO) {
						engine.current_level->col_two[engine.current_level->col_count] =
						&engine.current_level->hero.transformedBox;
					}

					else if(counter == MAP) {
						engine.current_level->col_two[engine.current_level->col_count] =
						&engine.current_level->map.transformedBox;
					}

					else {
						engine.current_level->col_two[engine.current_level->col_count] =
						&engine.current_level->entities[counter-ENTITY].transformedBox;
					}
				}

				// getting response to collision
				ff;

				// checking if keyword exists
				int kword = h_KeywordExists(tmp);

				if(kword < 0) {
					LOG("Syntax error, keyword %s doesn't exist.\n", tmp);
					return 1;
				}

				else {
					// take arguments accordingly for every instruction

					switch(kword) {
						case PRINT:
							h_EngineGetline(fp, tmp, sizeof(tmp));
							
							engine.current_level->col_action_instruction[engine.current_level->col_count] = kword;
							
							(void)snprintf(engine.current_level->col_action_arg1[engine.current_level->col_count],
							sizeof(engine.current_level->col_action_arg1[engine.current_level->col_count]),
							"%s", tmp);
							//xx
						break;
					}
				}
				
				engine.current_level->col_count++;
			}

			else {
				LOG("Syntax error, %s is unknown keyword in logic.\n", tmp);
				return 1;
			}

			
		}
	}

	fclose(fp);
	return 0;
}

u8
h_EngineLoadGame(const char *file) {

	FILE *fp = NULL;

	if(access(file, F_OK) == 0) {

		fp = fopen(file, "r");
		if(fp == NULL) {
			perror("Failed to open file.");
			return 1;
		}

		else {
			char tmp[U8];
			while(fscanf(fp, "%60s", tmp) == 1) {
				if(strcmp(tmp, "LEVEL") == 0) {
					fscanf(fp, "%60s", tmp);
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

u8
h_HammerMenuRun(void) {

	#define AR_SIZE 20
	char buttons[][AR_SIZE] = { {"New Game"}, {"Load Game"}, {"Options"}, {"Quit"} };
	
	char text[20];

	while(!WindowShouldClose()) {
		BeginDrawing();

			if(engine.debug) DrawFPS(10.0f, 10.0f);

			// drawing background image
			DrawTexture(engine.menu.background_texture, 0, 0, WHITE);

			// drawing buttons, newgame,loadgame,options and quit
			for(int i = 0; i < NUM_OF_BUTTONS; i++) {

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
				if(engine.menu.menu_switch == (NUM_OF_BUTTONS - 1)) {
					engine.menu.menu_switch = 0;
				}

				else {
					engine.menu.menu_switch++;
				}
			}
			
			else if(IsKeyPressed(KEY_UP)) {
				if(engine.menu.menu_switch == 0) {
					engine.menu.menu_switch = NUM_OF_BUTTONS - 1;
				}

				else {
					engine.menu.menu_switch--;
				}

			}

			if(IsKeyDown(KEY_ENTER)) {
				break;
			}
			
		EndDrawing();
	}

	return engine.menu.menu_switch;
}

h_Model
h_EngineModelLoad(const char *path) {

	h_Model model = {
		.animCount = 0,
		.currentFrame = 0,
		.currentAnimation = IDLE,
		.animate = false,
		.position = (Vector3) { 0.0f, 0.0f, 0.0f },
		.tint = WHITE,
		.angle = 0.0f,
		.scale = (Vector3) { 1.0f, 1.0f, 1.0f },
		.render = true,
	};

	model.model = LoadModel(path);
	model.animations = LoadModelAnimations(path, &model.animCount);

	// check if model actually contains animations, animate
	if(model.animCount > 0) {
		model.animate = true;
	}

	// if it doesn't contain then ditch animations
	else {
		UnloadModelAnimations(model.animations, model.animCount);
	}

	// getting only model name from path
	(void)snprintf(model.name, sizeof(model.name),
	"%s", basename((char *)path));

	LOG("Num of animations for model %s is %d.\n", model.name, model.animCount);

	// generating bounding box
	model.box = GetMeshBoundingBox(model.model.meshes[0]);

	for(int i = 1; i < model.model.meshCount; i++) {
		BoundingBox currentBox = GetMeshBoundingBox(model.model.meshes[i]);
		model.box = CombineBoundingBoxes(model.box, currentBox);
	}

	return model;
}

u8
h_EngineModelDraw(h_Model *model) {

	if(model->render) {

		if(model->animate) {
			model->currentFrame++;

			if(model->currentFrame == model->animations[model->currentAnimation].frameCount) {
				model->currentFrame = 1;
			}
			
			UpdateModelAnimation(model->model,
				model->animations[model->currentAnimation],
				model->currentFrame);
		}

		UpdateTransformedBoundingBox(model);

		DrawModelEx(model->model, model->position, (Vector3){0.0f, 1.0f, 0.0f},
		model->angle, model->scale, model->tint);

		if(engine.debug) {
			DrawBoundingBox(model->transformedBox, GREEN);
		}
	}

	else {
		return 0;
	}
	
	return 0;
}

u8
h_EngineModelExists(const char *name) {

	// returns index of model in current level
	// 0 = hero
	// 1 = map
	// >1 = entity
	// -1 = doesn't exist

	if(strcmp(name, engine.current_level->hero.name) == 0) {
		return HERO;
	}

	else if(strcmp(name, engine.current_level->map.name) == 0) {
		return MAP;
	}

	else {
		for(int i = 0; i < engine.current_level->entities_count; i++) {
			if(strcmp(name, engine.current_level->entities[i].name) == 0) {
				return i + ENTITY;
			}
		}
	}

	return -1;
}

BoundingBox
CombineBoundingBoxes(BoundingBox box1, BoundingBox box2) {
	BoundingBox combined;

	combined.min.x = fmin(box1.min.x, box2.min.x);
	combined.min.y = fmin(box1.min.y, box2.min.y);
	combined.min.z = fmin(box1.min.z, box2.min.z);
	combined.max.x = fmax(box1.max.x, box2.max.x);
	combined.max.y = fmax(box1.max.y, box2.max.y);
	combined.max.z = fmax(box1.max.z, box2.max.z);

	return combined;
}

void
UpdateTransformedBoundingBox(h_Model *model) {
	model->transformedBox.min = Vector3Add(model->box.min, model->position);
	model->transformedBox.max = Vector3Add(model->box.max, model->position);
}

void
h_Processor(int count, ...) {

	va_list args;
	va_start(args, count);

	switch(va_arg(args, int)) {
		case PRINT:
			LOG("%s\n", va_arg(args, const char *));
		break;
	}

	va_end(args); //xx
}

u8
h_EngineUnload(void) {

	UnloadFont(engine.menu.button_font);

	return 0;
}

u8
h_KeywordExists(const char *keyword) {

	// returns index of keyword if exists
	for(int i = 0; i < NUM_PROCESSOR_KEYWORDS; i++) {
		if(strcmp(keyword, Processor_Keywords[i][0]) == 0) {
			return i;
		}
	}

	return -1;
}

void
h_EngineGetline(FILE *fp, char *dst, size_t size) {

	(void)memset(dst, 0, size);
	
	u16 i = 0;
	char c = 0;
	u8 skip = 0;
	
	while(i != size) {
		fscanf(fp, "%c", &c);

		// skip first whitespace
		if(skip == 0) {
			skip++;
			continue;
		}
		
		if(c == 0x0A) {
			break;
		}
		
		else {
			dst[i] = c;
		}
		
		i++;
	}

}

#endif // HAMMER_ENGINE_IMPLEMENTATION end

#endif // HAMMER_ENGINE_H end
