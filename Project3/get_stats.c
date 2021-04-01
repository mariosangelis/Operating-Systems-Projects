#include <stdio.h>
#include "stats_wrapper.h"

int main(int argc, char* argv[]) {

	printf("allocated memory: %ld\n",slob_allocator_get_total_memory_alloc_wrapper());
	printf("free memory: %ld\n", slob_allocator_get_total_free_memory_wrapper());
	return 0;
}
