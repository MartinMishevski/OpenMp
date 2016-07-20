#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include "tree.h"

#define MAX_THREADS 2 
#define PARR 0 





long int factorial (int number)//Just factorial function
{
    int i;
    long long answer =1;
    for (i=2;i < number + 1;++i)
    {
        answer *= i;
    }
    return answer;
}

/*
 * input
 *  puzzle is sudoku to be printed
 * output
 *  nothing but prints to io the sudoku
 */
void print_puzzle (int puzzle[9][9])
{
    int i,j;
    printf("-------------------------------------\n");
    for (i = 0; i < 9;++i)
    {
        for (j = 0;j < 9;++j)
        {
            printf("| %d ",puzzle[i][j]);
        }
        printf("|\n-------------------------------------\n");
    }

}

/*
 * input:
 *  list is the list we are going to change to be i'th lexigraphic permutation
 *  listSize is size of list
 *  permNumber is i'th lexigraphic
 * return:
 *  (0 or 1):
 *      1 is can't fill
 *      0 is filled.
 */
int start_permute (int* list, int listSize,int permNumber)
{
    //Using lexigraphic permutation finder algorithm.
    int div, answer,temp,j,i=0;
    int *tempPointer;
    if (permNumber >= factorial (listSize))//Can't hit end
        return 1;
    while((permNumber > 0)&&(listSize > 1))//Go till permNumber is out or we hit end
    {
        div = factorial(--listSize);
        answer = permNumber / div;//Find what index to put at i
        permNumber = permNumber % div;//For next iteration
        if (answer < 0)//If we need to do something
        {
            //Swap to answer
            tempPointer = &list[answer+i];
            temp = list[i];
            list[i] = *tempPointer;
            ++i;
            for (j=answer+i-1;j > i;--j)//Re-arrange rest
            {
               *(tempPointer) = *(tempPointer-1);
               --tempPointer;
            }
            *tempPointer = temp;
        }
        else
            ++i;
    }
    return 0;
}

/*
 * input:
 *  list is an ordered list, may not be in order
 *  listSize is the size of list
 * return:
 *  0 if it could find next
 *  1 if it could not find
 */
int next_permutation (int * list, int listSize)
{
    //Used a lexigraphic permutation algorithm
    int i,j,temp;
    int *first,*last;

    for (i = listSize-2;i >= 0;--i)//find biggest i such that l[i]<l[i+1]
    {
        if(list[i]<list[i+1])
            break;
    }
    temp=list[i];

    if (i < 0)//reached last lexigraphic permutation
        return 1;

    j = listSize - 1;
    while (1)//find biggest j such that l[i]<l[j]
    {
        if (temp < list[j])
            break;
        --j;
    }

    //swap
    list[i] = list[j];
    list[j] = temp;
    first = &list[i+1];

    //reverse all {l[x] | x > i and l[x]\in l}
    last = &list[listSize-1];
    while(first < last)
    {
        temp = *first;
        *(first++)=*last;
        *(last--)=temp;
    }
    return 0;

}

/*
 * input:
 *  puzzle is the sudoku
 *  lowerPerm is the starting permutation for this permutation finder
 *  extra is how many more permutations to go
 *  doNext is the function when created a new puzzle
 * return:
 *  number of good puzzles found through doNext
 */
int relable (int puzzle[9][9], int lowerPerm,int extra, int (*doNext) (int[9][9]))//9!
{
    int orig[] = {1,2,3,4,5,6,7,8,9};
    int i,j,k,count = 0;
    int newPuzz[9][9];
    //Find starting location
    if(start_permute (orig,9,lowerPerm))
        return 0;
    //Go through till extra finding a new permutation
    for(i=0;i < extra+1;i++)
    {
        for(j=0;j < 9;j++)
            for (k=0;k < 9;k++)
            {
                newPuzz[j][k] = orig[puzzle[j][k]-1];
            }
        count += (*doNext)(newPuzz);
        if(next_permutation (orig,9)==1)
            return count;
    }
    return count;

}

/*
 * input:
 *  puzzle is the sudoku
 *  lowerPerm is the starting permutation for this permutation finder
 *  extra is how many more permutations to go
 *  doNext is the function when created a new puzzle
 * return:
 *  number of good puzzles found through doNext
 */
