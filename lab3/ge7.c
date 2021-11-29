//requires additional changes to the code to make it work

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <immintrin.h>

static double gtod_ref_time_sec = 0.0;

#define max(a, b) ((a > b) ? (a) : (b))

#define IDX(i, j, n) (((j) + (i) * (n)))

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

int ge(double *A, int SIZE)
{
  register unsigned int i, j, k;
  register double multiplier;
  register __m256d mm_multiplier;
  register __m256d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  for (k = 0; k < SIZE; k++)
  {
    for (i = k + 1; i < SIZE; i++)
    {
      multiplier = (A[IDX(i, k, SIZE)] / A[IDX(k, k, SIZE)]);
      mm_multiplier[0] = multiplier;
      mm_multiplier[1] = multiplier;
      for (j = k + 1; j < SIZE;)
      {
        if (j < max(SIZE - 8, 0))
        {
          tmp0 = _mm256_loadu_pd(A + IDX(i, j, SIZE));
          tmp4 = _mm256_loadu_pd(A + IDX(i, j + 4, SIZE));

          tmp1 = _mm256_loadu_pd(A + IDX(k, j, SIZE));
          tmp5 = _mm256_loadu_pd(A + IDX(k, j + 4, SIZE));

          tmp1 = _mm256_mul_pd(tmp1, mm_multiplier);
          tmp5 = _mm256_mul_pd(tmp5, mm_multiplier);

          tmp0 = _mm256_sub_pd(tmp0, tmp1);
          tmp4 = _mm256_sub_pd(tmp4, tmp5);

          _mm256_storeu_pd(A + IDX(i, j, SIZE), tmp0);
          _mm256_storeu_pd(A + IDX(i, j + 4, SIZE), tmp4);
          j += 8;
        }
        else
        {
          A[IDX(i, j, SIZE)] = A[IDX(i, j, SIZE)] - A[IDX(k, j, SIZE)] * multiplier;
          j++;
        }
      }
    }
  }
  return 0;
}

int main(int argc, const char *argv[])
{
  int i, j, k, iret;
  double dtime;
  int SIZE = 1500;
  double **matrix = (double **)malloc(SIZE * sizeof(double *));
  // TODO - make near optimal dynamic allocation
  double *matrix_ = (double *)malloc(SIZE * SIZE * sizeof(double));

  //allocate 2D matrices
  //set pointers to continous blocks
  for (i = 0; i < SIZE; i++)
  {
    matrix[i] = matrix_ + i * SIZE;
  }

  srand(1);
  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      matrix[i][j] = rand();
    }
  }
  printf("call GE");
  dtime = dclock();
  iret = ge(matrix[0], SIZE);
  dtime = dclock() - dtime;
  printf("Time: %le \n", dtime);

  double check = 0.0;
  for (i = 0; i < SIZE; i++)
  {
    for (j = 0; j < SIZE; j++)
    {
      check = check + matrix[i][j];
    }
  }
  printf("Check: %le \n", check);
  fflush(stdout);

  return iret;
}
