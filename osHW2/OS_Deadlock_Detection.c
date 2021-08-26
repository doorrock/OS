#include<stdio.h>
int ProcessNum, Rtype;
int Runit[10000];

struct process {
	int ok;
	int allocated[10000];
	int request[10000];
}p[10000];

void check() {
	int i, j;
	int flag = 1;
	while (flag) {
		flag = 0;
		for (i = 0;i < ProcessNum;i++) {
			if (!p[i].ok) { // reduction 되었는지 아닌지 판단
				int safe = 1; // 기본값으로 safe 하다고 가정
				for (j = 0;j < Rtype;j++) {
					if (p[i].request[j] > Runit[j]) { // 사용가능한 Resource unit 보다 적게 request 해야함
						safe = 0; break; // 해당 process는 reduction 될 수 없음
					}
				}
				if (safe) { // request하는 resource unit이 모두 사용가능한 상태
					for (j = 0;j < Rtype;j++) {
						Runit[j] = Runit[j] + p[i].allocated[j]; //Reduction하면서 allocated 했던 unit을 반납
						flag = 1; p[i].ok = 1; // reduction 처리
					}
				}
			}
		}
	}
}

int main() {
	int i, j;

	FILE* f;
	if ((f = fopen("input.txt", "r")) == NULL) {
		printf("FILE READ ERROR\n");
		return 0;
	}

	fscanf(f, "%d", &ProcessNum); // Process 개수
	fscanf(f, "%d", &Rtype); // Resource 개수
	for (i = 0;i < Rtype;i++) {
		fscanf(f, "%d", &Runit[i]); // Resource i의 unit 개수
	}

	for (i = 0;i < ProcessNum;i++) {
		p[i].ok = 0;
		for (j = 0;j < Rtype;j++) {
			fscanf(f, "%d", &p[i].allocated[j]);
			Runit[j] = Runit[j] - p[i].allocated[j]; // 남은 Resource unit을 구함
		}
	}

	for (i = 0;i < ProcessNum;i++) {
		for (j = 0;j < Rtype;j++) {
			fscanf(f, "%d", &p[i].request[j]);
		}
	}

	check();

	int target[10000], k=0; // target: deadlocked process list, k: num of target
	for (i = 0;i < ProcessNum;i++) {
		if (p[i].ok == 0) target[k++] = i + 1; // ok 값은 deadlock인지 아닌지 확인
	}
	if (k == 0) printf("No Deadlock\n");
	else {
		printf("Deadlock detected\n");
		printf("%d processes are deadlocked\n", k);
		printf("Process: ");
		for (i = 0;i < k;i++) printf("%d ", target[i]);
	}
	return 0;
}