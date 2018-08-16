#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#define READ_END 0
#define WRITE_END 1


const int MARKER = -1;
const int FINISHED = -3;

//Linkedlist methods and properties
typedef struct Node
{
    int val;
    struct Node *next;
}Node;

struct Node * head = NULL;
struct Node * tail = NULL;
void printList()
{
    struct Node * cur = head;
    while(cur != NULL)
    {
        printf("%d , ", cur->val);
        cur = cur->next;
    }
    printf(" .Done\n");
}

void add(int val)
{
    struct Node * nNode = (struct Node*) malloc(sizeof(struct Node));
    nNode->val = val;
    nNode->next = NULL;
    if(head == NULL)
    {
        head = nNode;
        tail = nNode;
    }
    else
    {
        tail->next = nNode;
        tail = nNode;
    }


}

void cleanList()
{
    struct Node * cur = head;
    while(cur != NULL)
    {
        head = head->next;
        free(cur);
        cur = head;

    }
}

int removeFromHead()
{
    if(head == NULL)
        return -2;
    int val = head->val;
    struct Node * cur = head;
    head = head->next;
    free(cur);
    return val;
}

int main(int argc, char** argv)
{

    if(argc != 3)
    {
        printf("Please enter 2 integer arguments\n");
        return -1;

    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    printf("\nN: %d, M: %d\n", N, M);



    int pipePrint[2];
    int pipePrimes[M + 1][2];

    if(pipe(pipePrint) == -1)
    {
        fprintf(stderr, "Pipe failed");

    }
    for(int i = 0; i < (M + 1); i++)
    {
        if(pipe(pipePrimes[i]) == -1)
        {
            fprintf(stderr, "Pipe failed");
        }


    }
    fcntl(pipePrimes[M][READ_END], F_SETFL, O_NONBLOCK);

    struct timeval start, end;

    gettimeofday(&start, NULL);


    pid_t pid;
    pid = fork();


    //Fork for print process
    if(pid < 0)
    {
        fprintf(stderr, "Fork failed in print");
        //return -1;
    }
    else if(pid > 0)
    {

        for(int i = 0; i < M; i++)
        {
            pid = fork();
            if(pid < 0)
            {
                fprintf(stderr, "Fork failed in child");
                return -1;
            }
            else if(pid > 0)
            {
                continue;
            }
            else
            {
                //Child
                while(1)
                {

                    int prime = 0;

                    //Get first int as prime
                    read(pipePrimes[i][READ_END], &prime, sizeof(int));

                    //If child gets MARKER as prime, means that it should finish its work
                    if(prime == MARKER)
                    {

                        write(pipePrimes[i + 1][WRITE_END], &MARKER, sizeof(int) );
                        write(pipePrimes[M][WRITE_END], &FINISHED, sizeof(int) );
                        exit(0);

                    }

                    //Send first int to print process
                    write(pipePrint[WRITE_END], &prime, sizeof(prime));

                    int current = prime;
                    while(1)
                    {
                        //Read value from pipe and send if not divisible by your prime to next pipe
                        close(pipePrimes[i][WRITE_END]);
                        read(pipePrimes[i][READ_END], &current, sizeof(int));

                        if(current % prime != 0)
                        {
                            close(pipePrimes[i + 1][READ_END]);
                            write(pipePrimes[i + 1][WRITE_END], &current, sizeof(int) );

                        }
                        if(current == MARKER)
                        {
                            close(pipePrimes[i + 1][READ_END]);
                            break;

                        }

                    }
                }
                break;


            }


        }

        //Parent
        int j = 2;
        int isMarkerSent = 0;
        int finished = 0;

        while(finished == 0)
        {

            //Send up to N, if it is done, send from linkedlist
            if(j < N)
            {
                write(pipePrimes[0][WRITE_END], &j, sizeof(int) );
                j++;

            }
            else if(isMarkerSent == 0)
            {
                write(pipePrimes[0][WRITE_END], &MARKER, sizeof(int));
                isMarkerSent = 1;

            }
            else
            {
                //Send from linkedlist
                int processVal;

                processVal = removeFromHead();
                if(processVal != -2)
                {
                    write(pipePrimes[0][WRITE_END], &processVal, sizeof(int));
                }

            }

            //Read if there is any data in pipe M
            int listVal;

            int result = read(pipePrimes[M][READ_END], &listVal, sizeof(int));
            //printf("I read : val: %d result : %d\n", listVal, result);
            if(result != -1)
            {
                //Check for if the whole sequence finished
                if(listVal == FINISHED)
                {
                    finished = 1;
                }
                else
                    add(listVal);
            }

        }

        //Send exit signal to all children
        write(pipePrint[WRITE_END], &FINISHED, sizeof(int));
        write(pipePrimes[0][WRITE_END], &MARKER, sizeof(int));

        //Close pipes
        for(int i = 0; i < M + 1; i++)
        {
            close(pipePrimes[i][WRITE_END]);
            close(pipePrimes[i][READ_END]);

        }
        //Close print pipe
        close(pipePrint[WRITE_END]);
        close(pipePrint[READ_END]);
        cleanList();



    }
    else
    {
        //Print process
        while(1)
        {
            int printVal;
            close(pipePrint[WRITE_END]);
            read(pipePrint[READ_END], &printVal, sizeof(int));
            if(printVal == FINISHED)
                exit(0);
            printf("Print: %d\n", printVal);

        }
    }

    gettimeofday(&end, NULL);
    printf("Execution time: %ld seconds\n", end.tv_sec - start.tv_sec);

    return 0;

}
