#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ppm.h"

typedef struct {
  char *type; // 0 = cylinder, 1 = sphere, 2 = teapot
  double color[3];
  union {
    struct {
      double width;
      double height;
    } camera;
    struct {
      double position[3];
      double radius;
    } sphere;
    struct {
      double position[3];
      double normal[3];
    } plane;
  };
} Object;

typedef struct {
  double origin[3];
  double direction[3];
} ray;

void shade_pixel(double *color, int row, int col, Image *image);
double sqr(double v);
void normalize(double* v);
double plane_intersection(double* Ro, double* Rd, double* position, double* normal);
double sphere_intersection(double* Ro, double* Rd, double* C, double r);
