#include "physics.h"
#include "constants.h"

b2WorldId worldId = {0};


void phys_init() {
  // Construct a world object, which will hold and simulate the rigid bodies.
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = (b2Vec2){0.0f, -10.0f};
  worldId = b2CreateWorld(&worldDef);
}

void phys_step() { b2World_Step(worldId, TIME_STEP, SUB_STEP_COUNT); }

b2BodyId phys_make_box_params(struct MakeBoxParams params) {
  b2BodyId bodyId = b2CreateBody(worldId, &params.body_def);
  b2Polygon bodyBox = b2MakeBox(params.extents.x, params.extents.y);
  b2CreatePolygonShape(bodyId, &params.shape_def, &bodyBox);

  return bodyId;
}
