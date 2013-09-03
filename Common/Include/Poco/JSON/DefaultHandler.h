//
// DefaultHandler.h
//
// $Id$
//
// Library: JSON
// Package: JSON
// Module:  DefaultHandler
//
// Definition of the DefaultHandler class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef JSON_DefaultHandler_INCLUDED
#define JSON_DefaultHandler_INCLUDED


#include "Poco/JSON/Handler.h"
#include <stack>


namespace Poco {
namespace JSON {


class  DefaultHandler : public Handler
	/// Provides a default handler for the JSON parser.
	/// This handler will build up an object or array based
	/// on the handlers called by the parser.
{
public:

	DefaultHandler();
		/// Default Constructor

	virtual ~DefaultHandler();
		/// Destructor

	void startObject();
		/// Handles a {, meaning a new object will be read

	void endObject();
		/// Handles a }, meaning the object is read

	void startArray();
		/// Handles a [, meaning a new array will be read

	void endArray();
		/// Handles a ], meaning the array is read

	void key(const std::string& k);
		/// A key is read

	Dynamic::Var result() const;
		/// Returns the result of the parser. Which is an object or an array.

	virtual void value(int v);
		/// An integer value is read

#if defined(POCO_HAVE_INT64)
	virtual void value(Int64 v);
		/// A 64-bit integer value is read
#endif

	virtual void value(const std::string& s);
		/// A string value is read.

	virtual void value(double d);
		/// A double value is read

	virtual void value(bool b);
		/// A boolean value is read

	virtual void null();
		/// A null value is read

private:
	void setValue(const Poco::Dynamic::Var& value);

	std::stack<Dynamic::Var> _stack;
	std::string              _key;
	Dynamic::Var             _result;
};


inline Dynamic::Var DefaultHandler::result() const
{
	return _result;
}


inline void DefaultHandler::value(int v)
{
	setValue(v);
}


#if defined(POCO_HAVE_INT64)
inline void DefaultHandler::value(Int64 v)
{
	setValue(v);
}
#endif


inline void DefaultHandler::value(const std::string& s)
{
	setValue(s);
}


inline void DefaultHandler::value(double d)
{
	setValue(d);
}


inline void DefaultHandler::value(bool b)
{
	setValue(b);
}


inline void DefaultHandler::null()
{
	Poco::Dynamic::Var empty;
	setValue(empty);
}


}} // namespace Poco::JSON


#endif // JSON_DefaultHandler_INCLUDED
