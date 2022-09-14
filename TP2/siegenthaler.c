#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

struct LFSR
{
    bool* p;
    bool* state;
    int L;
};

bool increment_LFSR(struct LFSR* pLFSR){
	bool output_bit = (pLFSR->state)[pLFSR->L-1];
	//printf("%d ",output_bit);
	for (int i=pLFSR->L-1; i>0; i--){
		(pLFSR->state)[i] = (pLFSR->state)[i-1]^(output_bit&(pLFSR->p)[i]);
		//printf("%d",(pLFSR->state)[i]);
	}
	(pLFSR->state)[0] = output_bit;
	//printf("%d\n",(pLFSR->state)[0]);
	return output_bit;
}

float* compute_p(bool* f, size_t f_size, int n){
	float* p = malloc(sizeof(float) * n);
	if (p == NULL){
		printf("error with malloc\n");
		return NULL;
	}
	for (int i=0 ; i< n; i++){
		p[i] = 0.0;
	}
	for (int j=0 ; j< (1 << n); j++){
		for (int i=0 ; i< n; i++){
			if ((j >> (n-1-i))%2 == f[j]){
				p[i]+=1;
			}
		}
	}
	for (int i=0 ; i< n; i++){
		p[i] = p[i]/(1 << n);
	}
	return p;
}

int compute_alpha(bool* key, size_t N, struct LFSR* pLFSR){
	int alpha = 0;
	for (int i=0 ; i< N; i++){
		if (increment_LFSR(pLFSR) == key[i]){
			alpha++;	
		}else{
			alpha--;
		}
	}
	return alpha;
}

int Nmin(int Li, float pi){
	if (pi == 0.5f){
		return 0;
	}
	return pow((sqrt(log(1<<(Li-1)))+3*sqrt(2*pi*(1-pi)))/(sqrt(2)*(0.5f-pi)),2);
}

int threshold(int Li, float pi,int N){
	if (pi == 0.5f){
		return 0;
	}
	float Ti = N*(2*pi-1);
	if (Ti>0){
		Ti -= 6*sqrt(N*pi*(1-pi));
		return (int) floor(Ti);
	}else{
		Ti += 6*sqrt(N*pi*(1-pi));
		return (int) ceil(Ti);
	}
	
}

