#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct {
	long mtype;
	char mtext[1024];
	int receiver;
	int sender;
} MsgType;

int choice();
void send_message(int que_id);
void send_public_message(int que_id);
void get_message(int type, int que_id);
void get_public_message(int type, int que_id);

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
				get_public_message(client, que_id);
				break;
			default :
				return 0;
		}
	}

	return 0;
}

int choice() {
	int n;
	printf("\n1. send private message\n");
	printf("2. send public message\n");
	printf("3. read private message\n");
	printf("4. read public message\n");
	printf("5. quit (and other keys)\n\n");
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
	if (nbytes < 0) fprintf(stderr, "message send error : %d\n", errno);;
}

void send_public_message(int que_id) {
	MsgType msg;
	int type, receiver, sender;
	char text[1024];
	int msg_size = sizeof(msg) - sizeof(msg.mtype);

	printf("\nInput message content :\n");
	scanf("%s", text);
	printf("\n");
	msg.mtype = 10;
	strcpy(msg.mtext, text);
	msg.receiver = 0;
	msg.sender = getpid();

	msgsnd(que_id, &msg, msg_size, IPC_NOWAIT);
}

void get_message(int type, int que_id) {
	MsgType msg;
	int msg_size = sizeof(msg) - sizeof(msg.mtype);

	int nbytes;
	while(1) {
	nbytes = msgrcv(que_id, &msg, msg_size, type, IPC_NOWAIT);
	if (nbytes > 0) {
		printf("[%d] %s\n", msg.sender, msg.mtext);
	}
	else {
		if (errno == ENOMSG) {
			printf("empty queue\n");
			break;
		}
	}}
}

void get_public_message(int type, int que_id) {

}
