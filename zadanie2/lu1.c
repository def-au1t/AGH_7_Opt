#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "dclock.c"

void LU(double **A, double **U, double **L, int size)
{

  for (int i = 0; i < size; i++)
  {
    for (int j = i; j < size; j++)
    {
      double sum = 0;
      for (int k = 0; k < i; k++)
      {
        sum += (L[i][k] * U[k][j]);
      }

      U[i][j] = A[i][j] - sum;
    }

    for (int j = i; j < size; j++)
    {
      if (i == j)
      {
        L[i][i] = 1;
      }
      else
      {
        double sum = 0;
        for (int k = 0; k < i; k++)
        {
          sum += (L[j][k] * U[k][i]);
        }

        L[j][i] = (A[j][i] - sum) / U[i][i];
      }
    }
  }

  int i, j;
}

int main(int argc, const char *argv[])
{
  if (argc != 4)
  {
    printf("Usage: %s <version> <size> <iterations>\n", argv[0]);
    return 1;
  }

  const char *version = argv[1];
  int size = atoi(argv[2]);
  int iterations = atoi(argv[3]);
  int i, j;

  double *A_ = (double *)calloc(size * size, sizeof(double));
  double *L_ = (double *)calloc(size * size, sizeof(double));
  double *U_ = (double *)calloc(size * size, sizeof(double));

  double **A = (double **)calloc(size, sizeof(double *));
  double **L = (double **)calloc(size, sizeof(double *));
  double **U = (double **)calloc(size, sizeof(double *));

  for (size_t l = 0; l < size; l++)
  {
    A[l] = A_ + l * size;
    L[l] = L_ + l * size;
    U[l] = U_ + l * size;
  }

  srand(1);

  for (i = 0; i < size; i++)
  {
    for (j = 0; j < size; j++)
    {
      A[i][j] = rand() / 1000;
    }
  }
  double dtime;
  double best_time = 1000000;

  for (i = 0; i < iterations; i++)
  {
    dtime = dclock();
    LU(A, U, L, size);
    dtime = dclock() - dtime;
    if (dtime < best_time || i == 0)
    {
      best_time = dtime;
    }
  }

  double check = 0.0;
  for (i = 0; i < size; i++)
  {
    for (j = 0; j < size; j++)
    {
      check = check + L[i][j];
      check = check + U[i][j];
    }
  }

  printf("%s,%d,%lf,%lf,%lf\n", version, size, best_time, 2.0e-9 * size * size * size / (3 * best_time), check);
  fflush(stdout);
  return 0;
}
