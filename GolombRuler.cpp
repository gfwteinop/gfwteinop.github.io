/*
 * GolombRuler.cpp
 *
 */

#include "stdio.h"
#include "stdlib.h"

#include <iostream>
#include <time.h>

#define BUF_SIZE 33

#define number 86  //Length+1 (for zero)
#define MARKS 12  //Order

#if number%32==0
#define SIZE number/32
#else
#define SIZE number/32+1
#endif

static const int OGRs[25]={0,1,3,6,11,17,25,34,44,55,72,85,106,127,151,177,199,216,246,
							283,333,356,372,425,480};

//#define OGR1 0
//#define OGR2 1
//#define OGR3 3
//#define OGR4 6
//#define OGR5 11
//#define OGR6 17
//#define OGR7 25
//#define OGR8 34
//#define OGR9 44
//#define OGR10 55
//#define OGR11 72
//#define OGR12 85
//#define OGR13 106
//#define OGR14 127
//#define OGR15 151
//#define OGR16 177
//#define OGR17 199
//#define OGR18 216
//#define OGR19 246
//#define OGR20 283
//#define OGR21 333
//#define OGR22 356
//#define OGR23 372
//#define OGR24 425
//#define OGR25 480

struct stack{
	int distance[SIZE];
	int listOfNum[SIZE];
	int listLength;
	struct stack *next;
};

typedef struct stack FIRST;
typedef FIRST *LINK;

LINK pushStack(int dist[],int list[],int listLength,LINK head,int stackCounter);
void printDL(int dist[],int list[]);
void printDec(int list[]);
char *int2bin(int a, char *buffer, int buf_size);
int checkReductions(int listLen,int numOfMarks);


int main(int argc,char** argv[]){
	
	int times,timed;
    	times=time(NULL);


	LINK head = NULL;
	LINK add = NULL;
	LINK current = NULL;
	int list[SIZE]={0};
	int dist[SIZE]={0};
	int i = 0;
	int j = 0;
	int shift_flag = 0;
	int collision_flag = 0;
	int lmPosition = 1;
	int fmPosition = 0;
	int tempLMPos = 0;
	int fFlag = 0;
	int emptyS = 0;
	int mpr_flag = 0;//Mid Point Reduction
	int temp_flag = 0;
	int pop_flag = 0;
	int numOfMarks = 0;
	int middle = 0;
	int listMaxSize = 0x80000000>>(number%32-1);
	int stack_counter = 0;
	int tempList[SIZE]={0};

	list[0] = 0x80000000;
	dist[0] = 0x80000000;
	/* Add first stack element */

	add = (LINK)malloc(sizeof(FIRST));
	add->next = head;
	head = add;
	for(i=0;i<SIZE;i++){
		add->distance[i] = dist[i];  // 10000000000...0000
		add->listOfNum[i] = list[i]; // 10000000000...0000
	}
	add->listLength = 1;
	stack_counter++;
	middle = (SIZE-1)*16+(number%32)/2; // SIZE*32  /  2
	while(1){
		temp_flag++;
		collision_flag = 0;
		for(i=0;i<SIZE;i++){
			if(((int)list[i]&dist[i])!=0){
				collision_flag = 1;
				break;
			}
		}// END AND CHECK
		if(collision_flag==1 && pop_flag==0){
			collision_flag = 0;
			do{	
				shift_flag = 0;
				// CHECKS
				/** MIDPOINT REDUCTION  **/
				if(numOfMarks==(MARKS/2)){
					for(i=0;i<SIZE;i++){
						tempList[i]= list[i];
					}
					fFlag = 0;
					for(i=SIZE-1;i>-1;i--){
						for(j=0;j<32;j++){
							tempLMPos++;
							if(tempList[i]&0x00000001==0x00000001){
								if(fFlag == 0){
									fFlag = 1;
									tempLMPos = 1;
								}
							}
							tempList[i]=tempList[i]>>1;
						}
					}
					
					if(tempLMPos>middle){ 
						pop_flag = 1;
						mpr_flag = 1;
						break;
					}
				}//END Midpoint Reduction check

				if(((int)list[SIZE-1]&listMaxSize) == listMaxSize){ 
					pop_flag = 1;
					break;
				}

				lmPosition++;
				if(checkReductions(lmPosition,numOfMarks)==1){
					pop_flag=1;
					break;
				}

				//**** SHIFT
				if(SIZE!=1){
					for(i=SIZE-1;i>0;i--){
						tempList[i] = list[i]>>1;
						if(list[i-1]&0x00000001==0x00000001){//0xXXXXXXX1
							tempList[i] = tempList[i]|0x80000000;
						}
						else{
							tempList[i] = tempList[i]&0x7FFFFFFF; 
						}
					}
				}

				tempList[0] = list[0]>>1;
				tempList[0] = tempList[0]&0x7FFFFFFF;
				//**** END SHIFT

				//*** AND CHECK
				for(i=0;i<SIZE;i++){
					list[i]= tempList[i];
				}
				for(i=0;i<SIZE;i++){
					if(((int)list[i]&dist[i])!=0){
						shift_flag = 1;
						break;
					}
				}// END AND CHECK
			}while(shift_flag==1);
		}

		if(pop_flag==0){

			//*****************************
			//           PUSH              *
			//*****************************//
			for(i=SIZE-1;i>-1;i--){
				tempList[i] = list[i];
			}
			fFlag = 0;
			for(i=SIZE-1;i>-1;i--){
				for(j=0;j<32;j++){
					tempLMPos++;
					if(tempList[i]&0x00000001==0x00000001){
						if(fFlag == 0){
							fFlag = 1;
							tempLMPos = 1;
							numOfMarks = 0;
						}
						numOfMarks++;
					}
					tempList[i]=tempList[i]>>1;
				}
			}
			fmPosition = tempLMPos;
			if(fmPosition<=middle*2+1){
				if(fmPosition<=middle*2){
					//printf("PUSH\n");
					head = pushStack(dist,list,lmPosition+1,head,stack_counter);
					stack_counter++;
				}

				list[0]=list[0]|0x80000000; //ADD ACE
				numOfMarks++;
				for(i=0;i<SIZE;i++){        //CREATE NEW DIST
					dist[i]=list[i]|dist[i];
					tempList[i]=list[i];
				}
				if(numOfMarks==MARKS){
					printf("Golomb ruler found:\n");
					printDL(dist,list);
					printDec(list);
				}
			}
			else{
				pop_flag = 1;
			}

			//*******  PUSH END ************/
		}
		else{     //if(pop_flag == 1){
			pop_flag = 0;
			mpr_flag = 0;
			//*****************************
			//           POP              *
			//*****************************//
			current = head;
			stack_counter--;
			if(stack_counter!=0){
				head = current->next;
				if(stack_counter==1){
					printDL(dist,list);				
				}
			}
			else{


					head = NULL;
					emptyS = 1;
					printf("Stack is empty!\n");
					break;
				
			}
			for(i=0;i<SIZE;i++){
				dist[i] = current->distance[i];
				list[i] = current->listOfNum[i];
			}
			lmPosition = current->listLength;
			free(current);

		}
	}

	timed=time(NULL);
	times=timed-times;
    printf("time from start to end: %d \n", times);

	return 0;
}
LINK pushStack(int dist[],int list[],int listLength,LINK head,int stackCounter){
	LINK add = NULL;
	int tempList[SIZE]={0};
	int i=0;

	if(SIZE!=1){
		for(i=SIZE-1;i>0;i--){
			tempList[i] = list[i]>>1;
			if(list[i-1]&0x00000001==0x00000001){//0xXXXXXXX1
				tempList[i] = tempList[i]|0x80000000;
			}
			else{
				tempList[i] = tempList[i]&0x7FFFFFFF;
			}
		}
	}
	tempList[0] = list[0]>>1;
	tempList[0] = tempList[0]&0x7FFFFFFF;

	add = (LINK)malloc(sizeof(FIRST));
	add->next = head;
	head = add;

	for(i=0;i<SIZE;i++){
		add->listOfNum[i] = tempList[i];
		add->distance[i] = dist[i];
	}
	add->listLength = listLength;
	return head;
} ///END OF pushStack(bool dist[],bool list[],LINK *head,int stackCounter)


