#include "raycast.h"

double sqr(double v) {
  return v*v;
}


void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}



void shade_pixel(double *color, int row, int col,Image *image) {
    //printf("shade_pixel starting works\n");
    image->color[(int)(row * image->width*4 + col*4)] = (char)(color[0]*255);
    image->color[(int)(row * image->width*4 + col*4+1)] = (char)(color[1]*255);
    image->color[(int)(row * image->width*4 + col*4+2)]= (char)(color[2]*255);
    image->color[(int)(row * image->width*4 + col*4+3)]= 255;
    //printf("shade_pixel finished works\n");
}


double plane_intersection(double* Ro, double* Rd, double* position, double* normal) {
  // formula for the plane intersection
  double a = (normal[0]* Rd[0])+(normal[1]* Rd[1])+(normal[2]* Rd[2]);

  // check absolute value
  if(fabs(a) < .0001) {
    return -1;
  }

  double b[3];

  for (int i=0; i<=2;i++){
	  b[i] = position[i]-Ro[i];
  }

  double d = (b[0]* normal[0])+(b[1]* normal[1])+(b[2]* normal[2]) ;

  double t = d/a;

  if (t < 0) {
    return -1;
  }

  return t;
}

double sphere_intersection(double* Ro, double* Rd, double* C, double r) {
  // Step 1. Find the equation for the object you are
  // interested in..  (e.g., cylinder)
  //
  // x^2 + z^2 = r^2
  //
  // Step 2. Parameterize the equation with a center point
  // if needed
  //
  // (x-Cx)^2 + (z-Cz)^2 = r^2
  //
  // Step 3. Substitute the eq for a ray into our object
  // equation.
  //
  // (Rox + t*Rdx - Cx)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
  //
  // Step 4. Solve for t.
  //
  // Step 4a. Rewrite the equation (flatten).
  //
  // -r^2 +
  // t^2 * Rdx^2 +
  // t^2 * Rdz^2 +
  // 2*t * Rox * Rdx -
  // 2*t * Rdx * Cx +
  // 2*t * Roz * Rdz -
  // 2*t * Rdz * Cz +
  // Rox^2 -
  // 2*Rox*Cx +
  // Cx^2 +
  // Roz^2 -
  // 2*Roz*Cz +
  // Cz^2 = 0
  //
  // Steb 4b. Rewrite the equation in terms of t.
  //
  // t^2 * (Rdx^2 + Rdz^2) +
  // t * (2 * (Rox * Rdx - Rdx * Cx + Roz * Rdz - Rdz * Cz)) +
  // Rox^2 - 2*Rox*Cx + Cx^2 + Roz^2 - 2*Roz*Cz + Cz^2 - r^2 = 0
  //
  // Use the quadratic equation to solve for t..
  double a = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
  double b = 2 * (Rd[0]*(Ro[0]-C[0]) + Rd[1]*(Ro[1]-C[1]) + Rd[2]*(Ro[2]-C[2]));
  double c = sqr(Ro[0]-C[0]) + sqr(Ro[1]-C[1]) + sqr(Ro[2]-C[2]) - sqr(r);

  //printf("a: %lf, b: %lf, c: %lf\n", a, b, c);

  double det = sqr(b) - 4 * a * c;
  //printf("det: %lf\n", det);
  if (det < 0) return -1;

  det = sqrt(det);

  double t0 = (-b - det) / (2*a);
  if (t0 > 0) return t0;

  double t1 = (-b + det) / (2*a);
  if (t1 > 0) return t1;

  return -1;
}

/*int main() {

  Object** objects;
  objects = malloc(sizeof(Object*)*2);
  objects[0] = malloc(sizeof(Object));
  objects[0]->type = 0;
  objects[0]->sphere.radius = 2;
  // object[0]->teapot.handle_length = 2;
  objects[0]->sphere.position[0] = 0;
  objects[0]->sphere.position[1] = 0;
  objects[0]->sphere.position[2] = 20;
  objects[1] = NULL;

  double cx = 0;
  double cy = 0;
  double h = 0.7;
  double w = 0.7;

  int M = 20;
  int N = 20;

  double pixheight = h / M;
  double pixwidth = w / N;
  for (int y = 0; y < M; y += 1) {
    for (int x = 0; x < N; x += 1) {
      double Ro[3] = {0, 0, 0};
      // Rd = normalize(P - Ro)
      double Rd[3] = { cx - (w/2) + pixwidth * (x + 0.5), cy - (h/2) + pixheight * (y + 0.5), 1 };
      normalize(Rd);

      double best_t = INFINITY;
      for (int i=0; objects[i] != 0; i += 1) {
	       double t = 0;

         switch(objects[i]->type) {
           case 0:
            t = cylinder_intersection(Ro, Rd, objects[i]->sphere.position, objects[i]->sphere.radius);
            break;
          default:
	        // Horrible error
	         exit(1);
	       }
	      if (t > 0 && t < best_t) best_t = t;
      }
      if (best_t > 0 && best_t != INFINITY) {
	printf("#");
      } else {
	       printf(".");
      }

    }
    printf("\n");
  }

  return 0;
}*/
