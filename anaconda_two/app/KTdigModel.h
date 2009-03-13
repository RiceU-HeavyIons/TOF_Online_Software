/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef KTdigModel_H_
#define KTdigModel_H_
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include <QtGui/QIcon>

#include "AnTcpu.h"

class KTdigModel : public QAbstractTableModel {
  Q_OBJECT
public:
  KTdigModel(QObject *parent = 0);

  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const { return 8; }
  int columnCount(const QModelIndex &parent = QModelIndex()) const { return 5; }
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  void setTcpu(AnTcpu *tcpu);

private:
	AnTcpu *m_tcpu;
	QIcon  m_statusIcon[5];
};

#endif /* KTdigModel_H_ */
