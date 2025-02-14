#include "window.h"

int h_WindowInit(void) {

	static h_Window w;

	// Raylib logging only display warnings and errors.
	SetTraceLogLevel(LOG_WARNING);

	// Accesing top-level cfg file
	if(access(HAMMERCFG, F_OK) == 0) {
		FILE *fp = fopen(HAMMERCFG, "r");
		char *tmp = malloc(20);

		while(fscanf(fp, "%s", tmp) != EOF) {
			if(strcmp(tmp, "width") == 0) {
				fscanf(fp, "%s", tmp);
				w.width = atoi(tmp);
			}

			else if(strcmp(tmp, "height") == 0) {
				fscanf(fp, "%s", tmp);
				w.height = atoi(tmp);
			}

			else {
				perror("Syntax error in hammercfg. Aborting.\n");
				return 1;
			}
		}

		free(tmp);
		fclose(fp);
	}
	
	InitWindow(w.width, w.height, w.title);

	/* Game will run at 30 FPS. */
	SetTargetFPS(30);

	window = &w;

	return 0;
}
