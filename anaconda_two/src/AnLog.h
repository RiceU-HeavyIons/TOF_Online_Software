/*
 * $Id$
 *
 *  Created on: Mar 18, 2009
 *      Author: koheik
 */

#ifndef AnLog_H_
#define AnLog_H_

#include <QtCore/QObject>
#include <QtCore/QDebug>

class AnLog
{
public:
	AnLog(QString path);
	~AnLog();
	void log(QString str);

private:
	FILE *m_fp;
	
};
#endif // AnLog_H_
