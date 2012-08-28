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
#include "locallibpcan.h"
//#include <net/if.h>
//#include <linux/can.h>
//#include <linux/can/raw.h>
#include "AnAddress.h"

class AnRdMsg {

public:
	AnRdMsg();
	//AnRdMsg(const quint8 devid, const struct can_frame& rmsg);
	AnRdMsg(const quint8 devid, const struct can_frame& msg);	
	virtual ~AnRdMsg();

	quint8  devid()     const { return m_devid; }
	quint32 id()        const { return m_id; }
	quint8  type()      const { return m_type; }
	quint8  len()       const { return m_len; }
	quint8  data(int i) const { return m_data[i]; }
	quint64 data() const;

	AnAddress source() const;
	quint8    payload() const;
	
	QString toString() const;

private:
	quint8          m_devid;
	quint32         m_id;
	quint8          m_type;
	quint8          m_len;
	quint8          m_data[8];

};

QDebug operator<<(QDebug dbg, const AnRdMsg &m);

Q_DECLARE_METATYPE(AnRdMsg);
#endif
