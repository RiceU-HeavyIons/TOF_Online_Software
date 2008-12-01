/*
 * AnCanObject.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANCANOBJECT_H_
#define ANCANOBJECT_H_
#include <QtCore/QObject>
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
	bool setActive(bool act) { return (m_active = act); }

	virtual QString name() const;
	virtual QString dump() const { return QString(""); }

	virtual void sync(int level = 0) = 0;
	virtual void reset()  = 0;
	virtual void config() = 0;

	virtual AnCanObject *at(int i) = 0;
	
	AnCanObject *root();

protected:
	AnAddress laddr()    const { return m_laddr; }
	AnAddress haddr()    const { return m_haddr; }

private:
	bool  m_active;
	const AnAddress m_laddr;
	const AnAddress m_haddr;

};

#endif /* ANCANOBJECT_H_ */
