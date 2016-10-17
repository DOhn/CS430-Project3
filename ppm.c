#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"

void p6Create(Image *img, FILE* input, int magicNumber) {

  size_t num;
  int size = img->width*img->height*4;
  printf("magic: %d\n", magicNumber);
  if (magicNumber == 6) {
    printf("inside\n");
    fprintf(input, "P%d\n%lf %lf\n%d\n", magicNumber, img->width, img->height, 255);
    for (int i=0; i<size; i++) {
      char c = img->color[i];
      if (i%4 != 3) {
        fwrite(&c, 1, 1, input);
      }
    }
  }
  fclose(input);
}
