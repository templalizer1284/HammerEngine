#ifndef H_HAMMER
#define H_HAMMER

/* There will be compatibility between minor versions always. */
#define VERSION_MAJOR 0 
#define VERSION_MINOR 1

/* --------------
 * Core C libraries
 * -------------- */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

/* ----------------
 * POSIX librarires
 * ---------------- */

#include <unistd.h>
#include <libgen.h>

/* ----------------
 * Third-party code
 * ---------------- */

/* Portable multimedia library */
#include <raylib.h>
#include <raymath.h>

/* -----------------
 * Main loop include 
 * ----------------- */
#include "processor.h"

#define HAMMERCFG "hammercfg"

#endif
