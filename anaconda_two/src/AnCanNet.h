/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#ifndef AnCanNet_H_
#define AnCanNet_H_
#include <QtCore/QObject>
#include <QtCore/QDebug>

#include "AnCanObject.h"
#include "AnBoard.h"

class AnCanNet;

class AnCanNet : public AnCanObject {
public:
	AnCanNet(AnCanObject *parent = 0);
	AnCanNet(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);
	AnCanNet(const AnCanNet &rhs);

	AnCanNet& operator=(const AnCanNet&);

//  Inherited from AnCanObject
	virtual AnCanObject *at(int i) { return (i > 0) ? m_map[i] : static_cast<AnCanObject*>(this); }
	virtual AnCanObject *hat(int i);

	virtual QString dump() const;

	virtual void sync(int level = 0) { /* do nothing */ }
	virtual void reset() { /* do nothing */ }
	virtual void config() { /* do nothing */ }
	virtual void write() { /* do nothing */ }	

//  own functions
	QList<AnBoard*> list() const { return m_map.values(); }
	int count() const { return m_map.count(); }

	AnBoard*& operator[](const int& idx) { return m_map[idx]; }
//	const AnBoard* operator[](const int& idx) const { return m_map[idx]; }
private:
	QMap<int, AnBoard*> m_map;
};

#endif /* AnCanNet_H_ */
