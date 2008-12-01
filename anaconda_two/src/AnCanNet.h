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

//  Inherited from AnCanObject
	AnCanObject *at(int i) { return (i > 0) ? m_list->at(i-1) : static_cast<AnCanObject*>(this); }

	virtual void sync(int level = 0) { /* do nothing */ }
	virtual void reset() { /* do nothing */ }
	virtual void config() { /* do nothing */ }

	void setList(QList<AnBoard*> *lst) { m_list = lst; }
private:
	QList<AnBoard*> *m_list;
};

#endif /* AnCanNet_H_ */
