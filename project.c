#include<stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define coordinator 1

int num_student,chairs,num_teacher;
int t_help; //total help needed from tutors
int stu[1000][3];
int chair[1000];
int help_id[1000];
int prio[1000];
int teachid[1000];
int tc=0,st=0; //teacher index, student index

sem_t m;
sem_t mut;

void *ofunc(void *p) //coordinator's function
{
    sem_wait(&m);
    printf("\nCoordinator is working.\n");
    sem_post(&m);
}

void *tfunc(void *p) //teacher's function
{
    int i, s, b, tc=(int)p;
    s=tc;
    tc=tc%num_teacher;

    for(i=0;i<num_student;i++)
    {
        if(help_id[s]==stu[i][0])
        {
          if(stu[i][2]==0)
          {
              b=0;
          }
        }
    }
    sem_wait(&m);
    if(b!=0);
    {
        printf("\nteacher ID %d is teaching",teachid[tc]);
    }
    sem_post(&m);
}

void *sfunc(void *p) //student's function
{
    int st=(int)p;
    int i, bt; //Burst Time
    sem_wait(&m);
    for(i=0; i<num_student; i++)
    {
        if(help_id[st]==stu[i][0])
        {
            bt=stu[i][2];
        }
    }
    if(bt!=0)
    {
        printf("\nStudent ID: %d is solving his/her problem\n",help_id[st]);
        for(i=0; i<num_student; i++)
        {
            if(help_id[st]==stu[i][0])
            {
                stu[i][1]=(stu[i][1])+1; //for each time for getting help the priority number will increase by one
                stu[i][2]=(stu[i][2])-1; //for each time for getting help the burst time will decrease by one
                break; //break to enter main function to run for another student
            }
        }
    }
    sem_post(&m);
}

int main()
{
    int i, j, k;
    printf("Enter the number of student: ");
    scanf("%d",&num_student);

    printf("\nEnter the number of teachers: ");
    scanf("%d",&num_teacher);

    printf("\nEnter the number of chairs: ");
    scanf("%d",&chairs);

    for(i=0; i<num_teacher; i++)
    {
        teachid[i]=i+1;
    }
    //input of students info
    for(i=0; i<num_student; i++)
    {
        for(j=0; j<3; j++)
        {
            if(j==0)
            {
                stu[i][j] = i+1; //student id is setting by increasing order depending on their arrival
            }
            else if(j==1)
            {
                stu[i][j] = 1; //primary priority set 1 for all
            }
            else
            {
                printf("\nEnter the burst time of student %d: ",i+1);
                scanf("%d", &stu[i][j]); //how many time they will need help from tutor
                t_help=t_help + stu[i][j]; //count the total help for all the students
            }

        }
    }
    /* semaphore m should be initialized by 1 */
    if (sem_init(&m, 0, 1) == -1)
    {
        perror("Could not initialize my lock semaphore");
        exit(1);
    }

    if (sem_init(&mut, 0, 1) == -1)
    {
        perror("Could not initialize my lock semaphore");
        exit(1);
    }

    //input of students who needs help
    for(i=0; i<t_help; i++)//will run until all the students got help
    {
        int hlp;

        printf("\nhow many needs help: ");
        scanf("%d",&hlp); //how many students need help from total students

        if(hlp<=chairs) //if we can accommodate students based on the number of chair
        {
            printf("\nenter the ID for who needs help: ");
            for(j=0; j<hlp; j++)
            {
                scanf("%d",&help_id[j]); //we will rearrange this array based on priority values
            }
            prio[hlp]=0;

            for(j=0; j<hlp; j++)
            {
                for(k=0; k<num_student; k++)
                {
                    if(stu[k][2]==0) //check if the student burst time is zero
                    {
                        continue; //if zero we will skip
                    }
                    if(help_id[j]==stu[k][0]) // if the burst time is not zero for the student we will copy his priority to the priority array
                    {
                        prio[j]=stu[k][1];
                    }
                }
            }

            for(j=0; j<hlp; j++) //priority swapping
            {
                for(k=j+1; k<hlp; k++)
                {
                    if(prio[j]>prio[k])
                    {
                        int temp1 = prio[j];
                        prio[j]=prio[k];
                        prio[k]= temp1;

                        int temp2 = help_id[j];
                        help_id[j]=help_id[k];
                        help_id[k]=temp2;
                    }
                }
            }

            pthread_t o_kid;
            pthread_t t_kid[hlp];
            pthread_t s_kid[hlp];

            sem_wait(&mut);
            pthread_create (&o_kid, NULL, ofunc, NULL); //coordinator thread
            sem_post(&mut);

            sem_wait(&mut);
            for(j=0; j<hlp; j++) //we will create threads for each of the students and use semaphore in assigning the teachers
            {
                pthread_create (&t_kid[j], NULL, tfunc, (void*) j); //teacher thread
                pthread_join(t_kid[j], NULL);


                pthread_create (&s_kid[j], NULL, sfunc, (void*) j); //student thread
                pthread_join(s_kid[i], NULL);
            }
            sem_post(&mut);

            t_help=t_help-hlp;
            i=-1; //the loop will end at i=-1, which will be increased to 0 on the next one
        }
        else if(hlp>chairs) //if the user defines more student than chair
        {
            printf("\nNot more than %d students can wait in the sitting area",chairs);
            i=i-1; //the loop will end at i=-1, which will be increased to 0 on the next one
        }
    }
    printf("\n");
    return 0;
}
