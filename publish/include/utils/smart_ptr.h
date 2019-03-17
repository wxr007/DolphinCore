/*
 * smart_ptr.h
 *
 *  Created on: 2015年10月10日
 *      Author: Administrator
 */

#ifndef _SMART_PTR_H__
#define _SMART_PTR_H__

#ifdef _USE_CXX11_

#include <memory>

namespace QCOOL
{
using std::shared_ptr;
using std::weak_ptr;
using std::enable_shared_from_this;
}

#else//not def _USE_CXX11_;
//using boost;
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
namespace QCOOL
{
using boost::shared_ptr;
using boost::weak_ptr;
using boost::enable_shared_from_this;
}

#endif

#endif //_SMART_PTR_H__
