#include "TimeInterval.h"


void TimeInterval::AddNewTime()
{
	timeval new_time;
	gettimeofday(&new_time, NULL);
	if (time_vec_.size() > 1)
	{
		time_vec_.erase(time_vec_.begin());
	}

	time_vec_.insert(time_vec_.begin(), new_time);
}

bool TimeInterval::IsTimeFree(const float& second)
{
	bool b = false;
	if (time_vec_.size() < 2)
	{
		return true;
	}

	std::vector<timeval>::iterator it = time_vec_.begin();

	timeval starttime = *it;
	timeval endtime = *(it+1);

	double  interval =  starttime.tv_sec - endtime.tv_sec;
	float interval_sec = interval;
	//log("interval = %f ", interval_sec);
	if ( interval_sec > second)
	{
		b= true;
		std::vector<timeval> copy_vec = time_vec_;
		time_vec_.clear();
		time_vec_.push_back(*copy_vec.begin());
	}

	return b;
}

float TimeInterval::GetInterval()
{
	std::vector<timeval>::iterator it = time_vec_.begin();
	if (time_vec_.size()<2)
	{
		return 0;
	}
	timeval starttime = *it;
	timeval endtime = *(it+1);

	double  interval = starttime.tv_sec - endtime.tv_sec;
	float interval_sec = interval;
	return interval_sec;
}

void TimeInterval::ClearTime()
{
	time_vec_.clear();
}

