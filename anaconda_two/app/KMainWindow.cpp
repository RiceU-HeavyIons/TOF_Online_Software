/*
 * $Id$
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QModelIndexList>
#include <QtCore/QModelIndex>

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFontMetrics>

#include "KMainWindow.h"
#include "KLevel2View.h"
#include "KTcpuView.h"

//-----------------------------------------------------------------------------
// Constructors

KMainWindow::KMainWindow(QWidget *parent) : QMainWindow(parent)
{

	QStatusBar *statusbar = statusBar();

	m_root = new AnRoot(0);

	QWidget *center = new QWidget();
	QGridLayout *gl = new QGridLayout(center);

	m_l1model = new KLevel1Model(m_root, this);
	m_l1view = new QTableView(this);
	m_l1view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_l1view->setSortingEnabled(true);
	m_l1view->setAlternatingRowColors(true);
	m_l1view->setModel(m_l1model);
	m_l1view->sortByColumn(1, Qt::AscendingOrder);
	m_l1view->setColumnWidth(0, 32);
	m_l1view->setColumnWidth(1, 64);
	m_l1view->setColumnWidth(3, 64);
	m_l1view->setColumnWidth(4, 64);

	QSizePolicy policy = m_l1view->sizePolicy();
	policy.setHorizontalStretch(2);
	policy.setVerticalStretch(1);	
	m_l1view->setSizePolicy(policy);

	createSelector();

	QHBoxLayout *hboxl = new QHBoxLayout();
	QLabel *label = new QLabel(tr("Mode:"));
	//  label->setAlignment(Qt::AlignLeft);
	m_combo = new QComboBox(this);
	m_combo->addItems(m_root->modeList());
	QObject::connect(m_combo, SIGNAL(activated(int)), this, SLOT(setMode(int)));

	// hboxl->addWidget(label);
	// hboxl->addWidget(m_combo);
	// hboxl->addStretch(0);
	//
	// gl->addLayout(hboxl, 0, 1, 1, 1);

	// gl->addWidget(label,      0, 1, 1, 1);
	// gl->addWidget(combo,      0, 2, 1, 1);

	gl->addWidget(m_selector, 1, 0, 1, 1);
	gl->addWidget(m_l1view,   1, 1, 2, 2);

//	gl->addWidget(tview, 1, 4, 2, 1);

	//  KLevel2View *l2view = new KLevel2View(this);
	//  l2view->setModel(model);
	//  l2view->setSelectionModel(view->selectionModel());
	//  gl->addWidget(l2view, 2, 5, 4, 1);

	m_l2view = new KLevel2View(tr("Detail View"), this);
	m_l2view->setSelectionModel(m_l1view->selectionModel());
	addDockWidget(Qt::RightDockWidgetArea, m_l2view);

	createActions();
	createToolBars();
	createMenus();


	QObject::connect(m_selector, SIGNAL(currentRowChanged(int)),
	                 this, SLOT(setSelection(int)));

	m_progress = new KProgressIndicator(m_root, this);
	// for (int i = 0; i < m_root->nAgents(); ++i) {
	//  	connect(m_root->agent(i), SIGNAL(progress(int, int)),
	// 		m_progress, SLOT(setProgress(int, int)));
	// 	connect(m_root->agent(i), SIGNAL(finished()),
	// 		this, SLOT(agentFinished()));
	// }

	// QObject::connect(m_root, SIGNAL(aboutStart()), this, SLOT(task_start()));
	QObject::connect(m_root, SIGNAL(finished()),  this, SLOT(agentFinished()));

	m_console = new KConsole(m_root, this);

	setCentralWidget(center);
}

KMainWindow::~KMainWindow()
{
	delete m_root;
}
//------------------------------------------------------------------------------
// Private Functions

void KMainWindow::createActions()
{
	m_InitAction = new QAction( QIcon(":images/user1.png"), tr("Init"), this);
	m_InitAction->setShortcut(tr("Ctrl+I"));
	m_InitAction->setEnabled(true);
	m_InitAction->setStatusTip(tr("Initialize Electronics"));
	m_InitAction->setToolTip(tr("Init"));
	QObject::connect(m_InitAction, SIGNAL(triggered()), this, SLOT(doInit()));

	m_ConfigAction = new QAction( QIcon(":images/config.png"), tr("Confg"), this);
	m_ConfigAction->setShortcut(tr("Ctrl+U"));
	m_ConfigAction->setEnabled(true);
	m_ConfigAction->setStatusTip(tr("Configure Electronics"));
	m_ConfigAction->setToolTip(tr("Config"));
	QObject::connect(m_ConfigAction, SIGNAL(triggered()), this, SLOT(doConfig()));
	
	m_ResetAction = new QAction( QIcon(":images/user3.png"), tr("Reset"), this);
	m_ResetAction->setShortcut(tr("Ctrl+R"));
	m_ResetAction->setStatusTip(tr("Reset Electronics"));
	m_ResetAction->setToolTip(tr("Reset"));
	QObject::connect(m_ResetAction, SIGNAL(triggered()), this, SLOT(doReset()));

	m_SyncAction = new QAction( QIcon(":images/sync.png"), tr("Sync"), this);
	m_SyncAction->setShortcut(tr("Ctrl+S"));
	m_SyncAction->setStatusTip(tr("Synchronize System Information"));
	m_SyncAction->setToolTip(tr("Sync"));
	QObject::connect(m_SyncAction, SIGNAL(triggered()),
						this, SLOT(doSync()));

	// m_ToggleToolbarAction = new QAction(tr("Toolbars"), this);
	// m_ToggleToolbarAction->setCheckable(true);
	// m_ToggleToolbarAction->setChecked(true);
	// QObject::connect(m_ToggleToolbarAction, SIGNAL(triggered()),
	// 					this, SLOT(toggleToolbar()));

	for (int i = 0 ; i < 4; ++i) {
		m_UserAction[i] = new QAction(
			QIcon(QString(":images/user%1.png").arg(i + 1) ),
			QString(tr("User Command %1")).arg(i + 1), this);
	}
	QObject::connect(m_UserAction[0], SIGNAL(triggered()), this, SLOT(doUser1()));
	QObject::connect(m_UserAction[1], SIGNAL(triggered()), this, SLOT(doUser2()));
	QObject::connect(m_UserAction[2], SIGNAL(triggered()), this, SLOT(doUser3()));
	QObject::connect(m_UserAction[3], SIGNAL(triggered()), this, SLOT(doUser4()));

	m_ToggleConsoleAction = new QAction(tr("Console"), this);
	m_ToggleConsoleAction->setShortcut(tr("Ctrl+J"));
	// m_ToggleConsoleAction->setCheckable(true);
	// m_ToggleConsoleAction->setChecked(false);
	QObject::connect(m_ToggleConsoleAction, SIGNAL(triggered()),
	                 this, SLOT(toggleConsole()));

	m_ToggleAutoSyncAction = new QAction(tr("Auto Sync"), this);
	m_ToggleAutoSyncAction->setCheckable(true);
	m_ToggleAutoSyncAction->setChecked(false);
	QObject::connect(m_ToggleAutoSyncAction, SIGNAL(triggered()),
	                 this, SLOT(toggleAutoSync()));

	setBusy(false);
}

//-----------------------------------------------------------------------------
void KMainWindow::createMenus()
{
	QMenu *menu;

	menu = menuBar()->addMenu(tr("Command"));
	menu->addAction(m_InitAction);
	menu->addAction(m_ConfigAction);
	menu->addAction(m_ResetAction);
	menu->addAction(m_SyncAction);
//	menu->addAction(m_comboAction);
	menu->addSeparator();
	menu->addAction(m_ToggleAutoSyncAction);

	menu = menuBar()->addMenu(tr("View"));
	menu->addAction(m_CommandToolbar->toggleViewAction());
	menu->addAction(m_l2view->toggleViewAction());
	menu->addAction(m_ToggleConsoleAction);
}

//-----------------------------------------------------------------------------
void KMainWindow::createToolBars()
{
	// Command Toolbar
	m_CommandToolbar = addToolBar(tr("Command Toolbar"));
	m_CommandToolbar->addAction(m_InitAction);
	m_CommandToolbar->addAction(m_ConfigAction);
	m_CommandToolbar->addAction(m_ResetAction);
	m_CommandToolbar->addAction(m_SyncAction);

	// Mode Toolbar
	QToolBar *bar = addToolBar(tr("Mode Toolbar"));
	// m_CommandToolbar->addSeparator();
	// QWidget *w = new QWidget();
	QLabel *lbl = new QLabel(tr("Mode:"));
	lbl->setMinimumHeight(32);
	bar->addWidget(lbl);
	// m_comboAction = bar->addWidget(m_combo);
	bar->addWidget(m_combo);

	// User Toolbar
	bar = addToolBar(tr("User Toolbar"));
	for (int i = 0; i < 4; ++i)
		bar->addAction(m_UserAction[i]);
}

//-----------------------------------------------------------------------------
void KMainWindow::createSelector()
{
	m_selector = new QListWidget(this);

	foreach(QListWidgetItem *itm, m_l1model->selectionList())
		m_selector->addItem(itm);

	m_selector->setSpacing(1);
	m_selector->setCurrentRow(0);
	m_selector->setAlternatingRowColors(true);
	QSizePolicy policy = m_selector->sizePolicy();
	policy.setHorizontalStretch(1);
//	policy.setVerticalPolicy(QSizePolicy::Fixed);
	m_selector->setSizePolicy(policy);
	int fh = QFontMetrics(m_selector->font()).height();
	m_selector->setMaximumHeight(m_selector->count()*(fh+2));

}

QList<AnBoard*> KMainWindow::selectedBoards()
{
	QList<AnBoard*> blist;
	if (m_l1view->selectionModel()->hasSelection()) {
		foreach(QModelIndex idx, m_l1view->selectionModel()->selectedRows()) {
			blist << static_cast<AnBoard*>(idx.internalPointer());
		}
	}

	return blist;
}

//------------------------------------------------------------------------------
// Public Slots

/**
 * Slot for initialization action
 */
