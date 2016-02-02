#include <boost/math/distributions/normal.hpp>
#include <iostream>

using namespace std;
using namespace boost::math;

int main(){
	normal_distribution<> myNormal(1.0, 10.0);

	normal_distribution<> otherNormal(3.0,5.0);




	cout << "Mean: " << myNormal.mean() << ", standard deviation: " << myNormal.standard_deviation() << endl;
	 
	// Distributional properties
	double x = 10.25;
	 
	cout << "pdf: " << pdf(myNormal, x) << endl;
	cout << "cdf: " << cdf(myNormal, x) << endl;
	 
	


	return 0;


}