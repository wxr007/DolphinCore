/*
 * function.h
 *
 *  Created on: 2015年10月10日
 *      Author: Administrator
 */

#ifndef _FUNCTION_H__
#define _FUNCTION_H__

#ifdef _USE_CXX11_

#include <functional>

namespace QCOOL
{
using std::function;
using std::bind;
}
using namespace std::placeholders;

#else//not def _USE_CXX11_;
//using boost;

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace QCOOL
{
using boost::function;
using boost::bind;

}
#endif

#endif //_FUNCTION_H__
