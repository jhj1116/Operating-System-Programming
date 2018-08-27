#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

#include <pthread.h>

void Display(int dim, float *mat);
void Fill(int size, float *data);
void MatMul(int dim, float *A, float *B, float *Product);
void MatMul_MT(int dim, float *A, float *B, float *Product, int noThread);

int main(int argc, char *argv[])
{
	int dim = 10;
	int noThread = 10;

	struct timeval t1, t2;
	float elapsed_time = 0.F;

	if(argc < 2){
		printf("Usage: %s <dim> <no_threads>\n", argv[0]);
		exit(0);
	}

	dim = atoi(argv[1]);
	if(dim > 1000)
		dim = 1000;
	noThread = atoi(argv[2]);

	srand(time(NULL));

	float *A = (float*)malloc(dim * dim * sizeof(float));
	float *B = (float*)malloc(dim * dim * sizeof(float));
	float *Product = (float*)malloc(dim * dim * sizeof(float));

	if(A == NULL || B == NULL || Product == NULL){
		printf("Failed to allocate memory.\n");
		exit(-1);
	}

	Fill(dim * dim, A);
	Fill(dim * dim, B);

	if(dim <= 10){
		printf("A = \n");
		Display(dim, A);

		printf("B = \n");
		Display(dim, B);
	}

	gettimeofday(&t1, NULL);

	MatMul(dim, A, B, Product);

	gettimeofday(&t2, NULL);
	elapsed_time = (t2.tv_sec + t2.tv_usec / 1000000.) - (t1.tv_sec + t1.tv_usec / 1000000.);
	printf("elapsed time (single thread)\t= %15f sec\n", elapsed_time);

	if(dim <= 10){
		printf("A * B (single thread) = \n");
		Display(dim, Product);
	}

	gettimeofday(&t1, NULL);

	MatMul_MT(dim, A, B, Product, noThread);

	gettimeofday(&t2, NULL);
	elapsed_time = (t2.tv_sec + t2.tv_usec / 1000000.) - (t1.tv_sec + t1.tv_usec / 1000000.);
	printf("elapsed time (%d threads)\t= %15f sec\n", noThread, elapsed_time);

	if(dim <= 10){
		printf("A * B (%d threads) = \n", noThread);
		Display(dim, Product);
	}


	free(A);
	free(B);
	free(Product);

	return 0;
}

void Display(int dim, float *mat)
{
	if(dim >= 10)
		return;
	for(int i = 0; i < dim; i++){
		for(int j = 0; j < dim; j++)
			printf("%.2f ", mat[i * dim + j]);
		printf("\n");
	}
}

void Fill(int size, float *data)
{
	int i = 0;
	for(i = 0; i < size; i++)
		data[i] = i;
}

void MatMul(int dim, float *A, float *B, float *Product)
{
	int i = 0, j = 0, k = 0;

	for(i = 0; i < dim; i++){
		for(j = 0; j < dim; j++){
			Product[i * dim + j] = 0.F;
			for(k = 0; k < dim; k++)
				Product[i * dim + j] += A[i * dim + k] * B[k * dim + j]; 
		}
	}
}

typedef struct {
	int id;					// thread index
	int noThread;
	int dim;
	float *A, *B, *Product;
} ThreadInfo;

void* MatMul_ThreadFn(void *param);

void MatMul_MT(int dim, float *A, float *B, float *Product, int noThread)
{
	pthread_t *tid = (pthread_t*)malloc(noThread*sizeof(pthread_t));// dynamically allocate pthread_t-type array whose length is noThread
	ThreadInfo *threadInfo = (ThreadInfo*)malloc(noThread*sizeof(ThreadInfo));// dynamically allocate ThreadInfo-type array whose length is noThread
	
	for(int i=0; i<noThread; i++){// Repeat for noThread times
		threadInfo[i].id = i;// Fill threadInfo[i] with appropriate values
		threadInfo[i].noThread = noThread;//	ex) threadInfo[i].id = i;
		threadInfo[i].dim = dim;// 		threadInfo[i].noThread = noThread;
		threadInfo[i].A = A;//		...
		threadInfo[i].B = B;//
		threadInfo[i].Product = Product;
		pthread_create(&tid[i], NULL, MatMul_ThreadFn, (void*)&threadInfo[i]);// create child thread passing &aThreadInfo[i] to the thread function
	}

	for(int j=0; j<noThread; j++)// wait for the children threads to terminate
		pthread_join(tid[j],NULL);// call pthread_join t-times

	free(tid);
	free(threadInfo);// deallocate pthread_t and ThreadInfo array
}

void* MatMul_ThreadFn(void *param)		// thread function
{
	ThreadInfo *pInfo = (ThreadInfo *)param;
	
	int id = pInfo->id;
	int dim = pInfo->dim;
	int noThread = pInfo->noThread;

	for(int tmp=0; tmp<noThread; tmp++){
		int i = id + tmp*noThread;
		if(i==dim) break;
		for(int j=0; j<dim; j++){
			pInfo->Product[i*dim +j] = 0.F;
			for(int k=0; k<dim; k++)
				pInfo->Product[i*dim +j] += pInfo->A[i*dim +k]*pInfo->B[k*dim +j];
		}
	}
	// multiply two matrics pInfo->A and pInfo->B put the Product in pInfo->Product
	// i-th thread computes only (i + k * noThread)-th rows
}
