/*
 * $Id$
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */

#include "KLevel1Model.h"
#include <QtCore/QDebug>

//-----------------------------------------------------------------------------
KLevel1Model::KLevel1Model(AnRoot *root, QObject *parent) :
	QAbstractTableModel(parent), m_root(root)
{
	// set default sort order
	m_sort_order = Qt::AscendingOrder;
	m_sort_column = 1;

	m_list = m_root->list(); // full copy the list

	m_rows = m_list.count();
	m_columns = 6;

	// load status icons
	m_statusIcon << QIcon(":icons/black16.png");
	m_statusIcon << QIcon(":icons/blue16.png");
	m_statusIcon << QIcon(":icons/green16.png");
	m_statusIcon << QIcon(":icons/greenred16.png");
	m_statusIcon << QIcon(":icons/red16.png");
	m_statusIcon << QIcon(":icons/gray16.png");	

	m_selectionList << new QListWidgetItem(tr("All"));
	m_selectionList << new QListWidgetItem(tr("THUBs"));
	m_selectionList << new QListWidgetItem(tr("Trays"));
	foreach(QString name, m_root->deviceNames())
		m_selectionList << new QListWidgetItem(name);
	m_selectionList << new QListWidgetItem(m_statusIcon[0], "Disabled"   );
	m_selectionList << new QListWidgetItem(m_statusIcon[1], "Standby"    );
	m_selectionList << new QListWidgetItem(m_statusIcon[2], "Healty"     );
	m_selectionList << new QListWidgetItem(m_statusIcon[3], "Warning"    );
	m_selectionList << new QListWidgetItem(m_statusIcon[4], "Error"      );
	m_selectionList << new QListWidgetItem(m_statusIcon[5], "Comm. Error");

	connect(m_root, SIGNAL(updated(AnBoard*)), this, SLOT(updated(AnBoard*)));
	connect(m_root, SIGNAL(updated()),         this, SLOT(updated()));
}
//-----------------------------------------------------------------------------
int KLevel1Model::rowCount(const QModelIndex &parent) const
{
  return m_rows;
}

//-----------------------------------------------------------------------------
int KLevel1Model::columnCount(const QModelIndex &parent) const
{
  return m_columns;
}

//-----------------------------------------------------------------------------
QVariant KLevel1Model::data(const QModelIndex &index, int role) const
{
	int r, c;

	r = index.row();
	c = index.column();

	AnBoard *cobj = dynamic_cast<AnBoard*>(m_list.at(r));
	if (cobj == NULL) return QVariant();

	if (role == Qt::DisplayRole) {
		switch (c) {
			case 0: return QVariant();
			case 1: return cobj->name();
			case 2: return m_root->deviceNameByDevid(cobj->hAddress().at(0));
			case 3: return cobj->lvString();
			case 4: return cobj->hvString();
			case 5: return cobj->maxTempString();
		}
	} else if (role == Qt::ToolTipRole) {
		switch(c) {
			case 1: return cobj->objectName();
			case 2: return cobj->hAddress().toString();
			default: return QVariant();
		}

	} else if (role == Qt::DecorationRole) {
		if (c == 0) return m_statusIcon[cobj->status()];

	} else if (role == Qt::TextAlignmentRole) {
		switch (c) {
			case 0: return Qt::AlignCenter;
			case 2: return Qt::AlignCenter;
			case 3: return Qt::AlignCenter;
			case 4: return Qt::AlignCenter;
			case 5: return Qt::AlignCenter;
		}
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
QVariant KLevel1Model::headerData(int section,
								Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical) {
    return QString("%1").arg(section+1);
  } else
    switch (section) {
	case 0: return QVariant();
    case 1: return QString(tr("Name"));
    case 2: return QString(tr("CANBus Name"));
    case 3: return QString(tr("LV"));
    case 4: return QString(tr("HV"));
    case 5: return QString(tr("Max Temp"));
    }
    return QString("Field %1").arg(section + 1);
}

//-----------------------------------------------------------------------------
Qt::ItemFlags KLevel1Model::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
void KLevel1Model::toggleMode(int i) {
//  qDebug() << "got " << i;

	if (i == 0) {
		m_rows = 2;
		m_columns = 2;
	} else {
		m_rows = 10;
		m_columns = 10;
	}
	reset();
}

//-----------------------------------------------------------------------------
void KLevel1Model::sort(int column, Qt::SortOrder order)
{

	QList<AnBoard*> wlist(m_list);
	m_list.clear();

	if (column == 0) { /* status sort */
		QMultiMap<int, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			AnAddress ad = bd->laddr();
			int ord = 10000*bd->status() + 100*ad.at(0) + ad.at(1);
			wmap.insert(ord, bd);
		}
		wlist = wmap.values();

	} else if (column == 1) { /* name sort */
		QMultiMap<int, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			AnAddress ad = bd->laddr();
			wmap.insert(1000*ad.at(0) + ad.at(1), bd);
		}
		wlist = wmap.values();
	} else if (column == 2) {
		QMultiMap<int, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			AnAddress ad = bd->laddr();
			int ord = 10000*m_root->deviceIdFromDevid(bd->haddr().at(0))
			        + 1000*ad.at(0) + ad.at(1);
			wmap.insert(ord, bd);
		}
		wlist = wmap.values();
	} else if (column == 3) { /* LV */
		QMultiMap<int, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			wmap.insert(bd->lvOrder(), bd);
		}
		wlist = wmap.values();
	} else if (column == 4) { /* HV */
		QMultiMap<int, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			wmap.insert(bd->hvOrder(), bd);
		}
		wlist = wmap.values();
	} else if (column == 5) { /* Max Temp. */
		QMultiMap<double, AnBoard*> wmap;
		foreach(AnBoard *bd, wlist) {
			wmap.insert(bd->maxTemp(), bd);
		}
		wlist = wmap.values();
	}

	if (order == Qt::AscendingOrder) {
		m_list << wlist;
	} else {
		for(; !wlist.isEmpty(); wlist.pop_back()) m_list << wlist.last();
	}

	m_sort_column = column;
	m_sort_order = order;
	reset();
}

//-----------------------------------------------------------------------------
QModelIndex KLevel1Model::index(int row, int column,
	const QModelIndex &parent) const
{
	return createIndex(row, column, m_list.at(row));
}

//-----------------------------------------------------------------------------
void KLevel1Model::setSelection(int slt) {
	if (m_selection == slt) return;
	m_selection = slt;

	m_list.clear();
	if (m_selection == 0) {
		m_list = m_root->list();

	} else if (m_selection == 1) {
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnThub*>(b)) m_list << b;

	} else if (m_selection == 2) {
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnTcpu*>(b)) m_list << b;

	} else if (m_selection <= 2 + m_root->nDevices()) {
		quint8 devid = m_root->devidByIndex(m_selection - 3);
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnBoard*>(b)->haddr().at(0) == devid) m_list << b;

	} else {
		int trgt = m_selection - (m_root->nDevices() + 3);
		foreach(AnBoard *b, m_root->list())
			if (b->status() == trgt) m_list << b;
	}

	m_rows = m_list.count();
	sort(m_sort_column, m_sort_order);
	
	reset();
}

void KLevel1Model::updated(AnBoard *brd)
{
	int row;
	if ( (row = m_list.indexOf(brd)) >= 0) {
		QModelIndex m1 = createIndex(row, 0, brd);
		QModelIndex m2 = createIndex(row, m_columns, brd);
		emit dataChanged(m1, m2);
	}
}
void KLevel1Model::updated()
{
	reset();
}
