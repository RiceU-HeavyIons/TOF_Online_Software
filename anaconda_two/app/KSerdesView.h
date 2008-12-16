/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef KSerdesView_H_
#define KSerdesView_H_
#include <QtCore/QModelIndex>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QItemSelectionModel>

#include "AnSerdes.h"

class KSerdesView : public QGroupBox {
	Q_OBJECT

public:
	KSerdesView(QWidget *parent = 0);
	virtual ~KSerdesView();

	QItemSelectionModel* selectionModel() const { return m_selectionModel; }
	void setSelectionModel(QItemSelectionModel *md);

public slots:
	void currentRowChanged(const QModelIndex&, const QModelIndex&);
	void selectionChanged(const QItemSelection&, const QItemSelection&);


private:
	QItemSelectionModel *m_selectionModel;
	QLabel              *m_laddr;
	QLabel              *m_haddr;
	QLabel              *m_firm;
	QLabel              *m_temp;
	QLabel              *m_ecsr;
};

#endif /* KTCPUVIEW_H_ */
