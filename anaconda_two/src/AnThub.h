/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef ANTHUB_H_
#define ANTHUB_H_
#include "AnAgent.h"
#include "AnBoard.h"
#include "AnSerdes.h"

class AnThub : public AnBoard {
public:
	AnThub(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);
	virtual ~AnThub();

//  inherited from AnCanbusObject
	virtual QString dump() const;
	virtual void sync(int level = 0);
	virtual void reset();
	virtual void config() { /* place holder */ }
	virtual void write() { /* place holder */ }	

	virtual AnCanObject *at(int i);
	virtual AnCanObject *hat(int i);

// from AnBoard
	virtual double maxTemp() const { return (temp(0) > temp(1)) ? temp(0) : temp(1); }
	virtual int status() const;

	bool isEast() const { return (lAddress().at(1) >  2); }
	bool isWest() const { return (lAddress().at(1) <= 2); }

	AnSerdes *serdes(int i) const { return m_serdes[i-1]; }

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual AnAgent* agent() const;

private:
	AnSerdes *m_serdes[8];
};

#endif /* ANTHUB_H_ */
