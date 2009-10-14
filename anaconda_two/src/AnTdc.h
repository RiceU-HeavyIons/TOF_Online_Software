/*
 * AnTdc.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTDC_H_
#define ANTDC_H_

#include "AnAgent.h"
#include "AnCanObject.h"

class AnTdc: public AnCanObject {
public:
	AnTdc(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);

//  AnCanObject
	virtual AnCanObject *at(int i) { Q_UNUSED(i); return this; }
	virtual AnCanObject *hat(int i) { Q_UNUSED(i); return this; }

	virtual QString dump() const;

	virtual void init  (int level);
	virtual void config(int level);
	virtual void reset (int level);
	virtual void qreset(int level) { Q_UNUSED(level); /* placeholder */ };
	virtual void sync  (int level);

	quint16 configId() const { return m_config; }
	quint16 setConfigId(quint16 cf) { return (m_config = cf); }

	quint64 status() const { return m_status; }
	quint64 setStatus(quint64 sw);
	QString statusString() const;
	QString statusTipString() const;

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual quint32 cantyp() const;
	virtual AnAgent* agent() const;

//  Own Functions
	quint8 mask() const { return m_mask; }
	quint8  setMask(quint8 msk) { return (m_mask = msk); }

private:
	void set_mask_msg(TPCANMsg &msg) const;
	void log(QString str) const;

	quint16 m_config;
	quint64 m_status;

	typedef struct {
		char    *name;
		int     start;
		int     length;
	} field_t;

	static field_t field[26];
	QString fkey(int i)   const;
	quint8  fvalue(int i) const;
	quint8  m_mask;
};

#endif /* ANTDC_H_ */
