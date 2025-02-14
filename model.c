#include "model.h"

h_Model
*h_ModelInit(char *file, bool animation) {
	h_Model *model = malloc(sizeof(h_Model) * 1);

	model->render = true;

	model->scale = 1.0f;
	model->tint = WHITE;
	model->angle = 0.0f;
	
	model->model = LoadModel(file);
	model->name = strdup(basename(file));

	// Automatic bounding box generation
	model->box = GetMeshBoundingBox(model->model.meshes[0]); // Get the bounding box for the first mesh

	// If there are multiple meshes, combine their bounding boxes
	for (int i = 1; i < model->model.meshCount; i++) {
		BoundingBox currentBox = GetMeshBoundingBox(model->model.meshes[i]);
		model->box = CombineBoundingBoxes(model->box, currentBox);
	}
		
	model->animCount = 0;
	model->currentAnim = 0;
	model->animate = true;

	if(animation == true) {
		model->animation = LoadModelAnimations(file, &model->animCount);
	}

	else {
		model->animation = NULL;
	}

	model->Draw = &h_ModelDraw;
	model->Destroy = &h_ModelDestroy;

	return model;
}

void
h_ModelDraw(h_Model *model) {

	if(model->animate == true) {
		if(model->animCount > 0) {
	
			model->currentAnim++;
			if(model->currentAnim == model->animCount ) {
				model->currentAnim = 0;
			}
	
			UpdateModelAnimation(model->model, model->animation[0], model->currentAnim++);
		}
	}

	// Compute the transformed bounding box based on the model's position
	model->transformedBox.min = Vector3Add(model->box.min, model->position);
	model->transformedBox.max = Vector3Add(model->box.max, model->position);
	
	// Calculate the bounding box center
	model->center = GetBoundingBoxCenter(model->transformedBox);

	DrawModel(model->model, model->position, model->scale, model->tint);
}

void
h_ModelDestroy(h_Model *model) {
	UnloadModel(model->model);

	free(model->name);

	if(model->animation != NULL) {
		UnloadModelAnimations(model->animation, model->animCount);
	}
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

Vector3 
GetBoundingBoxCenter(BoundingBox box) {
    return (Vector3){
        (box.min.x + box.max.x) / 2.0f,
        (box.min.y + box.max.y) / 2.0f,
        (box.min.z + box.max.z) / 2.0f
    };
}
