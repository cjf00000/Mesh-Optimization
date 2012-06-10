#include "LinearEquation_Special.h"
#include <cmath>
#include <assert.h>
using namespace std;

LinearEquation_Special::LinearEquation_Special(void)
{
}


LinearEquation_Special::~LinearEquation_Special(void)
{
}

bool LinearEquation_Special::solve( std::vector<std::vector<float> > matrix, std::vector<float> value, std::vector<float>& res )
{
	int n = matrix.size() ;

	for ( int i=0; i<n; ++i )
	{
		int mpj = i;
		for ( int j=i+1; j<n; ++j )
			if ( fabs(matrix[j][i]) > fabs(matrix[j][mpj]) )
				mpj = j;

		swap( matrix[i], matrix[mpj] );
		swap( value[i], value[mpj] );
		float base = matrix[i][i];
		if (fabs(base)<1e-6)
			return false;
		for ( int j=i; j<n; ++j ) matrix[i][j] /= base;
		value[i] /= base;

		for ( int j=0; j<n; ++j )
			if ( j!=i )
			{
				float rate = matrix[j][i] / matrix[i][i];
				for ( int k=0; k<n; ++k )
					matrix[j][k] -= matrix[i][k] * rate;

				value[j] -= value[i] * rate;
			}
	}
	res = value;
	return true;
}