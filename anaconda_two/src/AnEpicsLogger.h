/*
 * $Id$
 *
 */
#ifndef AnEPICS_LOGGER_H_
#define AnEPICS_LOGGER_H_ 1
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include "AnRoot.h"


struct _elog {
  int type;
  int tray;
  int board;
  double val;
};
  
class AnEpicsLogger : public QThread
{
	Q_OBJECT
public:
	AnEpicsLogger(AnRoot *root, QObject *parent = 0);
	virtual ~AnEpicsLogger();

public slots:
	void q_elog(int, int, int, double);

protected:
	virtual void run();

private:
	AnRoot               *m_root;
	QQueue<struct _elog>  m_q;
	QMutex                m_mutex;
	
};
#endif
