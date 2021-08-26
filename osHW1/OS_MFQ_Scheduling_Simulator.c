#include <stdio.h>
#define MAX_PROCESS 1000
#define MAX_TIME 100000
int p_time[1001][2]; // [][0] == TT, [][1] == WT
char gantt_chart[MAX_TIME + 1][MAX_PROCESS + 1];

typedef struct process {
	int PID;
	int AT;
	int QUEUE_NUM;
	int index;
	int max_index;
	int BT[31];
	int IO[30];
	int SUM;
}process;

typedef struct queue {
	process arr[100000];
	int front;
	int last;
}queue;
queue q[4];

typedef struct priority_queue {
	process arr[100000];
	int size;
}pq;
pq wait_q, IO_q;

void swap(process* a, process* b) {
	process temp = *a;
	*a = *b;
	*b = temp;
};

void wait_q_push(process p) {
	int sz = wait_q.size;
	int idx = p.index;
	wait_q.size++;
	wait_q.arr[sz] = p;
	while (sz > 0 && p.AT < wait_q.arr[(sz - 1) / 2].AT) {
		swap(&(wait_q.arr[sz]), &(wait_q.arr[(sz - 1) / 2]));
		sz = (sz - 1) / 2;
	}
}

void IO_q_push(process p) {
	int sz = IO_q.size;
	int idx = p.index;
	IO_q.size++;
	IO_q.arr[sz] = p;
	while (sz > 0 && p.IO[idx] < (IO_q.arr[(sz - 1) / 2]).IO[IO_q.arr[(sz - 1) / 2].index]) {
		swap(&(IO_q.arr[sz]), &(IO_q.arr[(sz - 1) / 2]));
		sz = (sz - 1) / 2;
	}
}

process wait_q_top() {
	return wait_q.arr[0];
}

process IO_q_top() {
	return IO_q.arr[0];
}

process wait_q_pop() {
	process target = wait_q_top(); // 제일 위에 있는 거 반환
	int sz = --wait_q.size;
	if (sz == 0) return target; // queue에 하나 밖에 없었다면 밑의 과정 불필요
	wait_q.arr[0] = wait_q.arr[sz]; // 제일 끝에 있는 거 제일 위로 올림

	int idx = 0;
	while (idx * 2 + 2 < sz) { // 자식이 둘다 있다
		process now = wait_q.arr[idx]; // 현재 프로세스
		process c1 = wait_q.arr[idx * 2 + 1]; // 자식 1;
		process c2 = wait_q.arr[idx * 2 + 2]; // 자식 2;
		if (now.AT < c1.AT && now.AT < c2.AT) break; //자식이 둘다 더 큰 값이면 끝
		else {//자식중에 더 작은 값이 있으면 둘 중에 더 작은 값이랑 바꿈
			if (c1.AT < c2.AT) {
				swap(&wait_q.arr[idx], &wait_q.arr[idx * 2 + 1]);
				idx = idx * 2 + 1;
			}
			else {
				swap(&wait_q.arr[idx], &wait_q.arr[idx * 2 + 2]);
				idx = idx * 2 + 2;
			}
			//자식 두개가 같은 값이면 뭐랑 바꿔야 하나 생각해봤는데 상관없음
		}
	}
	if (idx * 2 + 2 == sz) { // 자식이 하나있다
		process now = wait_q.arr[idx]; // 현재 프로세스
		process c1 = wait_q.arr[idx * 2 + 1]; // 자식 1;
		if (now.AT > c1.AT) { // 자식이 더 작은 값이면 바꿈
			swap(&wait_q.arr[idx], &wait_q.arr[idx * 2 + 1]);
		}
	}

	return target;
}

