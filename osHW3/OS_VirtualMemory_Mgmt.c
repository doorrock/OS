#include<stdio.h>
#include<stdlib.h>
int pageN, pageframeN, windowSize, stringLength, i, j;
int arr[1000000];
int table[20];

int printFLAG = 1;

void swap(int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
	return;
}

void swapping(int* arr, int start, int end, int last) {
	int k;
	for (k = start; k < end - 1;k++) {
		swap(&arr[k], &arr[k + 1]);
	}
	arr[end - 1] = last;
}
void reset_table() {
	for (i = 0;i < 20;i++) {
		table[i] = -1;
	}
	return;
}

void print_memory() {
	int k;
	printf("Memory residence set: ");
	for (k = 0;k < pageframeN;k++) {
		printf("%d ", table[k]);
	}
	printf("\n");
}

void print_WS(int* arr) {
	int k;
	printf("Memory residence set: ");
	for (k = 0;k < 100;k++) {
		if (arr[k] > 0) printf("%d ", k);
	}
	printf("\n");
}

int MIN() {
	reset_table(); // table �ʱ�ȭ
	int tf[100] = { 0, }; // index= == page number, page�� memory�� ������ 1, ������ 0
	int whereisNext[100]; // �������� ������ Ÿ�̹�
	int where[100]; // index == page number, page�� �����ϴ� page frame number
	for (i = 0;i < 100;i++) {
		whereisNext[i] = -1;
		where[i] = -1;
	} // �ʱ�ȭ

	int in = 0; // �ʱ� page frame ä��� �뵵
	int count = 0; // page fault Ƚ��
	for (i = 0;i < stringLength;i++) {
		int temp_in = arr[i]; // ��� page number
		// whereisNext ���
		////////////////////////////////////////////
		int j = 1;
		while (i + j < stringLength && arr[i + j] != temp_in) {
			j++;
		}
		int next;
		if (i + j == stringLength) next = 2147483647;
		else next = i + j;
		whereisNext[temp_in] = next;
		////////////////////////////////////////////
		if (tf[temp_in] == 1) continue; // �̹� memory�� �ִٸ� continue
		else { // page fault �߻� �κ�
			count++;
			if (in < pageframeN) { // �ʱ� page frame ä���
				table[in] = temp_in;
				where[temp_in] = in;
				tf[temp_in] = 1;
				in++;

				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: -1 / ", i, temp_in);
					print_memory();
				}
			}
			else { // victim �߻�
				int out = 0;
				int target = -1;
				for (j = 0;j < 20;j++) { // page frame���� page�� ���� ���߿� ������ page�� victim���� ��
					int temp = table[j];
					if (temp == -1) continue;
					if (target <= whereisNext[temp]) {
						target = whereisNext[temp];
						out = where[temp];
					}
				}
				
				// ������ �� victim�� table���� �����ϰ� ������� �� page�� ��ü
				int temp_out = table[out];
				if (temp_out != -1) {
					tf[temp_out] = 0;
					where[temp_out] = -1;
					whereisNext[temp_out] = -1;
				}

				table[out] = temp_in;
				tf[temp_in] = 1;
				where[temp_in] = out;

				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: %d / ", i, temp_in, temp_out);
					print_memory();
				}
			}
		}
	}

	printf("MIN > page fault count: %d\n\n", count);
	return count;
}
int FIFO(){
	reset_table(); // table �ʱ�ȭ
	int tf[100] = { 0, }; // index= == page number, page�� memory�� ������ 1, ������ 0
	int out = 0; // victim�� �� index
	int count = 0;
	for (i = 0;i < stringLength;i++) {
		int temp_in = arr[i];
		if (tf[temp_in] == 1) continue;
		else {
			count++;
			int temp_out = table[out];
			if (temp_out != -1) tf[temp_out] = 0;
			table[out] = temp_in;
			tf[temp_in] = 1;

			if (printFLAG) {
				printf("Page Fault at %d, IN: %d, OUT: %d / ", i, temp_in, temp_out);
				print_memory();
			}
			out = (out + 1) % pageframeN; // page fault�� �߻��ϸ� out�� 1�� �þ
		}
	}
	printf("FIFO > page fault count: %d\n\n", count);
	return count;
}

int LRU() {
	reset_table();
	int where[100];
	int which[20];
	for (i = 0;i < 100;i++) where[i] = -1;
	for (i = 0;i < 20;i++) which[i] = -1;
	
	int in = 0;
	int count = 0;
	for (i = 0;i < stringLength;i++) {
		int target = arr[i];
		int index = where[target];
		if (index != -1) { // �̹� table�� page ����
			int t; // recently used ó��
			for (j = 0;j < in;j++) {
				if (which[j] == index) {
					t = j;
					break;
				}
			}
			swapping(which, t, in, index); // �� �ڷ� ����
		}
		else {
			count++;
			if (in < pageframeN) {
				which[in] = in;
				table[in] = target;
				where[target] = in;
				in++;

				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: -1 / ", i, target);
					print_memory();
				}
			}
			else {
				int out = which[0]; // which[0]�� �������� ���� ������ page number
				swapping(which, 0, pageframeN, out);
				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: %d / ", i, target, table[out]);
				}
				where[table[out]] = -1;
				where[target] = out;
				table[out] = target;

				if (printFLAG) {
					print_memory();
				}
			}
		}
	}

	printf("LRU > page fault count: %d\n\n", count);
	return count;
}

