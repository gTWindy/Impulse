#pragma once
#include <string>
class Time
{
public:
   Time() = default;
   Time(int H, int M) :H(H), M(M) {}
   const Time& operator = (std::string time_);
   bool operator <(const Time& time);
   bool operator <=(const Time& time);
   Time operator -(const Time& time);
   void operator +=(const Time& time);

   int getHour() const;
   std::string str() const;

private:
   int H = 0;
   int M = 0;
};