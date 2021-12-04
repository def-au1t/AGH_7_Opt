#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "dclock.c"

#define IDX(i, j, n) (((j) + (i) * (n)))
#define max(a, b) ((a > b) ? (a) : (b))

#ifdef __x86_64__
#include <x86intrin.h>
#else
#include <arm_neon.h>
typedef float64x2_t __m128d;
#define _mm_loadu_pd(a) vld1q_f64(a)
#define _mm_mul_pd(a, b) vmulq_f64(a, b)
#define _mm_sub_pd(a, b) vsubq_f64(a, b)
#define _mm_add_pd(a, b) vaddq_f64(a, b)
#define _mm_storeu_pd(a, b) vst1q_f64(a, b)
#endif

void LU(double *A, double *U, double *L, int size)
{
  register int i, j, k;
  register double sumU, sumL;
  register __m128d tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

  for (i = 0; i < size; i++)
  {
    for (j = i; j < size; j++)
    {
      sumU = 0;
      sumL = 0;
      for (k = 0; k < i;)
      {

        if (k < max(i - 8, 0))
        {
          tmp0 = _mm_loadu_pd(L + IDX(i, k, size));
          tmp2 = _mm_loadu_pd(L + IDX(i, k + 2, size));
          tmp4 = _mm_loadu_pd(L + IDX(i, k + 4, size));
          tmp6 = _mm_loadu_pd(L + IDX(i, k + 6, size));

          tmp1 = _mm_loadu_pd(U + IDX(j, k, size));
          tmp3 = _mm_loadu_pd(U + IDX(j, k + 2, size));
          tmp5 = _mm_loadu_pd(U + IDX(j, k + 4, size));
          tmp7 = _mm_loadu_pd(U + IDX(j, k + 6, size));

          tmp0 = _mm_mul_pd(tmp0, tmp1);
          tmp2 = _mm_mul_pd(tmp2, tmp3);
          tmp4 = _mm_mul_pd(tmp4, tmp5);
          tmp6 = _mm_mul_pd(tmp6, tmp7);

          tmp0 = _mm_add_pd(tmp0, tmp2);
          tmp4 = _mm_add_pd(tmp4, tmp6);

          tmp0 = _mm_add_pd(tmp0, tmp4);

          sumU += tmp0[0] + tmp0[1];

          if (i != j)
          {
            tmp0 = _mm_loadu_pd(L + IDX(j, k, size));
            tmp2 = _mm_loadu_pd(L + IDX(j, k + 2, size));
            tmp4 = _mm_loadu_pd(L + IDX(j, k + 4, size));
            tmp6 = _mm_loadu_pd(L + IDX(j, k + 6, size));

            tmp1 = _mm_loadu_pd(U + IDX(i, k, size));
            tmp3 = _mm_loadu_pd(U + IDX(i, k + 2, size));
            tmp5 = _mm_loadu_pd(U + IDX(i, k + 4, size));
            tmp7 = _mm_loadu_pd(U + IDX(i, k + 6, size));

            tmp0 = _mm_mul_pd(tmp0, tmp1);
            tmp2 = _mm_mul_pd(tmp2, tmp3);
            tmp4 = _mm_mul_pd(tmp4, tmp5);
            tmp6 = _mm_mul_pd(tmp6, tmp7);

            tmp0 = _mm_add_pd(tmp0, tmp2);
            tmp4 = _mm_add_pd(tmp4, tmp6);

            tmp0 = _mm_add_pd(tmp0, tmp4);

            sumL += tmp0[0] + tmp0[1];
          }

          k += 8;
        }
        else
        {
          sumU += (L[IDX(i, k, size)] * U[IDX(j, k, size)]);
          if (i != j)
          {
            sumL += (L[IDX(j, k, size)] * U[IDX(i, k, size)]);
          }
          k++;
        }
      }

      L[IDX(j, i, size)] = i == j ? 1 : (A[IDX(j, i, size)] - sumL) / U[IDX(i, i, size)];

      U[IDX(j, i, size)] = A[IDX(i, j, size)] - sumU;
    }
  }
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
    LU(A[0], U[0], L[0], size);
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
