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
#include "libpcan.h"
#include "pcan.h"

class AnSock : public QObject {
public:
  AnSock();
  virtual ~AnSock();

  static int TCAN_DEBUG;
  static const char *PCAN_DEVICE_PATTERN;

  static void set_msg(TPCANMsg &msg, ...);
  static void print(const TPCANMsg &msg);
  static void print(const TPCANRdMsg &rmsg);

  int open(quint8 dev_id);
  quint64 write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
      unsigned int return_length, unsigned int time_out);

private:
  HANDLE handle;
  quint8 addr;
  QString dev_path;
};

#endif /* ANSOCK_H_ */
