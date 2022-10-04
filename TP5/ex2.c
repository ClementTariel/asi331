#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long int * bezout(long int a, long int b){
	long int u0 = 0;
	long int u1 = 1;
	long int v0 = 1;
	long int v1 = 0;
	long int q;
	while (a != 0){
		long int b_temp = b;
		q = b_temp/a;
		b = a;
		a = b_temp%a;
		long int v1_temp = v1;
		v1 = v0 - q*v1_temp;
		v0 = v1_temp;
		long int u1_temp = u1;
		u1 = u0 - q*u1_temp;
		u0 = u1_temp;
	}
	long int* res = malloc(3*sizeof(long int));
	if (res == NULL){
		printf("error with malloc");
		return NULL;
	}
	res[0] = b;
	res[1] = u0;
	res[2] = v0;
	printf("a=%ld, b=%ld, u0=%ld, u1=%ld, v0=%ld, v1=%ld\n",a,b,u0,u1,v0,v1);
	return res;
}

long int inv_euclide_etendu(long int y, long int p){
	long int * res = bezout(y,p);
	if (res[0]*res[0] != 1){
		printf("error gcd(%ld,%ld)!=1\n",y,p);
		return 0;
	}
	printf("inv(%ld) : %ld\n",y,res[1]);
	return res[1]*res[0];
}

long int exp_mod(long int a, long int b, long int n){
	if (n < 2){
		return 0;
	}
	long int result = 1;
	a = a%n;
	while (b>0)
    {
        if (b%2 != 0){
			result = (result*a)%n;
			b--;
		}
        b = b >> 1;
        a = (a*a)%n;
    }
    return result;
}

long int log_shanks(long int alpha, long int beta, long int n, long int sn){
	if (beta == 1){
		return 0;
	}
	long int m = (long int)ceil(sqrt(sn));
	long int* BS = malloc(m*sizeof(long int));
	if (BS == NULL){
		printf("error with malloc");
		return -1;
	}
	BS[0] = 1;
	for (int i=1; i<m; i++){
		BS[i] = (BS[i-1]*alpha)%n;
		if (BS[i] == beta){
			free(BS);
			return i;
		}
	}
	long int alpha_m = exp_mod(alpha, m, n);
	long int inv_alpha_m = inv_euclide_etendu(alpha_m, n);
	long int GS_i = beta;
	for (int i=1; i<m; i++){
		GS_i = (GS_i*inv_alpha_m)%n;
		for (int j=0; j<m; j++){
			if (BS[j] == GS_i){
				free(BS);
				return i*m+j;
			}
		}
	}
	printf("log_shanks failed\n");
	free(BS);
	return -1;
}

int main (){
	
	/*long int n = 383;
	long int sn = 191;
	long int alpha = 2;
	long int beta = 228;*/

	long int n = 113;
	long int sn = 113;
	long int alpha = 3;
	long int beta = 31;

	long int res = log_shanks(alpha, beta, n, sn);

	printf("log_shanks(%ld, %ld, %ld, %ld) = %ld\n",alpha,beta,n,sn,res);

	return 0;
}