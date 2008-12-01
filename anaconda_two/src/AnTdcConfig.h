/*
 * $Id$
 *
 *  Created on: Nov 27 2008
 *
 *      Author: koheik
 */

#ifndef AnTdcConfig_H_
#define AnTdcConfig_H_

#include <QtCore/QTypeInfo>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QStringList>

#include <cstdio>
#include "libpcan.h"
using namespace PCAN;

class AnTdcConfig;

class AnTdcConfig : public QObject {

public:
	AnTdcConfig(QObject *parent = 0);
	AnTdcConfig(const AnTdcConfig *rhs, QObject *parent = 0);

	~AnTdcConfig();

	AnTdcConfig& operator=(const AnTdcConfig &rhs);

	static quint8   bitsToNum(quint32 bs);

	quint32 loadFromFile(const char *path);
	quint32 loadFromString(QString& str);

	quint32 id() const { return m_id; }
	quint32 setId(quint32 id) { return (m_id = id); }
	quint32 length() const { return m_length; }
	quint32 checksum() const { return m_check_sum; }

	quint32 blockLength() const { return m_block_length; }
	void setBlockMsg(TPCANMsg *msg, int line);

private:

	quint32  m_id;
	quint32  m_check_sum;
	quint32  m_length;
	quint32  m_block_length;
	quint8   m_data[128];
};

#endif /* AnTdcConfig_H_ */
