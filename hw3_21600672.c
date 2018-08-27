//OS homework3
//21600672 hyejin Jeong
// perfectly done >_<

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define BUFFER_SIZE 1024


int repeat = 1;
void *receiver(void *param);


int main(int argc, char *argv[])
{
	typedef struct{
		long data_type;
		char string_buffer[BUFFER_SIZE];
	} t_data;
	
	t_data data;
	
		
        pthread_t tid;
	pthread_attr_t attr;

        if(argc<3){
                fprintf(stderr, "Usage: ./hw3 <snd_key> <rcv_key>\n");
                exit(0);
        }

        int snd_key = atoi(argv[1]);
        int rcv_key = atoi(argv[2]);

        int snd_qid = msgget((key_t)snd_key, IPC_CREAT|0666); // snd_queue
        int rcv_qid = msgget((key_t)rcv_key, IPC_CREAT|0666); // rcv_queue

	
	data.data_type = 1;
	
	printf("snd_key = %d, rcv_key = %d\n", snd_key, rcv_key);

        pthread_attr_init(&attr);
        pthread_create(&tid,&attr,receiver,rcv_qid);
	

        while(1){
		memset(data.string_buffer,'\0', BUFFER_SIZE);

                printf("[msg to send] ");
                fgets(data.string_buffer, BUFFER_SIZE, stdin);

		int len = strlen(data.string_buffer);
		data.string_buffer[len-1] = '\0';		

                if(strncmp(data.string_buffer,"exit",4)==0){
                        repeat=0;
                        break;
                }



                if(msgsnd(snd_qid,&data, sizeof(t_data)-sizeof(long), 0)==-1)
			perror("msgsnd error");

        }
	

        pthread_join(tid, NULL);

        msgctl(snd_qid,IPC_RMID,0);
        msgctl(rcv_qid,IPC_RMID,0);
        return 0;
}


void *receiver(void *param)
{

	typedef struct{
                long data_type;
                char message_buffer[BUFFER_SIZE];
        } t_data;

        t_data data;


        while(repeat){
	
	memset(data.message_buffer,'\0', BUFFER_SIZE);

	if(msgrcv((int)param, &data, sizeof(t_data)-sizeof(long), 0, IPC_NOWAIT)!=-1)
		printf("[Incoming] \"%s\"\n", data.message_buffer);

        usleep(1000);
        }

        pthread_exit(0);
}

