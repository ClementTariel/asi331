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
	long int inv_y = res[1]*res[0];
	free(res);
	return inv_y;
}

long int log_pollard(long int alpha, long int beta, long int n, long int sn){
	long int x_i = 1;
	long int x_2i = 1;
	long int a_i = 0;
	long int a_2i = 0;
	long int b_i = 0;
	long int b_2i = 0;
	int i = 0;

	while (x_i != x_2i || i==0){
		i++;
		switch (x_i%3){
			case 1://S1
				x_i = (beta*x_i)%n;
				a_i = a_i;
				b_i = (b_i+1)%sn;
				break;
			case 0://S2
				x_i = (x_i*x_i)%n;
				a_i = (2*a_i)%sn;
				b_i = (2*b_i)%sn;
				break;
			case 2://S3
				x_i = (alpha*x_i)%n;
				a_i = (a_i+1)%sn;
				b_i = b_i;
				break;
		}
		for (int j=0; j<2; j++){
			switch (x_2i%3){
				case 1://S1
					x_2i = (beta*x_2i)%n;
					a_2i = a_2i;
					b_2i = (b_2i+1)%sn;
					break;
				case 0://S2
					x_2i = (x_2i*x_2i)%n;
					a_2i = (2*a_2i)%sn;
					b_2i = (2*b_2i)%sn;
					break;
				case 2://S3
					x_2i = (alpha*x_2i)%n;
					a_2i = (a_2i+1)%sn;
					b_2i = b_2i;
					break;
			}
		}
		printf("i=%d, x_i = %ld, a_i = %ld, b_i = %ld, x_2i = %ld, a_2i = %ld, b_2i = %ld\n",i,x_i,a_i,b_i,x_2i,a_2i,b_2i);
	}
	
	return ((a_2i-a_i)*inv_euclide_etendu((b_i-b_2i),sn))%sn;
}

int main (){
	long int n = 383;
	long int sn = 191;
	long int alpha = 2;
	long int beta = 228;

	long int res = log_pollard(alpha, beta, n, sn);

	printf("log_pollard(%ld, %ld, %ld, %ld) = %ld\n",alpha,beta,n,sn,res);

	return 0;
}