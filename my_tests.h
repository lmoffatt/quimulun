#ifndef MY_TESTS_H
#define MY_TESTS_H
#include <fstream>
#include <functional>
template<class T>
bool test_output_extraction_operators(const T& data,const  std::string& filename )
{
  std::ofstream fo(filename.c_str());
  fo<<data;
  fo.close();
  std::ifstream fi(filename.c_str());
  T data_new;
  fi>>data_new;
  return data==data_new;

}

template<class T, class to_file, class from_file>
bool test_print_read(const to_file& p, const from_file& r, const T& data,const  std::string& filename )
{
  std::ofstream fo(filename.c_str());
  std::invoke(p,fo,data);
  fo.close();
  std::ifstream fi(filename.c_str());
  T data_new;
  std::invoke(r,fi,data_new);
  return data==data_new;

}





#endif // MY_TESTS_H
