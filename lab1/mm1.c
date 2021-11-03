#include <stdio.h>
#include <stdlib.h>

#define SIZE 1100

#include <sys/time.h>
#include <time.h>

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
  double the_time, norm_sec;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  if (gtod_ref_time_sec == 0.0)
    gtod_ref_time_sec = (double)tv.tv_sec;
  norm_sec = (double)tv.tv_sec - gtod_ref_time_sec;
  the_time = norm_sec + tv.tv_usec * 1.0e-6;
  return the_time;
}

int mm(double **first, double **second, double **multiply, int size)
{
  int i, j, k;
  double sum = 0;
  for (i = 0; i < size; i++)
  { //rows in multiply
    for (j = 0; j < size; j++)
    { //columns in multiply
      for (k = 0; k < size; k++)
      { //columns in first and rows in second
        sum = sum + first[i][k] * second[k][j];
      }
      multiply[i][j] = sum;
      sum = 0;
    }
  }
  return 0;
}

int main(int argc, const char *argv[])
{
  int i, j, iret;
  double *first_ = (double **)malloc(SIZE * SIZE * sizeof(double));
  double *second_ = (double **)malloc(SIZE * SIZE * sizeof(double));
  double *multiply_ = (double **)malloc(SIZE * SIZE * sizeof(double));

  double **first = (double **)malloc(SIZE * sizeof(double *));
  double **second = (double **)malloc(SIZE * sizeof(double *));
  double **multiply = (double **)malloc(SIZE * sizeof(double *));

  for (size_t l = 0; l < SIZE; l++)
  {
    first[l] = first_ + l * SIZE;
    second[l] = second_ + l * SIZE;
    multiply[l] = multiply_ + l * SIZE;
  }

  double dtime;
  for (i = 0; i < SIZE; i++)
  { //rows in first
    for (j = 0; j < SIZE; j++)
    { //columns in first
      first[i][j] = i + j;
      second[i][j] = i - j;
    }
  }
  dtime = dclock();
  iret = mm(first, second, multiply, SIZE);
  dtime = dclock() - dtime;
  printf("Time: %le \n", dtime);
  /*  for(i=0;i<SIZE;i++){
    for(j=0;j<SIZE;j++){
      printf("A[%d][%d]=%le \n",i,j,multiply[i][j]); 
    }
    }*/

  fflush(stdout);
  free(first);
  free(second);
  free(multiply);
  free(first_);
  free(second_);
  free(multiply_);
  return iret;
}
