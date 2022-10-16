#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <openssl/bn.h>
#include <openssl/sha.h>

char* txt_to_hex(char* txt){
	int size = 0;
	while(txt[size] != '\0'){
		size++;
	}
	char * hex = malloc(size*2+1);
	if (hex == NULL){
		printf("error with malloc\n");
		return NULL;
	}
	hex[2*size] = '\0';
	int i=0;
	while(txt[i] != '\0'){
		sprintf((char*)(hex+2*i),"%02X",txt[i]);
		i++;
	}
	return hex;
}

char* hex_to_txt(char* hex){
	int size = 0;
	while(hex[size] != '\0'){
		size++;
	}
	char * txt = malloc((size/2)+1);
	if (txt == NULL){
		printf("error with malloc\n");
		return NULL;
	}
	txt[size/2] = '\0';
	int i=0;
	int c=0;
	char buf[2];
	while(hex[i] != '\0'){
		if (i%2 == 0){
			buf[i%2]=hex[i];
		}else{
			buf[i%2]=hex[i];
			c =(int)strtol(buf, NULL, 16);
			txt[i/2] = (char)c;
		}
		i++;
	}
	return txt;
}

int gcd(int a, int b)
{
    int temp;
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

BIGNUM * encrypt(BIGNUM * m, BIGNUM * n, BIGNUM * e){
	BN_CTX *ctx;
	ctx = BN_CTX_new();
	if (!ctx) goto err;
	BIGNUM * c = BN_new();
	if (c==NULL){
		printf("error with BIGNUM operation\n");
		BN_CTX_free(ctx);
		return NULL;
	}
	if (!BN_mod_exp(c,m,e,n,ctx)) goto err;
	return c;
err:
	printf("error with BIGNUM operation\n");
	BN_free(c);
	return NULL;
}

BIGNUM * decrypt(BIGNUM * c, BIGNUM * n, BIGNUM * d){
	BN_CTX *ctx;
	ctx = BN_CTX_new();
	if (!ctx) goto err;
	BIGNUM * m = BN_new();
	if (m==NULL){
		printf("error with BIGNUM operation\n");
		BN_CTX_free(ctx);
		return NULL;
	}
	if (!BN_mod_exp(m,c,d,n,ctx)) goto err;
	return m;
err:
	printf("error with BIGNUM operation\n");
	BN_free(m);
	return NULL;
}

BIGNUM * hash_sha_256(BIGNUM * m){
	BIGNUM* h = BN_new();
	if (h==NULL){
		printf("error with BIGNUM operation\n");
		return NULL;
	}
	int bin_m_len = BN_num_bytes(m);
	char* bin_m = malloc(bin_m_len);
	if (bin_m == NULL){
		printf("error with malloc\n");
		return NULL;
	}
    uint8_t digestBuffer[256];
    SHA256_CTX sha_ctx;
    SHA256_Init (&sha_ctx);
    SHA256_Update (&sha_ctx, bin_m, bin_m_len);
    SHA256_Final (digestBuffer, &sha_ctx);

    if (!BN_bin2bn (digestBuffer, 256, h)){
    	printf("error with BIGNUM operation\n");
    	free(bin_m);
    	BN_free(h);
		return NULL;
    }
    return h;
}

BIGNUM * sign(BIGNUM * m, BIGNUM * n, BIGNUM * d){
	BN_CTX *ctx;
	ctx = BN_CTX_new();
	if (!ctx) goto err;
	BIGNUM * h = hash_sha_256(m);
	BIGNUM * s = BN_new();
	if (s==NULL){
		BN_free(h);
		goto err;
	}
	if (!BN_mod_exp(s,m,d,n,ctx) || !BN_mul(s,s,h,ctx)){
		printf("error with BIGNUM operation\n");
		BN_free(s);
		BN_free(h);
		goto err;
	}
	return s;	
err:
	printf("error with BIGNUM operation\n");
	BN_CTX_free(ctx);
	return NULL;
}

bool miller_rabin(BIGNUM * n, int t){
	BN_CTX *ctx;
	ctx = BN_CTX_new();
	if (!ctx) goto err;
	
	BIGNUM * n_1 = BN_dup(n);
	if (n_1==NULL) goto err;
	BN_sub_word(n_1,(BN_ULONG)1);
	BIGNUM * n_2 = BN_dup(n);
	if (n_2==NULL) goto err;
	BN_sub_word(n_2,(BN_ULONG)2);
	BIGNUM * r = BN_dup(n_1);
	if (r==NULL) goto err;
	int s = 0;
	while (!BN_is_odd(r)){
		BN_div_word(r,(BN_ULONG)2);
		s++;
	}
	BIGNUM * y = BN_new();
	if (y==NULL) goto err;
	BN_zero(y);
	BIGNUM * a = BN_new();
	if (a==NULL) goto err;
	BN_zero(a);
	for (int i=0; i<t; i++){
		if (!BN_rand(a, 1024, BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ODD)) goto err;
		if (!BN_mod(a,a,n_2,ctx)) goto err;
		BN_add_word(a,(BN_ULONG)2);
		if (!BN_mod_exp(y,a,r,n,ctx)) goto err;
		if (!BN_is_one(y) && BN_cmp(y, n_1)!=0){
			int j = 1;
			while(j <= s-1 && BN_cmp(y, n_1)!=0){
				BN_mod_sqr(y, y, n, ctx);
				if (BN_is_word(y, (BN_ULONG)1)){
					goto not_prime;
				}
				j++;
			}
			if (BN_cmp(y, n_1)!=0){
				goto not_prime;
			}
		}

	}
	BN_free(r);
	BN_free(y);
	BN_free(a);
	BN_free(n_1);
	BN_free(n_2);
	BN_CTX_free(ctx);
	return true;

not_prime:
	BN_free(r);
	BN_free(y);
	BN_free(a);
	BN_free(n_1);
	BN_free(n_2);
	BN_CTX_free(ctx);
	return false;
err:
	printf("error with BIGNUM operation\n");
	return NULL;

}

BIGNUM * generate_prime(int n){
	bool is_prime = false;
	BIGNUM * a = BN_new();
	if (a==NULL) goto err;
	while(!is_prime){
		if (!BN_rand(a, n, BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ODD)) goto err;
		if (miller_rabin(a,20)){
			is_prime = true;
		}
	}
	return a;
err:
	printf("error with BIGNUM operation\n");
	BN_free(a);
	return NULL;
}

int main (){
	BN_CTX *ctx;
	BIGNUM * p;
	BIGNUM * p_1;
	BIGNUM * q;
	BIGNUM * q_1;
	BIGNUM * n;
	BIGNUM * phi;
	int e;
	BIGNUM * e_bn;
	BIGNUM * d;

	char * p_hex;
	char * q_hex;
	char * n_hex;
	char * d_hex;

	ctx = BN_CTX_new();
	if(ctx==NULL){
		printf("error with BIGNUM operation\n");
		BN_CTX_free(ctx);
		return -1;
	}

	FILE * pub_key = fopen("public.rsa", "r");
	int sz;
	if (pub_key == NULL){   
		printf("Could not open file, let's assume no key has been generated yet.\n");
		sz = 0;
	}else{
		fseek(pub_key, 0L, SEEK_END);
		sz = ftell(pub_key);
		fclose(pub_key);
	}
	

	FILE * priv_key = fopen("prive.rsa", "r");
	int sz2;
	if (priv_key == NULL){   
		printf("Could not open file, let's assume no key has been generated yet.\n");
		sz2 = 0;
	}else{
		fseek(priv_key, 0L, SEEK_END);
		sz2 = ftell(priv_key);
		fclose(priv_key);
	}

	if (sz<2 || sz2<2){
		printf("Start keys generation ...\n");
		bool key_generation_valid = false;
		while(!key_generation_valid){
			p = generate_prime(1024);
			if(p==NULL){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				return -1;
			}
			char * p_dec = BN_bn2dec(p);
			p_hex = BN_bn2hex(p);
			printf("%s is probably a prime\n",p_dec);
			OPENSSL_free(p_dec);

			p_1 = BN_dup(p);
			if (p_1==NULL){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				BN_free(p);
				return -1;
			}
			BN_sub_word(p_1,(BN_ULONG)1);
			
			q = generate_prime(1024);
			while(BN_cmp(p, q)==0){
				BN_free(q);
				q = generate_prime(1024);
			}
			if(q==NULL){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				BN_free(p);
				BN_free(p_1);
				return -1;
			}
			char * q_dec = BN_bn2dec(q);
			q_hex = BN_bn2hex(q);
			printf("%s is probably a prime\n",q_dec);
			OPENSSL_free(q_dec);

			q_1 = BN_dup(q);
			if (q_1==NULL){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				BN_free(p);
				BN_free(p_1);
				BN_free(q);
				return -1;
			}
			BN_sub_word(q_1,(BN_ULONG)1);

			n = BN_new();
			if (n==NULL || !BN_mul(n,p,q,ctx)){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				BN_free(p);
				BN_free(p_1);
				BN_free(q);
				BN_free(q_1);
				return -1;
			}
			char * n_dec = BN_bn2dec(n);
			n_hex = BN_bn2hex(n);
			printf("n = %s\n",n_dec);
			printf("n (hex) = %s\n",n_hex);
			OPENSSL_free(n_dec);

			phi = BN_new();
			if (phi==NULL || !BN_mul(phi,p_1,q_1,ctx)){
				printf("error with BIGNUM operation\n");
				BN_CTX_free(ctx);
				BN_free(p);
				BN_free(p_1);
				BN_free(q);
				BN_free(q_1);
				BN_free(n);
				return -1;
			}

			e = (1<<16)+1;
			int phi_mod_e = (int) BN_mod_word(phi, e);
			int gcd_e_phi = gcd(e,phi_mod_e);
			int i = 0;
			while(gcd_e_phi != 1){
				i++;
				int e = (1<<16)+(1<<i)+1;
				phi_mod_e = (int) BN_mod_word(phi, e);
				gcd_e_phi = gcd(e,phi_mod_e);
			}
			key_generation_valid = (gcd_e_phi == 1);
			OPENSSL_free(p_hex);
			OPENSSL_free(q_hex);
			BN_free(p);
			BN_free(p_1);
			BN_free(q);
			BN_free(q_1);
			BN_free(n);
		}

		printf("Keys generated.\n");

		printf("e = %d\n",e);

		d = BN_new();
		if (d==NULL){
			printf("error with BIGNUM operation\n");
			BN_CTX_free(ctx);
			BN_free(phi);
			return -1;
		}

		e_bn = BN_new();
		if (e_bn==NULL){
			printf("error with BIGNUM operation\n");
			BN_CTX_free(ctx);
			BN_free(phi);
			BN_free(d);
			return -1;
		}
		BN_set_word(e_bn,(BN_ULONG)e);

		if (!BN_mod_inverse(d, e_bn, phi, ctx)){
			printf("error with BIGNUM operation\n");
			BN_CTX_free(ctx);
			BN_free(phi);
			BN_free(d);
			BN_free(e_bn);
			return -1;
		}

		pub_key = fopen("public.rsa", "w");
		// test for files not existing. 
		if (pub_key == NULL) 
		{   
			printf("Error! Could not open file\n"); 
			goto err;
		} 

		char e_hex[6];
		printf("e (hex) = %x\n", e);
		sprintf(e_hex, "%x", e);
		fprintf(pub_key, "%s\n%s\n",n_hex,e_hex); // write to file 
		OPENSSL_free(n_hex);

		fclose(pub_key);

		priv_key = fopen("prive.rsa", "w");
		if (priv_key == NULL) 
		{   
			printf("Error! Could not open file\n"); 
			goto err;
		} 

		d_hex = BN_bn2hex(d);
		printf("d = %s\n",d_hex);
		fprintf(priv_key, "%s\n",d_hex); // write to file 
		OPENSSL_free(d_hex);
		
		fclose(priv_key);

		BN_free(phi);
		BN_free(d);
		BN_free(e_bn);
	}

	printf("Read keys.\n");

	pub_key = fopen("public.rsa", "r");
	if (pub_key == NULL) 
	{   
		printf("Error! Could not open file\n"); 
		exit(-1); // must include stdlib.h 
	} 
	
	//
	// get n from public key
	//
	char n_hex_read[514];//512 (=2048/4) because its hex 513 for the \n 514 fot the 0 at the end
	n = BN_new();
	BN_zero(n);
	if (n==NULL){
		printf("error with BIGNUM operation\n");
		BN_CTX_free(ctx);
		return -1;
	}

	if(fgets(n_hex_read, sizeof(n_hex_read), pub_key) != NULL) {
		if(!BN_hex2bn(&n, n_hex_read)){
			printf("error with BIGNUM operation\n");
			OPENSSL_free(n_hex);
			BN_CTX_free(ctx);
			return -1;
		}
		n_hex = BN_bn2hex(n);
		printf("n = %s\n",n_hex);
	}

	//
	// get e from public key
	//
	char e_read_hex[6];	
	if(fgets(e_read_hex, sizeof(e_read_hex), pub_key) != NULL) {
		printf("e (hex) = %s\n",e_read_hex);
		e = (int)strtol(e_read_hex, NULL, 16);
		printf("e (dec) = %d\n",e);
	}
	n_hex = BN_bn2dec(n);
	fclose(pub_key);

	e_bn = BN_new();
	if (e_bn==NULL){
		printf("error with BIGNUM operation\n");
		goto err;
	}
	BN_set_word(e_bn,(BN_ULONG)e);

	priv_key = fopen("prive.rsa", "r");
	if (priv_key == NULL) 
	{   
		printf("Error! Could not open file\n"); 
		exit(-1); // must include stdlib.h 
	}

	//
	// get d from private key
	//
	char d_hex_read[514];
	d = BN_new();
	if (d==NULL){
		printf("error with BIGNUM operation\n");
		goto err;
	}
	if(fgets(d_hex_read, sizeof(d_hex_read), priv_key) != NULL) {
		if(!BN_hex2bn(&d, d_hex_read)){
			goto err;
		}
		d_hex = BN_bn2hex(d);
		printf("d (hex) = %s\n",d_hex);
	}
	OPENSSL_free(d_hex);
	fclose(priv_key);

	printf("Keys read.\n");

	//
	// convert message from ascii to hex and hex to ascii
	//
	char text[15] = "message secret";//len must be < 1024
	printf("txt = %s\n",text);
	char * hex_text = txt_to_hex(text);
	printf("hex text = %s\n",hex_text);
	char * test_text = hex_to_txt(hex_text);
	printf("txt = %s\n",test_text);
	free(test_text);
	
	//
	// convert the message to a "Big Num"
	//
	BIGNUM * m = BN_new();
	if (m==NULL) goto err;
	BN_zero(m);
	if(!BN_hex2bn(&m, hex_text)){
		goto err;
	}
	free(hex_text);

	char * m_hex = BN_bn2hex(m);
	printf("m1 = %s\n",m_hex);
	OPENSSL_free(m_hex);

	//
	// encrypt the message
	//
	BIGNUM * c = encrypt(m, n, e_bn);
	char * c_hex = BN_bn2hex(c);
	printf("c = %s\n",c_hex);
	char * code = hex_to_txt(c_hex);
	printf("code = %s\n",code);
	OPENSSL_free(c_hex);
	free(code);
	

	//
	// decrypt the message
	//
	BN_free(m);
	m = decrypt(c, n, d);
	m_hex = BN_bn2hex(m);
	printf("m2 = %s\n",m_hex);
	char * new_text = hex_to_txt(m_hex);
	printf("txt2 = %s\n",new_text);
	OPENSSL_free(m_hex);
	free(new_text);


	BN_free(m);
	BN_free(c);

	OPENSSL_free(n_hex);
	BN_CTX_free(ctx);
	BN_free(n);
	BN_free(e_bn);

	return 0;

err:
	printf("error with BIGNUM operation\n");
	OPENSSL_free(n_hex);
	BN_CTX_free(ctx);
	BN_free(n);
	BN_free(e_bn);
	return -1;

}