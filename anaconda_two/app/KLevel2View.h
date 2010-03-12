/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef KLevel2View_H_
#define KLevel2View_H_
#include <QtCore/QDebug>
#include <QtCore/QModelIndex>

#include <QtGui/QWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>

#include <QtGui/QHBoxLayout>

#include "AnTcpu.h"
#include "AnThub.h"
#include "KTcpuView.h"
#include "KThubView.h"

class KLevel2View : public QDockWidget {
  Q_OBJECT
public:
	KLevel2View(const QString &title, QWidget *parent = 0);
	virtual ~KLevel2View();

	QItemSelectionModel* selectionModel() const { return m_selectionModel; }
	void setSelectionModel(QItemSelectionModel *md);

public slots:
	void currentRowChanged(const QModelIndex &current, const QModelIndex &parent);
	void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
	KTcpuView           *m_tcpuView;
	KThubView           *m_thubView;
	QItemSelectionModel *m_selectionModel;
};

class L2Widget : public QWidget
{
public:
  L2Widget(QWidget *parent) : QWidget(parent)
    {
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    
    QSize sizeHint() const
    {
      return QSize(310, 500);
    }
};

#endif /* KLevel2View_H_ */
