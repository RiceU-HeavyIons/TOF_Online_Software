/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef AnSerdes_H_
#define AnSerdes_H_

#include "AnAgent.h"
#include "AnBoard.h"
#include "AnTdc.h"

class AnSerdes: public AnBoard {
public:
	AnSerdes(const AnAddress& ladrr, const AnAddress& laddr, AnCanObject *parent = 0);
	virtual ~AnSerdes();

// from AnCanObject
	virtual AnCanObject *at(int i) { return (AnCanObject*)this; } // no child objs

	virtual QString dump() const;
	virtual void sync(int level = 0);
	virtual void reset() { /* place holder */}
	virtual void config()  { /* place holder */}

	virtual QString firmwareString() const;

// from AnBoard
	virtual QString ecsrString() const;
	virtual int status() const;
	virtual bool isEast() const { return dynamic_cast<AnBoard*>(parent())->isEast(); }
	virtual bool isWest() const { return dynamic_cast<AnBoard*>(parent())->isWest(); }

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual quint32 cantyp() const;
	virtual AnAgent* agent() const;
};

#endif /* AnSerdes_H_ */
