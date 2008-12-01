/*
 * AnRoot.h
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#ifndef ANROOT_H_
#define ANROOT_H_
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QStringList>

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

class AnRoot : public AnCanObject {
public:
	AnRoot(AnCanObject *parent = 0);
 	virtual ~AnRoot();

	virtual void sync(int level = 0);
	virtual void reset();
	virtual void config();

	enum {
		TASK_SYNC   = 1,
		TASK_RESET  = 2,
		TASK_CONFIG = 3
	};

	inline QList<AnBoard*> list() const { return m_list[0] + m_list[1]; }
	inline int count() const { return m_list[0].count() + m_list[1].count(); }
	virtual AnCanObject* at(int i) { return (i > 0) ? m_cannet[i-1] : (AnCanObject*)this; }

	AnAgent* agent(int i) const { return m_agents[i]; }
	AnAgent* agentById(int i) const { return m_agents[ m_devid_list[i] ]; }

//  Own functions
	void setMode(int i);
	QStringList modeList() const;

	AnCanObject      *find(const AnAddress &lad);
	QList<AnAddress>  expand(const AnAddress &lad);
//	QList<AnAddress>  expand(AnAddress lad) { AnAddress &ad = lad; return expand(ad); }

	int nAgents()  const { return m_devid_list.count(); }
	int nDevices() const { return m_devid_list.count(); }
	bool isRunning() const;
	void stop() const;
	void terminate() const;
	void wait() const;

// configuration types
	enum {
		CT_TCPU_PLDREG02  = 11,
		CT_TDIG_THRESHOLD = 21,
		CT_TDC_CONFIG     = 31,
		CT_TDC_MASK       = 32
	};
	

private:
	struct mode {
		int       id;
		QString   name;
		QString   description;
	};

	QList<int>               m_devid_list;
	QList<AnBoard*>          m_list[2];
	QMap<int, AnAgent*>      m_agents;
	QMap<int, AnTdcConfig*>  m_tcnfs;
	AnCanNet                *m_cannet[2];
	QSqlDatabase             m_db;
	int                      m_mode;
	QList<mode>              m_mode_list;

	void readTdcConfig();
	void readModeList();
};
#endif /* ANROOT_H_ */
