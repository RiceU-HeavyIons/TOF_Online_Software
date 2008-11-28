/*
 * KSerdesView.h
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef KSerdesView_H_
#define KSerdesView_H_
#include <QtCore/QModelIndex>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>

#include "AnSerdes.h"

class KSerdesView : public QGroupBox {
  Q_OBJECT
public:
  KSerdesView(QWidget *parent = 0);
  virtual ~KSerdesView();

public slots:
  void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);

private:

  QLabel    *m_laddr;
  QLabel    *m_haddr;
  QLabel    *m_firm;
  QLabel    *m_temp;
  QLabel    *m_ecsr;
};

#endif /* KTCPUVIEW_H_ */
