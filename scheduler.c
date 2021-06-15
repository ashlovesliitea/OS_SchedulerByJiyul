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
//#include <io.h> //������� ������
#include<stdlib.h>
#include <semaphore.h>
#include <unistd.h> //�������� ���



typedef struct process {
	int class_num,pro_num, cpu_t, wait_t, pri, ta_t, rem_t, resp_t, abs_pri;
	int terminate_flag;
	//line 1: ť Ŭ���� ��ȣ,���μ��� ��ȣ, ���� �ð�,���ð�,�켱����, ��ü ���� �ð�
	//���� ���� �ð�(SJF,RR), ����ð�,���� �켱���� 
	//line 2
}process;

//��ü ���μ����� �޾ƿ��� ť
process ready_queue[MAX];
//��ü ���μ��� ����
int idx = 0;

//multi level queue 5��
process multilevel_queue1[MAX];
process multilevel_queue2[MAX];
process multilevel_queue3[MAX];
process multilevel_queue4[MAX];
process multilevel_queue5[MAX];


//�� ť�� �Ҵ�� ���μ��� ����
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
	//���ν��� ���� ������ �� �����Ƿ� process number�������� ť�� ���� ������� �����ٸ��Ѵ�.
	
	for (int i = 0; i < n; i++) {
	    //����ð��� üũ�ϱ� ���ؼ� processing_time�� 0�ʺ��� ����Ѵ�.
	    pro[i].resp_t = processing_t;

		for (int p = pro[i].cpu_t; p > 0; p--) {
			//cpu time ��ŭ ����ȴ�.
			processing_t++;
			printf("%d ", pro[i].pro_num);
			pro[i].rem_t--;
		}
		printf("\n");

        for (int j = i+1; j < n; j++) {
			//�ڿ� ���� �ٸ� ���μ����鿡 ����� �ð���ŭ wait time�� �����ش�.
			
					pro[j].wait_t += pro[i].cpu_t;
			
		}
		
		//�ش� ���μ����� ����� total time�� ��� �ð�+ ����� �ð��̴�.
		pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
		pro[i].terminate_flag = true;
	}

}


