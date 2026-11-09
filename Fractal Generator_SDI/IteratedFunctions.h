#pragma once

#include "Fractal.h"

int Init_DefaultMandelbrot(CFractal::Processor& proc);
int PreI_DefaultMandelbrot(CFractal::Processor& proc);
int Iter_DefaultMandelbrot(CFractal::Processor& proc);
int Post_DefaultMandelbrot(CFractal::Processor& proc);
int Draw_DefaultMandelbrot(CFractal::Processor& proc);
int Done_DefaultMandelbrot(CFractal::Processor& proc);