#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

typedef struct {
	long mtype;
	char mtext[1024];
	int receiver;
	int sender;
} MsgType;

int shmid;
key_t keyval;
MsgType *shmsg;
int shcnt = 0;
void *shared_memory = (void *)0;
char buffer[112640];

void send_message(MsgType msg, int * que_id);
void send_public_message(int sender, char mtext[1024]);

int main() {
	// shared
	keyval = 1234;
	shmid = shmget(keyval, (key_t)1024, IPC_CREAT | 0666);
	if (shmid == -1)
	{
		return -1;
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1)
	{
		perror("shmat failed : ");
		exit(0);
	}

	//queue
	key_t key[3];
	int que_id[3];

	for (int i = 0; i < 3; i++) {
		key[i] = 4070 + (i + 1);
		que_id[i] = msgget(key[i], IPC_CREAT | 0666);
		printf("%d th que_id : %d\n", i+1, que_id[i]);
	}

	MsgType received_msg;
	int msg_size = sizeof(received_msg) - sizeof(received_msg.mtype);
	int nbytes;

	while (1) {
		for (int i = 0; i < 3; i++) { //client message
			nbytes = msgrcv(que_id[i], &received_msg, msg_size, 10, IPC_NOWAIT);
			if (nbytes <= 0) {
				nbytes = msgrcv(que_id[i], &received_msg, msg_size, 20, IPC_NOWAIT);
				if (nbytes > 0) {
					printf("public message received from %d\n", received_msg.sender);
					send_public_message(received_msg.sender, received_msg.mtext);
				}
			}
			else {
				printf("message received from %d\n", received_msg.sender);
				send_message(received_msg, que_id);
			}
		}
	}

	shmdt(shared_memory);

	return 0;
}

void send_message(MsgType msg, int * que_id) {
	MsgType sended_msg;
	int msg_size = sizeof(sended_msg) - sizeof(sended_msg.mtype);

	sended_msg.mtype = (long)msg.receiver;
	strcpy(sended_msg.mtext, msg.mtext);
	sended_msg.receiver = msg.receiver;
	sended_msg.sender = msg.sender;

	if (sended_msg.receiver > 0) {
		int nbytes = msgsnd(que_id[sended_msg.mtype-1], &sended_msg, msg_size, IPC_NOWAIT);
		if (nbytes >= 0) printf("[%d] %s\n", sended_msg.sender, sended_msg.mtext);
		else fprintf(stderr, "message send error %d\n", errno);
	}
	else;
}

void send_public_message(int sender, char mtext[1024]) {
	int cnt_size = sizeof(shcnt);
	int sender_size = sizeof(sender);
	int msg_size = strlen(mtext) + 1;
	int total_size = sizeof(sender) + strlen(mtext) + 1;

	shcnt += 1;
	memcpy(buffer, &shcnt, cnt_size);
	memcpy(buffer + cnt_size + total_size * (shcnt-1), &sender, sender_size);
	memcpy(buffer + cnt_size + total_size * (shcnt-1) + sender_size, mtext, msg_size);
	memcpy(shared_memory, buffer, total_size * shcnt + cnt_size);
	printf("[%d] %s\n", sender, mtext);
}
