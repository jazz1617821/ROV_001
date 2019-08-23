/*****************************************************
 * Thsi file contains procedure to tranform a nxn matrix
 * into an upper triangular matrix by using Householder
 * matrix.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "solverdefinition.h"
#include "vec_mtx.h"




/*------------------------------------------------------
 * Create a Householder vector for a matrix. This vector
 * is used to create a Householder matrix H such that 
 * H*A makes the entries below A[i][j] of A[][j] be 0.
 */
void create_H_vec(double **A, int i, int j, double *v, int n)
{
	int      k;
	double   norm2;

	for (k = 0; k < i; k++) {
		v[k] = 0.0;
	}
	norm2 = 0.0;
	for (k = i; k < n; k++) {
		norm2 += A[k][j] * A[k][j];
	}
	norm2 = sqrt(norm2);

	if (A[i][j] >= 0.0) {
		v[i] = A[i][j] + norm2;
	}
	else {
		v[i] = A[i][j] - norm2;
	}

	for (k = i + 1; k < n; k++) {
		v[k] = A[k][j];
	}
	/*fprintf(stderr, " Householder vector=\n");
	for (k = 0; k < n; k++) {
		fprintf(stderr, " %lf", v[k]);
	}
	fprintf(stderr, "\n");*/
}


/*------------------------------------------------------
 * Procedure to reflect matrix A into an upper triangular
 * matrix by using a sequence of Householder matrices.
 *    H(n-2)H(n-3)...H(0)Ax = H(n-2)H(n-3)...H(0)b.
 * ---> Rx = b'.
 */
void QR_reflect(double **A, double *b, int n)
{
	int     i, j, k;
	double  *v, vv, vx;

	v = alloc_vec(n);
	//Eliminate each column to make A[][] into U[][].
	for(j=0;j<n-1;j++){
		// Create vector v[] = A.j + ||A.j||*e1.
		create_H_vec(A, j, j, v, n);
        vv = inner_product(v, v, n);
		// Update each column A.k, j<=k<=n-1.
		for(k=j;k<n;k++){
            // Compute vx=<v, A.k>
            vx = 0.0;
			for (i = j; i < n; i++) {
				vx += A[i][k] * v[i];
			}
			// A.k = A.k - 2(<v, A.k>/<v,v>)v ;
			for (i = j; i < n; i++) {
				A[i][k] = A[i][k] - 2.0*(vx / vv)*v[i];
			}
		}
		// Update b. b = b - 2(<v,b>/<v,v>)v ;
		vx = inner_product(v, b, n);
		for (i = j; i < n; i++) {
			b[i] = b[i] - 2.0*(vx / vv)*v[i];
		}
		/*fprintf(stderr," A[]=\n");
		print_mtx(A, n);
		fprintf(stderr," b[]=\n");
		print_vec(b, n);*/
	}
}

void QR_mxn_reflect(double **A, double *b, int m, int n, double **A_square, double* b_afterQR)
{
	int     i, j, k;
	double  *v, vv, vx;

	v = alloc_vec(m);
	//Eliminate each column to make A[][] into U[][].
	for (j = 0; j < n; j++) {
		// Create vector v[] = A.j + ||A.j||*e1.
		create_H_vec(A, j, j, v, m);
		vv = inner_product(v, v, m);
		// Update each column A.k, j<=k<=n-1.
		for (k = j; k < m; k++) {
			// Compute vx=<v, A.k>
			vx = 0.0;
			for (i = j; i < m; i++) {
				vx += A[i][k] * v[i];
			}
			// A.k = A.k - 2(<v, A.k>/<v,v>)v ;
			for (i = j; i < m; i++) {
				A[i][k] = A[i][k] - 2.0*(vx / vv)*v[i];
			}
		}
		// Update b. b = b - 2(<v,b>/<v,v>)v ;
		vx = inner_product(v, b, m);
		for (i = j; i < m; i++) {
			b[i] = b[i] - 2.0*(vx / vv)*v[i];
		}
		/*fprintf(stderr, " A[]=\n");
		print_mxn_mtx(A, m, n);
		fprintf(stderr, " b[]=\n");
		print_vec(b, m);*/
	}
	for (i = 0; i < n; i++) {
		b_afterQR[i] = b[i];
		for (j = 0; j < n; j++) {
			A_square[i][j] = A[i][j];
		}
	}
}


/*---------------------------------------------------
 * Procedure to solve a linear system by using QR-
 * decompsotion.
 * Algm:
 *    1. convert Ax=b into an upper triangular system.
 *    2. Solve the upper trianglular system by using 
 *       backward substitution.
 */
void QR_solver(double **A, double *x, double *b, int n)
{
	// Reflect A into an upper triangular matrix.
	QR_reflect(A, b, n);
    // Solve the upper triangular system by using backward
	// substitution.
    back_substitute(A, x, b, n);
}

void QR_mxn_solver(double **A, double *x, double *b, int m, int n)
{
	double **A_square;
	A_square = alloc_mtx(n);
	double *b_afterQR;
	b_afterQR = alloc_vec(n);

	// Reflect A into an upper triangular matrix.
	QR_mxn_reflect(A, b, m, n, A_square, b_afterQR);
	fprintf(stderr, "A[][]=\n");
	print_mtx(A_square, n);
	fprintf(stderr, "b[][]=\n");
	print_vec(b_afterQR, n);

	// Solve the upper triangular system by using backward
	// substitution.
	back_substitute(A_square, x, b_afterQR, n);
}

