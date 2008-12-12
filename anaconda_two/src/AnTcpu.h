/*
 * $Id$
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

	virtual AnCanObject *at(int i);
	virtual AnCanObject *hat(int i);

	virtual bool setActive(bool act);

	virtual QString dump() const;
	virtual void sync(int level = 0);
	virtual void reset();
	virtual void config();
	virtual void write();	

// Inherited from AnBoard

	virtual double maxTemp() const;
	virtual QString ecsrString() const;

	virtual bool isEast() const { return (lAddress().at(1) >  60); }
	virtual bool isWest() const { return (lAddress().at(1) <= 60); }

	virtual int status() const;

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual AnAgent* agent() const;

// Own Functions
	AnTdig *tdig(int i) const { return m_tdig[i-1]; }

	quint8  pldReg02() const { return m_pld02; }
	quint8  setPldReg02(quint8 v) { return (m_pld02 = v); }
	quint8  pldReg02Set() const { return m_pld02Set; }
	quint8  setPldReg02Set(quint8 v) { return (m_pld02Set = v); }

	quint8  pldReg03() const { return m_pld03; }
	quint8  setPldReg03(quint8 v) { return (m_pld03 = v); }

	quint8  pldReg0e() const { return m_pld0e; }
	quint8  setPldReg0e(quint8 v) { return (m_pld0e = v); }
	quint8  pldReg0eSet() const { return m_pld0eSet; }
	quint8  setPldReg0eSet(quint8 v) { return (m_pld0eSet = v); }

	QString pldReg02String() const;
	QString pldReg03String() const;	

	quint64 chipId() const { return m_chipid; }
	quint64 setChipId(quint64 cid) { return (m_chipid = cid); }
	QString chipIdString() const { return "0x" + QString::number(m_chipid, 16); }

	quint8 trayId() const { return m_tray_id; }
	quint8 setTrayId(quint8 id) { return (m_tray_id = id); }
	QString trayIdString() const { return QString::number(m_tray_id); }

	QString traySn() const { return m_tray_sn; }
	QString setTraySn(QString sn) { return (m_tray_sn = sn); }

	void setLvHv(int lb, int lc, int hb, int hc);
	virtual QString lvHvString() const;
	virtual QString lvString() const;
	virtual QString hvString() const;

private:
	AnTdig     *m_tdig[8];
	quint8      m_pld02, m_pld02Set;
	quint8      m_pld03; // read-only register	
	quint8      m_pld0e, m_pld0eSet;
	quint64     m_chipid;

	quint8      m_tray_id;
	QString     m_tray_sn;
	qint8       m_lv_box, m_lv_ch;
	qint8       m_hv_box, m_hv_ch;
};

#endif /* ANTCPU_H_ */
