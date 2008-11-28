/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTDIG_H_
#define ANTDIG_H_

#include "AnAgent.h"
#include "AnBoard.h"
#include "AnTdc.h"

class AnTdig: public AnBoard {
public:
	AnTdig(const AnAddress& ladrr, const AnAddress& laddr, AnCanObject *parent = 0);
	virtual ~AnTdig();

	AnTdc *tdc(int i) const { return (i == 255)? m_tdc[0] : m_tdc[i]; }


// Inherited from AnCanObject
	virtual AnCanObject *at(int i) { return (i > 0) ? m_tdc[i] : dynamic_cast<AnCanObject*>(this); }
	
	virtual void sync(int level = 0);
	virtual void reset();
	virtual void config();

// Inherited from AnBoard
	virtual bool isEast() const { return dynamic_cast<AnBoard*>(parent())->isEast(); }
	virtual bool isWest() const { return dynamic_cast<AnBoard*>(parent())->isWest(); }
	virtual QString ecsrString() const;
	virtual int status() const;

// Own Functions
	int threshold() const { return m_threshold; }
	int setThreshold(int t) { return (m_threshold = t); }
	QString thresholdString() const { return QString::number(m_threshold) + QString(" mV"); }
	QString tempString() const { return QString::number(temp()) + QString(" C");}
	quint64 chipId() const { return m_chipid; }
	quint64 setChipId(quint64 cid) { return (m_chipid = cid); }
	QString chipIdString() const { return "0x" + QString::number(m_chipid, 16); }
	
	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual AnAgent* agent() const;

private:
	AnTdc    *m_tdc[4];
	int      m_threshold;
	quint64  m_chipid;
};

#endif /* ANTDIG_H_ */
