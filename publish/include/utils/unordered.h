/*
 * unordered.h
 *
 *  Created on: 2015年10月10日
 *      Author: Administrator
 */

#ifndef _UNORDERED_H__
#define _UNORDERED_H__

#ifdef _USE_CXX11_

#include <unordered_map>
#include <unordered_set>

namespace QCOOL
{
using std::unordered_map;
using std::unordered_set;
}

#else//not def _USE_CXX11_;
//using boost;

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

namespace QCOOL
{
using boost::unordered_map;
using boost::unordered_set;
}

#endif

#endif //_UNORDERED_H__