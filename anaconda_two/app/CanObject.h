/*
 * CanObject.h
 *
 *  Created on: Nov 6, 2008
 *      Author: koheik
 */

#ifndef CANOBJECT_H_
#define CANOBJECT_H_
#include <QtCore/QObject>

class CanObject;

class CanObject : public QObject {
public:
  CanObject(CanObject *parent = NULL);
  virtual ~CanObject();
  quint8 devId() const { return m_devid; }
  quint8 setDevId(const quint8 devid) { return (m_devid = devid); }
  quint8 canId() const { return m_canid; }
  quint8 setCanId(const quint8 canid) { return (m_canid = canid); }
  double temp() const { return m_temp; }
  double setTemp(const double temp) { return (m_temp = temp); }


private:
  quint8    m_devid;
  quint8    m_canid;
  double    m_temp;
};

#endif /* CANOBJECT_H_ */