process IO_q_pop() {
	process target = IO_q_top(); // 제일 위에 있는 거 반환
	int sz = --IO_q.size;
	if (sz == 0) return target; // queue에 하나밖에 없었다면 밑의 과정 불필요
	IO_q.arr[0] = IO_q.arr[sz]; // 제일 끝에 있는 거 제일 위로 올림

	int idx = 0;
	while (idx * 2 + 2 < sz) { // 자식이 둘다 있다
		process now = IO_q.arr[idx]; // 현재 프로세스
		process c1 = IO_q.arr[idx * 2 + 1]; // 자식 1;
		process c2 = IO_q.arr[idx * 2 + 2]; // 자식 2;
		if (now.IO[now.index] < c1.IO[c1.index] && now.IO[now.index] < c2.IO[c2.index]) break; //자식이 둘다 더 큰 값이면 끝
		else {//자식중에 더 작은 값이 있으면 둘 중에 더 작은 값이랑 바꿈
			if (c1.IO[c1.index] < c2.IO[c2.index]) {
				swap(&IO_q.arr[idx], &IO_q.arr[idx * 2 + 1]);
				idx = idx * 2 + 1;
			}
			else {
				swap(&IO_q.arr[idx], &IO_q.arr[idx * 2 + 2]);
				idx = idx * 2 + 2;
			}
		}
	}
	if (idx * 2 + 2 == sz) { // 자식이 하나있다
		process now = IO_q.arr[idx]; // 현재 프로세스
		process c1 = IO_q.arr[idx * 2 + 1]; // 자식 1;
		if (now.IO[now.index] > c1.IO[c1.index]) { // 자식이 더 작은 값이면 바꿈
			swap(&IO_q.arr[idx], &IO_q.arr[idx * 2 + 1]);
		}
	}

	return target;
}

void push(queue* Q, process p) {
	int i = Q->last;
	Q->arr[i] = p;
	(Q->last)++;
}

process top(queue* Q) {
	int i = Q->front;
	return Q->arr[i];
}

process pop(queue* Q) {
	int i = Q->front;
	process target = Q->arr[i];
	(Q->front)++;

	return target;
}

int is_empty(queue* Q) {
	if (Q->front == Q->last) return 1;
	else return 0;
}

int check_ready_q() { // return 값은 time quntum, 다 비어있다면 1 return
	if (is_empty(&q[0]) == 0) return 2;
	else if (is_empty(&q[1]) == 0) return 4;
	else if (is_empty(&q[2]) == 0) return 8;
	else if (is_empty(&q[3]) == 0) return 1000000;
	else return 1;
}

process to_cpu(int i) {
	process p;
	if (i == 2) p = pop(&q[0]);
	else if (i == 4) p = pop(&q[1]);
	else if (i == 8) p = pop(&q[2]);
	else p = pop(&q[3]);

	return p;
}

void check_IO() {//우선순위 큐로 만들어야함
	int i;
	int sz = IO_q.size;
	if (sz == 0) return;

	for (i = 0;i < sz;i++) { //IO_q에 있는 process IO time 1씩 감소
		int idx = IO_q.arr[i].index;
		IO_q.arr[i].IO[idx]--;
	}
	while (IO_q.size != 0) { //process IO time이 0이 되면 wake up
		process top = IO_q_top();
		if (top.IO[top.index] == 0) {
			top.index++;
			int qn = top.QUEUE_NUM;
			// 원래 Q(i)에서 sleep했다면 Q(i-1)로 wake up
			if (qn > 0 && qn < 3) { // RQ(3)에서 RQ(2)로 이동 불가능
			//if(qn != 0){ // RQ(3)에서 RQ(2)로 이동가능
				qn--;
				(top.QUEUE_NUM)--;
			}
			push(&q[qn], top);
			IO_q_pop();
		}
		else break;
	}
}

int sum_BT_IO(process p) {
	int i;
	int sum = 0;
	int idx = p.max_index;
	for (i = 0;i < idx - 1;i++) {
		sum += p.BT[i];
		sum += p.IO[i];
	}sum += p.BT[i];

	return sum;
}

