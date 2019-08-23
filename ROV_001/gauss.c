#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**************************************************************
 * Perform forward elimination for a linear system Ax=b.
 * Partial pivoting is adopted.
 */
void gauss_elm(double **A, double *b, int n)
{
	int     p, i, j, k;
	double  maxEntry, t, r;

	for(i=0;i<n-1;i++){
		// Partial pivoting
		maxEntry = fabs(A[i][i]);
		p = i;
		for(k=i;k<n;k++)
			if(fabs(A[k][i])>maxEntry){
				p = k;
				maxEntry = fabs(A[k][i]);
			}
		if(p!=i){
			for(j=i;j<n;j++){
				t = A[p][j];
				A[p][j] = A[i][j];
				A[i][j] = t;
			}
			t = b[p];
			b[p] = b[i];
			b[i] = t;
		}
		//Forward elimination.
        for(k=i+1;k<n;k++){
			if(A[k][i]==0.0) continue;
			
			r = A[k][i]/A[i][i];
			for(j=i;j<n;j++)
				A[k][j] = A[k][j] - r*A[i][j];
			b[k] = b[k] - r*b[i];
		}
	}
}

			
					
