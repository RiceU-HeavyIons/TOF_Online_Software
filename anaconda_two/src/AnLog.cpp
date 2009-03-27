/*
 * $Id$
 *
 *  Created on: Mar 18, 2009
 *      Author: koheik
 */
#include <QtCore/QDateTime>
#include "AnLog.h"

AnLog::AnLog(QString path)
{
	m_fp = fopen(path.toStdString().c_str(), "a+");
}

AnLog::~AnLog()
{
	fclose(m_fp);
}

void AnLog::log(QString str)
{
	QDateTime now = QDateTime::currentDateTime();
	fprintf(m_fp, "[%s] ", now.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
	fprintf(m_fp, "%s", str.toStdString().c_str());
	fprintf(m_fp, "\n");
	fflush(m_fp);
}