void KMainWindow::doInit()
{
	setBusy(true);
	QList<AnBoard*> blist = selectedBoards();
	if (blist.count() > 0) {
		m_root->init(3, blist);
	} else {
		m_root->init(3);
	}
}

/**
 * Slot for configuration action
 */
void KMainWindow::doConfig()
{
	setBusy(true);
	QList<AnBoard*> blist = selectedBoards();
	if (blist.count() > 0) {
		m_root->config(3, blist);
	} else {
		m_root->config(3);
	}
}

//-----------------------------------------------------------------------------
void KMainWindow::doReset()
{
	setBusy(true);

	QList<AnBoard*> blist = selectedBoards();
	if (blist.count() > 0) {
		m_root->reset(3, blist);
	} else {
		m_root->reset(3);
	}
}

//-----------------------------------------------------------------------------
void KMainWindow::doSync()
{
	setBusy(true);

	QList<AnBoard*> blist = selectedBoards();
	if (blist.count() > 0) {
		m_root->sync(3, blist);
	} else {
		m_root->sync(3);
	}
}

//-----------------------------------------------------------------------------
void KMainWindow::doUser1()
{
	setBusy(true);
	m_root->doUserCmd(1);
}

//-----------------------------------------------------------------------------
void KMainWindow::doUser2()
{
	setBusy(true);
	m_root->doUserCmd(2);
}

