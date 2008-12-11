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

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "AnAgent.h"
#include "AnCanObject.h"
#include "AnCanNet.h"
#include "AnSock.h"
#include "AnThub.h"
#include "AnTcpu.h"
#include "AnTdig.h"
#include "AnTdcConfig.h"
#include "AnRdMsg.h"

class AnRoot : public AnCanObject {
	Q_OBJECT

public:
	AnRoot(AnCanObject *parent = 0);
 	virtual ~AnRoot();

 // inherited from AnCanObject
	virtual void sync(int level = 0);
	virtual void reset();
	virtual void config();
	virtual void write();

 	virtual QString dump() const;
	virtual AnCanObject* at(int i) { return (i > 0) ? &m_lnet[i] : (AnCanObject*)this; }
	virtual AnCanObject* hat(int i) { return &m_hnet[i]; }

	enum {
		TASK_SYNC   = 0x01,
		TASK_RESET  = 0x02,
		TASK_CONFIG = 0x04,
		TASK_WRITE  = 0x08
	};

	QList<AnBoard*> list() { return (m_lnet[1].list() + m_lnet[2].list()); }
	int count() { return (m_lnet[1].count() + m_lnet[2].count()); }

	AnAgent* agent(int i) const { return m_agents[i]; }
	AnAgent* agentById(int i) const { return m_agents[ m_devid_list[i] ]; }

//  Own functions
	void setMode(int i);
	QStringList modeList() const;
	
	AnThub *thubByDevId(int i) const { return m_devid_thub_map[i]; }

	AnCanObject      *find(const AnAddress &lad);
	QList<AnAddress>  expand(const AnAddress &lad);
	AnCanObject      *hfind(const AnAddress &had);

	int nAgents()  const { return m_devid_list.count(); }
	int nDevices() const { return m_devid_list.count(); }
	bool isRunning() const;
	void stop() const;
	void terminate() const;
	void wait() const;

	void startAutoSync();
	void stopAutoSync();

	void enableWatch();
	void disableWatch();

signals:
	void updated(AnBoard*);
	void updated();

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

	QMap<int, AnCanNet>     m_hnet;
	QMap<int, AnCanNet>     m_lnet;

	QList<int>              m_devid_list;
	QMap<int, AnAgent*>     m_agents;
	QMap<int, AnAgent*>     m_socket_agent_map;
	QMap<int, AnAgent*>     m_devid_agent_map;

	QMap<int, AnTdcConfig*> m_tcnfs;
	QSqlDatabase            m_db;
	int                     m_mode;
	QList<mode>             m_mode_list;

	// AutoSync timer and coursor
	QTimer                 *m_timer;
	int                     m_cur1;
	int                     m_cur2;

	QMap<int, QSocketNotifier*>  m_watch;

	void readTdcConfig();
	void readModeList();

	void initAutoSync();
};
#endif /* ANROOT_H_ */
