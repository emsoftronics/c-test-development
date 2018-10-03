#include <stdio.h>
#include <unistd.h>

#define DBG_ENABLE
#include "macros.h"

int main()
{
int i, a=3,b=5,c=4, m =0;
for (i=0;i<101;i++){
	_progress(i,'_','#');
	sleep(1);
}
	m=max(a,b,c);
	printf("\nmax = %d\n",m);
	_printv(i,d);
return 0;
}
