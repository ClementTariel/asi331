#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long int* fact(long int n){
	long int* res = malloc(2*sizeof(long int));
	if (res == NULL){
		printf("error with malloc");
		return NULL;
	}
	res[0] = 1;
	res[1] = n;
	long int sqrt_n = (int)ceil(sqrt(n));
	for (int s=1 ; s<= sqrt_n ; s++){
		long int t = (int)(sqrt(n+s*s));
		if (t*t == n+s*s){
			res[0] = t+s;
			res[1] = t-s;
			return res;
		}
	}
	return res;
}

int main (){
	long int n = 23360947609;
	long int* res = fact(n);
	printf("%ld = %ld*%ld\n",n,res[0],res[1]);
	return 0;
}