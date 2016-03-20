#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/stat.h>
#include <mpi.h>
#include "epanet2.h"

int MAXSTRING_LEN = 128;

//Declare functions
void set_file_name(char *, char *, int, char *, char *);
int run_epanet_simulation(char *, char *, double *);
void write_output_file(int, int, char *, char **, char *, double *);
void copy_current_best_solution(int, int, char *, char **, double *);

int main(int argc, char *argv[])
{
    char **file_names,
        *file_name,
        *file_path,
        *input_file,
        *report_file;

    int i,
        mpi_error_code,
        num_procs,
        rank,
        file_number,
        num_files,
        generation,
    	error,
        local_file_number;

    double fitness,
           *fitness_values;
    //char **file_names;




    //if (num_files > 192)
    //    num_procs = 192;
    //else
    //    num_procs = num_files;
    
    //printf("\nnum_procs: %d\n", num_procs);
    
    /*file_names = (char **)malloc(num_files*sizeof(char *));
    for(i=0;i<num_files;i++)
    {
        file_names[i] = (char *)malloc(MAXSTRING_LEN*sizeof(char));
    }

    fitness_values = (double *)malloc(num_files*sizeof(double));
    */
    // printf("\n%d\t%s\t%s\n", num_files, file_path, file_name);


    
    //Start up MPI
    mpi_error_code = MPI_Init(&argc, &argv);
    if (mpi_error_code != MPI_SUCCESS)
	{
		printf ("\nError starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, mpi_error_code);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    //printf("\nMy rank: %d or %d\n", rank, num_procs);
    
    /*
    for (i=0; i<argc; i++)
    {
        printf("arg %d, %s", i, argv[i]);
    }
    */
    
    //start_time = MPI_Wtime();
    
    //printf("\nStarting: %f\n", start_time);
    
    //fitness = (double *)malloc(1*sizeof(double));
    //local_file_number = (int *)malloc(1*sizeof(int));
    //char *input_file,
    //    *report_file;
    input_file = (char *)calloc(MAXSTRING_LEN, sizeof(char));
    report_file = (char *)calloc(MAXSTRING_LEN, sizeof(char));
    
    
    //file_name = (char *)malloc(MAXSTRING_LEN*sizeof(char));
    //file_path = (char *)malloc(MAXSTRING_LEN*sizeof(char));
    
    //char input_file[10] = "Fun.inp",
    //     report_file[10] = "Fun.rpt";
    generation = atoi(argv[1]);
    num_files = atoi(argv[2]);
    file_name = argv[argc-1];
    file_path = argv[argc-2];
    //
    //char file_name[10] = "this";
    //char file_path[20] = "/yay/";

    //generation = 1;
    //num_files = 2;

    //int *file_numbers[num_files];

    
    if (rank == 0)
    {
        //printf("\n\n%d\n", num_files);
       // for (i=0;i<num_files;i++)
           // file_numbers[i] = i;
    
        
        file_names = (char **)malloc(num_files * sizeof(char *));
        //file_names = (char *)calloc(num_files*MAXSTRING_LEN, sizeof(char ));
        for(i=0;i<num_files;i++)
        {
            file_names[i] = (char *)calloc(MAXSTRING_LEN, sizeof(char));
        }
        

        fitness_values = (double *)calloc(num_files, sizeof(double));

        /*
        for (i=0; i<num_files; i++)
        {
            printf("Fitness after init: %f\n", fitness_values[i]);
            printf("File name after init: %s\n", file_names[i]);
        }
        */
    }//7777
    
    //MPI_Bcast(file_path, MAXSTRING_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);
    //MPI_Bcast(file_name, MAXSTRING_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    //MPI_Scatter(file_numbers, 1, MPI_INT, &local_file_number, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //printf("local file number for rank %d: %d\n", rank, local_file_number);
   
    file_number = rank * (generation + 1);
    set_file_name(file_path, file_name, rank, ".inp", input_file);
    set_file_name(file_path, file_name, rank, ".rpt", report_file);
    
    //input_file = "Fun.inp";
    //report_file = "Fun.rpt";
    
    //printf("local inp file for rank %d: %s\n", rank, input_file);
    //printf("local rpt file for rank %d: %s\n", rank, report_file);
    
    error = run_epanet_simulation(input_file, report_file, &fitness);
    
    //fitness = 5.0;
    
    //printf("fitness for rank %d: %f\n", rank, fitness);
    
    
    MPI_Gather(&fitness, 1, MPI_DOUBLE, fitness_values, 1 , MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /*
    if (rank == 0)
    {
        for (i=0; i<num_files; i++)
        {
            printf("Fitness after gather: %f\n", fitness_values[i]);
        }

    }
    */
    
    
    if (rank != 0)
    {
        MPI_Send(input_file, MAXSTRING_LEN, MPI_CHAR, 0, rank, MPI_COMM_WORLD); 
        //printf("\nSent %s from process %d!\n", input_file, rank);
    }
    //MPI_Gather(&input_file, MAXSTRING_LEN, MPI_CHAR, file_names, MAXSTRING_LEN , MPI_CHAR, 0, MPI_COMM_WORLD);
    //printf("\n%x, %s", &input_file, input_file);

    if (rank == 0)
    {
        MPI_Status stat;
        strcpy(file_names[0], input_file);
        for (i=1;i<num_files;i++)
        {
            MPI_Recv(input_file, MAXSTRING_LEN, MPI_CHAR, i, i, MPI_COMM_WORLD, &stat);
            //printf("current input file: %s\n", input_file);
            strcpy(file_names[i], input_file);
            //printf("\nReceived %s with status %s!\n", file_names[i], stat);
        }

        /*
        for (i=0;i<num_files; i++)
        {
            printf("\nFile %d: %s", i, file_names[i]);
            printf("\nFitness %d: %f\n", i, fitness_values[i]);
        }
        */

        printf("Is this happening?\n");
        write_output_file(num_files, generation, file_path, file_names, file_name, fitness_values);
        copy_current_best_solution(generation, num_files, file_path, file_names, fitness_values);

    }
    
    /*
    if (rank != 0)
    {
        free(input_file);
        free(report_file);
    }
    */

    if (rank == 0)
    {
        //printf("num files: %d", num_files);
        for(i=0;i<num_files;i++)
        {
            free(file_names[i]);
        }
        free(file_names);
        free(fitness_values);
        //free(input_file);
        //free(report_file);
    }
    free(input_file);
    free(report_file);
    //free(file_name);
    //free(file_path);

    //if (rank == 0)
        //free(fitness_values);

    //printf("\n\nFree error?\n\n");

    MPI_FINALIZE();
    return 0;
}

void set_file_name(char *file_path, char *file_name, int file_number, char *file_type, char *file)
{
    char file_num[10];
    char *local_name;
    

    local_name = (char *)calloc(MAXSTRING_LEN, sizeof(char));

    file[0] = '\0';
    sprintf(file_num, "%d", file_number);
    strcat(local_name, file_path);
    strcat(local_name, "input/");
    strcat(local_name, file_name);
    strcat(local_name, "_");
    strcat(local_name, file_num);
    strcat(local_name, file_type);

    strcpy(file,local_name);
    

}

int run_epanet_simulation(char *input_file, char *report_file, double *fitness)
{
    int i,
        num_nodes,
        error_code = 0;
    long t, 
        hydraulic_time_step, 
        tstep;
    float pressure;
    double min_pressure = 20.0,
        max_pressure = 120.0,
        min_violation,
        max_violation,
        sum_pressure_violations = 0.0;
    //Open and initialize the hydraulic solver
   
    error_code = ENopen(input_file, report_file, "");
    if (error_code != 0)
    {
        //printf("\nerror_code: %d", error_code);
        sum_pressure_violations = -1.0;
        *fitness = sum_pressure_violations;
        ENclose();
    	return -1;
    }
    ENgetcount(EN_NODECOUNT, &num_nodes);
    ENgettimeparam(EN_HYDSTEP, &hydraulic_time_step);

	error_code = ENopenH();
    if (error_code != 0)
    {
        printf("\nEnopenH() error_code: %d", error_code);
        sum_pressure_violations = -1.0;
        *fitness = sum_pressure_violations;
        ENclose();
    	return -1;
    }
	error_code = ENinitH(0);
    if (error_code != 0)
    {
        printf("\nEninitH() error_code: %d", error_code);
        sum_pressure_violations = -1.0;
        *fitness = sum_pressure_violations;
        ENclose();
    	return -1;
    }
   
    pressure = 0.0;

	//Run the hydraulic solver one hydraulic time step at a time
	do{
		//printf("My rank: %d\n", rank);
		error_code = ENrunH(&t);
        if (error_code != 0)
        {
            printf("\nENrunH() error_code: %d", error_code);
            sum_pressure_violations = -1.0;
            *fitness = sum_pressure_violations;
            ENclose();
    	    return -1;
        }
		// Retrieve hydraulic results for time t
		if (t%hydraulic_time_step == 0)
		{
			for (i = 1; i <= num_nodes; i++)
			{
                ENgetnodevalue(i, EN_PRESSURE, &pressure);
                min_violation = min_pressure - pressure;
                max_violation = pressure - max_pressure;

                if(min_violation > 0 || max_violation > 0)
                {
                    //printf("Pressure: %f\tMin Violation: %f\tMax Violation: %f\n", pressure, min_violation, max_violation);
                    if(min_violation > max_violation)
                    {
                        // printf("\nPressure violation: %f", min_violation);
                        sum_pressure_violations += min_violation;
                    }
                    else
                    {
                        // printf("\nPressure violation: %f", max_violation);
                        sum_pressure_violations += max_violation;
                    }
                }
            }
        }		
		error_code = ENnextH(&tstep);

        if (error_code != 0)
        {
            printf("\nEnnextH() error_code: %d", error_code);
            sum_pressure_violations = -1.0;
            *fitness = sum_pressure_violations;
            ENclose();
    	    return -1;
        }
	} while (tstep > 0);
    //printf("\npressure violation: %f\n", sum_pressure_violations);
    *fitness = sum_pressure_violations;
    //printf("Closing hydralic simulation!\n");
    ENcloseH();
    //printf("Closing EPANET\n");
    ENclose();
    //return sum_pressure_violations;
    //printf("Closed!\n");
    return 0;
}

void write_output_file(int num_files, int generation, char *file_path, char **file_names, char *sub_string, double *fitness_values)
{
    FILE *fp;
    int i,
        len;
    char *output_file,
        *split_string,
        file_num[10];
    
    //printf("\n\t%f\n\t%f\n\t%f\n\t%f\n\t%f\n", fitness_values[0], fitness_values[1], fitness_values[2], fitness_values[3], fitness_values[4]);
    //printf("\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n", file_names[0], file_names[1], file_names[2], file_names[3], file_names[4]);
    output_file = (char *)malloc(MAXSTRING_LEN*sizeof(char));

    output_file[0] = '\0';
    sprintf(file_num, "%d", generation);
    strcat(output_file, file_path);
    strcat(output_file, "output/generation_");
    strcat(output_file, file_num);
    strcat(output_file, ".out");
    //printf("\nOutput file: %s\n", output_file);

    fp = fopen(output_file, "w+");
    if (!fp)
        return;
    fprintf(fp, "INP_FILE,FITNESS\n");
    for (i = 0; i < num_files; i++)
    {
        //printf("\n\n SEG FAULT FINDER 5\n");
        //split_string = strrchr(file_names[i], '/');
        split_string = strstr(file_names[i], sub_string);
        //fprintf(fp, "%s,%f\n", file_names[i], fitness_values[i]);                                     
        fprintf(fp, "%s,%f\n", split_string, fitness_values[i]);                                     
    }

    fclose(fp);
    //free(output_file);
}

void copy_current_best_solution(int generation, int num_values, char *path, char **file_names, double *fitness_values)
{
    int i, best_file_number;
    //float num_values = 0.0;
    double current_best = 1e20;
    char ch;
    char *best_file_name, *new_file_name, *file_num;
    FILE *source_file, *target_file;

    new_file_name = (char *)calloc(MAXSTRING_LEN, sizeof(char));
    best_file_name = (char *)calloc(MAXSTRING_LEN, sizeof(char));

    best_file_number = 0;
    //printf("\nnum vals: %d\n", num_values);
    //current_best = fitness_values[0];
    for (i=0; i< num_values; i++)
    {
    //report_file = (char *)calloc(MAXSTRING_LEN, sizeof(char));
        if (fitness_values[i] >= 0 && fitness_values[i] <= current_best)
        {
            best_file_number = i;
            current_best = fitness_values[i];
        }
    }
    //if (best_file_number == 0)
    //    return;
    strcpy(best_file_name, file_names[best_file_number]);
    strcat(new_file_name, path);
    strcat(new_file_name, "current_best/");
    strcat(new_file_name, "best_solution_");
    sprintf(file_num, "%d", generation);
    strcat(new_file_name, file_num);
    strcat(new_file_name, ".inp");
    
    printf("\nfile: %s\n", best_file_name);
    printf("\nnew file: %s\n", new_file_name);
    source_file = fopen(best_file_name, "r");
    target_file = fopen(new_file_name, "w+");

    while ((ch = fgetc(source_file)) != EOF)
              fputc(ch, target_file);

    fclose(source_file);
    fclose(target_file);

}
