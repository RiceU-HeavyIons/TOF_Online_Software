/*
 * AnTdc.h
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#ifndef ANTDC_H_
#define ANTDC_H_

#include "AnSock.h"
#include "AnCanObject.h"

class AnTdc: public AnCanObject {
public:
	AnTdc(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);

	virtual AnCanObject *at(int i) { return NULL; }
	virtual void sync(int lelve);
	virtual void reset(); 
	
	quint16 config() const { return m_config; }
	quint16 setConfig(quint16 cf) { return (m_config = cf); }
	
	quint64 status() const { return m_status; }
	quint64 setStatus(quint64 sw);
	QString statusTipString() const;

private:
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
};

#endif /* ANTDC_H_ */
