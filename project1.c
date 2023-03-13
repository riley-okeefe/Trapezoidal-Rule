// CSCI 3431: Operating Systems
// Project 1: Inter Process Communication
// Authors: Riley O’Keefe & Ali Alhusseini

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define A 0.0 // lower limit
#define B 2.0 // upper limit
#define READ_END 0 // read end of the pipe
#define WRITE_END 1 // write end of the pipe

// function for the f(x) calculation
double function(double x)
{
    return (x*x) + 1.0;
}

int main()
{
    // brief intro of the program
    system("clear");
    printf("\n\n\n\n\n\n\n\n\n\n");
    printf("\t\tCSCI 3431: Operating Systems\n");
    printf("\t\tProject 1: Inter Process Communication\n");
    printf("\t\tAuthors: Riley O'Keefe & Ali Alhusseini");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n");
    printf("Press enter to continue...");
    getchar();
    system("clear");
    printf("\n\n\nThis program uses the trapezoidal rule to approximate ");
    printf("the \nintegral of x^2 + 1dx from 0 (lower limit) to 2 (upper ");
    printf("limit).\n\nBy using N processes to sum up n trapezoids. \n");
    printf("N should vary from 2, 4, 8 while n can be 32 or 64.\n");

    // get user input for the number of processes
    int N; // number of child processes
    printf("\n\n\n\nEnter the number of processes (N): ");
    scanf("%d", &N);
    
    int fdsp[N][2], fdsc[N][2]; // create the fd for parent and child
    int childId; // variable for the child id
    double total_sum; // variable to hold the total sum
    double sum; // variable to read in the cumulative sums from each child
    double area; // variable for the area
    double s_i; // variable for the s_i values in child
    int n; // number of trapezoids
    pid_t child; // process identifier for child
    
    // get user input for the number of trapezoids
    printf("Enter the number of trapezoids (n): ");
    scanf("%d", &n);
    double deltaX = (B-A)/n; // calculation for delta x

    // create N pairs of pipes
    for(int i = 1; i < N + 1; i++)
    {
        pipe(fdsp[i]);
        pipe(fdsc[i]);
    }
   
    for (int i = 1; i < N + 1; i++)
    {
        // fork N child processes
        child = fork();
    
        if(child == 0) // child process
        {
            // read ids sent from parent
            close(fdsp[i][WRITE_END]);
            read(fdsp[i][READ_END], &childId, sizeof(int));

            // starting xi limit value for child process
            double startXi = (childId-1) * (deltaX) * (n/N);
            // ending xi limit value for current child process
            double endXi = (childId) * (deltaX) * (n/N);

            // if child id is 1, firstly calculate the f(x0)
            if(childId == 1)
            {
                s_i = function(A);
                close(fdsc[i][READ_END]);
                write(fdsc[i][WRITE_END], &s_i, sizeof(double));
            }
            // if the child id is N or the last one calculate the f(xn)
            if(childId == N)
            {
                s_i = function(B);
                close(fdsc[i][READ_END]);
                write(fdsc[i][WRITE_END], &s_i, sizeof(double));
            }
            
            // use of for loop to do remaining calculations for each child
            // using 2*f(xi) after f(x0) and f(xn)
            for(double xi = startXi + deltaX; xi <= endXi; xi += deltaX)
            {
                s_i = 2 * function(xi);
                close(fdsc[i][READ_END]);
                write(fdsc[i][WRITE_END], &s_i, sizeof(double));
            }
        return 0;
        }
        else if (child < 0) // fork failed
        {
            printf("Error: Unsuccesful fork by child %d. Program terminating.\n", i);
            return 1;
        }
        else // parent process
        {
            close(fdsp[i][READ_END]);
            write(fdsp[i][WRITE_END], &i, sizeof(int));
            
            // if i = 1 read in the x0 value and add to total sum
            if(i == 1)
            {
                double x0Val;
                close(fdsc[i][WRITE_END]);
                read(fdsc[i][READ_END], &x0Val, sizeof(double));
                total_sum += x0Val;
            }
            
            // use of for loop to read in each the correct number of
            // calculations from each child
            for(int k = 1; k < (n/N)+1; k++)
            {
                close(fdsc[i][WRITE_END]);
                read(fdsc[i][READ_END], &sum, sizeof(double));
                // adding the current child sum to the total_sum
                total_sum += sum;
                // wait for each child process to complete
                wait(NULL);
            }
        }
    }
    // calculate and print the final sum
    area = ((total_sum) * (deltaX/2.0));
    printf("\n\n\n>>>>>>>>>>>>>>>>>>>>\n");
    printf("Total area is: %.3f\n", area);
    printf("<<<<<<<<<<<<<<<<<<<<\n\n\n\n\n");
    return 0;
}
