/*
 * AnTcpu.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTCPU_H_
#define ANTCPU_H_

#include "AnBoard.h"
#include "AnTdig.h"

class AnTcpu: public AnBoard {
public:
	AnTcpu(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);
	virtual ~AnTcpu();
	
// Inherited from AnCanObject

	virtual AnCanObject *at(int i) { return (i > 0) ? m_tdig[i-1] : static_cast<AnCanObject*>(this); }

	virtual void sync(int level = 0);
	virtual void dump() const;
	virtual void reset();

// Inherited from AnBoard

	virtual double maxTemp() const;
	virtual QString ecsrString() const;

	virtual bool isEast() const { return (lAddress().at(1) <= 60); }
	virtual bool isWest() const { return (lAddress().at(1)  > 60); }
	
	virtual int status() const;

//
	AnTdig *tdig(int i) const { return m_tdig[i-1]; }
	quint8  setPldReg02(quint8 v) { return (m_pld02 = v); }

	QString pldRegString() const;

private:
	AnTdig *m_tdig[8];
	quint8  m_pld02, m_pld0e;
};

#endif /* ANTCPU_H_ */
