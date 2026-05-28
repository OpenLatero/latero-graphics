#pragma once

#include <stdio.h>

namespace latero::graphics {

template<class TYPE>
class AverageFilter
{
public:
	AverageFilter() : 
		wndSize_(0)
	{
		Reset();
	};

	~AverageFilter()
	{
		Reset();
	} 

	void Reset()
	{
		if (wndSize_) delete[] hist_;
		sum_= 0;
		n_ = 0;
		i_ = 0;
		first_ = 0;
	}

	// resets the filter (not ideal but simple)
	void SetSize(unsigned int size)
	{
		Reset();
		hist_ = new TYPE[size];
		wndSize_ = size;
		//printf("SetSize(%d): sum_(%f), first_(%d), i_(%d), n_(%d), avg(%f)\n",
		//	size, sum_, first_, i_, n_, Get());
	};
	
	void Insert(TYPE value)
	{
		if (n_ == wndSize_)
		{
			sum_ -= hist_[first_];
			first_ = (first_+1)%wndSize_;
		}
		else
		{
			n_++;
		}

		sum_ += value;
		hist_[i_] = value;
		i_ = (i_+1)%wndSize_;
	

		//printf("Insert(%f): sum_(%f), first_(%d), i_(%d), n_(%d), avg(%f)\n",
		//	(float)value, sum_, first_, i_, n_, Get());
	}

	inline double Get() { return (n_) ? sum_/n_ : 0; }
		
private:
	unsigned int i_; // current pointer location
	unsigned int first_;
	unsigned int n_; // number of items in the list
	TYPE *hist_;
	double sum_;
	unsigned int wndSize_;
};

//static void AverageFilterSelfTest();

} // namespace

