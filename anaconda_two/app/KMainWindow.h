/*
 * KMainWindows.cpp
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */
#ifndef KMAINWINDOW_H_
#define KMAINWINDOW_H_ 1
#include <QtCore/QDebug>

#include <QtGui/QWidget>
#include <QtGui/QDialog>

#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QProgressBar>

#include <QtGui/QTableView>
#include <QtGui/QStandardItem>

#include <QtGui/QListWidget>
#include <QtGui/QListView>
#include <QtGui/QStringListModel>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QSizePolicy>


#include <QtGui/QAction>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QProgressBar>

#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>

#include "KLevel1Model.h"
#include "AnRoot.h"

class KMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	KMainWindow(QWidget *parent = 0);
	~KMainWindow();
public slots:
	void doReset();
	void doSync();
	void doConfigure();
	void doToggleToolbar();

	void setMode(int i);
	void setProgress1(int i);
	void setProgress2(int i);

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createSelector();

	void updateBars();

	// Actions
	QAction *m_ResetAction;
	QAction *m_SyncAction;
	QAction *m_ConfigAction;
	QAction *m_ToggleToolbarAction;
	
	QToolBar *m_CommandToolbar;

	AnRoot *m_root;
	
	QProgressBar *bar1;
	QProgressBar *bar2;
	QProgressBar *bar0;
	QDialog      *progress_diag;

	//  models and views
	QTableView     *m_l1view;
	KLevel1Model   *m_l1model;
	QListWidget    *m_selector;
	
	QDockWidget    *m_l2view;
};
#endif
