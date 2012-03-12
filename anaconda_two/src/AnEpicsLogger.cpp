/*
 * $Id$
 */
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QMutexLocker>
#include "AnEpicsLogger.h"

#ifdef WITH_EPICS
#include <tsDefs.h> 
#include <cadef.h> 
#include <ezca.h>
#endif

const int q_maxSize = 200;

AnEpicsLogger::AnEpicsLogger(AnRoot *root, QObject *parent)
  : QThread(parent)
{
#ifdef WITH_EPICS
  // make ezca timeout a little quicker:
  // seems like default are a retryCount of 599 and
  // a timeout of 0.05 seconds (total of 30 sec timeout)
  ezcaSetRetryCount(2);
#endif
  m_root = root;
}

AnEpicsLogger::~AnEpicsLogger()
{
  // d-tor
}

void AnEpicsLogger::q_elog(int type, int tray, int board, double val)
{
  struct _elog elog;

  elog.type = type;
  elog.tray = tray;
  elog.board = board;
  elog.val = val;

  m_mutex.lock();
  if (m_q.size() < q_maxSize) m_q.enqueue(elog);
  m_mutex.unlock();
}

void AnEpicsLogger::run()
{
  struct _elog elog;
  char tempStr[24];

  m_root->log(QString("AnEpicsLogger::run: begin"));
  forever {
    while(!m_q.isEmpty()) {
      m_mutex.lock();
      elog = m_q.dequeue();
      m_mutex.unlock();
      //printf("AnEpicsLogger: type %d tray %d board %d val %f\n",
      //	     elog.type, elog.tray, elog.board, elog.val);


      if (elog.type > 0) {
	switch (elog.type) {
	case 1:
	  sprintf(tempStr, "TOFTempTray%03dTDIG%d", elog.tray, elog.board);
	  //sprintf(tempStr, "TOFTempTray%03dTDIG%d", 1, 1);
	  break;
	}
      
#ifdef WITH_EPICS
	if(ezcaPut(tempStr, ezcaDouble, 1, &(elog.val)) != EZCA_OK)
	  ezcaPerror("AnEpicsLogger:");
#endif
      }

    }
    
    usleep(200000); // wait 200ms

  }
}
