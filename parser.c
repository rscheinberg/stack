#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"
#include "stack.h"

/*======== void parse_file () ==========
Inputs:   char * filename
          struct stack * csystems
          struct matrix * edges,
          struct matrix * polygons,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:

     push: push a copy of the curent top of the coordinate system stack to the stack

     pop: remove the current top of the coordinate system stack

     All the shape commands work as follows:
        1) Add the shape to a temporary matrix
        2) Multiply that matrix by the current top of the coordinate system stack
        3) Draw the shape to the screen
        4) Clear the temporary matrix

         sphere: add a sphere to the POLYGON matrix -
                 takes 4 arguemnts (cx, cy, cz, r)
         torus: add a torus to the POLYGON matrix -
                takes 5 arguemnts (cx, cy, cz, r1, r2)
         box: add a rectangular prism to the POLYGON matrix -
              takes 6 arguemnts (x, y, z, width, height, depth)
         clear: clears the edge and POLYGON matrices

         circle: add a circle to the edge matrix -
                 takes 4 arguments (cx, cy, cz, r)
         hermite: add a hermite curve to the edge matrix -
                  takes 8 arguments (x0, y0, x1, y1, rx0, ry0, rx1, ry1)
         bezier: add a bezier curve to the edge matrix -
                 takes 8 arguments (x0, y0, x1, y1, x2, y2, x3, y3)
         line: add a line to the edge matrix -
               takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
         ident: set the transform matrix to the identity matrix -
         scale: create a scale matrix,
                then multiply the transform matrix by the scale matrix -
                takes 3 arguments (sx, sy, sz)
         move: create a translation matrix,
               then multiply the transform matrix by the translation matrix -
               takes 3 arguments (tx, ty, tz)
         rotate: create a rotation matrix,
                 then multiply the transform matrix by the rotation matrix -
                 takes 2 arguments (axis, theta) axis should be x y or z
         apply: apply the current transformation matrix to the edge and
                POLYGON matrices
         display: clear the screen, then
                  draw the lines of the edge and POLYGON matrices to the screen
                  display the screen
         save: clear the screen, then
               draw the lines of the edge and POLYGON matrices to the screen
               save the screen to a file -
               takes 1 argument (file name)
         quit: end parsing

See the file script for an example of the file format

IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct stack * csystems,
                  struct matrix * edges,
                  struct matrix * polygons,
                  screen s) {


  FILE *f;
  char line[256];
  clear_screen(s);

  if ( strcmp(filename, "stdin") == 0 ) 
    f = stdin;
  else
    f = fopen(filename, "r");
  
  //reads through file line by line
  while ( fgets(line, 255, f) != NULL ) {

    line[strlen(line)-1]='\0';

    double args[9];
    char new_line[64];

    //2d objects
    //test for line
    if(strcmp(line, "line") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3], &args[4], &args[5]);
      add_edge(edges, args[0], args[1],args[2], args[3], args[4], args[5]);
      matrix_mult(peek(csystems), edges);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_lines(edges, s, c);
      edges = new_matrix(4,1);
    }

    //test for circle
    else if(strcmp(line, "circle") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3]);
      add_circle(edges, args[0], args[1], args[2], args[3], 10);
      matrix_mult(peek(csystems), edges);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_lines(edges, s, c);
      edges = new_matrix(4,1);
    }
    
    //test for hermite
    else if(strcmp(line, "hermite") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf %lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3], &args[4], &args[5], &args[6], &args[7]);
      add_curve(edges, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], 10, HERMITE);
      matrix_mult(peek(csystems), edges);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_lines(edges, s, c);
      edges = new_matrix(4,1);
    }
    
    //test for bezier
    else if(strcmp(line, "bezier") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf %lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3], &args[4], &args[5], &args[6], &args[7]);
      add_curve(edges, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], 10, BEZIER);
      matrix_mult(peek(csystems), edges);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_lines(edges, s, c);
      edges = new_matrix(4,1);
    }


    //3d objects
    //test for box
    else if(strcmp(line, "box") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3], &args[4], &args[5]);
      add_box(polygons, args[0], args[1],args[2], args[3], args[4], args[5]);
      matrix_mult(peek(csystems), polygons);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_polygons(polygons, s, c);
      polygons = new_matrix(4, 1);
    }

    //test for sphere
    else if(strcmp(line, "sphere") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3]);
      add_sphere(polygons, args[0], args[1],args[2], args[3], 25);
      matrix_mult(peek(csystems), polygons);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_polygons(polygons, s, c);
      polygons = new_matrix(4, 1);
    }

    //test for torus
    else if(strcmp(line, "torus") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf %lf %lf", &args[0], &args[1], &args[2], &args[3], &args[4]);
      add_torus(polygons, args[0], args[1],args[2], args[3], args[4], 25);
      matrix_mult(peek(csystems), polygons);
      color c;
      c.red = 255;
      c.blue = 255;
      c.green = 255;
      draw_polygons(polygons, s, c);
      polygons = new_matrix(4, 1);
    }


    //relative coordinate system
    //push
    else if(strcmp(line, "push") == 0) push(csystems);

    //pop
    else if(strcmp(line, "pop") == 0) pop(csystems);


    //transformations
    //test for scale
    else if(strcmp(line, "scale") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf", &args[0], &args[1], &args[2]);
      struct matrix * scale = make_scale(args[0], args[1], args[2]);
      matrix_mult(peek(csystems), scale);
      copy_matrix(scale, peek(csystems));
    }

    //test for move/translate
    else if(strcmp(line, "translate") == 0 || strcmp(line, "move") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      sscanf(new_line, "%lf %lf %lf", &args[0], &args[1], &args[2]);
      struct matrix * translate = make_translate(args[0], args[1], args[2]);
      matrix_mult(peek(csystems), translate);
      copy_matrix(translate, peek(csystems));
    }
    
    //test for rotate
    else if(strcmp(line, "rotate") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      char axis;
      sscanf(new_line, "%c %lf", &axis, &args[0]);
      struct matrix *rot;
      if(axis == 'x') rot = make_rotX(args[0] * M_PI / 180);
      else if(axis == 'y') rot = make_rotY(args[0] * M_PI / 180);
      else rot = make_rotZ(args[0] * M_PI / 180);
      matrix_mult(peek(csystems), rot);
      copy_matrix(rot, peek(csystems));
    }


    //misc functions    
    //test for ident
    //OUT OF SERVICE
    else if(strcmp(line, "ident") == 0) printf("Ident function not supported\n"); 
      //ident(transform);

    //test for apply
    //OUT OF SERVICE
    else if(strcmp(line, "apply") == 0) {
      printf("Apply function not supported\n");
      //matrix_mult(transform, edges);
      //matrix_mult(transform, polygons);
    }
    
    //test for display
    else if(strcmp(line, "display") == 0) {
      display(s);
    }
    
    //test for save
    else if(strcmp(line, "save") == 0) {
      fgets(new_line, 64, f);
      new_line[strlen(new_line)-1]='\0';
      printf("%s\n", new_line);
      save_extension(s, new_line);
    }

    //test for comments
    else if(line[0] == '#');

    //test for clear
    //OUT OF SERVICE
    else if(strcmp(line, "clear") == 0) {
      printf("Clear function not supported\n");
      //edges = new_matrix(4,1);
      //polygons = new_matrix(4, 1);
    }
    
    //test for quit
    else if(strcmp(line, "quit") == 0)
    ;

  }
}
