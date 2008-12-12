/*
 * $Id$
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QModelIndexList>

#include <QtGui/QFontMetrics>

#include <QtGui/QApplication>
#include <QtGui/QDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>

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
	m_l1view->setModel(m_l1model);
	m_l1view->sortByColumn(1, Qt::AscendingOrder);
	m_l1view->setColumnWidth(0, 32);
	m_l1view->setColumnWidth(1, 64);
	m_l1view->setColumnWidth(3, 64);
	m_l1view->setColumnWidth(4, 64);

	QSizePolicy policy = m_l1view->sizePolicy();
	policy.setHorizontalStretch(2);
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

	m_l2view = new KLevel2View("Information", this);
	addDockWidget(Qt::RightDockWidgetArea, m_l2view);

	createActions();
	createToolBars();
	createMenus();

	QObject::connect(
		m_l1view->selectionModel(),
		SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		m_l2view,
		SLOT(currentRowChanged(const QModelIndex &, const QModelIndex &)));

	QObject::connect(m_selector, SIGNAL(currentRowChanged(int)),
						m_l1model, SLOT(setSelection(int)));

	m_progress = new KProgressIndicator(m_root, this);
	// for (int i = 0; i < m_root->nAgents(); ++i) {
	//  	connect(m_root->agent(i), SIGNAL(progress(int, int)),
	// 		m_progress, SLOT(setProgress(int, int)));
	// 	connect(m_root->agent(i), SIGNAL(finished()),
	// 		this, SLOT(agentFinished()));
	// }

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
	m_ResetAction = new QAction( QIcon(":images/undo.png"), tr("Reset"), this);
	m_ResetAction->setShortcut(tr("Ctrl+R"));
	m_ResetAction->setStatusTip(tr("Reset System"));
	m_ResetAction->setToolTip(tr("Reset"));
	QObject::connect(m_ResetAction, SIGNAL(triggered()),
						this, SLOT(doReset()));

	m_SyncAction = new QAction( QIcon(":images/sync.png"), tr("Sync"), this);
	m_SyncAction->setShortcut(tr("Ctrl+S"));
	m_SyncAction->setStatusTip(tr("Synchronize System Information"));
	m_SyncAction->setToolTip(tr("Sync"));
	QObject::connect(m_SyncAction, SIGNAL(triggered()),
						this, SLOT(doSync()));

	m_ConfigAction = new QAction( QIcon(":images/new.png"), tr("Upload"), this);
	m_ConfigAction->setShortcut(tr("Ctrl+U"));
	m_ConfigAction->setEnabled(true);
	m_ConfigAction->setStatusTip(tr("Upload Configurations to System"));
	m_ConfigAction->setToolTip(tr("Upload Configurations"));
	QObject::connect(m_ConfigAction, SIGNAL(triggered()),
	                 this, SLOT(doConfigure()));

	// m_ToggleToolbarAction = new QAction(tr("Toolbars"), this);
	// m_ToggleToolbarAction->setCheckable(true);
	// m_ToggleToolbarAction->setChecked(true);
	// QObject::connect(m_ToggleToolbarAction, SIGNAL(triggered()),
	// 					this, SLOT(toggleToolbar()));

	m_ToggleConsoleAction = new QAction(tr("Consle"), this);
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
	m_CommandToolbar = addToolBar(tr("Command Toolbar"));
	m_CommandToolbar->addAction(m_ConfigAction);
	m_CommandToolbar->addAction(m_ResetAction);
	m_CommandToolbar->addAction(m_SyncAction);

	QToolBar *bar = addToolBar(tr("Mode Toolbar"));
	// m_CommandToolbar->addSeparator();
	// QWidget *w = new QWidget();
	bar->addWidget(new QLabel(tr("Mode:")));
	// m_comboAction = bar->addWidget(m_combo);
	bar->addWidget(m_combo);
}

//-----------------------------------------------------------------------------
void KMainWindow::createSelector()
{
	QStringList slist = m_l1model->selectionList();
	m_selector = new QListWidget(this);
	m_selector->addItems(slist);
	m_selector->setCurrentRow(0);

	QSizePolicy policy = m_selector->sizePolicy();
	policy.setHorizontalStretch(1);
	policy.setVerticalPolicy(QSizePolicy::Fixed);
	m_selector->setSizePolicy(policy);
	// m_selector->setMaximumHeight(
	// 	1.2*slist.count()*QFontMetrics(QApplication::font()).height());

}

//------------------------------------------------------------------------------
// Public Slots

void KMainWindow::doReset()
{
	setBusy(true);
	m_root->reset();
	m_progress->start();
}
//-----------------------------------------------------------------------------
void KMainWindow::doSync()
{
	setBusy(true);
	m_progress->show();
	m_l1model->sync(1);
	m_progress->start();
}
//-----------------------------------------------------------------------------
void KMainWindow::doConfigure()
{
	setBusy(true);
	m_root->config();
	m_progress->start();
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
	m_ResetAction->setEnabled(!sw);
	m_SyncAction->setEnabled(!sw);
	m_ConfigAction->setEnabled(!sw);
	m_combo->setEnabled(!sw);
}

//-----------------------------------------------------------------------------
// TODO It may be good idea get this kind of signal from AnRoot instead of agents
void KMainWindow::agentFinished()
{
	if (!m_root->isRunning()) setBusy(false);
}
