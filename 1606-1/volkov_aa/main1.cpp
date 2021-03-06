
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
int new_log_2(const int a){
        int k=a;
        int i=0;
        while(k%2==0){
                i++;
                k=k/2;
        }
        if((int)(pow(2, (float)(i)))==a){
                return i;
		}
        else{
                return 0;
		}}
void new_sum(const void *arr1, void *arr2, int n, MPI_Datatype t){
        int i;
        if(t==MPI_INT){
                for(i=0; i<n; i++){
                        ((int *)arr2)[i]=((int *)arr1)[i] + ((int *)arr2)[i];
                }}
        if(t==MPI_FLOAT){
                for(i=0; i<n; i++){
                        ((float *)arr2)[i]=((float *)arr1)[i] + ((float *)arr2)[i];
                }}
        if(t==MPI_DOUBLE){
                for(i=0; i<n; i++){
                        ((double *)arr2)[i]=((double *)arr1)[i] + ((double *)arr2)[i];
				}}}
void NEW_MIN(const void *arr1, void *arr2, int n, MPI_Datatype t){
        int i;
        if(t==MPI_INT){
			for(i=0; i<n; i++){
                        if(((int *)arr2)[i]>((int *)arr1)[i]){
                                ((int *)arr2)[i]=((int *)arr1)[i];
                        }}}
        if(t==MPI_FLOAT){
                for(i=0; i<n; i++){
                        if(((float *)arr2)[i]>((float *)arr1)[i]){
                                ((float *)arr2)[i]=((float *)arr1)[i];
						}}}
        if(t==MPI_DOUBLE){
                for(i=0; i<n; i++){
                        if(((double *)arr2)[i]>((double *)arr1)[i]){
                                ((double *)arr2)[i]=((double *)arr1)[i];
						}}}}
int tree_reduce(void* send, void* recv, int count,int procarr, int  procnumber, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm){
        int h;
        h=(int)(log((float) procnumber)/log(2.0));
        if(new_log_2( procnumber)==0) h++;

		for(int i = 0; i<h;i++){        
                        int j=0;
                        while(j< procnumber){
                                int k =j+pow(2, (float)(i));
                                if(k< procnumber){
                                        if(procarr==j){
                                                MPI_Status status;
                                                MPI_Recv(recv,count,datatype,k,0,MPI_COMM_WORLD,&status);
                                                if(op==MPI_MIN) NEW_MIN(send,recv,count, datatype);
                                                if(op==MPI_SUM) new_sum(recv,send,count, datatype);
                                        }
                                        if(procarr==k){
                                                MPI_Send(send,count,datatype,j,0,MPI_COMM_WORLD);
                                        }}
                                j=j+pow(2, (float)(i+1));
						}}
    
                for (int i(1); i <  procnumber; i++){
                for (int j(0); j < i; j++){
                        int tmp(1);
                        for (int k(1); k < i; k++){
                                tmp = tmp * 2;
                        }
                        if (procarr == j){
                                if ((tmp + j <  procnumber) && (tmp + j != 0)){
                                        MPI_Send(send, count, datatype, tmp + j, 0, comm);
								}}
                        if ((procarr == tmp + j) && (tmp + j <  procnumber) && (tmp + j != 0)){
                                MPI_Status status;
                                MPI_Recv(send, count, datatype, j, 0, comm, &status);
						}}
                MPI_Barrier(comm);
				}
        return 0;
}

int main(int argc, char *argv[]){


        int procarr,  procnumber;
        double start, finish;
        MPI_Init(&argc, &argv);        
        MPI_Comm_size(MPI_COMM_WORLD, & procnumber);
        MPI_Comm_rank(MPI_COMM_WORLD, &procarr);
        start = MPI_Wtime();
        int *arr1 = new int[100];
        int *arr2 = new int[100];        
        for (int i = 0; i < 100; i++){
                arr1[i] = i;
        }
        for (int i = 0; i < 100; i++){
                arr2[i] = 0;
        }
        tree_reduce(arr1,arr2,100,procarr, procnumber,MPI_INT,MPI_SUM,MPI_COMM_WORLD);
        for (int k = 0; k < 100; k++)        
                if(procarr==1){
                        fprintf(stdout,"%d ",(int*)arr1[k]);
                        fflush(stdout);
                }
                 
        finish = MPI_Wtime();
        MPI_Finalize();
}