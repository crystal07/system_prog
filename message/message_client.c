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

int choice();
void send_message(int que_id);
void send_public_message(int que_id);
void get_message(int type, int que_id);
void get_public_message();
void all_public_message();

int main() {
	int client;

	printf("What's your client number? (1 or 2 or 3) ");
	scanf("%d", &client);

	key_t key = 4070 + client;
	int que_id;
	if ((que_id = msgget(key, IPC_CREAT | 0666)) < 0) {
		fprintf(stderr, "msgget error\n");
		return -1;
	}

	printf("que %d\n", que_id);

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
	
	int cmd;

	while(1) {
		cmd = choice();
		switch(cmd) {
			case 1 :
				send_message(que_id);
				break;
			case 2 :
				send_public_message(que_id);
				break;
			case 3 :
				get_message(client, que_id);
				break;
			case 4 :
				get_public_message();
				break;
			case 5:
				all_public_message();
				break;
			default :
				return 0;
		}
	}

	shmdt(shared_memory);

	return 0;
}

int choice() {
	int n;
	printf("\n1. send private message\n");
	printf("2. send public message\n");
	printf("3. read private message\n");
	printf("4. read public message\n");
	printf("5. read all public message\n");
	printf("6. quit (and other keys)\n\n");
	printf("choice : ");
	scanf("%d", &n);
	printf("\n");
	return n;
}

void send_message(int que_id) {
	MsgType msg;
	int type;
	char text[1024];
	int msg_size = sizeof(msg) - sizeof(msg.mtype);

	printf("What client do you want to send? ");
	scanf("%d", &type);
	printf("Input message content :\n");
	scanf("%s", text);
	printf("\n");
	msg.mtype = 10;
	strcpy(msg.mtext, text);
	msg.receiver = type;
	msg.sender = getpid();

	int nbytes = msgsnd(que_id, &msg, msg_size, IPC_NOWAIT);
	if (nbytes < 0) fprintf(stderr, "message send error : %d\n", errno);
}

void send_public_message(int que_id) {
	MsgType msg;
	char text[1024];
	int msg_size = sizeof(msg) - sizeof(msg.mtype);

	printf("Input public message content :\n");
	scanf("%s", text);
	printf("\n");
	msg.mtype = 20;
	strcpy(msg.mtext, text);
	msg.receiver = 0;
	msg.sender = getpid();

	int nbytes = msgsnd(que_id, &msg, msg_size, IPC_NOWAIT);
	if (nbytes < 0) fprintf(stderr, "message send error : %d\n", errno);
}

void get_message(int type, int que_id) {
	MsgType msg;
	int msg_size = sizeof(msg) - sizeof(msg.mtype);

	int nbytes;

	nbytes = msgrcv(que_id, &msg, sizeof(msg), type, IPC_NOWAIT);
	if (nbytes > 0) {
		printf("[%d] %s\n", msg.sender, msg.mtext);
	}
	else {
		if (errno == ENOMSG) {
			printf("empty queue\n");
		}
	}
}

void get_public_message() {
	int cnt = 0;
	int sender;
	char mtext[1024];

	int cnt_size = sizeof(shcnt);
	int sender_size = sizeof(sender);
	int msg_size = strlen(mtext) + 1;
	int total_size = sizeof(sender) + strlen(mtext) + 1;

	memcpy(&cnt, shared_memory, cnt_size);
	memcpy(buffer, shared_memory, total_size*cnt+cnt_size);
	//memcpy(&cnt, buffer, cnt_size);

	if (cnt <= shcnt)
	{
		printf("no more message\n");
		return;
	}

	memcpy(&sender, buffer + cnt_size + total_size * shcnt, sender_size);
	memcpy(mtext, buffer + cnt_size + total_size * shcnt + sender_size, msg_size);
	shcnt += 1;

	printf("[%d] %s\n", sender, mtext);
}

void all_public_message() {
	int cnt = 0;
	int sender;
	char mtext[1024];

	int cnt_size = sizeof(cnt);
	int sender_size = sizeof(sender);
	int msg_size = strlen(mtext) + 1;
	int total_size = sizeof(sender) + strlen(mtext) + 1;

	memcpy(&cnt, shared_memory, cnt_size);
	memcpy(buffer, shared_memory, total_size*cnt+cnt_size);
	//memcpy(&cnt, buffer, cnt_size);

	for (int i = 0; i < cnt; ++i)
	{
		memcpy(&sender, buffer + cnt_size + total_size * i, sender_size);
		memcpy(mtext, buffer + cnt_size + total_size * i + sender_size, msg_size);

		printf("[%d] %s\n", sender, mtext);
	}
}