int find_MIN(int *cnt, int* howlong) {
	int k;
	int r;
	int t = 2147483647;
	for (k = 0;k < pageframeN;k++) { // ���� ���� ������ page ã��
		int N = table[k];
		if (t > cnt[N]) {
			t = cnt[N];
			r = k;
		}
		else if (t == cnt[N]) { // tie-breaking rule
			if (howlong[table[r]] < howlong[table[k]]) { // lru
				r = k;
			}
		}
	}

	return r;
}
int LFU() {
	reset_table();
	int cnt_page[100] = { 0, };
	int where[100];
	int howlong[100] = { 0, }; // tie-breaking �� ����� �뵵, LRU
	for (i = 0;i < 100;i++) where[i] = -1;

	int in = 0;
	int out = 0;
	int count = 0;
	for (i = 0;i < stringLength;i++) {
		// page frame�� ����ִ� page���� �󸶳� �����ƴ����� ��Ÿ��
		for (j = 0;j < in;j++) howlong[table[j]]++;
		int target = arr[i];
		int index = where[target];
		if (index != -1) { // �̹� table�� �ִ� ���
			cnt_page[target]++; // ������ Ƚ�� ����
			howlong[target] = 0; // recently referenced ó��
		}
		else {
			count++;
			if (in < pageframeN) {
				cnt_page[target]++;
				table[in] = target;
				where[target] = in;
				in++;

				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: -1 / ", i, target);
					print_memory();
				}
			}
			else {
				int out = find_MIN(cnt_page, howlong); // ���� ���� ������ page�� victim���� ��

				if (printFLAG) {
					printf("Page Fault at %d, IN: %d, OUT: %d / ", i, target, table[out]);
				}
				howlong[table[out]] = 0;
				//cnt_page[table[out]] = 0;
				//cnt_page[target] = 1;
				cnt_page[target]++;
				where[table[out]] = -1;
				where[target] = out;
				table[out] = target;

				if (printFLAG) {
					print_memory();
				}
			}
		}
	}

	printf("LFU > page fault count: %d\n\n", count);
	return count;
}

int CLOCK() {
	reset_table();
	int tf[100] = { 0, };
	int zeroORone[20] = { 0, }; // page frame�� reference bit
	int where[100];
	for (i = 0;i < 100;i++) where[i] = -1;
	int out = 0;
	int count = 0;

	for (i = 0;i < stringLength;i++) {
		int temp_in = arr[i];
		if (tf[temp_in] == 1) {
			int w = where[temp_in];
			zeroORone[w] = 1;
		}
		else {
			count++;
			while (zeroORone[out] == 1) { // reference bit�� 0�� page�� ã�� ������
				zeroORone[out] = 0; // reference bit�� 1�̸� 0���� �ٲ�
				out = (out + 1) % pageframeN;
			}
			int temp_out = table[out];
			if (temp_out != -1) {
				tf[temp_out] = 0;
				where[temp_out] = -1;
			}

			table[out] = temp_in;
			zeroORone[out] = 1;
			tf[temp_in] = 1;
			where[temp_in] = out;

			if (printFLAG) {
				printf("Page Fault at %d, IN: %d, OUT: %d / ", i, temp_in, temp_out);
				print_memory();
			}
			out = (out + 1) % pageframeN;
		}
	}

	printf("CLOCK > page fault count: %d\n\n", count);
	return count;
}

int WS() {
	reset_table();
	int tf[100] = { 0, };
	int where[100];
	for (i = 0;i < 100;i++) where[i] = -1;
	int f;
	int count = 0;

	for (i = 0;i < stringLength;i++) {
		f = 0;
		int temp_in = arr[i];
		
		if (tf[temp_in] == 0) { // page frame�� ������
			count++;
			if (printFLAG) {
				printf("Page Fault at %d, IN: %d / ", i, temp_in);
				f = 1;
			}
		}
		tf[temp_in]++;

		if (i - windowSize > 0) { // window size�� ��� page
			int t = i - windowSize - 1;
			tf[arr[t]]--;
			if (printFLAG) {
				if (tf[arr[t]] == 0) {
					printf("At %d, OUT %d from page frame / ", i, arr[t]);
					f = 1;
				}
			}
		}

		if (printFLAG && f) {
			print_WS(tf);
		}
	}
	
	printf("WS > page fault count: %d\n\n", count);
	return count;
}

int main() {
	FILE* f;
	if ((f = fopen("input.txt", "r")) == NULL) {
		printf("FILE READ ERROR\n");
		return 0;
	}
	fscanf(f, "%d %d %d %d", &pageN, &pageframeN, &windowSize, &stringLength);
	for (i = 0;i < stringLength;i++) {
		fscanf(f, "%d", &arr[i]);
	}

	int c_min = MIN();
	int c_fifo = FIFO();
	int c_lru = LRU();
	int c_lfu = LFU();
	int c_clock = CLOCK();
	int c_ws = WS();
	printf("# of pages: %d\n", pageN);
	printf("# of page frame: %d\n", pageframeN);
	printf("window size: %d\n", windowSize);
	printf("length of refernce string: %d\n", stringLength);
	if (printFLAG) { printf("reference string: "); for (i = 0;i < stringLength;i++) printf("%d ", arr[i]); printf("\n");}
	printf("\n");
	printf("MIN: %d\nFIFO: %d\nLRU: %d\nLFU: %d\nCLOCK: %d\nWS: %d\n", c_min, c_fifo, c_lru, c_lfu, c_clock, c_ws);
	
	return 0;
}