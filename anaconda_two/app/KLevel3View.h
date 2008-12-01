/*
 * $Id$
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#ifndef KLEVEL3VIEW_H_
#define KLEVEL3VIEW_H_
#include <QtCore/QPoint>
#include <QtCore/QModelIndex>

#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QScrollBar>

#include <QtGui/QAbstractItemView>
#include <QtGui/QItemSelection>
#include <QtGui/QItemSelectionModel>

class KLevel3View : public QAbstractItemView {
public:
  KLevel3View(QWidget *parent = 0);
  QModelIndex indexAt(const QPoint &point) const;
  void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
  QRect visualRect(const QModelIndex &index) const;


protected:
  int horizontalOffset() const;
  int verticalOffset() const;

  bool isIndexHidden(const QModelIndex &index) const;
  QModelIndex moveCursor(CursorAction cursorAction,
                         Qt::KeyboardModifiers modifiers);
  void setSelection(const QRect &rect,
                    QItemSelectionModel::SelectionFlags flags);
  QRegion visualRegionForSelection(const QItemSelection &selection) const;

protected slots:
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected);

private:

  QGroupBox *m_mainBox;
  QLabel    *m_label;
  QLabel    *m_chipid;
  QLabel    *m_firmware;
  QLabel    *m_escr;

  QGridLayout *box_gl;
  void updateAll();
  void setDefault();

};

#endif /* KLEVEL3VIEW_H_ */
