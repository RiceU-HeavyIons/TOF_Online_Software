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

	virtual void init  (int level) { Q_UNUSED(level);/* placeholder */ }
	virtual void config(int level);
	virtual void reset (int level);
	virtual void qreset(int level);
	virtual void sync  (int level);
	void bunchReset(int level);
	void reloadFPGAs(int level);
	void recoverAlertMsg(int val);

	virtual AnCanObject *at(int i);
	virtual AnCanObject *hat(int i);

	virtual bool setInstalled(bool b);
	virtual bool setActive(bool b);

// from AnBoard
	virtual QString firmwareString() const;
	virtual QString ecsrString(bool hilit = false) const;
	virtual double maxTemp() const { return (temp(0) > temp(1)) ? temp(0) : temp(1); }
	virtual int status() const;

	bool isEast() const { return (lAddress().at(1) >  2); }
	bool isWest() const { return (lAddress().at(1) <= 2); }

	AnSerdes *serdes(int i) const { return m_serdes[i-1]; }

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual AnAgent* agent() const;

private:
	void log(QString str);

	AnSerdes *m_serdes[8];
	quint16   m_thubMCUFirmware;
	quint32   m_thubFPGAFirmware;
};

#endif /* ANTHUB_H_ */
