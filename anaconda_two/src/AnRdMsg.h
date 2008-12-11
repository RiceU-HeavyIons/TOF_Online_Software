/*
 * $Id$
 *
 *  Created on: Dec 10, 2008
 *      Author: koheik
 */
#ifndef AnRdMsg_H_
#define AnRdMsg_H_ 1
#include <QtCore/QDebug>
#include <QtCore/QMetaType>
#include "libpcan.h"

class AnRdMsg {

public:
	AnRdMsg();
	AnRdMsg(const quint8 devid, const PCAN::TPCANRdMsg& rmsg);
	AnRdMsg(const quint8 devid, const PCAN::TPCANMsg& msg);	
	virtual ~AnRdMsg();

	quint8  devid() const { return m_devid; }
	quint16 id() const { return m_id; }
	quint8  type() const { return m_type; }
	quint8  len()  const { return m_len; }
	quint8  data(int i) const { return m_data[i]; }

private:
	quint8          m_devid;
	quint16         m_id;
	quint8          m_type;
	quint8          m_len;
	quint8          m_data[8];

};

QDebug operator<<(QDebug dbg, const AnRdMsg &m);

Q_DECLARE_METATYPE(AnRdMsg);
#endif