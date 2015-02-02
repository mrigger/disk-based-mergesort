/*
 *
 * Name: Arun Athrey Chandrasekaran
 *
 */


#ifndef MERGE_H
#define MERGE_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include <sys/time.h>


#define DEBUG 0

//Cosntants for the project
#define IN_BUFF_SIZE 1000
#define OUT_BUFF_SIZE 1000

#define SUPER_RUN_NO 15

FILE *index_fp;
int in_buf[IN_BUFF_SIZE];
int out_buf[OUT_BUFF_SIZE];
char i_filename[100];
char o_filename[100];


#endif

                
