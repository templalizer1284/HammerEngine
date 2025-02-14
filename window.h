#ifndef H_WINDOW
#define H_WINDOW

#include "hammer.h"

typedef struct h_Window h_Window;

struct h_Window {
	int width, height, mode;
	char *title;
};

int h_WindowInit(void);
h_Window *window;

#endif
