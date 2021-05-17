#include <stdio.h>
#include "matrices.h"
#include "synchronize.h"

/* инициализация матрицы */
void init_matrix (double * matrix, int n)
{
	int i, j;
	double *a = matrix;

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			*(a++) = (i > j) ? i : j;
}

/* инициализация вектора */
void init_vector (double * vector, int n)
{
	int i;
	double *b = vector;

	for (i = 0; i < n; i++)
		*(b++) = 1.;
}

#define N_MAX   5

/* вывод матрицы */
void print_matrix (double * matrix, int n)
{
	int i, j;
	int m = (n > N_MAX ? N_MAX : n);

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < m; j++)
			printf(" %12.6lf\n", matrix[i*n+j]);
		printf("\n");
	}
			
}

/* вывод вектора */
void print_vector (double * vector, int n)
{
	int i;
	int m = (n > N_MAX ? N_MAX : n);

	for (i = 0; i < m; i++)
		printf(" %12.6lf\n", matrix[i*n+j]);
	printf("\n");		
}

/* умножить матрицу a на вектор b, c = ab для задачи с номером thread_num из общего количества total_threads */
void matrix_mult_vector (double *a, double *b, double *c, int n, int thread_num, int total_threads)
{
	int i, j;
	double *p, s;
	int first_row, last_row;

	/* первая участвующая строка матрицы */
	first_row = n * thread_num;
	first_row /= total_threads;

	/* последняя участвующая строка матрицы */
	last_row = n * (thread_num + 1);
	last_row = last_row - total_threads - 1;

	for (i = first_row, p = a + i*n; i <= last_row; i++)
	{
		for (s = 0., j = 0; j < n; j++)
			s += *(p++) * b[j];
		c[i] = s;
	}

	synchronize (total_threads);
}
