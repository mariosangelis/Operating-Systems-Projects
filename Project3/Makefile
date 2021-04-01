all: consume_memory get_stats

consume_memory: 
	gcc -Wall -g consume_memory.c -o consume_memory

get_stats: stats_wrapper.o libstats_wrapper.a
	gcc get_stats.c -o get_stats -lstats_wrapper -L.

stats_wrapper.o: stats_wrapper.c stats_wrapper.h
	gcc -Wall -c stats_wrapper.c -o stats_wrapper.o

libstats_wrapper.a: stats_wrapper.o
	ar rcs libstats_wrapper.a stats_wrapper.o

clean:
	rm -f consume_memory get_stats *.o *.a
