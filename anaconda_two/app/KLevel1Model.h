/*
 * $Id$
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */

#ifndef KLevel1Model_H_
#define KLevel1Model_H_

#include <QtCore/QAbstractTableModel>
#include <QtCore/QModelIndex>
#include <QtCore/QStringList>
#include <QtCore/QList>
#include <QtGui/QIcon>

#include "AnBoard.h"
#include "AnRoot.h"

class KLevel1Model : public QAbstractTableModel {
	Q_OBJECT

public:
	KLevel1Model(AnRoot *root, QObject *parent = 0);

	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int selction, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	QStringList selectionList() { return m_selectionList; }

	void sync(int level);

public slots:
	void toggleMode(int i);
	void setSelection(int slt);
	void updated(AnBoard *brd);

private:
	int m_rows;
	int m_columns;
	int m_sort_column;
	Qt::SortOrder m_sort_order;
	AnRoot *m_root;
	int m_selection;

	QStringList m_selectionList;
	QList<AnBoard*> m_list;

	QIcon m_statusIcon[4];
};

#endif /* KLevel1Model_H_ */
