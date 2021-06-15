#define HAVE_STRUCT_TIMESPEC 1
#define MAX 1001
#define FCFS 1
#define Priority 2
#define SJF 3
#define RR 4
#define AP 5
#define false 0
#define true 1

#include <stdio.h>
#include<pthread.h>
//#include <io.h> //윈도우용 컴파일
#include<stdlib.h>
#include <semaphore.h>
#include <unistd.h> //리눅스용 헤더



typedef struct process {
	int class_num,pro_num, cpu_t, wait_t, pri, ta_t, rem_t, resp_t, abs_pri;
	int terminate_flag;
	//line 1: 큐 클래스 번호,프로세스 번호, 동작 시간,대기시간,우선순위, 전체 동작 시간
	//남은 동작 시간(SJF,RR), 응답시간,절대 우선순위 
	//line 2
}process;

//전체 프로세스를 받아오는 큐
process ready_queue[MAX];
//전체 프로세스 개수
int idx = 0;

//multi level queue 5개
process multilevel_queue1[MAX];
process multilevel_queue2[MAX];
process multilevel_queue3[MAX];
process multilevel_queue4[MAX];
process multilevel_queue5[MAX];


//각 큐에 할당된 프로세스 개수
int c1 = 0;
int c2 = 0;
int c3 = 0;
int c4 = 0;
int c5 = 0;


sem_t semaphore1;
sem_t semaphore2;
sem_t semaphore3;
sem_t semaphore4;
sem_t semaphore5;


void FCFS_sched(process *pro, int n) {

	int processing_t = 0;
	//프로시저 도착 순서가 다 같으므로 process number기준으로 큐에 들어온 순서대로 스케줄링한다.
	
	for (int i = 0; i < n; i++) {
	    //응답시간을 체크하기 위해서 processing_time을 0초부터 계산한다.
	    pro[i].resp_t = processing_t;

		for (int p = pro[i].cpu_t; p > 0; p--) {
			//cpu time 만큼 실행된다.
			processing_t++;
			printf("%d ", pro[i].pro_num);
			pro[i].rem_t--;
		}
		printf("\n");

        for (int j = i+1; j < n; j++) {
			//뒤에 남은 다른 프로세스들에 수행된 시간만큼 wait time을 더해준다.
			
					pro[j].wait_t += pro[i].cpu_t;
			
		}
		
		//해당 프로세스가 실행된 total time은 대기 시간+ 실행된 시간이다.
		pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
		pro[i].terminate_flag = true;
	}

}


void priority_sched(process *pro, int n) {
    //모두 같은 시간에 프로세스가 도착했고, non-preemptive 하므로
	//priority 순으로 정렬하여 순서대로 실행한다.
	//priority가 같으면 process number순으로 정렬
	process temp;
	int processing_t = 0;

	for (int i = n - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (pro[j].pri > pro[j + 1].pri) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}

			else if (pro[j].pri == pro[j + 1].pri && pro[j].pro_num > pro[j + 1].pro_num) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}

     for (int i = 0; i < n; i++) {
		 //우선순위 순서대로 정렬되었으므로, 순서대로 스케줄링하여 실행한다.
		pro[i].resp_t = processing_t;
		for (int p = pro[i].cpu_t; p > 0; p--) {
			printf("%d ", pro[i].pro_num);
			processing_t++;
		}
		printf("\n");

		for (int j = 0; j < n; j++) {
			if (pro[j].terminate_flag == false){
				if (j != i) {
					pro[j].wait_t += pro[i].cpu_t;
				}}
	     }
		pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
		pro[i].terminate_flag = true;
	}


	 /*
	 //매번 스케줄링 decision 내릴때마다 우선순위 제일 높은 프로세스 선택하는 ver.
	 process temp;
	 int processing_t = 0;
	 int terminate_pronum = 0;

	 while (terminate_pronum != n) {

		 //다음 프로세스를 결정해야 하는 순간마다 우선순위가 가장 높은(작은) 프로세스를 골라서 스케줄링한다.
		 //arrival time이 다 똑같으므로 non-preemptive하다.
		 int min_priority = 1000;
		 int min_index = -1;

		 for (int i = 0; i < n; i++) {
			 if (pro[i].terminate_flag == false) {
				 //이미 terminate된 프로세스는 스케줄링에서 제외한다.

				 if (min_priority > pro[i].pri) {
					 //우선순위가 높은 프로세스를 골라서 스케줄링한다.
					 min_priority = pro[i].pri;
					 min_index = i;
				 }
				 else if (min_priority == pro[i].pri&&pro[min_index].pro_num > pro[i].pro_num) {
					 //우선순위가 같다면, process_number가 빠른 순서로 스케줄링한다.
					 min_priority = pro[i].pri;
					 min_index = i;
				 }
			 }
		 }

		 pro[min_index].resp_t = processing_t;
		 for (int p = 0; p < pro[min_index].cpu_t; p++) {
			 printf("%d ", pro[min_index].pro_num);
			 processing_t++;
		 }
		 printf("\n");

		 for (int j = 0; j < n; j++) {
			 if (pro[j].terminate_flag == false) {
				 if (j != min_index) {
					 pro[j].wait_t += pro[min_index].cpu_t;
				 }
			 }
		 }

		 pro[min_index].ta_t = pro[min_index].cpu_t + pro[min_index].wait_t;
		 pro[min_index].terminate_flag = true;
		 terminate_pronum++;
	 }
	 */
}



