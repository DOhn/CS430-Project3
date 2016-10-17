#include "json_parse.h"

int line = 1;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
  int c = next_c(json);
  if (c == d) return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE* json) {
  double value;
  fscanf(json, "%lf", &value);

  if (feof(json)) {
    fprintf(stderr, "ERROR: Unexpected end of file.\n");
    exit(1);
  }

  if (ferror(json)) {
    fprintf(stderr, "ERROR: Error reading json file\n");
    exit(1);
  }
  return value;
}

double* next_vector(FILE* json) {
  double* v = malloc(3*sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}


struct objMem read_scene(FILE* json) {
  int c;
  // create a new object from struct
  objMem object;
  object.objArray = NULL;
  object.objNumber = 0;

  // skip any possible white space in the begining
  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');

  skip_ws(json);

  // Find the objects

  while (1) {
    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: please check the number of objects in the json file.\n");
      fclose(json);
      return object;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char* key = next_string(json);
      if (strcmp(key, "type") != 0) {
	       fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
         exit(1);
      }

      skip_ws(json);

      expect_c(json, ':');

      skip_ws(json);

      char* value = next_string(json);

      // we have another object
      object.objNumber += 1;
      // realloc based on new object
      object.objArray = realloc(object.objArray, sizeof(Object)*object.objNumber);
      // set the type of object to whatevery next_string returns to value
      object.objArray[object.objNumber-1].type = value;

      // will break if value is not a cameral, sphere, plane, or light
      if (strcmp(value, "camera") == 0) {
        printf("------------CAMERA--------------\n");
      } else if (strcmp(value, "sphere") == 0) {
        printf("------------SPHERE--------------\n");
      } else if (strcmp(value, "plane") == 0) {
        printf("------------PLANE--------------\n");
      } else if (strcmp(value, "light") == 0) {
        printf("------------LIGHT--------------\n");
      } else {
        fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
        exit(1);
      }

      skip_ws(json);

      while (1) {
        c = next_c(json);
        if (c == '}') {
          // stop parsing this object
          break;
        } else if (c == ',') {
          // read another field
          skip_ws(json);
          // find the next property
          char* key = next_string(json);
          skip_ws(json);
          expect_c(json, ':');
          skip_ws(json);

          // check the key with width, height, radius, angular, or radial values
          if ((strcmp(key, "width") == 0) ||
              (strcmp(key, "height") == 0) ||
              (strcmp(key, "radius") == 0) ||
              (strcmp(key, "radial-a0") == 0) ||
              (strcmp(key, "radial-a1") == 0) ||
              (strcmp(key, "radial-a2") == 0) ||
              (strcmp(key, "angular-a0") == 0)) {
            // store the value of camera.width, camera.height, or sphere.radius
            double value = next_number(json);
            // error check the value
            printf("%lf\n", value);
            if (value <= 0) {
              fprintf(stderr, "ERROR: the width, height, and radius must be greater than 0\n");
              exit(1);
            } else {
              // store the width in the struct
              if ((strcmp(key, "width") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "camera") == 0)) {
                object.objArray[object.objNumber-1].camera.width = value;
              }
              // store the height in the struct
              else if ((strcmp(key, "height") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "camera") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].camera.height = value;
              }
              // store the radius in the struct
              else if ((strcmp(key, "radius") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "sphere") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].sphere.radius = value;
              }
              // store the radial-a0 in the struct
              /*else if ((strcmp(key, "radial-a0") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].light.radiala0 = value;
              }
              // store the radial-a1 in the struct
              else if ((strcmp(key, "radial-a1") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].light.radiala1 = value;
              }
              // store the radial-a2 in the struct
              else if ((strcmp(key, "radial-a2") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].light.radiala2 = value;
              }
              // store the angular-a0 in the struct
              else if ((strcmp(key, "angular-a0") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
                printf("%s - %lf\n", object.objArray[object.objNumber-1].type, value);
                object.objArray[object.objNumber-1].light.angulara0 = value;
              }*/
            }
            // check if color, position, normal, direction, diffuse_color, or specular_color
          } else if ((strcmp(key, "color") == 0) || (strcmp(key, "position") == 0) || (strcmp(key, "normal") == 0) || (strcmp(key, "direction") == 0) || (strcmp(key, "diffuse_color") == 0) || (strcmp(key, "specular_color") == 0)) {
            double* value = next_vector(json);
            // store the color of either a sphere or plane
            if ((strcmp(key, "color") == 0) && ((strcmp(object.objArray[object.objNumber-1].type, "sphere") == 0) || (strcmp(object.objArray[object.objNumber-1].type, "plane") == 0) || (strcmp(object.objArray[object.objNumber-1].type, "light") == 0))) {
              object.objArray[object.objNumber-1].color[0] = value[0];
              object.objArray[object.objNumber-1].color[1] = value[1];
              object.objArray[object.objNumber-1].color[2] = value[2];
              printf("Color: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].color[0],
                                                 object.objArray[object.objNumber-1].color[1],
                                                 object.objArray[object.objNumber-1].color[2]);
            }
            // store the position of the sphere
            else if ((strcmp(key, "position") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "sphere") == 0)) {
              object.objArray[object.objNumber-1].sphere.position[0] = value[0];
              object.objArray[object.objNumber-1].sphere.position[1] = value[1];
              object.objArray[object.objNumber-1].sphere.position[2] = value[2];
              printf("Position: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].sphere.position[0],
                                                              object.objArray[object.objNumber-1].sphere.position[1],
                                                              object.objArray[object.objNumber-1].sphere.position[2]);
            }
            // store the position of the plane
            else if ((strcmp(key, "position") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "plane") == 0)) {
              object.objArray[object.objNumber-1].plane.position[0] = value[0];
              object.objArray[object.objNumber-1].plane.position[1] = value[1];
              object.objArray[object.objNumber-1].plane.position[2] = value[2];
              printf("Position: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].plane.position[0],
                                                              object.objArray[object.objNumber-1].plane.position[1],
                                                              object.objArray[object.objNumber-1].plane.position[2]);
            }
            // store the normal of the plane
            else if ((strcmp(key, "normal") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "plane") == 0)) {
              object.objArray[object.objNumber-1].plane.normal[0] = value[0];
              object.objArray[object.objNumber-1].plane.normal[1] = value[1];
              object.objArray[object.objNumber-1].plane.normal[2] = value[2];
              printf("Normal: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].plane.normal[0],
                                                            object.objArray[object.objNumber-1].plane.normal[1],
                                                            object.objArray[object.objNumber-1].plane.normal[2]);
            }
            // store the direction of the light
            /*else if ((strcmp(key, "direction") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
              object.objArray[object.objNumber-1].light.direction[0] = value[0];
              object.objArray[object.objNumber-1].light.direction[1] = value[1];
              object.objArray[object.objNumber-1].light.direction[2] = value[2];
              printf("Direction: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].light.direction[0],
                                                            object.objArray[object.objNumber-1].light.direction[1],
                                                            object.objArray[object.objNumber-1].light.direction[2]);
            }
            // store the diffuse_color of the sphere
            else if ((strcmp(key, "diffuse_color") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "sphere") == 0)) {
              object.objArray[object.objNumber-1].sphere.diffuse_color[0] = value[0];
              object.objArray[object.objNumber-1].sphere.diffuse_color[1] = value[1];
              object.objArray[object.objNumber-1].sphere.diffuse_color[2] = value[2];
              printf("Diffuse_color: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].sphere.diffuse_color[0],
                                                            object.objArray[object.objNumber-1].sphere.diffuse_color[1],
                                                            object.objArray[object.objNumber-1].sphere.diffuse_color[2]);
            }
            // store the position of the light
            else if ((strcmp(key, "position") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "light") == 0)) {
              object.objArray[object.objNumber-1].light.position[0] = value[0];
              object.objArray[object.objNumber-1].light.position[1] = value[1];
              object.objArray[object.objNumber-1].light.position[2] = value[2];
              printf("Position: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].light.position[0],
                                                              object.objArray[object.objNumber-1].light.position[1],
                                                              object.objArray[object.objNumber-1].light.position[2]);
            }
            // store the specular_color of the sphere
            else if ((strcmp(key, "specular_color") == 0) && (strcmp(object.objArray[object.objNumber-1].type, "sphere") == 0)) {
              object.objArray[object.objNumber-1].sphere.specular_color[0] = value[0];
              object.objArray[object.objNumber-1].sphere.specular_color[1] = value[1];
              object.objArray[object.objNumber-1].sphere.specular_color[2] = value[2];
              printf("Specular_color: [%lf, %lf, %lf]\n", object.objArray[object.objNumber-1].sphere.specular_color[0],
                                                            object.objArray[object.objNumber-1].sphere.specular_color[1],
                                                            object.objArray[object.objNumber-1].sphere.specular_color[2]);
            }*/
          } else {
            fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n", key, line);
            exit(1);
          }
          skip_ws(json);
        } else {
          fprintf(stderr, "Error: Unexpected value on line %d\n", line);
          exit(1);
        }
      }
      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
        skip_ws(json);
      } else if (c == ']') {
        fclose(json);
        return object;
      } else {
        fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
        exit(1);
      }
    }
  }
}
