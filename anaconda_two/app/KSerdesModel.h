/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef KSerdesModel_H_
#define KSerdesModel_H_
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include <QtGui/QIcon>

#include "AnThub.h"

class KSerdesModel : public QAbstractTableModel {
  Q_OBJECT
public:
  KSerdesModel(QObject *parent = 0);

  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return 8; }
  int columnCount(const QModelIndex &parent = QModelIndex()) const { Q_UNUSED(parent); return 3; }
  QModelIndex index(int row, int column, const QModelIndex &parent) const;

  void setThub(AnThub *thub);

private:
	AnThub  *m_thub;            // pointer to parent Thub
	QIcon    m_statusIcon[6];
};

#endif /* KSerdesModel_H_ */
