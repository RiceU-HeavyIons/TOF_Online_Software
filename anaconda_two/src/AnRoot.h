/*
 * $Id$
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#ifndef ANROOT_H_
#define ANROOT_H_
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QSocketNotifier>
#include <QtCore/QMutex>

#include <QtSql/QSqlDatabase>

#include "AnAgent.h"
#include "AnCanObject.h"
#include "AnCanNet.h"
#include "AnThub.h"
#include "AnTcpu.h"
#include "AnTdig.h"
#include "AnTdcConfig.h"
#include "AnRdMsg.h"
#include "AnLog.h"

#define DB_PATH_ENV         "ANA_DB_PATH"           // absolute path or relative to working dir
#define DB_PATH_DEFAULT     "db/configurations.db"  // relative to binary image
#ifdef MTD
#define LOCK_FILENAME       "/tmp/AnacondaIIMTD.lock"  // lock file to prevent multiple instances of application
#else
#define LOCK_FILENAME       "/tmp/AnacondaII.lock"  // lock file to prevent multiple instances of application
#endif

class AnMaster;
class AnEpicsLogger;

class AnRoot : public AnCanObject {
	Q_OBJECT

public:
	AnRoot(AnCanObject *parent = 0);
 	virtual ~AnRoot();

	// static members
	static int TCAN_AUTOREPAIR;

	static int autorepair() { return TCAN_AUTOREPAIR; }
	static int setAutorepair(int d) { return (TCAN_AUTOREPAIR = d); }
	
	static void handler(int sigNum);  // Signal handler

 // inherited from AnCanObject
 	virtual QString dump() const;

	virtual void init  (int level);
	virtual void config(int level);
	virtual void reset (int level);
	virtual void qreset(int level);
	virtual void sync  (int level);


	virtual AnCanObject* at(int i) { return (i > 0) ? &m_lnet[i] : (AnCanObject*)this; }
	virtual AnCanObject* hat(int i) { return &m_hnet[m_devid_map[i]]; }

	enum {
		TASK_INIT         = 0x01,
		TASK_CONFIG       = 0x02,
		TASK_RESET        = 0x04,
		TASK_QRESET       = 0x08,
		TASK_SYNC         = 0x10,
		TASK_RELINK       = 0x20,
		TASK_TCPURECOVERY = 0x40,
		TASK_THUBRECOVERY = 0x80
	};

//  Own functions

	QList<AnBoard*> list() { return (m_lnet[1].list() + m_lnet[2].list()); }
	int count() { return (m_lnet[1].count() + m_lnet[2].count()); }

	int nAgents()  const { return m_agents.count(); }
	QMap<int, AnAgent*> agents() { return m_agents; }
	AnAgent* agent(int i) const { return m_agents[ m_devid_map[i] ]; }
	AnAgent* agentById(int i) const { return m_agents[i]; }

	void setMode(int i);
	int modeIdx() const { return m_mode_idx; }
	QStringList modeList() const;

	void doUserCmd(int i);

	void relink (int level, const QList<AnBoard*>& blist);	
	void init   (int level, const QList<AnBoard*>& blist);	
	void config (int level, const QList<AnBoard*>& blist);
	void reset  (int level, const QList<AnBoard*>& blist);
	void qreset (int level, const QList<AnBoard*>& blist);
	void sync   (int level, const QList<AnBoard*>& blist);
	void tcpurecovery(int level, const QList<AnBoard*>& blist);
	void thubrecovery(int level, const QList<AnBoard*>& blist);
	
	int status();

	AnThub *thubByDevid(int i) const { return m_devid_thub_map[i]; }

	AnCanObject      *find(const AnAddress &lad);
	QList<AnBoard*>   find(const QList<AnAddress>& alist);

	QList<AnAddress>  expand(const AnAddress &lad);
	AnCanObject      *hfind(const AnAddress &had);

	int nDevices() const { return m_hnet.count(); }
	quint8 devidByIndex(int i) const { return m_hnet.values().at(i).devid(); };
	QStringList deviceNames() const;
	QString deviceNameByDevid(int d) const { return m_hnet[m_devid_map[d]].name(); }
	int deviceIdFromDevid(int d) { return m_devid_map[d]; }

	bool isRunning() const;
	void stop() const;
	void terminate() const;
	void wait() const;

	void startAutoSync();
	void stopAutoSync();

	void enableWatch();
	void enableWatch(int id);
	void disableWatch();
	void disableWatch(int id);
	void watchStatus();

	void emit_updated()  { emit updated(); }
	void emit_finished() { emit finished(); }
	
	void log(QString str);
	void tlog(QString str, int type=0, int tray=0, int board=0, double val = 0.0);	
	//JS: temporarily:
	bool isDoingRecovery() const {return bDoingRecovery;}
	void doingRecovery(bool val) {bDoingRecovery = val;}

signals:
	void updated(AnBoard*);
	void updated();

	void aboutStart();
	void finished();

public slots:
	void autosync();
	void watcher(int sock);
	void agentFinished(int id);
	void received(AnRdMsg rmsg);

private:
	struct mode {
		int       id;
		QString   name;
		QString   description;
	};

	QMap<int, AnThub*>      m_devid_thub_map;

	QMap<int, int>          m_devid_map;
	QMap<int, AnCanNet>     m_hnet;
	QMap<int, AnCanNet>     m_lnet;

	AnMaster               *m_master;
	AnEpicsLogger          *m_epicsLogger;
	QMap<int, AnAgent*>     m_agents;
	QMap<int, AnAgent*>     m_socket_agent_map;
	QMap<int, AnAgent*>     m_devid_agent_map;

	QMap<int, AnTdcConfig*> m_tcnfs;
	QSqlDatabase            m_db;
	int                     m_mode;
	int                     m_mode_idx;
	QList<mode>             m_mode_list;

	AnLog                  *m_log;
	AnLog                  *m_tlog;

	int                     m_numThub, m_numTcpu;

	// AutoSync timer and coursor
	QTimer                         *m_timer;
	QList<AnBoard*>::const_iterator m_autoSyncIter;
	QList<AnBoard*>                 m_autoSyncList;

	QMap<int, QSocketNotifier*>  m_watch;

	void readTdcConfig();
	void readModeList();

	void initAutoSync();
	
	QMutex mtex_log;
	QMutex mtex_tlog;
	QMutex mtex_watch;

	int lockFileFd;

	//JS: temporarily, to prevent multiple recovery actions:
	bool bDoingRecovery;
};
#endif /* ANROOT_H_ */
