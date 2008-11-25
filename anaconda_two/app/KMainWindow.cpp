/*
 * KMainWindows.cpp
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
#include "Config.h"

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

	QSizePolicy policy = m_l1view->sizePolicy();
	policy.setHorizontalStretch(2);
	m_l1view->setSizePolicy(policy);

	createSelector();

	QHBoxLayout *hboxl = new QHBoxLayout();
	QLabel *label = new QLabel(tr("Mode:"));
	//  label->setAlignment(Qt::AlignLeft);
	QComboBox *combo = new QComboBox(this);
	QStringList str_list;
	str_list << "Stanby" << "minBias" << "ppFullEnergy" << "Test";
	combo->addItems(m_root->modeList());
	QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(setMode(int)));

	hboxl->addWidget(label);
	hboxl->addWidget(combo);
	hboxl->addStretch(0);

	gl->addLayout(hboxl, 0, 1, 1, 1);

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
	m_ResetAction->setStatusTip(tr("Reset System"));
	m_ResetAction->setToolTip(tr("Reset"));
	QObject::connect(m_ResetAction, SIGNAL(triggered()),
						this, SLOT(doReset()));

	m_SyncAction = new QAction( QIcon(":images/sync.png"), tr("Sync"), this);
	m_SyncAction->setStatusTip(tr("Synchronize System Information"));
	m_SyncAction->setToolTip(tr("Sync"));
	QObject::connect(m_SyncAction, SIGNAL(triggered()),
						this, SLOT(doSync()));

	m_ConfigAction = new QAction( QIcon(":images/new.png"), tr("Upload"), this);
	m_ConfigAction->setEnabled(true);
	m_ConfigAction->setStatusTip(tr("Upload Configurations to System"));
	m_ConfigAction->setToolTip(tr("Upload Configurations"));
	QObject::connect(m_ConfigAction, SIGNAL(triggered()),
						this, SLOT(doConfigure()));

	// m_ToggleToolbarAction = new QAction(tr("Toolbars"), this);
	// m_ToggleToolbarAction->setCheckable(true);
	// m_ToggleToolbarAction->setChecked(true);
	// QObject::connect(m_ToggleToolbarAction, SIGNAL(triggered()),
	// 					this, SLOT(doToggleToolbar()));
}

//-----------------------------------------------------------------------------
void KMainWindow::createMenus()
{
	QMenu *menu;

	menu = menuBar()->addMenu(tr("Command"));
	menu->addAction(m_ConfigAction);
	menu->addAction(m_ResetAction);
	menu->addAction(m_SyncAction);


	menu = menuBar()->addMenu(tr("View"));
	menu->addAction(m_CommandToolbar->toggleViewAction());
	menu->addAction(m_l2view->toggleViewAction());
}

//-----------------------------------------------------------------------------
void KMainWindow::createToolBars()
{
	m_CommandToolbar = addToolBar(tr("Command Toolbar"));
	m_CommandToolbar->addAction(m_ConfigAction);
	m_CommandToolbar->addAction(m_ResetAction);
	m_CommandToolbar->addAction(m_SyncAction);
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
	m_selector->setMaximumHeight(slist.count()*QFontMetrics(QApplication::font()).height());

}

//------------------------------------------------------------------------------
// Public Slots

void KMainWindow::doReset()
{
	m_root->reset();
	qDebug() << m_selector->size();
}
//-----------------------------------------------------------------------------
void KMainWindow::doSync()
{
	m_l1model->sync(1);
}
//-----------------------------------------------------------------------------
void KMainWindow::doConfigure()
{
	m_l1model->sync(1);
}

//-----------------------------------------------------------------------------
void KMainWindow::doToggleToolbar()
{
	m_CommandToolbar->setVisible(m_ToggleToolbarAction->isChecked());
}

//-----------------------------------------------------------------------------
void KMainWindow::setProgress1(int i) {
  bar1->setValue(i);
  updateBars();
}

//-----------------------------------------------------------------------------
void KMainWindow::setProgress2(int i) {
  bar2->setValue(i);
  updateBars();
}

//-----------------------------------------------------------------------------
void KMainWindow::updateBars() {
  bar0->setValue((bar1->value() + bar2->value())/2);
  if(bar1->value() == 100 && bar2->value() == 100) {
    progress_diag->close();
  }
}

//-----------------------------------------------------------------------------
void KMainWindow::setMode(int i) {

	m_root->setMode(i);

	if (i == 1) {
		QList<AnCanObject*> canlist;
		QModelIndexList lmi = m_l1view->selectionModel()->selectedRows();
		foreach(QModelIndex idx, lmi)
			canlist << static_cast<AnCanObject*>(idx.internalPointer());
//		m_root->Reset(canlist);
		qDebug() << canlist;
	}

  if (i == 3) {
    progress_diag = new QDialog(this);
    QGridLayout *gl = new QGridLayout(progress_diag);
    bar0 = new QProgressBar();
    bar1 = new QProgressBar();
    bar2 = new QProgressBar();
    QDialogButtonBox *box = new QDialogButtonBox(Qt::Horizontal);
    box->addButton(QDialogButtonBox::Cancel);
    bar0->setRange(0, 100);
    bar1->setRange(0, 100);
    bar2->setRange(0, 100);
    gl->addWidget(new QLabel("Total"), 0, 0, 1, 1);
    gl->addWidget(new QLabel("CanBus 1"), 1, 0, 1, 1);
    gl->addWidget(new QLabel("CanBus 2"), 2, 0, 1, 1);
    gl->addWidget(bar0, 0, 1, 1, 1);
    gl->addWidget(bar1, 1, 1, 1, 1);
    gl->addWidget(bar2, 2, 1, 1, 1);
    gl->addWidget(box, 3, 0, 1, 2);

    Config *conf;
    conf = new Config(60);
    QObject::connect(conf, SIGNAL(step(int)), this, SLOT(setProgress1(int)));
    conf->start();

    conf = new Config(40);
    QObject::connect(conf, SIGNAL(step(int)), this, SLOT(setProgress2(int)));
    conf->start();
    progress_diag->resize(400, 60);
    progress_diag->setWindowTitle("Now loading configuration....");
    progress_diag->exec();
  }

}
