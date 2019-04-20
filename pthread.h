#define PGSIZE 4096

int thread_create(void (*func)(void*), void *arg);
int thread_join();