int main() {
	FILE* f;
	if ((f = fopen("input.txt", "r")) == NULL) {
		printf("FILE READ ERROR\n");
		return 0;
	}

	int i, j;
	for (i = 0;i < 4;i++) {
		q[i].front = 0;
		q[i].last = 0;
	}

	for (i = 0;i < MAX_TIME + 1;i++) {
		for (j = 1;j < MAX_PROCESS + 1;j++) {
			gantt_chart[i][j] = '-';
		}
	}

	int num_process;
	fscanf(f, "%d", &num_process);

	for (i = 0;i < num_process;i++) {
		//입력값으로 process p를 세팅
		process p;
		p.index = 0;
		fscanf(f, "%d %d %d %d", &p.PID, &p.AT, &p.QUEUE_NUM, &p.max_index);
		for (j = 0; j < p.max_index - 1; j++) {
			fscanf(f, "%d %d", &p.BT[j], &p.IO[j]);
		}fscanf(f, "%d", &p.BT[j]);
		p.SUM = sum_BT_IO(p);

		wait_q_push(p); // wait_q에서 AT가 될때까지 대기
	}
	fclose(f);

	int time = 0; // while 루프를 한번 돌때마다 1씩 증가
	int flag = 0; // cpu 점유시간
	int done = 0; // 스케줄링 끝난 프로세스 수
	process cpu;

	while (done != num_process) {

		while(wait_q.size != 0 && wait_q_top().AT == time) {
			process p = wait_q_pop();
			int qn = p.QUEUE_NUM;
			push(&q[qn], p);
		}

		time++;

		int nothing = 0; // 기본값으로 0을 준다, 1이면 cpu가 할 스케줄링이 없음
		if (flag == 0) { // cpu가 노는중이면
			flag = check_ready_q(); //RQ 0~3 차례대로 ready상태의 process 있는지 확인
			// 스케줄링 받게 될 process의 RQ의 time quntum
			//  RQ(0)면 time quntum 2 반환, RQ(1)면 time quntum 4 반환, RQ(2)면 time quntum 8 반환, RQ(3)면 time quntum 100000 반환
			if (flag == 1) { // RQ에 대기중인 process가 없음
				nothing = 1;
			}
			else cpu = to_cpu(flag); // 해당 process의 RQ를 pop함
			// cpu 일하는 중 flag가 다시 0 될때까지, flag는 루프마다 1씩 감소예정
		}
		
		int is_quntum = 1; // time slice를 모두 소모했음을 기본값으로 함
		if (nothing != 1) {
			int idx = cpu.index;
			int qn = cpu.QUEUE_NUM;

			cpu.BT[idx]--; // 루프 돌때마다 cpu가 스케줄링중인 process의 burst time 1씩 감소
			gantt_chart[time - 1][cpu.PID] = '#';

			if (cpu.BT[idx] == 0) { // cpu가 스케줄링중인 process의 burst time이 0이 되면
				if (cpu.index == cpu.max_index - 1) { // index와 max_index가 같으면 process의 종료
					int TT = time - cpu.AT;
					int WT = TT - cpu.SUM;
					done++;
					p_time[cpu.PID][0] = TT;
					p_time[cpu.PID][1] = WT;
					flag = 1;
					is_quntum = 0;
				}
				else {
					cpu.IO[cpu.index]++; // 여기서 시간 소모 했는데 밑에서 또 소모하기 때문
					IO_q_push(cpu); // sleep, 즉 IO_q로 보냄
					flag = 1; // 남은 time quntum에 상관없이 밑에서 flag--되면서 cpu가 놀고있다는 것을 알림
					is_quntum = 0; // 주어진 time quntum을 다 소모해서 cpu preemption 된 것이 아님
				}
			}
		}

		check_IO(); // IO_q 관리 : IO time 1씩 감소, 0되면 wake up

		flag--; // cpu 스케줄링 시간을 1만큼 소모

		if (nothing == 0 && flag == 0 && is_quntum == 1) { //time quntum 소모
			cpu.QUEUE_NUM++;
			push(&q[cpu.QUEUE_NUM], cpu);
		}
	}

	float avg_TT = 0, avg_WT = 0;
	printf("\n- RESULT -\n");
	for (i = 1;i <= num_process;i++) {
		printf("PID: %d , TT: %d, WT: %d\n", i, p_time[i][0], p_time[i][1]);
		avg_TT += p_time[i][0];
		avg_WT += p_time[i][1];
	}
	avg_TT /= num_process;
	avg_WT /= num_process;
	printf("Average TT: %.02f, Average WT: %.02f\n", avg_TT, avg_WT);
	printf("\n\n- GANTT CHART -\nTIME     ");
	for (i = 1;i <= num_process;i++) printf("P%d ", i);
	printf("\nㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
	for (i = 0;i < time;i++) {
		printf("  %d   :  ", i);
		for (j = 1;j <= num_process;j++) {
			printf("%c  ", gantt_chart[i][j]);
		}
		printf("\n");
	}

	return 0;
}