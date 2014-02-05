#include "MyIostream.hpp"
#include <unistd.h>
#include <sstream>

namespace usu
{
	//
	// Yes, this IS a global object, so deal with it!  This is the object we want to maintain
	// the streaming output state.
	MyIostream cout;

	/////////////////////////////////////////////////////////////////
	//
	// This is the overloaded streaming output operator.  It uses the Linux
	// POSIX system API to get the output to the console.
	//
	MyIostream& MyIostream::operator<<(const std::string& string)
	{
		write(1,string.c_str(),string.size());
		return *this;
	}

	MyIostream& MyIostream::operator<<(float number)
        {
		std::stringstream out;
                out << number;
                write(1,out.str().c_str(),out.str().size());
                return *this;
        }

}