void RR_sched(process *pro, int n, int Quantum) {
	//프로세스들 사이에 우선순위를 두지 않고 시간단위로 돌아가면서 프로세스를 할당한다.
	int terminate_pronum = 0;
	int rr = 0;
	int processing_t = 0;
	
	//모든 프로세스가 terminate될때까지 round-robin 돌면서 스케줄링한다.
    while (terminate_pronum != n) {
		rr++;
		
		for (int i = 0; i < n; i++) {
			int qtime = 0;

			if (pro[i].terminate_flag == false) {
                //terminate 안된 프로세스만 스케줄링한다.
				if (rr == 1) {
					//response_time은 맨 처음 프로세스가 스케줄링되었을때만 측정되어야 하므로, 
					//round-robin이 한바퀴 돌았을때만 측정한다.
					pro[i].resp_t = processing_t;
				}

				for (int j = 0; j < Quantum; j++) {
					if (pro[i].rem_t == 0) {
						//quantum만큼 수행해주되, remain_time이 0이되면 바로 다음 프로세스 스케줄링한다.
						break;
					}
					else {

						printf("%d ", pro[i].pro_num);
						processing_t++;
						pro[i].rem_t = pro[i].rem_t - 1;
						qtime++;//quantum 만큼 실행이 다 됐으면 qtime=Quantum일것이다. 
					}
				}
				//다음 프로세스가 스케줄링 되면 다음 줄에 출력해야 한다.
				printf("\n");

				for (int p = 0; p < n; p++) {
					if (pro[p].terminate_flag != true) {
						if (p != i) {
							pro[p].wait_t += qtime; 
							//이전 프로세스가 수행된 시간만큼 나머지 프로세스의 wait time에 더해준다.
						}
					}
				}

				if (pro[i].rem_t == 0) {
					//remain_time이 0이 되어 terminate된 프로세스는 
					//terminate_flag와 ,turnaround_time을 측정해준다.
					pro[i].terminate_flag = true;
					pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
					terminate_pronum++;
				}
			}
			else {
				//terminate 되었다면 다음 프로세스로 넘어가서 스케줄링한다.
				continue;
			}
		}

	}
}


