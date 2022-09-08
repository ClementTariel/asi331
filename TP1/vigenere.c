#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char* addKey(char* message, int message_len, char* key, int key_len, int sign){
   char* result = malloc(sizeof(char) * (message_len+1));
   if (result == NULL){
      printf("error with malloc\n");
      return NULL;
   }
   result[message_len] = 0;
   for (int i = 0; i < message_len; i++){
      result[i] = 'A' + (toupper(message[i])-'A' + sign*(toupper(key[i%key_len])-'A') + 26)%26;
   }
   return result;
}

char* encode(char* message, int message_len, char* key, int key_len){
   return addKey(message, message_len, key, key_len, 1);
}

char* decode(char* message, int message_len, char* key, int key_len){
   return addKey(message, message_len, key, key_len, -1);
}

float IC(char* message, int message_len, int i, int m){
   int f_i[26];
   for (int k = 0; k < 26; k++){
      f_i[k] = 0;
   }
   int sample_size = (message_len-i-1)/m + 1;
   //message_len must be >= 2*m
   for (int k = 0; k < sample_size; k++){
      f_i[toupper(message[k*m+i])-'A']++;
   }
   float result = 0;
   for (int k = 0; k < 26; k++){
      result += f_i[k] * (f_i[k]-1);
   }
   result = result/(sample_size*(sample_size-1));
   return result;
}

float ICM(char* message, int message_len, int i, int j, int g, int m){
   int f_i[26];
   for (int k = 0; k < 26; k++){
      f_i[k] = 0;
   }
   int sample_size_i = (message_len-i-1)/m + 1;
   //message_len must be >= 2*m
   for (int k = 0; k < sample_size_i; k++){
      f_i[toupper(message[k*m+i])-'A']++;
   }

   int f_j[26];
   for (int k = 0; k < 26; k++){
      f_j[k] = 0;
   }
   int sample_size_j = (message_len-j-1)/m + 1;
   //message_len must be >= 2*m
   for (int k = 0; k < sample_size_j; k++){
      f_j[toupper(message[k*m+j])-'A']++;
   }

   float result = 0;
   for (int k = 0; k < 26; k++){
      result += f_i[k] * f_j[(k-g+26)%26];
   }
   result = result/(sample_size_i*(sample_size_j));
   return result;
}

int find_key_len(char* message, int message_len){
   float basic_IC = 0.038;
   float french_IC = 0.071;
   float threshold = (basic_IC + french_IC)/2;
   float average_IC = -1;
   int m = 2;
   while(average_IC<threshold && m<message_len/2){
      m++;
      average_IC = 0;
      //printf("m = %d\n",m);
      for (int i=0; i<m; i++){
         float IC_i_m = IC(message, message_len, i, m);
         //printf("i = %d, IC(yi) = %f\n",i,IC_i_m);
         average_IC += IC_i_m;
      }
      average_IC /= m;
   }
   if (m<message_len/2){
      return m;
   }
   printf("Could not find the sizeof the key.\nDefault size is %d (length of the message).\n",message_len);
   return message_len;
}

char* find_key(char* message, int message_len, int m){
   char* key = malloc(sizeof(char) * (m+1));
   if (key == NULL){
      printf("error with malloc\n");
      return NULL;
   }
   key[m]=0;
   for (int i=0; i<m; i++){
      float ICM_max = 0;
      key[i] = 'A';
      for(int g=0; g<26; g++){
         float ICM_0_i_g = ICM(message, message_len, 0, i, g, m);
         if (ICM_0_i_g > ICM_max){
            key[i] = 'A' + g;
            ICM_max = ICM_0_i_g;
         }
      }
      //'invert' the key because it is decoding not encoding
      key[i] = 'A'+(26+'A'-key[i])%26;
   }
   
   int freq[26];
   for (int i=0; i<26; i++){
      freq[i]=0;
   }
   for (int i=0; i<message_len; i++){
      freq[(26+toupper(message[i])-toupper(key[i%m]))%26]++;
   }
   int E_pos = 0;
   for (int i=0; i<26; i++){
      if(freq[i]>freq[E_pos]){
         E_pos = i;
      }
   }
   int offset = (26+'A'+E_pos-'E')%26;
   for (int i = 0; i< m; i++){
      key[i] = 'A'+(26+key[i]-'A'+offset)%26;
   }
   return key;
}

int main ()
{
   FILE* f;
   f = fopen("Clair.txt","r");
   if(f == NULL){
      return -1;
   }
   fseek(f, 0, SEEK_END); // seek to end of file
   int message_len = ftell(f); // get current file pointer
   fseek(f, 0, SEEK_SET); // seek back to beginning of file
   char* message = malloc(sizeof(char) * (message_len+1));
   if (message == NULL){
      printf("error with malloc\n");
      fclose(f);
      return -1;
   }
   fgets(message, message_len+1,f);
   fclose(f);
   
   while(message[message_len-1]=='\n'){
      message[message_len-1] = 0;
      message_len--;
   }
   printf("message : \n%s\n",message);

   char key[5];
   strcpy(key, "BEAU");
   key[4] = 0;
   char* encoded_message = encode(message,message_len,key,4);
   printf("message encoded with 'beau' : \n%s\n",encoded_message);
   int key_size = find_key_len(encoded_message, message_len);
   printf("key size : %d\n",key_size);
   char* cracked_key = find_key(encoded_message,message_len, key_size);
   printf("cracked key of message encoded with 'beau' : %s\n",cracked_key);
   char* decoded_message = decode(encoded_message,message_len,cracked_key,key_size);
   printf("message encoded with 'beau', decoded with the cracked key: \n%s\n",decoded_message);
   free(cracked_key);
   free(encoded_message);
   free(decoded_message);
   
   printf("==========\n");

   f = fopen("Vigenere_Singh.txt","r");
   if(f == NULL){
      free(message);
      return -1;
   }
   fseek(f, 0, SEEK_END); // seek to end of file
   int code_len = ftell(f); // get current file pointer
   fseek(f, 0, SEEK_SET); // seek back to beginning of file
   char* code = malloc(sizeof(char) * (code_len+1));
   if (code == NULL){
      printf("error with malloc\n");
      fclose(f);
      free(message);
      return -1;
   }
   fgets(code, code_len+1,f);
   fclose(f);

   while(code[code_len-1]=='\n'){
      code[code_len-1] = 0;
      code_len--;
   }
   printf("%s\n",code);

   key_size = find_key_len(code, code_len);
   printf("key size : %d\n",key_size);
   cracked_key = find_key(code,code_len, key_size);
   printf("key : %s\n",cracked_key);
   decoded_message = decode(code,code_len,cracked_key,key_size);
   printf("%s\n",decoded_message);
   
   free(message);
   free(code);
   free(decoded_message);
   free(cracked_key);
   
   return 0;
}