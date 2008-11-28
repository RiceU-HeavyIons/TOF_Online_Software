/*
 * AnTcpu.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTCPU_H_
#define ANTCPU_H_

#include "AnBoard.h"
#include "AnAgent.h"
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
	virtual void config();
	

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

	quint64 chipId() const { return m_chipid; }
	quint64 setChipId(quint64 cid) { return (m_chipid = cid); }
	QString chipIdString() const { return "0x" + QString::number(m_chipid, 16); }

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual AnAgent* agent() const;

private:
	AnTdig   *m_tdig[8];
	quint8   m_pld02, m_pld0e;
	quint64  m_chipid;
};

#endif /* ANTCPU_H_ */
