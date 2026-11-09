#pragma once

#include "Processor.h"

int Init_DefaultMandelbrot(CProcessor* proc);
int PreI_DefaultMandelbrot(CProcessor* proc);
int Iter_DefaultMandelbrot(CProcessor* proc);
int Post_DefaultMandelbrot(CProcessor* proc);
int Draw_DefaultMandelbrot(CProcessor* proc);
int Done_DefaultMandelbrot(CProcessor* proc);