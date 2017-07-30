/*******************************************************************************
* Deep Winkelman
*
* Copyright (c) 2017 by Daniel Winkelman <dwinkelman3@gmail.com>
* 27 April 2017
*
* Custom errors for the application
*/

#ifndef DEEP_WINKELMAN_ERRORS
#define DEEP_WINKELMAN_ERRORS

#include <exception>
#include <string>

class DeepWinkelmanException : public std::exception {
protected:
	char * message;

public:
	DeepWinkelmanException(char * message) {
		this->message = message;
	}

protected:
	virtual const char * what() const throw()
	{
		return message;
	}
};

#endif