void SJF_sched(process *pro, int n) {
	int terminate_pronum = 0;
	int processing_t = 0;

	//큐 안의 모든 프로세스가 terminate될때까지 스케줄링된다.
	while (terminate_pronum != n) {

		//다음 프로세스를 결정해야 하는 순간마다 cpu burst time이 가장 적은 프로세스를 골라서 스케줄링한다.
		//arrival time이 다 똑같으므로 non-preemptive하다.
		int min_time = 1000;
		int min_index = -1;

		for (int i = 0; i < n; i++) {
			if (pro[i].terminate_flag == false) {
				//이미 terminate된 프로세스는 스케줄링에서 제외한다.

				if (min_time > pro[i].cpu_t) {
					//cpu burst time이 가장 적은 프로세스를 골라서 스케줄링한다.
					min_time = pro[i].cpu_t;
					min_index = i;
				}
				else if (min_time == pro[i].cpu_t&&pro[min_index].pro_num > pro[i].pro_num) {
					//cpu burst time이 같다면, process_number가 빠른 순서로 스케줄링한다.
					min_time = pro[i].cpu_t;
					min_index = i;
				}
			}
		}

		//해당 프로세스를 cpu time만큼 실행한다
		pro[min_index].resp_t = processing_t;
		for (int i = 0; i < pro[min_index].cpu_t; i++) {
			processing_t++;
			printf("%d ", pro[min_index].pro_num);
			
		}

		//이미 terminate된 프로세스와, 수행중인 프로세스를 제외하고
	//아직 수행되지 못한 프로세스들에 현재 수행중인 프로세스의 cpu time(수행시간)만큼 wait time을 더해준다.
		for (int i = 0; i < n; i++) {
			if (pro[i].terminate_flag == false) {
				if (i != min_index) {
					pro[i].wait_t += pro[min_index].cpu_t;
				}
			}
		}

		//terminate된 프로세스는 프로세싱하는데 걸린 시간과, terminate flag를 true로 바꿔준다.
		pro[min_index].terminate_flag = true;
		pro[min_index].ta_t = pro[min_index].wait_t + pro[min_index].cpu_t;
		terminate_pronum++;
		printf("\n");
	}
}


//나만의 스케줄러 하나 짜기
void absolute_priority_sched(process *pro,int n,int Quantum) {

	process temp;

	for (int i = n - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (pro[j].abs_pri > pro[j + 1].abs_pri) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}

			else if (pro[j].abs_pri == pro[j + 1].abs_pri && pro[j].pri > pro[j + 1].pri) {
				//절대 우선순위가 같으면 우선순위가 높은 프로세스 먼저 실행한다.
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}


	//arrival time이 모두 같으므로,절대 우선순위 순서로 프로세스를 정렬하였다.
	//해당 큐의 순서대로 RR scheduling을 진행한다.

	int terminate_pronum = 0;
	int rr = 0;
	int processing_t = 0;

	while (terminate_pronum != n) {
		rr++;

		for (int i = 0; i < Quantum; i++) {
			//프로세스가 퀀텀만큼만 동작되어야 한다.
			int qtime = 0;

			if (pro[i].terminate_flag == false) {
				if (rr == 1) { 
					// round-robin방식으로 모든 프로세스를 최초로 한번씩만 스케줄링 했을때만 응답 시간을 측정.
					pro[i].resp_t = processing_t;
				}

				for (int j = 0; j < Quantum; j++) {
					//quantum만큼 수행해주되, remain_time이 0이되면 바로 terminate 안된 다음 프로세스 스케줄링한다.
					if (pro[i].rem_t == 0) {
				        break;
					}
					else {
                        printf("%d ", pro[i].pro_num);
						processing_t++;
						pro[i].rem_t = pro[i].rem_t - 1;
						qtime++;//quantum 만큼 실행이 다 됐으면 qtime=Q일것이다. 
					}
				}

				printf("\n");

				for (int p = 0; p < n; p++) {
					if (pro[p].terminate_flag != true) {
						if (p != i) {
							pro[p].wait_t += qtime; 
							//이전 프로세스가 수행된 시간만큼 나머지 프로세스의 wait time에 더해준다.
						}
					}
				}

				if (pro[i].rem_t == 0) {
					pro[i].terminate_flag = true;
					pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
					terminate_pronum++;
				}
			}
			else {
				continue;
			}
		}

	}

}

