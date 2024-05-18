#include "Time.h"
#include <stdexcept>

bool isNumber(const std::string& str_);

const Time& Time::operator =(std::string time_)
{
	if (time_.size() != 5 || time_[2] != ':')
		throw std::runtime_error("no time");

	std::string sH = time_.substr(0, 2);
	std::string sM = time_.substr(3, 4);



	if (sH[0] == '0')
		sH = sH[1];

	if (!isNumber(sH))
		throw std::runtime_error("no time");
	H = std::atoi(sH.c_str());

	if (sM[0] == '0')
		sM = sM[1];

	if (!isNumber(sM))
		throw std::runtime_error("no time");
	M = std::atoi(sM.c_str());

	return *this;
}
bool Time::operator <(const Time& time)
{
	if (H == time.H)
		return M < time.M;
	return H < time.H;
}
bool Time::operator <=(const Time& time)
{
	if (H != time.H)
		return H < time.H;
	return M <= time.M;
}

Time Time::operator -(const Time& time)
{
	int newM = M - time.M;
	int newH = H - time.H;
	if (newM < 0)
	{
		--newH;
		newM += 60;
	}
	return Time(newH, newM);
}

void Time::operator +=(const Time& time)
{
	H += time.H;
	M += time.M;
	if (M >= 60)
	{
		++H;
		M -= 60;
	}
}

std::string Time::str() const
{
	std::string res;
	res += H < 10 ? "0" + std::to_string(H) : std::to_string(H);
	res += ":";
	res += M < 10 ? "0" + std::to_string(M) : std::to_string(M);
	return res;
}

int Time::getHour() const
{
	if (M > 0)
		return H + 1;
	return H;
}