/*
 * AnSock.h
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#ifndef ANSOCK_H_
#define ANSOCK_H_

#include <cstdio>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QMap>

#include "libpcan.h"
#include "pcan.h"
using namespace PCAN;

class AnSock : public QObject {
public:
  AnSock();
  virtual ~AnSock();

  static QMap<int, AnSock*> AnSock::open(QList<int> &dev_id_list);
  static int TCAN_DEBUG;
  static const char *PCAN_DEVICE_PATTERN;

  static void set_msg(TPCANMsg &msg, ...);
  static void print(const TPCANMsg &msg);
  static void print(const TPCANRdMsg &rmsg);

  int open(quint8 dev_id);
  quint64 write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
      unsigned int return_length, unsigned int time_out = 4000000);

private:
  HANDLE handle;
  quint8 addr;
  QString dev_path;
};

#endif /* ANSOCK_H_ */
