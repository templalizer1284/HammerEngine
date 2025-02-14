#include "processor.h"

int h_MainLoop(void) {

	Camera camera = { 0 };
	camera.position = (Vector3){ 0.0f, 5.0f, -7.0f };
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	return 0;
}
