#ifndef _MYIOSTREAM_HPP_
#define _MYIOSTREAM_HPP_

#include <string>

namespace usu
{
	/////////////////////////////////////////////////////////////////
	//
	// This is a custom class used to replace the std::cout object
	// and overloaded operators.
	//
	class MyIostream
	{
	public:

		MyIostream& operator<<(const std::string& string);
		MyIostream& operator<<(float number);
	};
	
	//
	// This declaration makes the 'cout' object visible to an code that includes this header file
	extern MyIostream cout;
}

#endif
