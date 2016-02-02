#include <gsl/gsl_statistics.h>
#include <gsl/gsl_multimin.h>
#include <iostream>
#include <math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_multimin.h>
#include <vector>

#define PI 3.14159265

using namespace std;


double student_t_dist(double x, double ddof) {

	//double ddof = (double) dof;
	double val = 0.0;
	val = pow(1.0+(x*x/ddof),(-ddof-1.0)/2.0)*gsl_sf_gamma((ddof + 1.0) / 2.0) / (sqrt(PI * ddof) * gsl_sf_gamma(ddof/2.0));

	return val;
}

double t_dist_likelihood(double ddof, vector<double> jumps) {
	double val = 0.0;

	for(int i = 0; i < jumps.size(); i++) {
		val += student_t_dist(jumps[i], ddof);
	}

	return val;
}

void compare() {

	cout << gsl_ran_tdist_pdf(1.0,1) << endl;
	cout << student_t_dist(1.0,1) << endl;
}


void minimizer_test() {

	size_t iter = 0;
	int status;

	const gsl_multimin_fdfminimizer_type *T;
	gsl_multimin_fdfminimizer *min;

	min = 


}

int main() {

	

	return 0;
}

/*

To do: 
-write max likelihood est for gsl student t dist










*/