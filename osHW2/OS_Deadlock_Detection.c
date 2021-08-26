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
			if (!p[i].ok) { // reduction �Ǿ����� �ƴ��� �Ǵ�
				int safe = 1; // �⺻������ safe �ϴٰ� ����
				for (j = 0;j < Rtype;j++) {
					if (p[i].request[j] > Runit[j]) { // ��밡���� Resource unit ���� ���� request �ؾ���
						safe = 0; break; // �ش� process�� reduction �� �� ����
					}
				}
				if (safe) { // request�ϴ� resource unit�� ��� ��밡���� ����
					for (j = 0;j < Rtype;j++) {
						Runit[j] = Runit[j] + p[i].allocated[j]; //Reduction�ϸ鼭 allocated �ߴ� unit�� �ݳ�
						flag = 1; p[i].ok = 1; // reduction ó��
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

	fscanf(f, "%d", &ProcessNum); // Process ����
	fscanf(f, "%d", &Rtype); // Resource ����
	for (i = 0;i < Rtype;i++) {
		fscanf(f, "%d", &Runit[i]); // Resource i�� unit ����
	}

	for (i = 0;i < ProcessNum;i++) {
		p[i].ok = 0;
		for (j = 0;j < Rtype;j++) {
			fscanf(f, "%d", &p[i].allocated[j]);
			Runit[j] = Runit[j] - p[i].allocated[j]; // ���� Resource unit�� ����
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
		if (p[i].ok == 0) target[k++] = i + 1; // ok ���� deadlock���� �ƴ��� Ȯ��
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