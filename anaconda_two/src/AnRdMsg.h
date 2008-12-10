/*
 * $Id$
 *
 *  Created on: Dec 10, 2008
 *      Author: koheik
 */
#ifndef AnRdMsg_H_
#define AnRdMsg_H_ 1
#include <QtCore/QTypeInfo>

class AnRdMsg {

public:
	AnRdMsg();
	virtual ~AnRdMsg();

	quint16 id() const { return m_id; }
	quint8  type() const { return m_type; }
	quint8  len()  const { return m_len; }
	quint8  data(int i) const { return m_data[i]; }

private:
	quint16         m_id;
	quint8          m_type;
	quint8          m_len;
	quint8          m_data[8];

};
#endif