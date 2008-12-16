/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef AnSerdes_H_
#define AnSerdes_H_

#include "AnAgent.h"
#include "AnBoard.h"
#include "AnTcpu.h"

class AnSerdes: public AnBoard {
public:
	AnSerdes(const AnAddress& ladrr, const AnAddress& laddr, AnCanObject *parent = 0);
	virtual ~AnSerdes();

// from AnCanObject
	virtual AnCanObject *at(int i)  { return this; } // no child objs
	virtual AnCanObject *hat(int i) { return this; } // no child objs	

	virtual QString dump() const;

	virtual void init (int level) { /* place holder */}
	virtual void config(int level);
	virtual void reset (int level) { /* place holder */}
	virtual void sync  (int level);


	virtual QString firmwareString() const;

// from AnBoard
	virtual QString ecsrString() const;
	virtual int status() const;
	virtual bool isEast() const { return dynamic_cast<AnBoard*>(parent())->isEast(); }
	virtual bool isWest() const { return dynamic_cast<AnBoard*>(parent())->isWest(); }

	virtual quint32 canidr() const;
	virtual quint32 canidw() const;
	virtual quint32 cantyp() const;
	virtual AnAgent* agent() const;
	
	
//  own functions
	AnTcpu *tcpu(int i) const { return m_tcpu[i-1]; }
	AnTcpu *setTcpu(int i, AnTcpu *tcpu) { return (m_tcpu[i-1] = tcpu); }
	
	quint8 pld9xSet() const;

	quint8 pld9xBase() const { return m_pld9xBase; }
	quint8 setPld9xBase(quint8 s) { return (m_pld9xBase = s); }
	
	QString pld9xString() const;

private:
	enum { NPORT = 4 };
	AnTcpu *m_tcpu[NPORT];
	quint8  m_pld9xBase;

};

#endif /* AnSerdes_H_ */
