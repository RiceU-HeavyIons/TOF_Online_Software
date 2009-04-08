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

	virtual bool setInstalled(bool b);
	virtual bool setActive(bool b);

	virtual QString dump() const;
	virtual void init  (int level);
	virtual void config(int level);
	virtual void reset (int level);
	virtual void qreset(int level);
	virtual void sync  (int level = 0);
	void resync(int level);

// Inherited from AnBoard

	virtual double maxTemp() const;
	virtual QString ecsrString(bool hilit = false) const;
	virtual QString ecsrToolTipString() const;

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
	QString pldReg02ToolTipString() const;

	quint8  pldReg03() const { return m_pld03; }
	quint8  setPldReg03(quint8 v) { return (m_pld03 = v); }
	quint8  pldReg03Set() const { return m_pld03Set; }
	quint8  setPldReg03Set(quint8 v) { return (m_pld03Set = v); }
	QString pldReg03ToolTipString() const;

	quint8  pldReg0e() const { return m_pld0e; }
	quint8  setPldReg0e(quint8 v) { return (m_pld0e = v); }
	quint8  pldReg0eSet() const { return m_pld0eSet; }
	quint8  setPldReg0eSet(quint8 v) { return (m_pld0eSet = v); }

	QString pldReg02String(bool hlite = false) const;
	QString pldReg03String(bool hlite = false) const;	
	QString pldReg0eString(bool hlite = false) const;	

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
	virtual int lvOrder() const { return 100*m_lv_box + m_lv_ch; }
	virtual int hvOrder() const { return 100*m_hv_box + m_hv_ch; }

	QString haddrString() const;

	bool fibermode() const;
	
	QString errorDump() const;

private:
	
	void log(QString str);

	AnTdig     *m_tdig[8];
	quint8      m_pld02, m_pld02Set;
	quint8      m_pld03, m_pld03Set; // read-only register
	quint8      m_pld0e, m_pld0eSet;
	quint64     m_chipid;

	quint8      m_tray_id;
	QString     m_tray_sn;
	qint8       m_lv_box, m_lv_ch;
	qint8       m_hv_box, m_hv_ch;
};

#endif /* ANTCPU_H_ */