void printDL(int dist[],int list[]){
	char buffer[BUF_SIZE];
	buffer[BUF_SIZE - 1] = '\0';
	int i=0;
	printf("DIST: ");
	for(i=0;i<SIZE;i++){
		int2bin(dist[i], buffer, BUF_SIZE - 1);
	    printf("%s", buffer);
	}
	printf("\n");
	printf("LIST: ");
	for(i=0;i<SIZE;i++){
		int2bin(list[i], buffer, BUF_SIZE - 1);
	    printf("%s", buffer);
	}
	printf("\n\n");
	//printDec(dist);
}

char *int2bin(int a, char *buffer, int buf_size) {
    buffer += (buf_size - 1);
    for (int i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';
        a >>= 1;
    }
    return buffer;
}

void printDec(int list[]){
	int tempList[SIZE];
	int i = 0;
	int j = 0;
	int counter = 0;
	int firstAceFlag = 0;
	printf("RULER: ");
	for(i=SIZE-1;i>-1;i--){
		tempList[i] = list[i];
	}
	for(i=SIZE-1;i>-1;i--){
		for(j=31;j>=0;j--){
			if(firstAceFlag == 0){
				if(tempList[i]&0x000000001==000000001){
					firstAceFlag = 1;
					printf("%d ",counter);
					counter++;
					tempList[i]=tempList[i]>>1;
				}
				else{
					tempList[i]=tempList[i]>>1;
				}
			}else{
				if(tempList[i]&0x000000001==000000001){
					printf("%d ",counter);
				}
				tempList[i]=tempList[i]>>1;
				counter++;
			}
		}
	}
	printf("\n-----------------------------\n");
}

int checkReductions(int listLen,int numOfMarks){
	if(OGRs[numOfMarks]<=listLen <=number -OGRs[MARKS-numOfMarks-1]){
		return 0;
	}
	else{
		return 1;
	}
}

