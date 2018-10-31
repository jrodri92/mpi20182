/*
 *  vecsum-serial.c
 *
 *  Created by Edwin Montoya on 10/16/18.
 *  Copyright 2018 __Universidad EAFIT__. All rights reserved.
 *
 */
 
 #include <stdio.h>
 #include <math.h>

 #define F_x 30000
 #define F_y 10000

 long vec_sum[F_x];
 long Fi[F_x][F_y];

int processRow(long index) {
	long i;
	long result = 0;
	for (i=0;i<F_x;i++)
		result = result + Fi[index][i];
	return result;	
}

int doSerial() {
	long i;
	for (i=0;i<F_x;i++) {
		vec_sum[i] = processRow(i);
	}
}

 void fillMatrix() {
	long i,j;
	long val_i, val_j;
	int prob;
	for (i=0;i<F_x;i++) 
		for (j=0;j<F_y;j++) {
			val_i = rand()%256;
			val_j = rand()%256;
			Fi[i][j] = val_i;
		}
 }
 void main(int argc, char **argv) {
	fillMatrix();
	doSerial();
 }

