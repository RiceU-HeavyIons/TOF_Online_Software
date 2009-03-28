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
	virtual AnCanObject *hat(int i);

	virtual bool setInstalled(bool b);
	virtual bool setActive(bool b);
	virtual QString dump() const;

	virtual void init  (int level);
	virtual void config(int level);
	virtual void reset (int level);
	virtual void qreset(int level);
	virtual void sync  (int level);

// Inherited from AnBoard
	virtual bool isEast() const { return dynamic_cast<AnBoard*>(parent())->isEast(); }
	virtual bool isWest() const { return dynamic_cast<AnBoard*>(parent())->isWest(); }
	virtual QString ecsrString(bool hilit = false) const;
	virtual QString ecsrToolTipString() const;
	virtual int status() const;

// Own Functions
	int threshold() const { return m_threshold; }
	int setThreshold(int t) { return (m_threshold = t); }

	QString thresholdString() const { return QString::number(m_threshold) + QString(" mV"); }
	QString tempString() const { return QString::number(temp()) + QString(" C");}
	quint64 chipId() const { return m_chipid; }
	quint64 setChipId(quint64 cid) { return (m_chipid = cid); }
	QString chipIdString() const { return "0x" + QString::number(m_chipid, 16); }

	quint8  pldReg03() const { return m_pld03; }
	quint8  setPldReg03(quint8 v) { return (m_pld03 = v); }
	quint8  pldReg03Set() const { return m_pld03Set; }
	quint8  setPldReg03Set(quint8 v) { return (m_pld03Set = v); }

	QString pldReg03String(bool hlite) const;
	QString pldReg03ToolTipString() const;

	QString errorDump() const;

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual quint32 cantyp() const;
	virtual AnAgent* agent() const;

private:
	AnTdc      *m_tdc[4];
	int         m_threshold;
	quint64     m_chipid;
	quint8      m_pld03, m_pld03Set; // read-only register
};

#endif /* ANTDIG_H_ */
