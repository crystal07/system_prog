#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/time.h>

static void sig_process(int);

static int pid1;
static int pid2;
static int pid3;
static struct timeval startTime, endTime;
static int max_count = 0;
static char* file;

void main(int argc, char* argv[])
{
	max_count = atoi(argv[1]);
	file = argv[2];
	int temp1, temp2, temp3;

	FILE *fp = fopen(file, "a");
	fclose(fp);

	signal(SIGUSR1, sig_process);
	signal(SIGALRM, sig_process);
	signal(SIGUSR2, sig_process);

	temp1 = fork();
	if (temp1 != 0)
	{
		temp2 = fork();
	}

	if(temp2 != 0)
	{
		temp3 = fork();
	}
	
	gettimeofday( &startTime, NULL );
	
	if(temp1 > 0 && temp2 > 0 && temp3 > 0)
	{
		FILE *ff = fopen("pid.txt", "w");
		fprintf(ff, "%d %d %d",temp1, temp2, temp3);
		fclose(ff);
		
		kill(pid1, SIGUSR1);
	}	
	else
	{
		for (; ; )
			pause();
	}
}

static void sig_process(int signo)
{
	FILE *ff = fopen("pid.txt", "r");
	fscanf(ff, "%d %d %d", &pid1, &pid2, &pid3);
	fclose(ff);
	FILE *fp = fopen(file, "r+");
	int count;
	fscanf(fp, "%d", &count);

	if(count >= max_count)
	{
		gettimeofday( &endTime, NULL );

		struct timeval gepTime;
		gepTime.tv_sec  = endTime.tv_sec - startTime.tv_sec;
		gepTime.tv_usec = endTime.tv_usec - endTime.tv_usec;
		
		printf("%ld.%ld\n", gepTime.tv_sec, gepTime.tv_usec);

		fclose(fp);
		return;
	}
	else
	{

		fseek(fp, 0, SEEK_SET);
		count += 1;

		fprintf(fp, "%10d", count);
	
		fclose(fp);
	}	

	if (signo == SIGUSR1)
		kill(pid2, SIGALRM);
	else if (signo == SIGALRM)
		kill(pid3, SIGUSR2);
	else if (signo == SIGUSR2)
		kill(pid1, SIGUSR1);
}

