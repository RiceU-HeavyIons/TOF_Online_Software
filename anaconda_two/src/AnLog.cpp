/*
 * $Id$
 *
 *  Created on: Mar 18, 2009
 *      Author: koheik
 */
#include <QtCore/QDateTime>
#include <sys/types.h>
#include <sys/stat.h>
#include "AnLog.h"

AnLog::AnLog(QString path)
{
  int cm;
  m_fp = fopen(path.toStdString().c_str(), "a+");
  if (m_fp == (FILE *)NULL) perror("Log File");
  else
    cm = chmod(path.toStdString().c_str(), 00666);
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
