/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#ifndef KThubView_H_
#define KThubView_H_
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>
#include <QtGui/QFrame>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>

#include <QtGui/QTableView>
#include <QtGui/QStandardItemModel>

#include "AnTcpu.h"
#include "KSerdesView.h"
#include "KSerdesModel.h"

class KThubView : public QGroupBox {
Q_OBJECT
public:
	KThubView(QWidget *parent = 0);
	virtual ~KThubView();

public slots:
	void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);

protected:
	void mouseReleaseEvent(QMouseEvent *event) { Q_UNUSED(event); m_tblview->clearSelection(); }

private:
	QGroupBox *m_box;
	QLabel    *m_laddr;
	QLabel    *m_haddr;
	QLabel    *m_firm;
	QLabel    *m_temp1;
	QLabel    *m_temp2;
	QLabel    *m_ecsr;

	KSerdesModel  *m_model;
	KSerdesView   *m_view;
	QTableView    *m_tblview;

};

#endif /* KThubView_H_ */
