#ifndef CLOCK_H
#define CLOCK_H
#include <string>
class Clock
{
private:
  int hour{};
  int minute{};
  int second{};

  void set_time(int, int, int);
  void increment_time();
  void decrement_time();

public:
  Clock();
  Clock(int h, int m, int s);

  std::string to_string(bool format_24h = true,char fill = '0') const;

  int get_hour() const;
  int get_minute() const;
  int get_second() const;
  int get_all_time_as_second() const;

  Clock operator=(Clock const & rhs) ;
  Clock operator+(int rhs) const;
  Clock operator-(int rhs) const;
  // Komplettering: Se standard deklaration av prefix och postfix
  // operatorerna på cpppreference.
  Clock& operator++();
  Clock operator++(int);
  Clock& operator--();
  Clock operator--(int);

  bool operator<(Clock const &rhs) const;
  bool operator>(Clock const &rhs) const;
  bool operator==(Clock const &rhs) const;
  bool operator<=(Clock const &rhs) const;
  bool operator>=(Clock const &rhs) const;
  bool operator!=(Clock const &rhs) const;
};

#endif
