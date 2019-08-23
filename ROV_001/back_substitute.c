/*----------------------------------------------------------------
 * Procedure to solve a lower triangular system
 *    L*x = b.
 */
void back_substitute(double **U, double *x, double *b, int n)
{
	int   i,  j;

	for(i=n-1;i>=0;i--){
		x[i] = b[i]/U[i][i];
		for(j=i-1;j>=0;j--)
			b[j] = b[j] - U[j][i]*x[i];
	}
}	   
