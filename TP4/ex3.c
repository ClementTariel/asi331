#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

long int gcd(long int a, long int b)
{
    long int temp;
    if (a == 0){
    	return b;
    }
    while (b != 0)
    {
        temp = a % b;
        a = b;
        b = temp;
    }
    return a;
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

long int fast_pow(long int a, long int b){
	long int result = 1;
	while (b>0)
    {
        if (b%2 != 0){
			result = result*a;
			b--;
		}
        b = b >> 1;
	    a = a*a;
    }
    return result;
}

long int* fact(long int n){
	long int* res = malloc(2*sizeof(long int));
	if (res == NULL){
		printf("error with malloc");
		return NULL;
	}
	res[0] = 1;
	res[1] = n;
	int primes_size = 1;
	long int* primes = malloc(primes_size*sizeof(long int));
	if (primes == NULL){
		printf("error with malloc");
		return NULL;
	}
	double* primes_log = malloc(primes_size*sizeof(double));
	if (primes_log == NULL){
		printf("error with malloc");
		return NULL;
	}
	primes[0] = 2;
	primes_log[0] = log(2);
	int prime_index = 0;
	long int q = 1;
	long int B = 1;
	double log_n = log(n);
	srand((unsigned)time(NULL));
	long int a = rand() + (((long int)rand()) << (8*sizeof(int)));
	a = a%n;
	q = gcd(a,n);
	//printf("gcd(a,n) = %ld)\n",q);
	if (q != 1){
		res[0] = q;
		res[1] = n/q;
		return res;
	}
	while(q == 1){
		B = B << 1;
		printf("B = %ld",B);
		while(primes[prime_index]<=B){
			prime_index ++;
			if (!(prime_index<primes_size)){
				primes_size = primes_size << 1;
				primes = realloc(primes, primes_size * sizeof(long int));
				if (primes == NULL){
					printf("error with realloc");
					return NULL;
				}
				primes_log = realloc(primes_log, primes_size * sizeof(double));
				if (primes_log == NULL){
					printf("error with realloc");
					return NULL;
				}
			}
			long int m = primes[prime_index-1];
			long int p = m+1;
			while (m < p){
				long int sqrt_p = (long int)ceil(sqrt(p));
				for (int i=0; i<prime_index; i++){
					if (p%primes[i]==0){
						//p is not a prime
						break;
					}
					if (primes[i]>=sqrt_p){
						//p is a prime
						m=p;
						p--;
						break;
					}
				}
				p++;
			}
			primes[prime_index] = p;
			if (n%p == 0){
				res[0] = p;
				res[1] = n/p;
				printf("\n");
				return res;
			}
			primes_log[prime_index] = log(p);
		}
		printf(" (%d primes found)\n",prime_index+1);

		long int a_Q = a;
		long int Q = 1;
		long int overflow_limit = ((long int)1 << (8*sizeof(long int)-3)) / n;
		for (int i=0; i<prime_index; i++){
			Q *= fast_pow(primes[i],(long int)(log_n/primes_log[i]));
			/*long int q_i=1;
			while(q_i*primes[i]<=n){
				q_i *= primes[i];
			}
			Q *= q_i;*/
			if (Q >= overflow_limit){
				a_Q = exp_mod(a_Q,Q,n);
				Q = 1;
			}
		}
		a_Q = exp_mod(a_Q,Q,n);
		q = gcd(a_Q-1+n,n);
		//printf("gcd(a_Q-1+n,n) = %ld)\n",q);
		if (q != 1 && q != n){
			res[0] = q;
			res[1] = n/q;
			return res;
		}
	}
	printf("B too big!\n");
	long int low_B = B >> 1;
	long int high_B = B;
	while(q == 1 || q == n){
		long int next_B = (high_B+low_B) >> 1;
		printf("B low : %ld, B high : %ld\n", low_B, high_B);
		printf("B = %ld\n",next_B);
		if (high_B == next_B || low_B == next_B){
			printf("fact not found\n");
			return res;
		}
		long int a_Q = a;
		long int Q = 1;
		long int overflow_limit = ((long int)1 << (8*sizeof(long int)-2)) / n;
		for (int i=0; i<prime_index; i++){
			if (primes[i] <= B){
				Q *= fast_pow(primes[i],(long int)(log_n/primes_log[i]));
				/*long int q_i=1;
				while(q_i*primes[i]<=n){
					q_i *= primes[i];
				}
				Q *= q_i;*/
				if (Q >= overflow_limit){
					a_Q = exp_mod(a_Q,Q,n);
					Q = 1;
				}
			}
		}
		a_Q = exp_mod(a_Q,Q,n);
		q = gcd(a_Q-1+n,n);
		if (q != 1 && q != n){
			res[0] = q;
			res[1] = n/q;
			return res;
		}
		
		if (q == n){
			printf("B too big!\n");
			high_B = next_B;
		}else{
			low_B = next_B;
		}
	}
	return res;
}

int main (){
	//long int n = 444853;
	//long int n = 2057574960;
	//long int n = 5270033701;
	long int n = 1593351640742417;
	long int* res = fact(n);
	printf("%ld = %ld*%ld\n",n,res[0],res[1]);
	return 0;
}