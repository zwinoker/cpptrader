#include <fftw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <math.h>

using namespace std;

int main() {

	int N = 50;

	fftw_complex *in, *out;
	fftw_plan p;
	
	in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);

	srand((unsigned)time((long *)0)); 


	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_MEASURE);

	/*Make input vector all random values*/
	for (int i; i < N ; i++) {
		in[i][0] = rand()/(double)RAND_MAX;
		//in[i][0] = 1.0;
		in[i][1] = 0.0;
	}
	
	fftw_execute(p); /* repeat as needed */

	

	for (int i; i < N ; i++) {
		cout << sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]) << endl;
	}
	
	fftw_destroy_plan(p);
	fftw_free(in); 
	fftw_free(out);


	return 0;

}