/*
 * TableViewModel.h
 *
 *  Created on: Nov 6, 2008
 *      Author: koheik
 */

#ifndef TABLEVIEWMODEL_H_
#define TABLEVIEWMODEL_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QModelIndex>
#include <QtCore/QList>

#include "AnRoot.h"

class TableViewModel : public QAbstractTableModel {
  Q_OBJECT

public:
  TableViewModel(QObject *parent = 0);

  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int selction, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
public slots:
  void toggleMode(int i);


private:
  int m_rows;
  int m_columns;
  int m_sort_column;
  Qt::SortOrder m_sort_order;
  AnRoot *m_root;
};

#endif /* TABLEVIEWMODEL_H_ */
