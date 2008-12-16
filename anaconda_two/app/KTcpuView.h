/*
 * $Id$
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

#include "KTdigView.h"
#include "KTdigModel.h"

#include "AnTcpu.h"

class KTcpuView : public QGroupBox {
  Q_OBJECT
public:
  KTcpuView(QWidget *parent = 0);
  virtual ~KTcpuView();

public slots:
  void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);

protected:
	void mouseReleaseEvent(QMouseEvent *event) { m_view->clearSelection(); }

private:

	AnTcpu      *m_tcpu;

	QGroupBox   *m_box;
	QLabel      *m_tray;
	QLabel      *m_laddr;
	QLabel      *m_haddr;
	QLabel      *m_firm;
	QLabel      *m_chip;
	QLabel      *m_temp;
	QLabel      *m_ecsr;
	QLabel      *m_pld02;
	QLabel      *m_pld03;


	KTdigModel  *m_model;
	KTdigView   *m_tdigView;
	QTableView  *m_view;
};

#endif /* KTCPUVIEW_H_ */
