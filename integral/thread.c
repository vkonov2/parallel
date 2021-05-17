#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "integral.h"

/* все параметры для простоты задаются константами */
static double a = 0.;	/* левый конец интервала */
static double b = 1.;	/* правый конец интервала */
static int n = 100000000;  /* число точек разбиения */

/* результат: интегррал */
static double total = 0;

/* объект типа mutex для синхронизации доступа к total */
static pthread_mutex_t total_mutex = PTHREAD_MUTEX_INITIALIZER;

/* общее число процессов */
static int p;

/* функция, работающая в задаче с номером my_rank */
void * process_function (void *pa)
{
	/* номер текущей задачи */
	int my_rank = (int) pa;
	/* длина отрезка интегрированияя текущего процесса */
	double len = (b - a) / p;
	/* число точек разбиение для текущего процесса */
	int local_n = n / p;
	/* левый конец интервала для текущего процесса */
	double local_a = a + my_rank * len;
	/* правый конец интервала для текущего процесса */
	double local_b = local_a + len;
	/* значение интеграла в текущем процессе */
	double integral;

	/* вычислить интеграл в каждой из задач */
	integral = integrate (local_a, local_b, local_n);

	/* "захватить" mutex для работы с total */
	pthread_mutex_lock (&total_mutex);

	/* сложить все ответы */
	total += integral;

	/* "освободить" mutex */
	pthread_mutex_unlock (&total_mutex);

	return 0;
}

void reduce_sum (int p, double *a, int n)
{
	/* mutex, который "защищает счетчики" */
	static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	/* входное условие */
	static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
	/* выходное условие */
	static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
	/* счетчик вошедших потоков*/
	static int t_in = 0;
	/* счетчик вышедших потоков*/
	static int t_out = 0;
	/* указатель на результат */
	static double * p_a = 0;
	/*локальная переменная в стеке*/
	int i; 

	if (p <= 1)
		return;

	pthread_mutex_lock (&m);

	if (!p_a)
		p_a = a;
	else
	{
		for (i = 0; i < n; i++)
			p_a[i] += a[i];
	}

	/* ожидание */
	t_in++;

	if (t_in >= p) // вошел последний
	{
		t_out = 0;

		pthread_cond_broadcast (&c_in);
	}
	else
	{
		while (t_in < p)
			pthread_cond_wait (&c_in, &m);
	}

	if (p_a != a)
	{
		for (i = 0; i < n; i++)
			a[i] = p_a[i];
	}

	/* выход */
	t_out++;

	if (t_out >= p) // выходит последний
	{
		t_in = 0;
		p_a = 0;

		pthread_cond_broadcast (&c_out);
	}
	else
	{
		while (t_out < p)
			pthread_cond_wait (&c_out, &m);
	}

	pthread_mutex_unlock (&m);

	return;
}

int main(int argc, char const *argv[])
{
	/* массив идентификаторов созданных задач */
	pthread_t * threads;
	int i;

	if (argc != 2)
	{
		printf(">>>usage: %s <instances>\n", argv[0]);
		return 1;
	}

	/* получаем количество процессов */
	p = (int) strtol (argv[1], 0, 10);

	if (!(threads = (pthread_t*) malloc (p * sizeof (pthread_t))))
	{
		fprintf(stderr, ">>>allocation error\n");
		return 1;
	}

	/* запускаем задаче */
	for (i = 0; i < p; i++)
	{
		if (pthread_create (threads + i, 0, process_function, (void*)i))
		{
			fprintf(stderr, ">>>can't create thread #%d!\n", i);
			return 2;
		}
	}

	/* ожидаем окончания задач */
	for (i = 0; i < p; i++)
	{
		if (pthread_join (threads[i], 0))
			fprintf(stderr, ">>>can't wait thread #%d!\n", i);
	}

	/* освобождаем память */
	free (threads);

	printf("integral from %lf to %lf = %.18lf\n", a, b, total);

	return 0;
}







