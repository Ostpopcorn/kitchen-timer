#include "clock.h"
#include <iostream>
#include <sstream>
#include <ios>
#include "iomanip"
using namespace std;

void add_time(int &h, int &m, int &s);

Clock::Clock() {}

Clock::Clock(int h, int m, int s) : hour{h}, minute{m}, second{s}
{
  //set_time(h, m, s);
}

// Komplettering: Vi vill, som skapare av en modul, aldrig skriva ut
// error meddelande direkt till terminalen.
void Clock::set_time(int h, int m, int s)
{
  //: hour{h}, minute{m}, second{s}
  if (h < 0 || h > 23)
  {
    cout << "Felaktig inmatning! Out of bounds, timma" << endl;
  }
  if (m < 0 || m > 59)
  {
    cout << "Felaktig inmatning! Out of bounds, minut" << endl;
  }
  if (s < 0 || s > 59)
  {
    cout << "Felaktig inmatning! Out of bounds, sekund" << endl;
  }
  hour = h;
  minute = m;
  second = s;
}

string Clock::to_string(bool format_24h,char fill) const
{
  ostringstream o_string_stream{};
  bool has_larger{false};
  if (format_24h)
  {
    if (hour>0){
      o_string_stream << setw(2) << setfill(fill) << hour;
      o_string_stream << ':';
      has_larger = true;
      fill = '0';
    }else{
      o_string_stream << setw(2) << setfill(fill) << ' ';
      o_string_stream << ' ';
    }

  }
  else
  {
    if (hour > 12)
    {
      o_string_stream << setw(2) << setfill(fill) << (hour - 12);
    }
    else if (hour == 0)
    {
      o_string_stream << setw(2) << setfill(fill) << 12;
    }
    else
    {
      o_string_stream << setw(2) << setfill(fill) << 12;
    }
  }

  if(minute!=0){
    o_string_stream << setw(2) << setfill(fill) << minute;
    has_larger = true;
    fill = '0';
      o_string_stream << ':';
  }else
  {
    if ( has_larger){
      o_string_stream << setw(2) << setfill(fill) << '0';
      o_string_stream << ':';
    }else{
      o_string_stream << setw(2) << setfill(fill) << ' ';
      o_string_stream << ' ';
    }
  }
  if(second!=0){
    o_string_stream << setw(2) << setfill(fill) << second;
    has_larger = true;
    fill = '0';
  }else
  {
    if ( has_larger){
      o_string_stream << setw(2) << setfill(fill) << '0';
    }else{
      o_string_stream << setw(2) << setfill(fill) << '0';
    }
  }
  
  
  /*AM/PM eller inget*/
  if (!format_24h)
  {
    if (hour < 12)
    {
      o_string_stream << " am";
    }
    else
    {
      o_string_stream << " pm";
    }
  }
  string time_string{o_string_stream.str()};
  return time_string;
}

void Clock::increment_time()
{
  second += 1;
  add_time(hour, minute, second);
}
void Clock::decrement_time()
{
  second -= 1;
  add_time(hour, minute, second);
}

void add_time(int &h, int &m, int &s)
{
  int temp{};
  temp = s / 60;
  m += temp;
  s = s - (60 * temp);
  if (s < 0)
  {
    m -= 1;
    s += 60;
  }

  temp = m / 60;
  h += temp;
  m = m - (60 * temp);
  if (m < 0)
  {
    h -= 1;
    m += 60;
  }

  h %= 24;
  if (h < 0)
  {
    h += 24;
  }
}

Clock Clock::operator=(Clock const & rhs)
{
  second = rhs.get_second();
  minute = rhs.get_minute();
  hour = rhs.get_hour();
  return *this;
}

Clock Clock::operator+(int rhs) const
{
  int h{this->get_hour()};
  int m{this->get_minute()};
  int s{this->get_second()};
  s += rhs;

  add_time(h, m, s);

  Clock output{h, m, s};
  return output;
}

Clock Clock::operator-(int rhs) const
{
  Clock return_clock = *this +(-rhs);
  return return_clock;
}

Clock Clock::operator++(int)
{
  Clock return_clock{};
  return_clock = *this;
  this->increment_time();
  return return_clock;
}
Clock& Clock::operator++()
{

  Clock return_clock{};
  this->increment_time();
  return *this;
}
Clock Clock::operator--(int)
{
  Clock return_clock{};
  return_clock = *this;
  this->decrement_time();
  return return_clock;
}
Clock& Clock::operator--()
{

  Clock return_clock{};
  this->decrement_time();
  return *this;
}

int Clock::get_hour() const
{
  return hour;
}
int Clock::get_minute() const
{
  return minute;
}
int Clock::get_second() const
{
  return second;
}
int Clock::get_all_time_as_second() const{
  return this->second + (60*this->minute) + (60*60*this->hour);
}

bool Clock::operator<(Clock const &rhs) const
{
  // Jämför först timmar
  if (this->get_hour() < rhs.get_hour())
  {
    return true;
  }
  else if (this->get_hour() > rhs.get_hour())
  {
    return false;
  }

  // Om det är samma timme så undersöks minuter
  if (this->get_minute() < rhs.get_minute())
  {
    return true;
  }
  else if (this->get_minute() > rhs.get_minute())
  {
    return false;
  }

  // Om det är samma minut så undersöks sekund
  if (this->get_second() < rhs.get_second())
  {
    return true;
  }
  return false;
}

bool Clock::operator>(Clock const &rhs)const
{
// om a>b = false
// om a+1>b = false
// -> a <b
// a = this
// b = rhs
  if(!(*this < rhs) && !((*this-1)< rhs)){
    return true;
  }
  return false;
}

bool Clock::operator==(Clock const &rhs)const
{
  return !( (*this < rhs) || (*this > rhs) );
}

bool Clock::operator<=(Clock const &rhs)const
{
  return !(*this > rhs);
}
bool Clock::operator>=(Clock const &rhs)const
{
  return !(*this < rhs);
}
bool Clock::operator!=(Clock const &rhs)const
{
  return !(*this == rhs);
}
