#include "json_parse.h"
#include "json_parse.c"
#include "ppm.c"
#include "ppm.h"

int main(int argc, char *argv[]) {
  // error check number of arguments
  if (argc != 5) {
    fprintf(stderr, "ERROR: you must include 4 arguments. `file <width> <height> <json.json> <output.ppm>`");
    exit(1);
  }

  //check the width and height are greater than 0
  if (atoi(argv[1]) <= 0) {
    fprintf(stderr, "ERROR: the width must be greater than 0\n");
    exit(1);
  }

  if (atoi(argv[2]) <= 0) {
    fprintf(stderr, "ERROR: the height must be greater than 0\n");
    exit(1);
  }

  // open the json file
  FILE* input = fopen(argv[3], "rb");

  if (input == NULL) {
    fprintf(stderr, "ERROR: json file failed to open\n");
    exit(1);
  }

  // declare the object
  objMem object;

  // declare all variables for the object
  // TODO Probably delete some
  int numberOfObjects;

  double cameraHeight;
  double cameraWidth;

  // pixel width
  int M = atoi(argv[1]);
  // pixel height
  int N = atoi(argv[2]);

  // set object equal to the jason file
  /*
  ----------------camera----------------
  width: 0.500000
  height: 0.500000
  ----------------sphere----------------
  Here is the color: 1.000000, 0.000000, 0.000000
  Here is the position: 0.000000, 0.000000, 5.000000
  radius: 2.000000
  ----------------plane----------------
  Here is the color: 0.000000, 0.000000, 1.000000
  Here is the position: 0.000000, 0.000000, 0.000000
  Here is the normal: 0.000000, 1.000000, 0.000000
  */
  object = read_scene(input);

  // get the number of objects from json file
  numberOfObjects = object.objNumber;

  int i;

  int magicNumber = 6;

  // loop through the number of objects
  for (i = 0; i < numberOfObjects; i++) {
    // if there is a camera object, set the width and height of the camera
    if (object.objArray[i].camera.width && object.objArray[i].camera.height) {
      printf("There is a camera\n");
      cameraWidth = object.objArray[i].camera.width;
      cameraHeight = object.objArray[i].camera.height;
    }
  }

  // error check for the camera width and height
  if (!cameraWidth || !cameraHeight) {
    fprintf(stderr, "ERROR: There is no valid camera width or height\n");
    exit(1);
  }

  // set the pixel width and height
  double pixheight = cameraHeight / M;
  double pixwidth = cameraWidth / N;

  // create the image
  Image *img = (Image *)malloc(sizeof(Image));
  img->height = N;
  img->width = M;
  img->color = (char*)malloc(sizeof(char) * img->height * img->width * 4);


  // loop through the pixels
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < M; x++) {
      double Ro[3] = {0, 0, 0};

      double Rd[3] = { object.objArray[i].sphere.position[0] - (cameraWidth/2) + pixwidth * (x + 0.5), object.objArray[i].sphere.position[1] - (cameraHeight/2) + pixheight * (y + 0.5), 1 };
      normalize(Rd);

      int colorHelper = 0;
      double best_t = INFINITY;
      // loop through the objects figuring out which one to trace
      for (i=0; i < numberOfObjects; i++) {
        double t = 0;

        // if the current object is a sphere then call intersect
        if (object.objArray[i].sphere.position && object.objArray[i].sphere.radius) {
          t = sphere_intersection(Ro, Rd, object.objArray[i].sphere.position, object.objArray[i].sphere.radius);
        // if the current object is a plane then call intersect
        } else if (object.objArray[i].plane.position && object.objArray[i].plane.normal) {
          t = plane_intersection(Ro, Rd, object.objArray[i].plane.position, object.objArray[i].plane.normal);
        }
        //printf("This is T: %lf\n", t);
        if (t > 0 && t < best_t) {
          best_t = t;
          colorHelper = i;
        }
      }
      if (best_t > 0 && best_t != INFINITY) {
        if (object.objArray[colorHelper].sphere.position && object.objArray[colorHelper].sphere.radius) {
          shade_pixel(object.objArray[colorHelper].color, y, x, img);
        } else if (object.objArray[i].plane.position && object.objArray[i].plane.normal) {
          shade_pixel(object.objArray[colorHelper].color, y, x, img);
        }
      }
    }
  }

  FILE* ppmoutput = fopen(argv[4], "w+");
  p6Create(img, ppmoutput, magicNumber);


  return 0;
}
