#include "hammer.h"

i8 h_HammerRun(void) {

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

i8 h_WindowInit(void) {
	if(access(HAMMERCFG, F_OK) == 0) {
		FILE *fp = fopen(HAMMERCFG, "r");
		char *tmp = malloc(20);

		#define ff fscanf(fp, "%s", tmp)
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
		free(tmp);
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

i8 h_Loop(void) {
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