//Process number 순서대로 정렬하여 simulation 결과 print하기 위한 함수.
void resort(process *pro, int n) {
	process temp;

	for (int i = n - 1; i > 0; i--) {
		for (int j = 0; j < i; j++) {
			if (pro[j].pro_num > pro[j + 1].pro_num) {
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}

	for (int i = 0; i < n; i++) {
	
		pro[i].rem_t = pro[i].cpu_t;
	}
}

void simulation_print(process *pro, int n, int schednum) {
	
	//각 스케줄링 알고리즘들의 시간을 비교하기 위해서, 시뮬레이션을 돌려 
	//가장 효율적인 스케줄링 알고리즘이 무엇인지 확인할 수 있다.
    float tat = 0;
	float totalwait= 0;
	float totalwork = 0;
	float cpuutil = 0;

	printf("\n");
	switch (schednum) {
	case 1:
		printf("First come first Serve (FCFS)\n====================FCFS======================\n");
		break;

	case 2:
		printf("Priority \n====================Priority======================\n");
		break;

	case 3:
		printf("SJF(Shortest-Job-First) \n====================SJF======================\n");
		break;

	case 4:
		printf("RR(Round-Robin) \n====================RR======================\n");
		break;

	case 5:
		printf("Absoulte-Priority \n==============Absoulute_Priority================\n");
		break;

	}
	printf("C#      P#      BT      Pri     WT      RT      A_PRI     TAT\n");
	    //순서대로 queue class number, prcess number, cpu burst time, priority, 
	   //waiting_time, Response_time, Absolute_priority, Turnaround time

	for (int p = 0; p < n; p++) {
       tat = tat + pro[p].ta_t;
		totalwait += pro[p].wait_t;
		totalwork+= totalwork + pro[p].cpu_t;
		printf("%d       %d        %d       %d       %d        %d        %d       %d\n",
   	   pro[p].class_num, pro[p].pro_num, pro[p].cpu_t, pro[p].pri, pro[p].wait_t,pro[p].resp_t, pro[p].abs_pri,pro[p].ta_t);

	}
	
	cpuutil = (totalwork / (totalwork + totalwait)) * 100;

	printf("total waiting time:%2f\n", totalwait);
	printf("average turnaround time: %.2f\n", tat / n);
	printf("CPU Utilization rate: %.2f%\n",cpuutil);
	printf("==============================================\n\n");
	
}



void *multiqueue_sched(void *param);
void *queue1_sched(void *param);
void *queue2_sched(void *param);
void *queue3_sched(void *param);
void *queue4_sched(void *param);
void *queue5_sched(void *param);

int main() {
	FILE *fp;
	//알고리즘 성능 분석용 케이스 1
	fp= fopen("process.txt","r");

	//랜덤으로 cpu time, priority 할당한 프로세스 케이스 2
	//fp = fopen("process2.txt", "r");
    printf("file input start..\n");
	
	while (!feof(fp)) {
		fscanf(fp, "%d", &ready_queue[idx].class_num);
		fscanf(fp, "%d", &ready_queue[idx].pro_num);
		fscanf(fp, "%d", &ready_queue[idx].cpu_t);
		fscanf(fp, "%d", &ready_queue[idx].pri);
		ready_queue[idx].rem_t = ready_queue[idx].cpu_t;
		ready_queue[idx].terminate_flag = false;
		ready_queue[idx].resp_t = 0;
		ready_queue[idx].abs_pri = (ready_queue[idx].pri) + (ready_queue[idx].cpu_t) ;
		
		idx++;
	}
	printf("file input finish...\n\n");

	printf("------------Process List----------\n");
	printf("C#      P#      BT      Pri      RT\n");

	for (int p = 0; p < idx; p++) {
       //모든 프로세스가 제대로 입력되었는지 체크하기 위해서 프로세스 목록을 한번 출력해준다.
		printf("%d        %d        %d        %d       %d\n", 
	  ready_queue[p].class_num, ready_queue[p].pro_num, ready_queue[p].cpu_t, ready_queue[p].pri,ready_queue[p].rem_t);

	}

	printf("\n\n");
	//스레드를 생성한다.
	//큐간의 스케줄링 담당하는 main thread를 만들어 실행시켜준다.
	pthread_t mainthread;
	pthread_create(&mainthread, NULL, multiqueue_sched, (void *)idx);
	pthread_join(mainthread, NULL);

	printf("fin.....");
   }


void *multiqueue_sched(void *param) {
	//class number대로 multilevel queue로 각 프로세스 할당하고
	//class number 순서에 따라 priority scheduling하여 순서대로 실행함.

	int num = (int)param;
	printf("multi-level queue scheduling in process...");

	//같은 시간에 도착한 프로세스들을 class number에 따라 각 큐에 할당한다.
	//프로세스 number 순서대로 큐에 들어가게 되어 있다.
	for (int i = 0; i < num; i++) {
		int k = ready_queue[i].class_num;
		if (k == 1) {
			multilevel_queue1[c1] = ready_queue[i];
			c1++;
		}

		else if (k == 2) {
			multilevel_queue2[c2] = ready_queue[i];
			c2++;
		}

		else if (k == 3) {
			multilevel_queue3[c3] = ready_queue[i];
			c3++;
		}
		else if (k == 4) {
			multilevel_queue4[c4] = ready_queue[i];
			c4++;
		}
		else if (k == 5) {
			multilevel_queue5[c5] = ready_queue[i];
			c5++;
		}
		
	}

	printf("\n\n");

    //class number를 priority로 하여 수행하지만, arrival time이 다 같으므로 큐 순서대로 실행된다.
	sem_init(&semaphore1, 0, 1);
	sem_init(&semaphore2, 0, 0);
	sem_init(&semaphore3, 0, 0);
	sem_init(&semaphore4, 0, 0);
	sem_init(&semaphore5, 0, 0);

	pthread_t t[5];
	pthread_create(&t[0], NULL, queue1_sched, (void*) c1);
	pthread_create(&t[1], NULL, queue2_sched, (void*)c2);
	pthread_create(&t[2], NULL, queue3_sched, (void*)c3);
	pthread_create(&t[3], NULL, queue4_sched, (void*)c4);
	pthread_create(&t[4], NULL, queue5_sched, (void*)c5);
	
	pthread_join(t[0], NULL);
	pthread_join(t[1], NULL);
	pthread_join(t[2], NULL);
	pthread_join(t[3], NULL);
	pthread_join(t[4], NULL);
	
	
	// 모든 스레드가 종료되었음을 알리는 문장, 그 후 semaphore와 관련 리소스를 모두 파괴한다.
	printf("Every multilevel-queue scheduling is terminated...\n");
	sem_destroy(&semaphore1);
	sem_destroy(&semaphore2);
	sem_destroy(&semaphore3);
	sem_destroy(&semaphore4);
	sem_destroy(&semaphore5);


	printf("Algorithm simulation result print...\n");
	//큐 들 사이 알고리즘 성능을 비교하기 위해서 시뮬레이션 결과 출력함.
	simulation_print(multilevel_queue1, c1, FCFS);
	simulation_print(multilevel_queue2, c2, Priority);
	simulation_print(multilevel_queue3, c3, RR);
	simulation_print(multilevel_queue4, c4, SJF);
	simulation_print(multilevel_queue5, c4, AP);

   
	pthread_exit(0);
	
}

void *queue1_sched(void *param) {
	int num = (int)param;

	   //해당 스레드의 semaphore인 semaphore1이 1로 초기화 되어있으므로, 바로 스레드 1이 실행된다.
	   //0으로 초기화되어있는 상태인 다른 스레드들은 모두 block된다.
	    sem_wait(&semaphore1);
		printf("first queue scheduling in process...\n");

		FCFS_sched(multilevel_queue1, num);
		resort(multilevel_queue1, num);
		
		//다음 스레드의 semaphore를 1 증가시켜주고, 해당 스레드는 종료처리된다.
		sem_post(&semaphore2);
		pthread_exit(0);
	
}

void *queue2_sched(void *param){
	int num = (int)param;
	  
	//0으로 초기화되어있는 상태였다가 바로 이전 스레드가 종료되기 이전에 값을
	//1로 증가시켜줘서 바로 대기 상태에서 빠져 나오게된다.
	   sem_wait(&semaphore2);
		printf("second queue scheduling in process...\n");

		priority_sched(multilevel_queue2, num);
		resort(multilevel_queue2, num);
		
	//다음스레드의 semaphore를 1 증가시켜주고, 해당 스레드는 종료처리된다.
		sem_post(&semaphore3);
		pthread_exit(0);
}


void *queue3_sched(void *param) {
	   int num = (int)param;

	
		sem_wait(&semaphore3);
		printf("third queue scheduling in process...\n");

		RR_sched(multilevel_queue3, num, 3);
		resort(multilevel_queue3, num);

		sem_post(&semaphore4);
		pthread_exit(0);
	
}

void *queue4_sched(void *param) {

	int num = (int)param;
	
		sem_wait(&semaphore4);
		printf("forth queue scheduling in process...\n");

		SJF_sched(multilevel_queue4, num);
		resort(multilevel_queue4, num);
		
		sem_post(&semaphore5);
		pthread_exit(0);
	
}

void *queue5_sched(void *param) {

	    int num = (int)param;
	
		sem_wait(&semaphore5);
		printf("fifth queue scheduling in process...\n");

		absolute_priority_sched(multilevel_queue5, num, 3);
		resort(multilevel_queue5, num);
        
		//마지막 스레드이므로 바로 종료시켜준다. 어차피 바로 semaphore가 destroy되게 된다.
		pthread_exit(0);
}

