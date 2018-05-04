
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>


char * tokeniz(char *str,uint64_t *valores, int *numval){
    
    *numval = 0;
    char *token;
    int len, len2;
    int i;
    
    token = strtok(str," ");
    len = strlen(token);
    
    while( token != NULL ) {
        sscanf(token,"%lld",&valores[*numval]);
        (*numval)++;
        token = strtok(NULL, " ");
    }
    
    return token;
}

int isprime(uint64_t value)
{
    if(value == 0) return 0;
    if(value == 1) return 0;
    for (uint64_t i=2; i< value; i++) {
        if(value%i == 0) return 0;
    }
    return 1;
}

int main() {
    
    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED | MAP_ANON;
    
    int i=0;
    uint64_t val[1024]; /*vetor de numeros a ser analisado */
    int n_processes; /* numero de processos = max(4) */
    int num_val; /*numero de valores lidos do arquivo*/
    pid_t pid[4];
    /* index do processo */
    int process_number=0;
    
    char *s=malloc(sizeof(char)*10000);
    
    fgets (s, 10000, stdin);
    s[strlen(s)-1] = '\0';
    tokeniz(s,val,&num_val);
    
    /* vetor de resultado do tratamento dos numeros */
    int *n_prime;
    n_prime = (int*) mmap(NULL, sizeof(int)*4, protection, visibility, 0, 0);
    if (n_prime==(int*)-1) printf("Erro de alocacao!\n");
    
    process_number=-1;
    
    /* criacao dos filhos */
    for(i=0;i<4;i++){
        pid[i] = fork();
        if(pid[i] == 0) {
            process_number = i;
            
            /* atribui os valores para cada processo */
            for (int iv=process_number; iv<num_val; iv+=4) {
                int isprime_return = isprime(val[iv]);
                if(isprime_return) n_prime[process_number]++;
//                printf("process number %d iv %d, value %lld prime return %d\n",i,iv,val[iv],isprime_return);
                
            }
            break;
        }
        else {
        }
    }
    int total_prime = 0; /* valor final numero de primos */
    /* processo pai */
    if(process_number==-1){
        int status;
        for(i=0;i<4;i++){
            waitpid(pid[i], &status, 0); /* espera os filhos terminarem */
            total_prime += n_prime[i];
        }
        printf("%d\n",total_prime);
    }
    return 0;
    
}
