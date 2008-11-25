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

#include "AnCanObject.h"
#include "AnCanNet.h"
#include "AnSock.h"
#include "AnThub.h"
#include "AnTcpu.h"
#include "AnTdig.h"

class AnRoot : public AnCanObject {
public:
	AnRoot(AnCanObject *parent = 0);
 	virtual ~AnRoot();

	virtual void sync(int level = 0);
	virtual void reset();


	inline QList<AnBoard*> list() const { return m_list[0] + m_list[1]; }
	inline int count() const { return m_list[0].count() + m_list[1].count(); }
	inline AnSock* sock(int i) const { return m_socks[i]; }
	virtual AnCanObject* at(int i) { return (i > 0) ? m_cannet[i-1] : (AnCanObject*)this; }
	

//  Own functions
	void setMode(int i);
	QStringList modeList() const;

	AnCanObject      *find(AnAddress &lad);
	QList<AnAddress>  expand(AnAddress &lad);

// configuration types
	enum {
		CT_TCPU_PLDREG02  = 11,
		CT_TDIG_THRESHOLD = 21,
		CT_TDC_CONFIG     = 31,
		CT_TDC_MASK       = 32
	};

private:
	QList<int>          m_devid_list;
	QList<AnBoard*>     m_list[2];
	QMap<int, AnSock*>  m_socks;
	
	AnCanNet           *m_cannet[2];

	struct mode {
		int       id;
		QString   name;
		QString   description;
	};

	QSqlDatabase        m_db;
	int                 m_mode;
	QList<mode>         m_mode_list;

	void readModeList();
};

#endif /* ANROOT_H_ */
