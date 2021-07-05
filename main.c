#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const size_t INIT_SIZE = 5;

struct point
{
  float x;
  float y;
};

struct coefficient
{
  float value;
  float std;
};

struct point *load_data (FILE *stream, size_t *n)
{
  size_t size = INIT_SIZE;
  struct point *data = (struct point*) calloc (sizeof(struct point), size);

  if(!data)
  {
    fputs("Blad alokacji pamieci wewnatrz funkcji load_data", stderr);
    exit (1);
  }

  int i = 0;
  char buffer[100];
  fgets(buffer, 100, stream); //pominięcie pierwszej linii
  while (!feof (stream))
  {
    if(i >= size)
    {
      size = size + INIT_SIZE;
      data = (struct point*) realloc (data, size * sizeof(struct point));
    }
    fscanf(stream, "%f %f\n", &data[i].x, &data[i].y);
    i++;
  }
  size = i;
  data = (struct point*) realloc (data, size * sizeof(struct point));
  *n = size;
  return data;
}

void least_squares (const struct point data [], size_t n, struct coefficient *a, struct coefficient *b)
{
  float sx = 0, sy = 0, sxx = 0, syy = 0, sxy = 0;
  float sigma_y = 0;

  for (int i = 0; i < n; i++)
  {
    sx += data[i].x;
    sy += data[i].y;
    sxx += data[i].x*data[i].x;
    syy += data[i].y*data[i].y;
    sxy += data[i].x*data[i].y;
  }
  float delta = n*sxx - sx*sx;

  a->value = (n*sxy - sx*sy) / delta;
  b->value = (sxx*sy - sx*sxy) / delta;

  sigma_y = syy - a->value*sxy - b->value*sy;
  a->std = n*sigma_y / ((n-2)*delta);
  b->std = a->std *sxx/n;
  a->std = sqrt (a->std);
  b->std = sqrt (b->std);
}

void save_fit (FILE *stream, const struct point data[], size_t n, double a_value, double b_value)
{
  fprintf(stream, "x\tu\tf(x)\n");
  for(int i = 0; i < n; i++)
    {
      fprintf(stream, "%.2f\t%.2f\t %.2f\n", data[i].x, data[i].y, a_value*data[i].x+b_value);
    }
}


int main(void)
{
  size_t size = 0;
  struct coefficient a = {0,0};
  struct coefficient b  = {0,0};

  struct point *liczby = load_data (stdin, &size);
  least_squares(liczby, size, &a, &b);

  fprintf(stderr, "a = %.5f\tstd(a) = %.5f\nb = %.5f\tstd(b) = %.5g\n", a.value, a.std, b.value, b.std);

  save_fit(stdout, liczby, size, a.value, b.value);

  free(liczby);
  return 0;
}