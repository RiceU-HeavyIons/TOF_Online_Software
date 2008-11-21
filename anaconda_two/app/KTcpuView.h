/*
 * KTcpuView.h
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef KTCPUVIEW_H_
#define KTCPUVIEW_H_
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>

#include <QtGui/QTableView>
#include <QtGui/QStandardItemModel>

#include "AnTcpu.h"
#include "KTdigView.h"
#include "TdigTableModel.h"

class KTcpuView : public QGroupBox {
  Q_OBJECT
public:
  KTcpuView(QWidget *parent = 0);
  virtual ~KTcpuView();

public slots:
  void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);

private:
  QGroupBox *m_box;
  QLabel *m_laddr;
  QLabel *m_haddr;
  QLabel *m_firm;
  QLabel *m_temp;
  QLabel *m_ecsr;

  TdigTableModel *m_model;
  KTdigView *m_tdigView;
};

#endif /* KTCPUVIEW_H_ */
