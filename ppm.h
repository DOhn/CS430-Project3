#ifndef ppm_h
#define ppm_h

typedef struct Image {
  double width;
  double height;
  char* color;
}Image;

void p6Create(Image *img, FILE* input, int magicNumber);
#endif
