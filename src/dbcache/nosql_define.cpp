#include "dbcache/nosql_define.h"
#include <boost/algorithm/string.hpp>

void StringSplit(std::string& src_str, std::string delimiter, StringArray& out_array)
{
	boost::split(out_array, src_str, boost::is_any_of(delimiter));
}

void StringTrim(std::string& src_str)
{
	boost::trim(src_str);
}
