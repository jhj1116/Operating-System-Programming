#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>


#define NO_MESSAGE 12
#define MESSAGE_LEN 1024

// https://iamfearlesssoul.com/12-beautiful-and-inspiring-quotes-on-faith-love-and-hope/
char messages[][MESSAGE_LEN] = {
	"Only in the darkness can you see the stars. ―Martin Luther King Jr.",
	"You must not lose faith in humanity. Humanity is like an ocean; if a few drops of the ocean are dirty, the ocean does not become dirty. ― Mahatma Gandhi",
	"They say a person needs just three things to be truly happy in this world: someone to love, something to do, and something to hope for. ― Tom Bodett",
	"None of us knows what might happen even the next minute, yet still we go forward. Because we trust. Because we have Faith. ― Paulo Coelho", 
	"Follow your bliss and the universe will open doors for you where there were only walls. ― Joseph Campbell",
	"Being deeply loved by someone gives you strength, while loving someone deeply gives you courage. ― Lao Tzu",
	"Keep Going. Your hardest times often lead to the greatest moments of your life. Keep going. Tough situations build strong people in the end. ― Roy T. Bennett",
	"You cannot swim for new horizons until you have courage to lose sight of the shore.” ― William Faulkner",
	"Love is the only force capable of transforming an enemy into a friend.  ― Martin Luther King, Jr.",
	"The reason birds can fly and we can’t is simply because they have perfect faith, for to have faith is to have wings. ― J.M. Barrie",
	"When we love, we always strive to become better than we are. When we strive to become better than we are, everything around us becomes better too. ― Paulo Coelho",
	"You can cut all the flowers but you cannot keep Spring from coming. ― Pablo Neruda"
};


// shared buffer
#define BUFFER_SIZE 10
char buffer[BUFFER_SIZE][MESSAGE_LEN];
int in = 0, out = 0;

// mutex and semaphores
pthread_mutex_t mutex;
sem_t empty, full;


// thread functions
void* Encryptor(void *param);
void* Decryptor(void *param);
int thread_cont = 1;

// interval of Encryptor and Decryptor to control their speed
int enc_interval = 0;
int dec_interval = 0;


int main(int argc, char *argv[])
{
	int ret = 0;
	if(argc < 3){
		printf("%s <encryptor_interval> <decryptor_interval>\n", argv[0]);
		exit(0);
	}

	srand(time(NULL));

	enc_interval = atoi(argv[1]);
	dec_interval = atoi(argv[2]);
	// TO DO: retrieve enc_interval and dec_interval from command line arguments
	int state_mtx, state_emptysem, state_fullsem;

	state_mtx = pthread_mutex_init(&mutex,NULL);
	state_emptysem = sem_init(&empty,0,BUFFER_SIZE);
	state_fullsem = sem_init(&full,0,0);
	// TO DO: create mutex, full, and empty
	
	if(state_mtx == -1){
		printf("init mutex error");
		exit(-1);
	}
	if(state_emptysem == -1 || state_fullsem == -1){
		printf("init semaphore error");
		exit(-1);
	}
	// on failure, print appropriate error message and terminate


	pthread_t tid1;
	pthread_t tid2;
	pthread_attr_t attr;
	// TO DO: create Encryptor and Decryptor threads
	pthread_attr_init(&attr);
	pthread_create(&tid1,&attr,Encryptor,NULL);
	pthread_create(&tid2,&attr,Decryptor,NULL);

	// wait for 30 seconds, the set thread_cont to zero to terminate Decryptor and Encryptor
	sleep(30);
	thread_cont = 0;

	// call sem_post full and empty to ensure Encryptor or Decryptor released from waiting
	sem_post(&full);
	sem_post(&empty);

	// wait for Encryptor and Decryptor to terminate
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);


	// TO DO: destroy mutex and semaphores
	pthread_mutex_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);

	return 0;
}


void* Encryptor(void *param)
{
	int full_value = 0, empty_value = 0;
	char org_msg[MESSAGE_LEN];
	char encrypted_msg[MESSAGE_LEN];
	

	while(thread_cont){
		// produce an encrypted message
		// 	TO DO: pickup a message and copy it in org_msg (choose a message using rand())
		srand(time(NULL));
		int idx = rand()%NO_MESSAGE; // 0~11
		strcpy(org_msg, messages[idx]);

		//	TO DO: encrypt org_msg to encrypted_msg (encrypted_msg[i] = org_msg[i] + 3, for all i)
		int len = strlen(org_msg);
		for(int i=0; i<len; i++){
			encrypted_msg[i] = org_msg[i] + 3;
		}

		// TO DO: implement entry section following the pseudo code in chap. 5
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);

		// TO DO: put encrypted_msg into buffer folling the pseudo code in chap. 3
		strcpy(buffer[in], encrypted_msg);
		in = (in+1)%BUFFER_SIZE;

		// TO DO: implement exit section following the pseudo code in chap. 5
		pthread_mutex_unlock(&mutex);
		sem_post(&full);

		// TO DO: retrieve current value of full and empty (use sem_getvalue())
		sem_getvalue(&full, &full_value);
		sem_getvalue(&empty, &empty_value);
		printf("Encryptor (full = %d, empty = %d) [%s]\n\t-> [%s]\n", full_value, empty_value, org_msg, encrypted_msg);
		fflush(stdout);


		// sleep for enc_interval seconds
		sleep(enc_interval);
		}
	// print message to notice Encrypter is terminating
	printf("===== Encrypter terminating\n");
}

void* Decryptor(void *param)
{
	int full_value = 0, empty_value = 0;
	char encrypted_msg[MESSAGE_LEN];
	char decrypted_msg[MESSAGE_LEN];

	while(thread_cont){
		strcpy(decrypted_msg,"");

		// TO DO: implement entry section following the pseudo code in chap. 5
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		
		// TO DO: retrieve a message from buffer into encrypted_msg following the pseudo code in chap. 3
		//		after deletion, put an empty string ("") into the deleted slot
		
		strcpy(encrypted_msg, buffer[out]);
		strcpy(buffer[out],"");
		out = (out+1)%BUFFER_SIZE;

		// TO DO: implement exit section following the pseudo code in chap. 5
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);

		// TO DO: decrypt encrypted_msg into decrypted_msg (decrypted_msg[i] = encrypted_msg[i] - 3, for all i)
		int len = strlen(encrypted_msg);
                for(int i=0; i<len; i++){
                        decrypted_msg[i] = encrypted_msg[i] - 3;
                }

		// TO DO: retrieve current value of full and empty (use sem_getvalue())
		sem_getvalue(&full, &full_value);
		sem_getvalue(&empty, &empty_value);

		printf("Decryptor (full = %d, empty = %d) [%s]\n\t-> [%s]\n", full_value, empty_value, encrypted_msg, decrypted_msg);
		fflush(stdout);

		// sleep for enc_interval seconds
		sleep(dec_interval);
		}
	printf("===== Decrypter terminating\n");
}

