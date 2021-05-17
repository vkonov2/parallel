#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrices.h"
#include "get_time.h"

/* аргументы для потока */
typedef struct _ARGS
{
	double *matrix;		/* матрица */
	double *vector;		/* вектор */
	double *result;		/* результирующий вектор */
	int n;				/* размер матрицы и векторов */
	int thread_num; 	/* номер задачи */
	int total_threads;	/* общее число задач */
} ARGS;

/* суммарное время работы всех задач */
static long int threads_total_time = 0;
/* сихранизатор для доступа к threads_total_time */
static pthread_mutex_t threads_total_time_mutex = PTHREAD_MUTEX_INITIALIZER;

/* количество тестов (для отладки) */
#define N_TESTS 10

/* умножение матрицы на вектор для одной задачи */
void * matrix_mult_vector_threaded (void *pa)
{
	ARGS *pargs = (ARGS*)pa;
	long int t;
	int i;

	printf(">>>thread %d started\n", pargs->thread_num);

	t = get_time ();	/* время начала работы */

	for (i = 0; i < N_TESTS; i++)
	{
		matrix_mult_vector (pargs->matrix, pargs->vector, pargs->result, pargs->n, pargs->thread_num, pargs->total_threads);

		printf(">>>thread %d mult %d times\n", pargs->thread_num, i);
	}

	t = get_time () - t;	/* время конца работы */

	/* суммируем времена работы */
	/* "захватить" mutex для работы с threads_total_time */
	pthread_mutex_lock (&threads_total_time_mutex);

	printf(">>>thread %d finished, time = %ld\n", pargs->thread_num, t);

	return 0;
}

int main (int argc, char const *argv[])
{
	/* массив идентификаторов созданных задач */
	pthread_t * threads;
	/* массив аргументов для созданных задач */
	AGRS * args;
	/* число созданных задач */
	int nthreads;
	/* время работы всего процесса */
	long int t_full;

	int n;				/* размер матрицы и векторов */
	double *matrix;		/* матрица */
	double *vector;		/* вектор */
	double *result;		/* результирующий вектор */
	int i, l;

	if (argc != 2)
	{
		printf(">>>usage: %s <instances>\n", argv[0]);
		return 1;
	}

	/* получаем количество процессов */
	nthreads = (int) strtol (argv[1], 0, 10);

	if (!(threads = (pthread_t*) malloc (nthreads * sizeof (pthread_t))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 1;
	}

	if (!(args = (ARGS*) malloc (nthreads * sizeof (ARGS))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 2;
	}

	printf(">>>matrix size = ");
	scanf ("%d", &n);

	/* выделение памяти под массивы */
	if (!(matrix = (double*) malloc (n * n * sizeof (double))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 3;
	}

	if (!(vector = (double*) malloc (n * sizeof (double))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 4;
	}

	if (!(result = (double*) malloc (n * sizeof (double))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 5;
	}

	/* инициализация массивов */
	init_matrix (matrix, n);
	init_vector (vector, n);
	printf(">>>matrix:\n");
	print_matrix (matrix, n);
	printf(">>>vector:\n");
	print_vector (vector, n);

	l = (n * n + 2 * n) * sizeof (double);
	printf(">>>allocated %d bytes (%dKb or %dMb) of memory\n", l, l >> 10, l >> 20);

	/* инициализация аргументов задач */
	for (i = 0; i < nthreads; i++)
	{
		args[i].matrix = matrix;
		args[i].vector = vector;
		args[i].result = result;
		args[i].n = n;
		args[i].thread_num = i;
		args[i].total_threads = nthreads;
	}

	/* засекаем время с начала работы задач */
	t_full = get_full_time ();

	/* запускаем задачи */
	for (i = 0; i < nthreads; i++)
	{
		if (pthread_create (threads + i, 0, matrix_mult_vector_threaded, args + i))
		{
			fprintf(stderr, ">>>can't create thread #%d!\n", i);
			return 10;
		}
	}

	/* ожидаем окончания задач */
	for (i = 0; i < nthreads; i++)
	{
		if (pthread_join (threads[i], 0))
			fprintf(stderr, ">>>can't wait thread #%d!\n", i);
	}

	t_full = get_full_time () - t_full;

	if (t_full == 0)
		t_full = 1; /* очень быстрый компьютер... */

	/* здесь можно работать с результатом */
	print_vector (result, n);

	/* освобождаем память */
	free (threads);
	free (args);
	free (matrix);
	free (vector);
	free (result);

	printf(">>>total full time = %ld, \
		total threads time = %ld (%ld%%), per thread = %ld\n", t_full, threads_total_time, (threads_total_time * 100) / t_full, threads_total_time / nthreads);

	return 0;
}







