/*
 * $Id$
 *
 *  Created on: Oct 30, 2008
 *      Author: koheik
 */

#include "AnTdcConfig.h"

AnTdcConfig::AnTdcConfig(QObject *parent) : QObject(parent) 
{
  // TODO Auto-generated constructor stub

}

AnTdcConfig::AnTdcConfig(const AnTdcConfig *rhs, QObject *parent) : QObject(parent),
 m_id(rhs->m_id), m_length(rhs->m_length),
 m_check_sum(rhs->m_check_sum), m_block_length(rhs->m_block_length)
{
	for(quint32 i = 0; i < m_length; ++i) m_data[i] = rhs->m_data[i];
}

AnTdcConfig::~AnTdcConfig()
{
  // TODO Auto-generated destructor stub
}

AnTdcConfig& AnTdcConfig::operator=(const AnTdcConfig &rhs) {

//	setParent(rhs.parent());
	qDebug() << "a";
	m_id = rhs.m_id;
	m_length = rhs.m_length;
	m_check_sum = rhs.m_check_sum;
	m_block_length = rhs.m_block_length;
	for(int i = 0; i < rhs.m_length; ++i) m_data[i] = rhs.m_data[i];
	return *this;
}

//-----------------------------------------------------------------------------
quint8 AnTdcConfig::bitsToNum(quint32 bs)
{
  quint8 c = 0;

  for(quint8 i = 0; i < 8; i++)
    c |= ((bs >> 4*i) & 0x1) << i;

  return c;
}
//-----------------------------------------------------------------------------
quint32 AnTdcConfig::loadFromString(QString& str)
{
	quint32 cline = 0;
	quint32 csum  = 0;
	foreach(QString tkn, str.split(QRegExp("\\r?\\n"))) {
		if (tkn.length()) {
			csum += (m_data[cline++] = tkn.toUShort(0, 2));
		}
	}
	m_length       = cline;
	m_check_sum    = csum;
	m_block_length = (m_length / 7) + ((m_length % 7) ? 1 : 2);

	return m_check_sum;
}

//-----------------------------------------------------------------------------
quint32 AnTdcConfig::loadFromFile(const char *path)
{
  FILE  *fp;

  quint32 cline = 0;
  quint32 ln    = 0;
  quint32 csum  = 0;

  fp = fopen(path, "r");

  while(fscanf(fp, "%x", &ln) != EOF) {
    m_data[cline] = bitsToNum(ln);
    cline++;
    csum += bitsToNum(ln);
  }
  m_length       = cline;
  m_check_sum    = csum;
  m_block_length = (m_length / 7) + ((m_length % 7) ? 1 : 2);

  return m_check_sum;
}

//-----------------------------------------------------------------------------
void AnTdcConfig::setBlockMsg(TPCANMsg *msg, int line) {

	msg->DATA[0] = 0x20;

	int len;

	if (0 <= line && line < m_block_length - 1)
		len = 8;
	else if ((line == m_block_length - 1) && ((m_length % 7) != 0))
		len = 1 + (m_length % 7);
	else
		len = 0;

  msg->LEN = len;

  for (int j = 1; j < len; ++j)
    msg->DATA[j] = m_data[7*line + (j-1)];
}
