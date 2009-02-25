/*
 * $Id$
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

#include "KProgressIndicator.h"
#include "KConsole.h"
#include "KSimpleWindow.h"
#include "KLevel1Model.h"
#include "KLevel2View.h"
#include "AnRoot.h"

// #define CMD_INIT   1
// #define CMD_CONFIG 1
#define CMD_RESET  1

class KMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	KMainWindow(QWidget *parent = 0);
	~KMainWindow();
	
	AnRoot *root() const { return m_root; }
	
	bool expertMode() const { return m_expertMode; }

public slots:
	void doInit();
	void doConfig();
	void doReset();
	void doSync();

	void doUser1();
	void doUser2();
	void doUser3();
	void doUser4();

	void toggleToolbar();
	void toggleConsole();
	void toggleAutoSync();

	void setMode(int i);
	void setExpertMode(bool em);

protected:
	void mouseReleaseEvent(QMouseEvent *event);

protected slots:
	void agentFinished();
	void setSelection(int select);


private:
	QList<AnBoard*> selectedBoards();

	void createActions();
	void createMenus();
	void createToolBars();
	void createSelector();

	void updateBars();

	void setBusy(bool sw = true);


	bool                m_busy;

	// Actions
#ifdef CMD_INIT
	QAction            *m_InitAction;
#endif
#ifdef CMD_CONFIG
	QAction            *m_ConfigAction;
#endif
	QAction            *m_ResetAction;
	QAction            *m_SyncAction;

	QAction            *m_ToggleToolbarAction;
	QAction            *m_ToggleConsoleAction;
	QAction            *m_ToggleAutoSyncAction;
	QAction            *m_UserAction[4];

	QToolBar           *m_CommandToolbar;
	QComboBox          *m_combo;

	AnRoot             *m_root;

	QDialog            *progress_diag;
	KProgressIndicator *m_progress;
	KConsole           *m_console;
	KSimpleWindow      *m_simple;

	//  models and views
	QTableView         *m_l1view;
	KLevel1Model       *m_l1model;
	QListWidget        *m_selector;

	KLevel2View        *m_l2view;
	
	bool                m_expertMode;
};
#endif
