#include "KMainWindow.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>

#include <QtCore/QModelIndexList>

#include <QtGui/QDialog>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QDialogButtonBox>

#include "KLevel2View.h"
#include "KTcpuView.h"

#include "DetailView.h"
#include "Config.h"

//-----------------------------------------------------------------------------

KMainWindow::KMainWindow(QWidget *parent) : QMainWindow(parent)
{
	
	setWindowTitle("Feeling Fuckin' Good");

	createActions();
	createMenus();
	createToolBars();

	QStatusBar *statusbar = statusBar();
	
	m_root = new AnRoot();
  	foreach(AnCanObject *cobj, m_root->list()) cobj->sync(0);

	QWidget *center = new QWidget();
	QGridLayout *gl = new QGridLayout(center);

	m_l1model = new TableViewModel();
	m_l1view = new QTableView(this);
	m_l1view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_l1view->setSortingEnabled(true);
	m_l1view->setModel(m_l1model);
	m_l1view->sortByColumn(0, Qt::AscendingOrder);

	QStringList str_list;
	str_list << "All" << "East" << "West" << "THUBs" << "Errors";
	QListWidget *listw = new QListWidget(this);
	listw->addItems(str_list);
	QSizePolicy policy = listw->sizePolicy();
	policy.setHorizontalStretch(1);
	policy.setVerticalPolicy(QSizePolicy::Minimum);
	listw->setSizePolicy(policy);

	policy = m_l1view->sizePolicy();
	policy.setHorizontalStretch(3);
	m_l1view->setSizePolicy(policy);

	QLabel *label = new QLabel("Operation Mode");
		//  label->setAlignment(Qt::AlignLeft);
	QComboBox *combo = new QComboBox(this);
	str_list.clear();
	str_list << "Stanby" << "minBias" << "ppFullEnergy" << "Test";
	combo->addItems(str_list);
	QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(setMode(int)));

	QComboBox *cb_actions = new QComboBox(this);
	str_list.clear();
	str_list << "Concigure" << "Reset" << "Sync";
	cb_actions->addItems(str_list);

	gl->addWidget(label, 0, 1, 1, 1);
	gl->addWidget(combo, 0, 2, 1, 1);
	gl->addWidget(cb_actions, 0, 3, 1, 1);
	gl->addWidget(listw, 1, 0, 1, 1);
	gl->addWidget(m_l1view, 1, 1, 1, 3);

	KTcpuView *tview = new KTcpuView(this);
	gl->addWidget(tview, 1, 4, 1, 1);

		//  KLevel2View *l2view = new KLevel2View(this);
		//  l2view->setModel(model);
		//  l2view->setSelectionModel(view->selectionModel());
		//  gl->addWidget(l2view, 2, 5, 4, 1);


	QObject::connect(
		m_l1view->selectionModel(),
		SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		tview,
		SLOT(currentRowChanged(const QModelIndex &, const QModelIndex &)));

	setCentralWidget(center);
}

//------------------------------------------------------------------------------
// Private Functions

void KMainWindow::createActions()
{
	m_ResetAction = new QAction( QIcon(":images/new.png"), tr("Reset"), this);
	m_ResetAction->setStatusTip(tr("Reset THUB / TRAY"));
	m_ResetAction->setToolTip(tr("Reset"));
	QObject::connect(m_ResetAction, SIGNAL(triggered()), this, SLOT(doReset()));

	m_SyncAction = new QAction( QIcon(":images/sync.png"), tr("Sync"), this);
	m_SyncAction->setStatusTip(tr("Synchronize THUB / TRAY data"));
	m_SyncAction->setToolTip(tr("Sync"));
	QObject::connect(m_SyncAction, SIGNAL(triggered()), this, SLOT(doSync()));

	m_ToggleToolbarAction = new QAction(tr("Toolbars"), this);
	m_ToggleToolbarAction->setCheckable(true);
	m_ToggleToolbarAction->setChecked(true);
	QObject::connect(m_ToggleToolbarAction, SIGNAL(triggered()), this, SLOT(doToggleToolbar()));
}

void KMainWindow::createMenus()
{
	QMenu *menu;

	menu = menuBar()->addMenu(tr("Command"));
	menu->addAction(m_ResetAction);
	menu->addAction(m_SyncAction);

	menu = menuBar()->addMenu(tr("View"));
	menu->addAction(m_ToggleToolbarAction);
}

void KMainWindow::createToolBars()
{
	m_CommandToolbar = addToolBar(tr("Command"));
	m_CommandToolbar->addAction(m_ResetAction);
	m_CommandToolbar->addAction(m_SyncAction);	
}

//------------------------------------------------------------------------------
// Public Slots

void KMainWindow::doReset()
{
	qDebug() << "KMainWIndow::doReset()";

  	foreach(AnCanObject *cobj, m_root->list())
		cobj->sync(1);
}

void KMainWindow::doSync()
{
	qDebug() << "KMainWIndow::doSync()";

  	foreach(AnCanObject *cobj, m_root->list())
		cobj->sync(1);
}

void KMainWindow::doToggleToolbar()
{
	qDebug() << "KMainWIndow::doToogleToolbar()";
	m_CommandToolbar->setVisible(m_ToggleToolbarAction->isChecked());
}


void KMainWindow::setProgress1(int i) {
  bar1->setValue(i);
  updateBars();
}

void KMainWindow::setProgress2(int i) {
  bar2->setValue(i);
  updateBars();
}

void KMainWindow::updateBars() {
  bar0->setValue((bar1->value() + bar2->value())/2);
  if(bar1->value() == 100 && bar2->value() == 100) {
    progress_diag->close();
  }
}


void KMainWindow::setMode(int i) {
  qDebug() << "mode: " << i;

	if(i == 1) {
		QList<AnCanObject*> canlist;
		QModelIndexList lmi = m_l1view->selectionModel()->selectedRows();
		foreach(QModelIndex idx, lmi)
			canlist << static_cast<AnCanObject*>(idx.internalPointer());
//		m_root->Reset(canlist);
		qDebug() << canlist;
	}

  if(i == 3) {
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
    conf = new Config(3600);
    QObject::connect(conf, SIGNAL(step(int)), this, SLOT(setProgress1(int)));
    conf->start();

    conf = new Config(3400);
    QObject::connect(conf, SIGNAL(step(int)), this, SLOT(setProgress2(int)));
    conf->start();
    progress_diag->resize(400, 60);
    progress_diag->setWindowTitle("Now loading configuration....");
    progress_diag->exec();
  }
}
