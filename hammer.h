#ifndef HAMMER_H
#define HAMMER_H

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
typedef uint8_t i8;
typedef uint16_t i16;

// datadecl
typedef struct h_Window h_Window;
typedef struct h_Model h_Model;
typedef struct h_Entity h_Entity;

// fundecl
extern i8 h_HammerRun(void);
static i8 h_WindowInit(void);
static i8 h_Loop(void);

// datadef
struct h_Window {
	i16 width;
	i16 height;
	char *title;
};

// globals
#define HAMMERCFG "hammercfg"
static h_Window window = { 0 };
static Camera camera = { 0 };

#endif
