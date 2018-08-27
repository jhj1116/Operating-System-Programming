// OS HW6 21600672 HyejinJeong
// Questions and Answers
// 1. What is the maximum number of page table entries?
// Answer : 16
// 2^BitsForPageNo = 2^4 = 16

// 2. What is the size of a page (in bytes)?
// Answer : 4096 bytes
// 2^BitsForPageOffset = 2^12 = 4096 bytes (=4KB)

// 3. You have a whose size is 52018 bytes, whate is the value of PTLR?
// Answer : 13
// The value of PTLR is 12. PTLR is a length of page table.
// ProcessSize/(2^BitsForPageOffset) = 52018 / (2^12) = 12.xxx = 13 (ceil)
// the length of page table, PTLR = 13



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BitsForPageNo 4
#define BitsForPageOffset 12

#define ProcessSize 52018

void init_page_table(int table[], int no_page);
void to_binary(unsigned short addr, char bit_string[]);
unsigned short logical2physical(unsigned short logical_addr, int page_table[]);


void init_page_table(int table[], int no_page)
{
	int i = 0;
	
	for(i=0; i<no_page; i++)
		table[i] = i;

	for(i=0; i<no_page; i++){
		int j = (i + (1<<i)) % no_page;
		int temp = table[i];
		table[i] = table[j];
		table[j] = temp;
	}

	for(i=0; i<no_page; i++){
		printf("page_table[%d, 0x%x] = %d\n", i, i, table[i]);
	}
}

void to_binary(unsigned short addr, char bit_string[])
{
	memset(bit_string, '\0', 17);
	for(int i=0; i<16; i+=4){
		int tmp = addr%16;
		if(tmp == 0){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '0';
			bit_string[15-(i+1)] = '0';
			bit_string[15-i] = '0';
		}
		else if(tmp == 1){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '0';
			bit_string[15-(i+1)] = '0';
			bit_string[15-i] = '1';
		}
		else if(tmp == 2){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 3){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '1';
		}
		else if(tmp == 4){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 5){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '1';
		}
		else if(tmp == 6){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 7){
			bit_string[15-(i+3)] = '0';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '1';
		}
		else if(tmp ==8){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 9){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '1';
		}
		else if(tmp == 10){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 11){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '0';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '1';
		}
		else if(tmp == 12){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 13){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '0';
                        bit_string[15-i] = '1';
		}
		else if(tmp == 14){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '0';
		}
		else if(tmp == 15){
			bit_string[15-(i+3)] = '1';
			bit_string[15-(i+2)] = '1';
                        bit_string[15-(i+1)] = '1';
                        bit_string[15-i] = '1';
		} 
	
		addr = (addr>>4)&0xFFF;
	}
}

unsigned short logical2physical(unsigned short logical_addr, int page_table[])
{
	unsigned short page_num = (logical_addr>>BitsForPageOffset)&0xF;
	unsigned short offset = logical_addr&0xFFF;
	unsigned short frame_num = page_table[page_num];
	unsigned short physical_addr = (frame_num<<BitsForPageOffset)|offset;
	return physical_addr;
}

int main()
{
	int no_page = 0;
	int *page_table = NULL;

	unsigned short logical_addr[] = { 0x071e, 0x0431, 0x21dd, 0x0829, 0x08f0, 0x0ad1, 0x0fb8, 0x0b18, 0x38ab, 0x07a0 };

	int i = 0;

	no_page = ceil(ProcessSize/pow(2,BitsForPageOffset));
	page_table = (int*)malloc(no_page*sizeof(int));
	if(page_table == NULL){
		printf("Failed to allocate memory!\n");
		exit(-1);
	}

	init_page_table(page_table, no_page);

	for(i = 0; i<10; i++){
		char bits[17];
		unsigned short physical_addr = logical2physical(logical_addr[i], page_table);
		
		to_binary(logical_addr[i], bits);
		printf("0x%04x (%s) ", logical_addr[i], bits);

		to_binary(physical_addr, bits);
		printf("--> 0x%04x (%s)\n", physical_addr, bits);
	}
	free(page_table);

	return 0;
}



