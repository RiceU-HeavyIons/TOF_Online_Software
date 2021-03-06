/*
 * TdigTableModel.h
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef TDIGTABLEMODEL_H_
#define TDIGTABLEMODEL_H_
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QModelIndex>
#include <QtCore/QAbstractTableModel>

#include "AnTcpu.h"

class TdigTableModel : public QAbstractTableModel {
  Q_OBJECT
public:
  TdigTableModel(QObject *parent = 0);

  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const { return 8; }
  int columnCount(const QModelIndex &parent = QModelIndex()) const { return 3; }
  QModelIndex index(int row, int column, const QModelIndex &parent) const;
  void setTcpu(AnTcpu *tcpu);

private:
  AnTcpu *m_tcpu;
};

#endif /* TDIGTABLEMODEL_H_ */
