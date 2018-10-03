

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#ifdef ARM_LINUX
#define DL_LIBM "/usr/lib/libm.so"
#else
#define DL_LIBM "/usr/lib/x86_64-linux-gnu/libm.so"
#endif

typedef double (*do_func_di)(double);

int main(int argc, char *argv[])
{
	void *sym = NULL;
	double output, input;
	do_func_di squareroot = NULL;

	if (argc < 2) return -1;
	else input = strtod(argv[1], &sym);

	sym = dlopen(DL_LIBM, RTLD_LAZY);
	if (!sym) {
		printf("%s\n", dlerror());
		return -1;
	}
	dlerror();
	squareroot = dlsym(sym,"sqrt");
	if (!squareroot) {
		printf("%s\n", dlerror());
		return -1;
	}
	dlerror();
	output = squareroot(input);
	printf(" ____\n");
	printf("V %.3lf = %.3lf\n", input, output);
	return 0;	
}