void priority_sched(process *pro, int n) {
    //��� ���� �ð��� ���μ����� �����߰�, non-preemptive �ϹǷ�
	//priority ������ �����Ͽ� ������� �����Ѵ�.
	//priority�� ������ process number������ ����
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
		 //�켱���� ������� ���ĵǾ����Ƿ�, ������� �����ٸ��Ͽ� �����Ѵ�.
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
	 //�Ź� �����ٸ� decision ���������� �켱���� ���� ���� ���μ��� �����ϴ� ver.
	 process temp;
	 int processing_t = 0;
	 int terminate_pronum = 0;

	 while (terminate_pronum != n) {

		 //���� ���μ����� �����ؾ� �ϴ� �������� �켱������ ���� ����(����) ���μ����� ��� �����ٸ��Ѵ�.
		 //arrival time�� �� �Ȱ����Ƿ� non-preemptive�ϴ�.
		 int min_priority = 1000;
		 int min_index = -1;

		 for (int i = 0; i < n; i++) {
			 if (pro[i].terminate_flag == false) {
				 //�̹� terminate�� ���μ����� �����ٸ����� �����Ѵ�.

				 if (min_priority > pro[i].pri) {
					 //�켱������ ���� ���μ����� ��� �����ٸ��Ѵ�.
					 min_priority = pro[i].pri;
					 min_index = i;
				 }
				 else if (min_priority == pro[i].pri&&pro[min_index].pro_num > pro[i].pro_num) {
					 //�켱������ ���ٸ�, process_number�� ���� ������ �����ٸ��Ѵ�.
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
	//���μ����� ���̿� �켱������ ���� �ʰ� �ð������� ���ư��鼭 ���μ����� �Ҵ��Ѵ�.
	int terminate_pronum = 0;
	int rr = 0;
	int processing_t = 0;
	
	//��� ���μ����� terminate�ɶ����� round-robin ���鼭 �����ٸ��Ѵ�.
    while (terminate_pronum != n) {
		rr++;
		
		for (int i = 0; i < n; i++) {
			int qtime = 0;

			if (pro[i].terminate_flag == false) {
                //terminate �ȵ� ���μ����� �����ٸ��Ѵ�.
				if (rr == 1) {
					//response_time�� �� ó�� ���μ����� �����ٸ��Ǿ������� �����Ǿ�� �ϹǷ�, 
					//round-robin�� �ѹ��� ���������� �����Ѵ�.
					pro[i].resp_t = processing_t;
				}

				for (int j = 0; j < Quantum; j++) {
					if (pro[i].rem_t == 0) {
						//quantum��ŭ �������ֵ�, remain_time�� 0�̵Ǹ� �ٷ� ���� ���μ��� �����ٸ��Ѵ�.
						break;
					}
					else {

						printf("%d ", pro[i].pro_num);
						processing_t++;
						pro[i].rem_t = pro[i].rem_t - 1;
						qtime++;//quantum ��ŭ ������ �� ������ qtime=Quantum�ϰ��̴�. 
					}
				}
				//���� ���μ����� �����ٸ� �Ǹ� ���� �ٿ� ����ؾ� �Ѵ�.
				printf("\n");

				for (int p = 0; p < n; p++) {
					if (pro[p].terminate_flag != true) {
						if (p != i) {
							pro[p].wait_t += qtime; 
							//���� ���μ����� ����� �ð���ŭ ������ ���μ����� wait time�� �����ش�.
						}
					}
				}

				if (pro[i].rem_t == 0) {
					//remain_time�� 0�� �Ǿ� terminate�� ���μ����� 
					//terminate_flag�� ,turnaround_time�� �������ش�.
					pro[i].terminate_flag = true;
					pro[i].ta_t = pro[i].cpu_t + pro[i].wait_t;
					terminate_pronum++;
				}
			}
			else {
				//terminate �Ǿ��ٸ� ���� ���μ����� �Ѿ�� �����ٸ��Ѵ�.
				continue;
			}
		}

	}
}


void SJF_sched(process *pro, int n) {
	int terminate_pronum = 0;
	int processing_t = 0;

	//ť ���� ��� ���μ����� terminate�ɶ����� �����ٸ��ȴ�.
	while (terminate_pronum != n) {

		//���� ���μ����� �����ؾ� �ϴ� �������� cpu burst time�� ���� ���� ���μ����� ��� �����ٸ��Ѵ�.
		//arrival time�� �� �Ȱ����Ƿ� non-preemptive�ϴ�.
		int min_time = 1000;
		int min_index = -1;

		for (int i = 0; i < n; i++) {
			if (pro[i].terminate_flag == false) {
				//�̹� terminate�� ���μ����� �����ٸ����� �����Ѵ�.

				if (min_time > pro[i].cpu_t) {
					//cpu burst time�� ���� ���� ���μ����� ��� �����ٸ��Ѵ�.
					min_time = pro[i].cpu_t;
					min_index = i;
				}
				else if (min_time == pro[i].cpu_t&&pro[min_index].pro_num > pro[i].pro_num) {
					//cpu burst time�� ���ٸ�, process_number�� ���� ������ �����ٸ��Ѵ�.
					min_time = pro[i].cpu_t;
					min_index = i;
				}
			}
		}

		//�ش� ���μ����� cpu time��ŭ �����Ѵ�
		pro[min_index].resp_t = processing_t;
		for (int i = 0; i < pro[min_index].cpu_t; i++) {
			processing_t++;
			printf("%d ", pro[min_index].pro_num);
			
		}

		//�̹� terminate�� ���μ�����, �������� ���μ����� �����ϰ�
	//���� ������� ���� ���μ����鿡 ���� �������� ���μ����� cpu time(����ð�)��ŭ wait time�� �����ش�.
		for (int i = 0; i < n; i++) {
			if (pro[i].terminate_flag == false) {
				if (i != min_index) {
					pro[i].wait_t += pro[min_index].cpu_t;
				}
			}
		}

		//terminate�� ���μ����� ���μ����ϴµ� �ɸ� �ð���, terminate flag�� true�� �ٲ��ش�.
		pro[min_index].terminate_flag = true;
		pro[min_index].ta_t = pro[min_index].wait_t + pro[min_index].cpu_t;
		terminate_pronum++;
		printf("\n");
	}
}


//������ �����ٷ� �ϳ� ¥��
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
				//���� �켱������ ������ �켱������ ���� ���μ��� ���� �����Ѵ�.
				temp = pro[j + 1];
				pro[j + 1] = pro[j];
				pro[j] = temp;
			}
		}
	}


	//arrival time�� ��� �����Ƿ�,���� �켱���� ������ ���μ����� �����Ͽ���.
	//�ش� ť�� ������� RR scheduling�� �����Ѵ�.

	int terminate_pronum = 0;
	int rr = 0;
	int processing_t = 0;

	while (terminate_pronum != n) {
		rr++;

		for (int i = 0; i < Quantum; i++) {
			//���μ����� ���Ҹ�ŭ�� ���۵Ǿ�� �Ѵ�.
			int qtime = 0;

			if (pro[i].terminate_flag == false) {
				if (rr == 1) { 
					// round-robin������� ��� ���μ����� ���ʷ� �ѹ����� �����ٸ� �������� ���� �ð��� ����.
					pro[i].resp_t = processing_t;
				}

				for (int j = 0; j < Quantum; j++) {
					//quantum��ŭ �������ֵ�, remain_time�� 0�̵Ǹ� �ٷ� terminate �ȵ� ���� ���μ��� �����ٸ��Ѵ�.
					if (pro[i].rem_t == 0) {
				        break;
					}
					else {
                        printf("%d ", pro[i].pro_num);
						processing_t++;
						pro[i].rem_t = pro[i].rem_t - 1;
						qtime++;//quantum ��ŭ ������ �� ������ qtime=Q�ϰ��̴�. 
					}
				}

				printf("\n");

				for (int p = 0; p < n; p++) {
					if (pro[p].terminate_flag != true) {
						if (p != i) {
							pro[p].wait_t += qtime; 
							//���� ���μ����� ����� �ð���ŭ ������ ���μ����� wait time�� �����ش�.
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

//Process number ������� �����Ͽ� simulation ��� print�ϱ� ���� �Լ�.
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
	
	//�� �����ٸ� �˰������ �ð��� ���ϱ� ���ؼ�, �ùķ��̼��� ���� 
	//���� ȿ������ �����ٸ� �˰����� �������� Ȯ���� �� �ִ�.
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
	    //������� queue class number, prcess number, cpu burst time, priority, 
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
	//�˰��� ���� �м��� ���̽� 1
	fp= fopen("process.txt","r");

	//�������� cpu time, priority �Ҵ��� ���μ��� ���̽� 2
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
       //��� ���μ����� ����� �ԷµǾ����� üũ�ϱ� ���ؼ� ���μ��� ����� �ѹ� ������ش�.
		printf("%d        %d        %d        %d       %d\n", 
	  ready_queue[p].class_num, ready_queue[p].pro_num, ready_queue[p].cpu_t, ready_queue[p].pri,ready_queue[p].rem_t);

	}

	printf("\n\n");
	//�����带 �����Ѵ�.
	//ť���� �����ٸ� ����ϴ� main thread�� ����� ��������ش�.
	pthread_t mainthread;
	pthread_create(&mainthread, NULL, multiqueue_sched, (void *)idx);
	pthread_join(mainthread, NULL);

	printf("fin.....");
   }


void *multiqueue_sched(void *param) {
	//class number��� multilevel queue�� �� ���μ��� �Ҵ��ϰ�
	//class number ������ ���� priority scheduling�Ͽ� ������� ������.

	int num = (int)param;
	printf("multi-level queue scheduling in process...");

	//���� �ð��� ������ ���μ������� class number�� ���� �� ť�� �Ҵ��Ѵ�.
	//���μ��� number ������� ť�� ���� �Ǿ� �ִ�.
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

    //class number�� priority�� �Ͽ� ����������, arrival time�� �� �����Ƿ� ť ������� ����ȴ�.
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
	
	
	// ��� �����尡 ����Ǿ����� �˸��� ����, �� �� semaphore�� ���� ���ҽ��� ��� �ı��Ѵ�.
	printf("Every multilevel-queue scheduling is terminated...\n");
	sem_destroy(&semaphore1);
	sem_destroy(&semaphore2);
	sem_destroy(&semaphore3);
	sem_destroy(&semaphore4);
	sem_destroy(&semaphore5);


	printf("Algorithm simulation result print...\n");
	//ť �� ���� �˰��� ������ ���ϱ� ���ؼ� �ùķ��̼� ��� �����.
	simulation_print(multilevel_queue1, c1, FCFS);
	simulation_print(multilevel_queue2, c2, Priority);
	simulation_print(multilevel_queue3, c3, RR);
	simulation_print(multilevel_queue4, c4, SJF);
	simulation_print(multilevel_queue5, c4, AP);

   
	pthread_exit(0);
	
}

void *queue1_sched(void *param) {
	int num = (int)param;

	   //�ش� �������� semaphore�� semaphore1�� 1�� �ʱ�ȭ �Ǿ������Ƿ�, �ٷ� ������ 1�� ����ȴ�.
	   //0���� �ʱ�ȭ�Ǿ��ִ� ������ �ٸ� ��������� ��� block�ȴ�.
	    sem_wait(&semaphore1);
		printf("first queue scheduling in process...\n");

		FCFS_sched(multilevel_queue1, num);
		resort(multilevel_queue1, num);
		
		//���� �������� semaphore�� 1 ���������ְ�, �ش� ������� ����ó���ȴ�.
		sem_post(&semaphore2);
		pthread_exit(0);
	
}

void *queue2_sched(void *param){
	int num = (int)param;
	  
	//0���� �ʱ�ȭ�Ǿ��ִ� ���¿��ٰ� �ٷ� ���� �����尡 ����Ǳ� ������ ����
	//1�� ���������༭ �ٷ� ��� ���¿��� ���� �����Եȴ�.
	   sem_wait(&semaphore2);
		printf("second queue scheduling in process...\n");

		priority_sched(multilevel_queue2, num);
		resort(multilevel_queue2, num);
		
	//������������ semaphore�� 1 ���������ְ�, �ش� ������� ����ó���ȴ�.
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
        
		//������ �������̹Ƿ� �ٷ� ��������ش�. ������ �ٷ� semaphore�� destroy�ǰ� �ȴ�.
		pthread_exit(0);
}

