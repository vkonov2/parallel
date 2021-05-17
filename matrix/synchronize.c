#include <pthread.h>
#include "synchronize.h"

/* в текущем потоке ждем другие потоки из общего числа total_threads */
void synchronize (int total_threads)
{
	/* объекты синхронизации */
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
	static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
	/* число пришедших задач */
	static int threads_in = 0;
	/* число ожидающих выхода задач */
	static int threads_out = 0;

	if (total_threads <= 1)
		return;

	/* "захват" mutex для работы с threads_in и threads_out */
	pthread_mutex_lock (&mutex);

	/* увеличение количества прибывших задач */
	threads_in++;

	/* проверяем количество прибывших задач */
	if (threads_in >= total_threads)
	{
		/* текущий поток пришел последним */
		/* устанавливаем начальное значение для threads_out */
		threads_out = 0;

		/* разрешаем остальным продолжать работу */
		pthread_cond_broadcast (&condvar_in);
	}
	else 
	{
		/* есть еще не пришедшие потоки */

		/* ожидаем, пока не придут все потоки */
		while (threads_in < total_threads)
		{
			/* ожидаем разрешения продолжить работу: освободить mutex и ждать сигнала condvar, затем "захватить" mutex опять */
			pthread_cond_wait (&condvar_in, &mutex);
		}
	}

	/* увеличение количества ожидающих выхода задач */
	threads_out++;

	/* проверяем количество прибывших задач */
	if (threads_out >= total_threads)
	{
		/* текущий поток пришел в очередь последним */

		/* устанавливаем начальное значение для threads_in */
		threads_in = 0;

		/* разрешаем остальным продолжать работу */
		pthread_cond_broadcast (&condvar_out);
	}
	else
	{
		/* в очереди ожидания есть еще потоки */

		/* ожидаем, пока в очередь ожидания не придет последний поток */
		while (threads_out < total_threads)
		{
			/* ожидаем разрешения продолжить работу: освободить mutex и ждать сигнала от condvar, затем "захватить" mutex опять */
			pthread_cond_wait (&condvar_out, &mutex);
		}
	}

	/* "освободить" mutex */
	pthread_mutex_unlock (&mutex);
}





