/*
 * $Id$
 *
 *  Created on: Nov 9, 2008
 *      Author: koheik
 */

#include "AnTdc.h"
#include "AnRoot.h"
#include "AnExceptions.h"

AnTdc::AnTdc(const AnAddress &laddr, const AnAddress &haddr,
	     AnCanObject *parent)
  : AnCanObject(laddr, haddr, parent)
{
  setObjectName(QString("TDC ") + lAddress().toString());

  m_status = 0x0;
  m_mask   = 0x0;
}

/**
 * Dump Object Information
 **/
QString AnTdc::dump() const
{
  QStringList sl;

  sl << QString().sprintf("AnTdc(%p):", this);
  sl << QString("  Name              : ") + name();
  sl << QString("  Hardware Address  : ") + haddr().toString().toStdString().c_str();
  sl << QString("  Logical Address   : ") + laddr().toString().toStdString().c_str();
  sl << QString("  Installed         : ") + (installed() ? "yes" : "no");
  sl << QString("  Active            : ") + (active() ? "yes" : "no");
  sl << QString("  Status            : ") + QString::number(status());
  sl << QString("  Status Word       : 0x%1").arg(QString::number(m_status, 16));
  sl << QString("  Channel Mask      : 0x%1").arg(QString::number(m_mask, 16));
  sl << QString("  Config ID         : %1").arg(QString::number(configId(), 10));
  //	sl << QString("  East / West       : ") + (isEast()? "East" : "West");
  sl << QString("  Synchronized      : ") + synced().toString();

  return sl.join("\n");
}


/**
 * Initialize TDC
 **/
void AnTdc::init(int level)
{

  if (active() && level >= 1 && commError() == 0) {
    quint8  data0 = 0x04 | hAddress().at(3);

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;

    try {
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0xff);
      agent()->write_read(msg, rmsg, 2);
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0x3f);
      agent()->write_read(msg, rmsg, 2);
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 6, data0, 0xf4, 0xff, 0xff, 0xff, 0x1f);
      agent()->write_read(msg, rmsg, 2);
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 6, data0, 0xe4, 0xff, 0xff, 0xff, 0x9f);
      agent()->write_read(msg, rmsg, 2);
    } catch (AnExCanError ex) {
      qDebug() << "CAN error occurred: 0x" << hex << ex.status() << dec;
      incCommError();
    }
  }
}

/**
 * Configure TDC
 */
void AnTdc::config(int level)
{

  if (active() && level >= 1) {
    quint64 rdata;
    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    AnTdcConfig *tc = agent()->tdcConfig(configId());
    qDebug() << "AnTdc::config()" << tc;

    try {
      // block start //
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 1, 0x10);
      agent()->write_read(msg, rmsg, 2);

      // blocks //
      for (unsigned int l = 0; l < tc->blockLength(); ++l) {
	tc->setBlockMsg(&msg, l);
	agent()->write_read(msg, rmsg, 2);
      }

      // block end //
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 1, 0x30);
      rdata = agent()->write_read(msg, rmsg, 8);
      quint8  sts = (0x0000FF & rdata);
      quint32 len = (0x00FFFF & rdata >> 8);
      quint32 csm = (0xFFFFFF & rdata >> 24);
      if (sts != 0 || tc->length() != len || tc->checksum() != csm) {
	qDebug() << objectName();
	qFatal("Block Sending Error: "
	       "config_id=%d, status=%d, length=%d, checksum=%d",
	       tc->id(), sts, len, csm);
      }
      // finalize
      AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 1, 0x40 | haddr().at(3));
      rdata = agent()->write_read(msg, rmsg, 2);


      // set mask
      set_mask_msg(msg);
      agent()->write_read(msg, rmsg, 2);
    } catch (AnExCanError ex) {
      qDebug() << "CAN error occurred: 0x" << hex << ex.status() << dec;
      incCommError();
    }
  }
}

/**
 * Set TPCANMsg for write channel mask
 */
void AnTdc::set_mask_msg(TPCANMsg &msg) const
{
  // 40bit control word for setting mask is...
  // 0b 100h hhhg gggf fffe eeed dddc cccb bbba aaa0 1000
  // where a-h are 0/1 for disabled/enabled channels.
  // initialized control word with base word
  quint64 cwrd = 0x8000000004ULL;

  // m_mask is simple 8bit flag for channel mask
  // in the form of '0b HGFE DCBA'
  // so expand each bit as
  // 0b0 -> 0b1111 (0x0->0xf)
  // 0b1 -> 0b0000 (0x1->0x0)
  // and set on 40bit control word 'cwrd.'
  for (int i = 0; i < 8; ++i)
    cwrd |= ((0x1 & (m_mask >> i)) ? 0x0ULL : 0xfULL) << (4*i + 5);

  msg.ID = canidw();
  msg.MSGTYPE = MSGTYPE_EXTENDED;
  msg.LEN = 6;
  msg.DATA[0] = 0x40 | haddr().at(3);
		
  for (int i = 0; i < 5; i++)
    msg.DATA[i + 1] = 0xff & (cwrd >> 8*i);
}


/**
 * Reset TDC
 */
