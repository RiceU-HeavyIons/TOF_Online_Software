/*
 * $Id$
 *
 *  Created on: Jan 6, 2009
 *      Author: koheik
 */
#ifndef AnExceptions_H_
#define AnExceptions_H_ 1
#include <QtCore/QString>

class AnException {};

class AnExCanError : public AnException
{
public:
	AnExCanError(int status) { m_status = status; }
	int status() const { return m_status; }
private:
	int m_status;
};

class AnExCanTimeOut : public AnExCanError
{
public:
	AnExCanTimeOut(int status) : AnExCanError(status) { }
};

#endif // AnExceptions_H_