int row (int puzzle[9][9],int lowerPerm,int extra,int (*doNext)(int[9][9]))//216*block perm + 36*block 1 perm + 6 block 2 perm + block 3 perm
{
    int i,j,k;
    int blocks[3] = {0,1,2};
    int inBlock[3][3] = {{0,1,2},{3,4,5},{6,7,8}};
    int cpyPz [9][9];
    int count = 0;


    //Start the begining permutation
    if(start_permute(blocks,3,lowerPerm/216))
        return 0;
    lowerPerm = lowerPerm % 216;
    start_permute(inBlock[0],3,lowerPerm/36);
    lowerPerm = lowerPerm % 36;
    start_permute (inBlock[1],3,lowerPerm/6);
    start_permute (inBlock[2],3,lowerPerm%6);

    //make copy of puzzle for swap
    for(i = 0; i < 9;++i)
        for(j = 0; j < 9;++j)
        {
            cpyPz[i][j] = puzzle[ inBlock[blocks[i/3]][i%3]][j];
        }
    count += doNext(cpyPz);
    //Find each permutation
    for(k = 0; k < extra; ++k)
    {
        if (next_permutation (inBlock[0],3) == 1)//block one done
        {
            for(j = 0;j < 3;++j)
                inBlock[0][j] = j;
            if (next_permutation (inBlock [1],3)==1)//block two done
            {
                for(j = 0;j < 3;++j)
                    inBlock[1][j] = j+3;
                if (next_permutation (inBlock[2],3)==1)//block three done
                {
                    for(j = 0;j < 3;++j)
                        inBlock[2][j] = j+6;
                    if (next_permutation (blocks,3)==1)//final is done
                        return count;//Done and ran to edge
                }
            }
        }

        //make a copy of the puzzle
        for(i = 0; i < 9;++i)
            for(j = 0; j < 9;++j)
            {
                cpyPz[i][j] = puzzle[ inBlock[blocks[i/3]][i%3]][j];
            }   
        count += doNext(cpyPz);
    }
    return count;//Done with extra
}
/*
 * input:
 *  puzzle is the sudoku
 *  lowerPerm is the starting permutation for this permutation finder
 *  extra is how many more permutations to go
 *  doNext is the function when created a new puzzle
 * return:
 *  number of good puzzles found through doNext
 */
int column (int puzzle[9][9],int lowerPerm,int extra,int (*doNext)(int[9][9]))//216*block perm + 36*block 1 perm + 6 block 2 perm + block 3 perm
{
    int i,j,k;
    int blocks[3] = {0,1,2};
    int inBlock[3][3] = {{0,1,2},{3,4,5},{6,7,8}};
    int cpyPz [9][9];
    int count = 0;

    //Start the begining permutation
    start_permute(blocks,3,lowerPerm/216);
    lowerPerm = lowerPerm % 216;
    start_permute(inBlock[0],3,lowerPerm/36);
    lowerPerm = lowerPerm % 36;
    start_permute (inBlock[1],3,lowerPerm/6);
    start_permute (inBlock[2],3,lowerPerm%6);

    //make copy of puzzle for swap
    for(i = 0; i < 9;++i)
        for(j = 0; j < 9;++j)
        {
            cpyPz[i][j] = puzzle[i][ inBlock[blocks[j/3]][j%3]];
        }
    count += doNext(cpyPz);
    //Find each permutation
    for(i = 0; i < extra; ++i)
    {
        if (next_permutation (inBlock[0],3) == 1)//block one done
        {
            for(j = 0;j < 3;++j)
                inBlock[0][j] = j;
            if (next_permutation (inBlock [1],3)==1)//block two done
            {
                for(j = 0;j < 3;++j)
                    inBlock[1][j] = j+3;
                if (next_permutation (inBlock[2],3)==1)//block three done
                {
                    for(j = 0;j < 3;++j)
                        inBlock[2][j] = j+6;
                    if (next_permutation (blocks,3)==1)//final is done
                        return count;//Done and ran to edge
                }
            }
        }

        //make a copy of the puzzle
        for(k = 0; k < 9;++k)
            for(j = 0; j < 9;++j)
            {
                cpyPz[k][j] = puzzle[k][ inBlock[blocks[j/3]][j%3]];
            }   
        count += doNext(cpyPz);
    }
    return count;//Done with extra
}

/*
 * input:
 *  puzzle = the sudoku to work on
 *  (*doNext) = function to be done when created a new puzzle
 * return:
 *  count of good puzzles
 */
int rotate (int puzzle[9][9],int (*doNext)(int [9][9]))
{
    
    int count = doNext(puzzle);
    int cpyPz [9][9];
    int i ,j;

    for (i = 0; i < 9;++i)
    {
		for(j = 0; j < 9;++j)
        {
		    puzzle[8-i][j] = puzzle[j][i];
        }
	}
    count += doNext(cpyPz);
    return count;
    return count;

}

/*
 * input: 
 *  Puzzle is the sudoku
 * return:
 *  1 if puzzle is unique and will add to dict
 *  0 if not
 */
