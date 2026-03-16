#ifndef PHYSICS_H
#define PHYSICS_H

#include "box2d.h"

void phys_init();
void phys_step();

struct MakeBoxParams {
  b2BodyDef body_def;
  b2ShapeDef shape_def;
  // The extents are the half-widths of the box.
  b2Vec2 extents;
};

b2BodyId phys_make_box_params(struct MakeBoxParams params);
#define phys_make_box(...) phys_make_box_params((struct MakeBoxParams){.body_def = b2DefaultBodyDef(), .shape_def = b2DefaultShapeDef(), __VA_ARGS__})

#endif // PHYSICS_H
