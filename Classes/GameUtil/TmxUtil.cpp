#include "TmxUtil.h"

std::vector<std::string> TmxUtil::SplitToStr( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::vector<int> TmxUtil::SplitToInt( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	split(s, delim, elems);

	std::vector<int> int_elems;
	for (auto it = elems.begin(); it != elems.end(); it++)
	{
		std::string str = (*it);
		int num = atoi(str.c_str());
		int_elems.push_back(num);
	}
	return int_elems;
}

std::vector<float> TmxUtil::SplitToFloat( const std::string &s, char delim )
{
	std::vector<std::string> elems;
	split(s, delim, elems);

	std::vector<float> float_elems;
	for (auto it = elems.begin(); it != elems.end(); it++)
	{
		std::string str = (*it);
		float num = atof(str.c_str());
		float_elems.push_back(num);
	}
	return float_elems;
}

std::vector<std::string> & TmxUtil::split( const std::string &s, char delim, std::vector<std::string> &elems )
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

int TmxUtil::getIntDataByRate( const std::string& str1, const std::string& str2 )
{
	auto dataVec = TmxUtil::SplitToInt(str1,',');			//ID列表
	auto rateVec = TmxUtil::SplitToFloat(str2,',');		//各个ID对应的概率列表
	if (dataVec.empty())
	{
		return 0;
	}
	if (rateVec.empty())
	{
		rateVec.push_back(1.0f);	//如果没填，却有数据，那么默认为1.0
	}
	int index = getRateIndex(rateVec);
	int iData = dataVec.at(index);
	return iData;
}

float TmxUtil::getFloatDataByRate( const std::string& str1, const std::string& str2 )
{
	auto dataVec = TmxUtil::SplitToFloat(str1,',');			//ID列表
	auto rateVec = TmxUtil::SplitToFloat(str2,',');		//各个ID对应的概率列表
	if (dataVec.empty())
	{
		return 0;
	}
	if (rateVec.empty())
	{
		rateVec.push_back(1.0f);	//如果没填，却有数据，那么默认为1.0
	}
	int index = getRateIndex(rateVec);
	float fdata = dataVec.at(index);
	return fdata;
}

int TmxUtil::getRateIndex( std::vector<float> vec )
{
	float rate = rand_0_1();
	int idx = 0;
	float sumRate = 0.0f;
	for (int index = 0; index < vec.size();index++)
	{
		sumRate = sumRate + vec.at(index);
		if (rate <= sumRate)
		{
			break;
		}
		idx += 1; 
	}
	return idx;
}

std::vector<std::string> TmxUtil::getStrVec(const std::string& str)
{
	std::vector<std::string> StrVec = TmxUtil::SplitToStr(str,'#');
	return StrVec;
}

