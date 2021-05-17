#include <sys/time.h>
#include <sys/resource.h>
#include "get_time.h"

/* вернуть процессорное время, затраченное на текущий процесс, в сотых долях секунды; берется время только самого процесса, время работы системных вызовов не прибавлется */
long int get_time ()
{
	struct rusage buf;

	getrusage (RUSAGE_SELF, &buf);
			/* преобразуем время в секундах в сотые доли секунды */
	return 	buf.ru_utime.tv.sec * 100
			/* преобразуем время в микросекундах в сотые доли секунды */
			+ buf.ru_utime.tv_usec / 10000;
}

/* возвращает время в сотых долях секунды */
long int get_full_time ()
{
	struct timeval buf;

	gettimeofday (&buf, 0);
			/* преобразуем время в секундах в сотые доли секунды */
	return 	buf.tv_sec * 100
			/* преобразуем время в микросекундах в сотые доли секунды */
			+ buf.tv_usec / 10000;
}