//-----------------------------------------------------------------------------
void KMainWindow::doUser3()
{
	setBusy(true);
	m_root->doUserCmd(3);
}

//-----------------------------------------------------------------------------
void KMainWindow::doUser4()
{
	setBusy(true);
	m_root->doUserCmd(4);
}

//-----------------------------------------------------------------------------
void KMainWindow::toggleToolbar()
{
	m_CommandToolbar->setVisible(m_ToggleToolbarAction->isChecked());
}

//-----------------------------------------------------------------------------
void KMainWindow::toggleConsole()
{
	m_console->setVisible(m_console->isHidden());
//	if (m_ToggleConsoleAction->isChecked()) m_console->exec();

}

//-----------------------------------------------------------------------------
void KMainWindow::toggleAutoSync()
{
	if (m_ToggleAutoSyncAction->isChecked())
		m_root->startAutoSync();
	else
		m_root->stopAutoSync();
}

//-----------------------------------------------------------------------------
void KMainWindow::setMode(int i) {

	setBusy(true);
	m_root->setMode(i);
}

/**
 * When time consuming process is about to start, actions which trigger another
 * taks have to be disabled. This utility function takes can be used for the
 * purpose.
 *
 * @param sw    true for setting busy and false for unsetting busy
 */
void KMainWindow::setBusy(bool sw)
{
	m_busy = sw;
	m_InitAction->setEnabled(!sw);
	m_ConfigAction->setEnabled(!sw);
	m_ResetAction->setEnabled(!sw);
	m_SyncAction->setEnabled(!sw);
	for (int i = 0; i < 4; ++i) m_UserAction[i]->setEnabled(!sw);
	m_combo->setEnabled(!sw);
}

/**
 * Capture mouse release event
 */
void KMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	m_l1view->clearSelection();
}

//-----------------------------------------------------------------------------
// TODO It may be good idea get this kind of signal from AnRoot instead of agents
void KMainWindow::agentFinished()
{
	if (!m_root->isRunning()) setBusy(false);
}

void KMainWindow::setSelection(int select)
{
	m_l1view->clearSelection();
	m_l1model->setSelection(select);
}

