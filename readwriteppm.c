// File readwriteppm.c
#include "a4.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// Read and write PPM file

/* die gracelessly */

static void die(char *message)
{
  fprintf(stderr, "ppm: %s\n", message);
  exit(1);
}

/* check a dimension (width or height) from the image file for reasonability */

static void checkDimension(int dim)
{
  if (dim < 1 || dim > 4000) 
    die("file contained unreasonable width or height");
}


/* read a header: verify format and get width and height */

static void readPPMHeader(FILE *fp, int *width, int *height, int *maxval)
{
  char ch;

  if (fscanf(fp, "P%c\n", &ch) != 1 || ch != '3') 
    die("file is not in ppm ASCII format; cannot read");

  /* skip comments */
  ch = getc(fp);
  while (ch == '#')
  {
    do {
      ch = getc(fp);
    } while (ch != '\n');	/* read to the end of the line */
    ch = getc(fp);            /* thanks, Elliot */
  }

  if (!isdigit(ch)) die("cannot read header information from ppm file");

  ungetc(ch, fp);		/* put that digit back */

  /* read the width, height, and maximum value for a pixel */
  fscanf(fp, "%d%d%d\n", width, height, maxval);

  checkDimension(*width);
  checkDimension(*height);
}

PPM_IMAGE *read_ppm(const char *file_name)
{
    int width, height, maxval, num, size;
    unsigned char *p;

    PPM_IMAGE *image = (PPM_IMAGE *) malloc(sizeof(PPM_IMAGE));
    FILE  *fp    = fopen(file_name, "r");

    if (!image) die("cannot allocate memory for new image");
    if (!fp)    die("cannot open file for reading");

    readPPMHeader(fp, &width, &height, &maxval);

    size              = width * height;
    image->data       = (PIXEL *) malloc(size * sizeof(PIXEL));
    image->width      = width;
    image->height     = height;
    image->max_color  = maxval;

    if (!image->data) die("cannot allocate memory for new image");

    num = fread((void *) image->data, sizeof(PIXEL), (size_t) size, fp);

    if (num != size) die("cannot read image data from file");

    fclose(fp);

    return image;
}

void write_ppm(const char *file_name, const PPM_IMAGE *image)
{
  int num;
  int size = image->width * image->height; // * 3

  FILE *fp = fopen(file_name, "w");

  if (!fp) die("cannot open file for writing");

  fprintf(fp, "P6\n%d %d\n%d\n", image->width, image->height, image->max_color);

  num = fwrite((void *) image->data, sizeof(PIXEL), (size_t) size, fp);

  if (num != size) die("cannot write image data to file");

  fclose(fp);
}

