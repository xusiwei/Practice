#include <stdio.h>
#include <string.h> // strdup

#include <unistd.h>
#include <pthread.h>
#include <sys/syscall.h>

pthread_t thread;

pthread_key_t key, key2;

pid_t gettid()
{
	return syscall(SYS_gettid);
}

void test_key(const char* msg)
{
	void* ptr = NULL;
	char* str = NULL;

	ptr = pthread_getspecific(key);
	if(NULL == ptr) {
		pthread_setspecific(key, strdup(msg));
	}
	
	str = (char*)pthread_getspecific(key);
	printf("[%d] get %s\n", gettid(), str);
}

void* foo(void* args)
{
	test_key("in foo()");
	return NULL;
}

void delete_key(void* value)
{
	printf("delete_key(%p) in [%d].\n", value, gettid());
}

int main(int argc, char **argv)
{
	void* ptr = NULL;
	char* str = NULL;

	printf("key: %d\n", key);
	pthread_key_create(&key, delete_key);
	pthread_key_create(&key2, delete_key);
	printf("key: %d, %d\n", key, key2);

	pthread_create(&thread, NULL, foo, NULL);

	test_key("in main()");	

	pthread_join(thread, NULL);
	pthread_key_delete(key);

	return 0;
}
