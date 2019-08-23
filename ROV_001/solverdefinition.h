/*---------------------------------------------------
 * Procedure to solve a linear system by using QR-
 * decompsotion.
 * Algm:
 *    1. convert Ax=b into an upper triangular system.
 *    2. Solve the upper trianglular system by using 
 *       backward substitution.
 */
void QR_solver(double **A, double *x, double *b, int n);

void QR_mxn_solver(double **A, double *x, double *b, int m, int n);

/**************************************************************
 * Perform forward elimination for a linear system Ax=b.
 * Partial pivoting is adopted.
 */
void gauss_elm(double **A, double *b, int n);

/*----------------------------------------------------------------
 * Procedure to solve a lower triangular system
 *    L*x = b.
 */
void back_substitute(double **U, double *x, double *b, int n);