void AnTdc::reset(int level) {

  if (active() && level >= 1) {
    quint8  data0 = 0x90 | hAddress().at(3);

    TPCANMsg    msg;
    TPCANRdMsg  rmsg;

    AnAgent::set_msg(msg, canidw(), MSGTYPE_EXTENDED, 1, data0);
    agent()->write_read(msg, rmsg, 2, 20000000);
  }
}

/**
 * Sync TDC information
 */
void AnTdc::sync(int level)
{
  if (active() and level >= 1) {
    QStringList btrace;
    TPCANMsg    msg;
    TPCANRdMsg  rmsg;
    try {
      quint8  data0 = 0x04 | hAddress().at(3);
      AnAgent::set_msg(msg, canidr(), MSGTYPE_EXTENDED, 1, data0);
      btrace << AnRdMsg(haddr().at(0), msg).toString();
      m_status = agent()->write_read(msg, rmsg, 10);
      btrace << AnRdMsg(haddr().at(0), rmsg).toString();
      setSynced();
    } catch (AnExCanError ex) {
      if (ex.status() == CAN_ERR_QRCVEMPTY) {
	// probably harmless, only print log message
	log(QString("sync: CAN QRCVEMPTY error occurred: 0x%1").arg(ex.status(),0,16));
	log(btrace.join("\n"));
      } 
      else {
	log(QString("sync: CAN error occurred: 0x%1").arg(ex.status(),0,16));
	log(btrace.join("\n"));
	incCommError();
      }
      log(QString("sync: latest msg " + AnRdMsg(haddr().at(0), msg).toString() + "\n"));
    }
  }
}

//-----------------------------------------------------------------------------
quint32 AnTdc::canidr() const
{
  return (haddr().at(2) << 4 | 0x4) << 18 | haddr().at(1);
}

quint32 AnTdc::canidw() const
{
  return (haddr().at(2) << 4 | 0x2) << 18 | haddr().at(1);
}

quint32 AnTdc::cantyp() const
{
  return MSGTYPE_EXTENDED;
}

AnAgent *AnTdc::agent() const
{
  return dynamic_cast<AnRoot*>
    (parent()->parent()->parent())->agent(hAddress().at(0));
}


quint64 AnTdc::setStatus(quint64 sw)
{
  m_status = sw;
  return sw;
}

AnTdc::field_t AnTdc::field[26] = {
  {"NOT USED",                    62,  2},
  {"Parity",                      61,  1},
  {"DLL lock",                    60,  1},
  {"Trigger FIFO Empty",          59,  1},
  {"Trigger FIFO Full",           58,  1},
  {"Trigger FIFO Occupancy",      54,  4},
  {"Group0 L1 Occupancy",         46,  8},
  {"Group1 L1 Occupancy",         38,  8},
  {"Group3 L1 Occupancy",         30,  8},
  {"Group2 L1 Occupancy",         22,  8},
  {"Readout FIFO Empty",          21,  1},
  {"Readout FIFO Full",           20,  1},
  {"Readout FIFO Occupancy",      12,  8},
  {"Have Token",                  11,  1},
  {"Vernier Error",               10,  1},
  {"Course Error",                 9,  1},
  {"Channel Select Error",         8,  1},
  {"L1 Buffer Parity Error",       7,  1},
  {"Trigger FIFO Parity Error",    6,  1},
  {"Trigger Matching State Error", 5,  1},
  {"Readout FIFO Parity Error",    4,  1},
  {"Readout State Error",          3,  1},
  {"Setup Parity Error",           2,  1},
  {"Control Parity Error",         1,  1},
  {"JTAG Instruction Error",       0,  1},
  {NULL,                           0,  0}
};

QString AnTdc::fkey(int i)  const { return QString(field[i].name); }
//------------------------------------------------------------------------------
quint8 AnTdc::fvalue(int i) const {
  return (m_status >> field[i].start) & (0xFF >> (8 - field[i].length));
}
//------------------------------------------------------------------------------
QString AnTdc::statusString() const
{
  return QString("0x") + QString::number(m_status, 16);
}
//------------------------------------------------------------------------------
QString AnTdc::statusTipString() const
{
  QString ret = QString("<h4>Status Word of TDC %1</h4>").arg(haddr().at(3));
  ret += QString("<table>");
  for(int i = 0; i < 25; ++i) {
    field_t f = field[i];
    if(f.length == 1) {
      ret += QString("<tr><td>[%1]</td><td>%2</td><td>= %3</td></tr>")
	.arg(f.start)
	.arg(f.name)
	.arg(fvalue(i));
    } else {
      ret += QString("<tr><td>[%1..%2]</td><td>%3</td><td>= %4</td></tr>")
	.arg(f.start)
	.arg(f.start + f.length - 1)
	.arg(f.name)
	.arg(fvalue(i));
    }
  }
  ret += "</table>";

  return ret;
}
//------------------------------------------------------------------------------
void AnTdc::log(QString str) const
{
  foreach(QString s, str.split("\n")) {
    agent()->root()->log(QString("AnTdc[%1.%2.%3]: " + s)
			 .arg(laddr().at(1)).arg(laddr().at(2)).arg(laddr().at(3)));
  }
}

