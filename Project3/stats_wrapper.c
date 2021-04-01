#include <sys/syscall.h>
#include <unistd.h>
#include "stats_wrapper.h"

long slob_allocator_get_total_memory_alloc_wrapper(void){
	return( syscall(__NR_slob_total_mem_alloc));
}
long slob_allocator_get_total_free_memory_wrapper(void){
	return( syscall(__NR_slob_total_free_space));
}

