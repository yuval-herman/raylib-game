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
