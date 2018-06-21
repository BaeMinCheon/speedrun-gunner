#pragma once

#ifndef _GAMEERROR_H
#define _GAMEERROR_H
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace NS_GameError
{
	const int FATAL_ERROR = -1;
	const int WARNING = 1;
}

class GameError : public std::exception
{
public:
	GameError() throw()
		: m_errorCode(NS_GameError::FATAL_ERROR), m_message("Undefined Error")
	{
		;
	}
	GameError(const GameError& e) throw()
		: std::exception(e), m_errorCode(e.m_errorCode), m_message(e.m_message)
	{
		;
	}
	GameError(const int c, const std::string& s) throw()
		: m_errorCode(c), m_message(s)
	{
		;
	}
	virtual ~GameError() throw()
	{
		;
	}

	GameError& operator=(const GameError& e) throw()
	{
		std::exception::operator=(e);
		m_errorCode = e.m_errorCode;
		m_message = e.m_message;
	}

	virtual const char* what() const throw()
	{
		return m_message.c_str();
	}
	const char* getMessage() const throw()
	{
		return m_message.c_str();
	}
	int getErrorCode() const throw()
	{
		return m_errorCode;
	}

private:
	int m_errorCode;
	std::string m_message;
};

#endif