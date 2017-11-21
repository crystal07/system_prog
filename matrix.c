#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MATRIX_SIZE 4000
int thread_num;
int mat1[MATRIX_SIZE][MATRIX_SIZE], mat2[MATRIX_SIZE][MATRIX_SIZE], result[MATRIX_SIZE][MATRIX_SIZE];

void matrix_mul();
void matrix_print(int mat[MATRIX_SIZE][MATRIX_SIZE]);
void thread_execution();
void * mul_th(void * arg);

int main() {
	int choose, i, j;
	FILE * matrix1 = fopen("./sample1.txt", "r");
	FILE * matrix2 = fopen("./sample2.txt", "r");



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

	for (i = 0; i < MATRIX_SIZE; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			result[i][j] = 0;
		}
	}

	//printf("%d %d %d %d\n", mat1[0][0], mat1[0][1], mat1[0][3999], mat1[1][0]);

	//matrix_mul(mat1, mat2, result);
	//matrix_print(result);

	thread_num = 20;

	thread_execution(mat1, mat2, result);

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

	int thread_n = *(int*)arg;
	unsigned long long int start_op, end_op;

	  if (thread_n == 0) {
	    // First thread does more job
	    start_op = n_operations * thread_n;
	    end_op = (n_operations * (thread_n + 1)) + rest_operations;
	  }
	  else {
	    start_op = n_operations * thread_n + rest_operations;
	    end_op = (n_operations * (thread_n + 1)) + rest_operations;
	  }
	  printf("Start %d , end %d\n", start_op, end_op);

	    for (int op = start_op; op < end_op; ++op) {
    const int row = op % MATRIX_SIZE;
    const int col = op / MATRIX_SIZE;
    int r;
    for (int i = 0; i < MATRIX_SIZE; ++i) {
      const int e1 = mat1[row][i];
      const int e2 = mat2[i][col];
      r += e1 * e2;
    }

    result[row][col] = r;
  }

	  /*
    for (i = start_op; i < end_op; i++) {
		for (j = 0; j < MATRIX_SIZE; j++) {
			for (k = 0; k < MATRIX_SIZE; k++) {
				printf("result : %d, mat1 : %d, mat2 : %d\n", result[i][j], mat1[i][k], mat2[k][j]);
				result[i][j] += mat1[i][k]*mat2[k][j];
				printf("result : %d, mat1 : %d, mat2 : %d\n", result[i][j], mat1[i][k], mat2[k][j]);

			}
		}
	}*/
		  printf("Start %d , end %d\n", start_op, end_op);

}

void thread_execution() {
	struct timeval start, end;
	float operating_time;
	pthread_t thread[thread_num];
	int id[thread_num];
	gettimeofday(&start, NULL);

	for (int i = 0; i < thread_num; ++i) {
		id[i] = i;
		pthread_create(&thread[i] , NULL , mul_th , (void*)&id[i]);
	}

	for (int i = 0; i < thread_num; ++i) {
    	pthread_join(thread[i], NULL);
 	}

	gettimeofday(&end, NULL);
	operating_time = (double)(end.tv_sec)+(double)(end.tv_usec)/1000000.0-(double)(start.tv_sec)-(double)(start.tv_usec)/1000000.0;
	printf("%f\n",operating_time);
}
