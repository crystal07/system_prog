#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MATRIX_SIZE 4000
int thread_num;
unsigned long long int * s;

unsigned long long int sum = 0;
int mat1[MATRIX_SIZE][MATRIX_SIZE], mat2[MATRIX_SIZE][MATRIX_SIZE];
unsigned long long int result[MATRIX_SIZE][MATRIX_SIZE];
pthread_mutex_t mutex;

void matrix_mul();
void matrix_print(int mat[MATRIX_SIZE][MATRIX_SIZE]);
void thread_execution();
void * mul_th(void * arg);

int main() {
	int choose, i, j;

	FILE * matrix1 = fopen("./sample1.txt", "r");
	FILE * matrix2 = fopen("./sample2.txt", "r");
    
    pthread_mutex_init(&mutex, NULL);

	/*
	srand(time(NULL));
	fseek(matrix1, 0L, SEEK_SET);
	fseek(matrix2, 0L, SEEK_SET);

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			if (j == MATRIX_SIZE-1) {
				fprintf(matrix1, "%d\n", rand()%10000);
				fprintf(matrix2, "%d\n", rand()%10000);
			}
			else {
				fprintf(matrix1, "%d\t", rand()%10000);
				fprintf(matrix2, "%d\t", rand()%10000);			
			}

			fseek(matrix1, sizeof(int), SEEK_CUR);
			fseek(matrix2, sizeof(int), SEEK_CUR);
			fseek(matrix1, sizeof(char), SEEK_CUR);
			fseek(matrix2, sizeof(char), SEEK_CUR);
		}
	}
	*/
    
    //parsing file for getting matrix1, matrix2
	fseek(matrix1, 0L, SEEK_SET);
	fseek(matrix2, 0L, SEEK_SET);
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {

			fscanf(matrix1, "%d", &mat1[i][j]);
			fscanf(matrix2, "%d", &mat2[i][j]);

			fseek(matrix1, sizeof(int), SEEK_CUR);
			fseek(matrix2, sizeof(int), SEEK_CUR);
			fseek(matrix1, sizeof(char)*2, SEEK_CUR);
			fseek(matrix2, sizeof(char)*2, SEEK_CUR);
		}
	}
    
    //result matrix ini
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			result[i][j] = 0;
		}
	}
    
    

	//matrix_mul(mat1, mat2, result);
	//matrix_print(result);

	thread_num = 20;
    
    s = (unsigned long long int *) malloc(sizeof(unsigned long long int) * thread_num);


	thread_execution(mat1, mat2, result);

    free(s);
	fclose(matrix1);
	fclose(matrix2);

	return 0;
}

void matrix_mul() {
	int i, j, k;                                                                                                                             
	
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			for (k = 0; k < MATRIX_SIZE; k++) {
				result[i][j] += mat1[i][k]*mat2[k][j];
			}
		}
	}
}

void matrix_print(int mat[MATRIX_SIZE][MATRIX_SIZE]) {
	int i, j;
	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			printf("%4d\t", mat[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void * mul_th(void * arg) {
	const unsigned long long int n_elements = (MATRIX_SIZE * MATRIX_SIZE);
	const unsigned long long int n_operations = n_elements / thread_num;
	const unsigned long long int rest_operations = n_elements % thread_num;	
	unsigned long long int i, j, k;
    int row, col;

	int thread_n = *(int*)arg;
	unsigned long long int start_op, end_op;
    unsigned long long int r = 0;


	  if (thread_n == 0) {
	    // First thread does more job
	    start_op = n_operations * thread_n;
	    end_op = (n_operations * (thread_n + 1)) + rest_operations;
	  }
	  else {
	    start_op = n_operations * thread_n + rest_operations;
	    end_op = (n_operations * (thread_n + 1)) + rest_operations;
	  }
	  printf("Start thread %d\n", thread_n);
    
	    for (int op = start_op; op < end_op; ++op) {
            row = op % MATRIX_SIZE;
            col = op / MATRIX_SIZE;
            r = 0;
            for (int i = 0; i < MATRIX_SIZE; ++i) {
                int e1 = mat1[row][i];
                int e2 = mat2[i][col];
                r += e1 * e2;
            }
        
        result[row][col] = r;
        s[thread_n] += r;

  }
    
    printf("End thread %d\n", thread_n);

}

void thread_execution() {
	struct timeval start, end;
	float operating_time;
	pthread_t thread[thread_num];
	int id[thread_num];
    
    for (int i =0; i<thread_num; i++) s[i]=0;

    gettimeofday(&start, NULL);

	for (int i = 0; i < thread_num; ++i) {
		id[i] = i;
		pthread_create(&thread[i] , NULL , mul_th , (void*)&id[i]);
	}

	for (int i = 0; i < thread_num; ++i) {
    	pthread_join(thread[i], NULL);
 	}
    
    sum = 0;
    for (int i = 0; i <thread_num; i++) sum += s[i];
    
    printf("sum : %lld\n", sum);
    
	gettimeofday(&end, NULL);
	operating_time = (double)(end.tv_sec)+(double)(end.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
	printf("%f\n",operating_time);
    

}
