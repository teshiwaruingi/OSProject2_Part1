shm_proc: shm_processes.c
	gcc shm_processes.c -D_SVID_SOURCE -pthread -std=c99 -lpthread  -o shm_proc
example: example.c
	gcc example.c -pthread -std=c99 -lpthread  -o example
shared_memory: shared_memory.c
	gcc shared_memory.c -D_SVID_SOURCE -pthread -std=c99 -lpthread  -o shared_memory