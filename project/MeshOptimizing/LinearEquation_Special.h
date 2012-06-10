#pragma once

#include <vector>

class LinearEquation_Special
{
public:
	LinearEquation_Special(void);
	~LinearEquation_Special(void);

	static bool solve( std::vector<std::vector<float> > matrix, std::vector<float> value, std::vector<float>& res );
};

