# OS_SchedulerByJiyul

1) 프로젝트의 목적

- 실제 컴퓨터에서 프로세스들을 스케줄링 하는 방법인 Multi-level queue를 구현해보고자 합니다.
 컴퓨터에서는 프로세스의 우선순위에 따라 n개의 큐에다가 각각 프로세스들을 할당하고, 큐의 우선순위가 높은 순서대로 각 큐의 알고리즘에 따라 CPU에 프로그램을 할당합니다. 
이번 Project에서는 이러한 Multi-level queue를 다섯 단계의 큐로 구성하고, 각 큐 안의 스케줄링 방식으로는 기존에 있는 SJF, RR, FCFS, SJF 등의 스케줄링 알고리즘들과 
직접 구상해본 나만의 스케줄링 알고리즘을 채택하여 실제 C 코드로 구현해보았습니다.  
 
 - 그 이후에는 적절한 process input file을 입력하고 컴파일 하여 각 큐가 scheduling 된 시뮬레이션 결과를 토대로,
예상했던 결과와 코드로 실행한 결과가 동일한 지 비교하여 c 코드가 잘 구현되었는지 확인하고, 
CPU Scheduling criteria를 직접 측정한 결과를 통해 각 알고리즘의 성능을 비교하여 각각의 알고리즘이 어떤 점에는 우수하고, 
어떤 점에는 개선이 필요한지 분석해 보았습니다.


2) 사용 프로그램 및 환경
  
 - C 코드 에디터: Visual studio 2017
 - 컴파일 환경 : Kali Linux의 Terminal에서 gcc 명령어를 통해 컴파일