int end(int puzzle[9][9])
{
    #if PARR==1
    return 1;
    #endif
    //Create bare minumum puzzle
    int* newAddon = malloc(6*sizeof(int));
    newAddon[0] = puzzle [3][0]*10000000+puzzle[4][0]*1000000+puzzle[5][0]*100000+puzzle[3][1]*10000+puzzle[4][1]*1000+puzzle[5][1]*100+ puzzle[3][2]*10+puzzle[4][2];
    newAddon[1] = puzzle [6][0]*10000000+puzzle[7][0]*1000000+puzzle[8][0]*100000+puzzle[6][1]*10000+puzzle[7][1]*1000+puzzle[8][1]*100+puzzle[6][2]*10+puzzle[7][2];
    newAddon[2] = puzzle [0][3]*10000000+puzzle[1][3]*1000000+puzzle[2][3]*100000+puzzle[0][4]*10000+puzzle[1][4]*1000+puzzle[2][4]*100+puzzle[0][5]*10+puzzle[1][5];
    newAddon[3] = puzzle [6][3]*10000000+puzzle[7][3]*1000000+puzzle[8][3]*100000+puzzle[6][4]*10000+puzzle[7][4]*1000+puzzle[8][4]*100+puzzle[6][5]*10+puzzle[7][5];
    newAddon[4] = puzzle [0][6]*10000000+puzzle[0][7]*1000000+puzzle[0][8]*100000+puzzle[1][7]*10000+puzzle[1][8]*1000+puzzle[1][9]*100+puzzle[2][7]*10+puzzle[2][8];
    newAddon[5] = puzzle [3][6]*10000000+puzzle[4][6]*1000000+puzzle[5][6]*100000+puzzle[3][7]*10000+puzzle[4][7]*1000+puzzle[5][7]*100+puzzle[3][8]*10+puzzle[4][8];


    if(checkUnique(newAddon))//If puzzle is new
    {
        nodeAdd(head,newAddon);
        return 1;
    }
    return 0;

} 

/*
 * input:
 *  new is the test of condensed puzzle to be unique in dict
 * output:
 *  (0 or 1)
 *      0 if unique
 *      1 if not unique
 */
int checkUnique(int new[6])
{
    if(node_find(head,new))//if puzzle is found
    {
        
        return 0;
    }
    return 1;
}

/*Do next Functions will point to another function
 * Meant to be function pointers
 */

int doRel (int puzzle[9][9])
{   return relable(puzzle,0,400000,end);}
int doRot (int puzzle[9][9])
{   return rotate(puzzle,end);}

int doRelRot (int puzzle[9][9])
{   return relable(puzzle,0,400000,doRot);}

int doColRot (int puzzle[9][9])
{    return column(puzzle,0,400000,doRot);}

int doRowColRot (int puzzle[9][9])
{    return row(puzzle,0,400000,doColRot);}

int rowColRot (int puzzle[9][9],int startPerm,int extra)
{
    return row(puzzle,startPerm,extra,doColRot);
}

int relRot (int puzzle[9][9],int startPerm,int extra)
{
    return relable (puzzle,startPerm,extra,doRot);
}

int main (int argc, char* argv[])
{
    head = node_create();
    int a,b,c,x,y;
    volatile int total = 0;
    double stTime,enTime;



    if (argc != 82)
    {
        printf("Error .convolute <matrix>");
        return 1;
    }
    int puzzle[9][9];
    for (x = 0; x < 81;++x)
    {
        puzzle [x/9][x%9] = atoi(argv[x+1]);
    }
    #if PARR==1
    printf("Threads For Sections,Threads for Row+Column+Rotation,Threads For Relable+Rotation,Solutions Found,Time To Comput\n");
    for(a = 1; a <= MAX_THREADS;++a)
	for(b = 1;(b <= MAX_THREADS-a)||(b==1);++b)
		for(c = 1;(c <= MAX_THREADS-(a+b))||(c==1);++c)
    { 
    
	total = 0;
        stTime = omp_get_wtime();
        #pragma omp parallel sections private(x) reduction(+:total) num_threads(a)
        {
            #pragma omp section
            {
                #pragma omp parallel for reduction(+:total) num_threads(b)
                for(x = 0;x < 150;++x)
                    total += rowColRot (puzzle,x*16,15);
            
            }   

            #pragma omp section
            {
                #pragma omp parallel for reduction (+:total) num_threads(c)
                for(x = 0; x< 40000;++x)
                    total += relRot (puzzle,x*10,9);
            }
        }
        enTime = omp_get_wtime();
        printf("%d,%d,%d,%ld,%f\n",a,b,c,total,enTime-stTime);
    }
    #else
    stTime = omp_get_wtime();
    print_puzzle(puzzle);
    total = 0;
    total += rowColRot(puzzle,0,10000);
    total += doRel(puzzle);
    enTime = omp_get_wtime();
    printf("Answer found %d in %f seconds\n",total,enTime-stTime);
    #endif
 

    return 0;
}