int main (){
	bool f[] = {0,1,0,0,0,1,0,0,1,1,0,1,1,1,0,1};
	size_t f_size = sizeof(f);
	size_t n = 4;
	struct LFSR* LFSRs[4];
	for (int i=0; i<n; i++){
		LFSRs[i] = malloc(sizeof(struct LFSR));
		if (LFSRs[i] == NULL){
			printf("error with malloc\n");
			return -1;
		}
	}
	LFSRs[0]->L = 7;
	LFSRs[1]->L = 6;
	LFSRs[2]->L = 9;
	LFSRs[3]->L = 8;
	for (int i=0; i<n; i++){
		LFSRs[i]->p = calloc(LFSRs[i]->L+1,sizeof(bool));
		if (LFSRs[i]->p  == NULL){
			printf("error with calloc\n");
			return -1;
		}
		LFSRs[i]->state = calloc(LFSRs[i]->L,sizeof(bool));
		if (LFSRs[i]->state  == NULL){
			printf("error with calloc\n");
			return -1;
		}
	}
	//p
	(LFSRs[0]->p)[0] = true;
	(LFSRs[0]->p)[1] = true;
	(LFSRs[0]->p)[7] = true;

	(LFSRs[1]->p)[0] = true;
	(LFSRs[1]->p)[1] = true;
	(LFSRs[1]->p)[3] = true;
	(LFSRs[1]->p)[4] = true;
	(LFSRs[1]->p)[6] = true;

	(LFSRs[2]->p)[0] = true;
	(LFSRs[2]->p)[4] = true;
	(LFSRs[2]->p)[9] = true;

	(LFSRs[3]->p)[0] = true;
	(LFSRs[3]->p)[2] = true;
	(LFSRs[3]->p)[3] = true;
	(LFSRs[3]->p)[4] = true;
	(LFSRs[3]->p)[8] = true;

	//init
	(LFSRs[0]->state)[3] = true;

	(LFSRs[1]->state)[1] = true;
	(LFSRs[1]->state)[2] = true;

	(LFSRs[2]->state)[6] = true;
	(LFSRs[2]->state)[5] = true;

	(LFSRs[3]->state)[6] = true;

	int N=0;//message size

	float* p = compute_p(f, f_size, n);
	for (int i=0 ; i<n ; i++){
		printf("p[%d] : %f\n",i,p[i]);
	}
	for (int i=0 ; i< n; i++){
		int Ni = Nmin(LFSRs[i]->L,p[i]);
		if(Ni>N){
			N = Ni;
		}
	}

	bool* key = malloc(N*sizeof(bool));
	if (key == NULL){
		printf("error with malloc\n");
		return -1;
	}
	printf("key : ");
	for (int i=0 ; i<N ; i++){
		key[i] = f[(increment_LFSR(LFSRs[0]) << 3) 
			+ (increment_LFSR(LFSRs[1]) << 2) 
			+ (increment_LFSR(LFSRs[2]) << 1) 
			+ increment_LFSR(LFSRs[3])
		];
		printf("%d",key[i]?1:0);
		
	}
	printf("\n");

	int int_states[4];
	printf("N : %d\n",N);
	for (int i=0 ; i< n; i++){
		int Ti = threshold(LFSRs[i]->L,p[i],N);
		if (Ti !=0 ){
			for (int j=1 ; j< (1 << LFSRs[i]->L); j++){
				for (int k=0; k<LFSRs[i]->L; k++){
					(LFSRs[i]->state)[k] = (j >> k)%2;
				}
				int alpha = compute_alpha(key, N, LFSRs[i]);
				if((Ti > 0 && alpha > Ti)|| (Ti<0 && alpha < Ti)){
					int_states[i] = j;
					break;
				}
			}
		}
	}
	int L_tot = 0;
	for (int i=0 ; i< n; i++){
		int Ti = threshold(LFSRs[i]->L,p[i],N);
		if (Ti == 0){
			int_states[i] = 0;
			L_tot += LFSRs[i]->L;
		}
	}

	for (int j=1 ; j< (1 << L_tot); j++){
		int L_partial_sum = 0;

		for (int i=0 ; i< n; i++){
			if (int_states[i] == 0){
				for (int k=0; k<LFSRs[i]->L; k++){
					(LFSRs[i]->state)[k] = (j >> (k+L_partial_sum))%2;
				}
				L_partial_sum += LFSRs[i]->L;

			}else{
				for (int k=0; k<LFSRs[i]->L; k++){
					(LFSRs[i]->state)[k] = (int_states[i] >> k)%2;
				}
			}
		}

		int diff = 0;
		for (int i=0 ; i< N; i++){
			int pos = (increment_LFSR(LFSRs[0]) << 3) 
					+ (increment_LFSR(LFSRs[1]) << 2) 
					+ (increment_LFSR(LFSRs[2]) << 1) 
					+ increment_LFSR(LFSRs[3]);
			bool next_bool = f[pos];

			if(key[i]^next_bool){
				diff++;
			}
		}
		if(diff == 0){
			L_partial_sum = 0;
			for (int i=0 ; i< n; i++){
				if (int_states[i] == 0){
					for (int k=0; k<LFSRs[i]->L; k++){
						(LFSRs[i]->state)[k] = (j >> (k+L_partial_sum))%2;
					}
					int_states[i] = (j >> L_partial_sum) - ((j >> (L_partial_sum+LFSRs[i]->L)) << LFSRs[i]->L);
					L_partial_sum += LFSRs[i]->L;
				}
			}
			break;
		}
	
	}

	for (int i=0 ; i<n; i++){
		printf("%d\n",int_states[i]);
	}

	for (int i=0 ; i< n; i++){
		for (int k=0; k<LFSRs[i]->L; k++){
			(LFSRs[i]->state)[k] = (int_states[i] >> k)%2;
		}
	}
	printf("generated key : ");
	for (int i=0 ; i<N ; i++){
		printf("%d",(f[(increment_LFSR(LFSRs[0]) << 3) 
			+ (increment_LFSR(LFSRs[1]) << 2) 
			+ (increment_LFSR(LFSRs[2]) << 1) 
			+ increment_LFSR(LFSRs[3])
		])?1:0);
		
	}
	printf("\n");

	return 0;
}