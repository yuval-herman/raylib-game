#ifndef CONSTRUCT_PAINTER_H
#define CONSTRUCT_PAINTER_H

#include "construct.h"

typedef struct Construct_Painter Construct_Painter;

// Create a new construct painter that will draw the given construct
Construct_Painter *construct_painter_make(Construct *construct);

// Update the visual representation of the construct
void construct_painter_update(Construct_Painter *painter);

// Free the painter and its resources
void construct_painter_destroy(Construct_Painter *painter);

#endif // CONSTRUCT_PAINTER_H