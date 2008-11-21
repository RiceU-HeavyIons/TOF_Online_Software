/*
 * KTcpuView.h
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef KTDIGVIEW_H_
#define KTDIGVIEW_H_
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>

#include <QtGui/QTableView>
#include <QtGui/QStandardItemModel>

#include "AnTdig.h"

class KTdigView : public QGroupBox {
  Q_OBJECT
public:
  KTdigView(QWidget *parent = 0);
  virtual ~KTdigView();

public slots:
  void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);

private:
  QGroupBox *m_box;
  QLabel *m_laddr;
  QLabel *m_haddr;
  QLabel *m_firm;
  QLabel *m_temp;
  QLabel *m_ecsr;
};

#endif /* KTCPUVIEW_H_ */
