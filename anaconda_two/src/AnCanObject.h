/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANCANOBJECT_H_
#define ANCANOBJECT_H_
#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QDebug>

#include "AnAddress.h"

class AnCanObject;

class AnCanObject : public QObject {
public:
	AnCanObject(AnCanObject *parent = 0);
	AnCanObject(const AnCanObject &rhs);
	AnCanObject(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);

	AnAddress lAddress() const { return m_laddr; }
	AnAddress hAddress() const { return m_haddr; }

	bool active() const { return m_active; }
	virtual bool setActive(bool act) { return (m_active = (act & m_installed)); }
	
	bool installed() const { return m_installed; }
	virtual bool setInstalled(bool inst) { return (m_installed = inst); }

	virtual QString name() const;
	QString setName(const QString& name);

	virtual QString dump() const = 0;

	virtual void init  (int level) = 0;
	virtual void config(int level) = 0;
	virtual void reset (int level) = 0;
	virtual void sync  (int level) = 0;

	virtual AnCanObject *at(int i) = 0;
	virtual AnCanObject *hat(int i) = 0;

	AnCanObject *root();

	QTime synced() const { return m_synced; }
	QTime setSynced(QTime t = QTime::currentTime()) { return (m_synced = t); }

	AnAddress laddr() const { return m_laddr; }
	AnAddress& laddr() { return m_laddr; }
	AnAddress haddr() const { return m_haddr; }
	AnAddress& haddr() { return m_haddr; }
	
	int commError() const { return m_comm_err; }
	int setCommError(int ce) { return (m_comm_err = ce); }
	int incCommError() { return ++m_comm_err; }
	int decCommError() { return (m_comm_err > 0) ? --m_comm_err : 0; }
	void clearCommError() { m_comm_err = 0; }

private:
	bool            m_active;
	bool            m_installed;

	QTime           m_synced;
	AnAddress       m_laddr;
	AnAddress       m_haddr;

	QString         m_name;
	
	int             m_comm_err;

};

#endif /* ANCANOBJECT_H_ */
