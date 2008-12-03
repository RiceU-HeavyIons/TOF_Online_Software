/*
 * $Id$
 *
 *  Created on: Nov 28, 2008
 *      Author: koheik
 */

#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

#include <QtCore/QDebug>
#include <QtCore/QList>

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>

#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

#include "AnAddress.h"
#include "KConsole.h"

//-----------------------------------------------------------------------------
KConsole::KConsole(AnRoot *root, QWidget *parent) : QDialog(parent),
	m_root(root)
{
	setWindowTitle(QString(tr("%1::Console")).arg(QApplication::applicationName()));
	resize(QSize(600, 400));

	QVBoxLayout *vbox = new QVBoxLayout(this);
	QHBoxLayout *hbox = new QHBoxLayout();

	m_button = new QPushButton("GO");
	m_button->setEnabled(false);

	m_clearb = new QPushButton("Clear");

	m_font  = QFont("Courier", 10);
	m_ledit = new QLineEdit();
	m_ledit->setFont(m_font);
	m_tedit = new QTextEdit();
	m_tedit->setReadOnly(true);
	m_tedit->setFont(m_font);

	hbox->addWidget(new QLabel("Command:"));
	hbox->addWidget(m_ledit);
	hbox->addWidget(m_button);
	hbox->addWidget(m_clearb);

	vbox->addLayout(hbox);
	vbox->addWidget(m_tedit);

	connect(m_ledit,  SIGNAL(textChanged(const QString &)),
	        this,     SLOT  (textChanged(const QString &)));

	connect(m_button, SIGNAL(clicked(bool)),
	        this,     SLOT  (clicked(bool)));

	connect(m_clearb, SIGNAL(clicked(bool)),
	        m_tedit,  SLOT  (clear()));
}


//-----------------------------------------------------------------------------
void KConsole::textChanged(const QString &text)
{
	m_text = text;
	m_button->setEnabled(text.length() != 0);
}


//-----------------------------------------------------------------------------
void KConsole::clicked(bool b)
{
	QStringList cmdl = m_text.split(QRegExp("\\s+"));
	while(cmdl.removeOne("")) { } // effectively trimming
	QString cmd = cmdl[0].toUpper();

	if (cmd == "HELP") {
		QString str;
		str += "HELP                               show this help message\n";
		str += "CLEAR                              clear console\n";

		str += "EXPAND  <addr>                     expand wild card address\n";
		str += "SHOW    <addr>                     show device information\n";
		str += "SYNC    <addr> <level=3>           synchronize devices at address\n";
		str += "RESET   <addr>                     reset device at address\n";
		str += "CONFIG  <addr>                     configure device at address\n";

		str += "READ    <addr> <par1> <par2> ...   read from hardware address\n";
		str += "WRITE   <addr> <par1> <par2> ...   write to hardware address \n";

		str += "*MSG    <arg1> <arg2> ...          lazy replacement of pc's command\n";
		str += "---";
		m_tedit->setText(str);
		m_ledit->clear();

	} else if (cmd == "CLEAR") {
		m_tedit->clear();
		m_ledit->clear();
	}
	else if (cmd == "EXPAND"|| cmd == "E") { cmd_expand(cmdl); }
	else if (cmd == "SHOW"  || cmd == "S") { cmd_show  (cmdl); }
	else if (cmd == "SYNC")                { cmd_sync  (cmdl); }
	else if (cmd == "RESET")               { cmd_reset (cmdl); }
	else if (cmd == "CONFIG")              { cmd_config(cmdl); }

	else if (cmd == "READ"  || cmd == "R") { cmd_read  (cmdl); }
	else if (cmd == "WRITE" || cmd == "W") { cmd_write (cmdl); }

	else if (cmd == "SELECT" || cmd == "INSERT" || cmd == "UPDATE") {
		cmd_sql(cmdl);
	}
	else if (cmd == "ABOUT")               { cmd_about (cmdl); }
	else {
		m_tedit->append(QString(tr("Syntax Error: %1: command unknown")).arg(cmd));
	}
}

//-----------------------------------------------------------------------------
void KConsole::cmd_sync(const QStringList& cmdl)
{
	tm_start();

	if (cmdl.count() > 1) {
		AnAddress addr(cmdl[1]);
		int level = (cmdl.count() > 2) ? cmdl[2].toInt() : 3;

		QStringList sl;
		foreach (AnAddress ad, m_root->expand(addr)) {
			AnCanObject* brd = m_root->find(ad);
			if (brd) {
				m_tedit->append(QString("synchronizing (%1)...").arg(brd->name()));
				brd->sync(level);
			} else
				m_tedit->append(QString("device is not found at %1").arg(ad.toString()));
		}

	} else {
		m_tedit->append("invalid parameters");
	}
	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_reset(const QStringList& cmdl)
{
	tm_start();
	if (cmdl.count() >= 2) {
		AnAddress addr(cmdl[1]);
		foreach (AnAddress ad, m_root->expand(addr)) {
			AnCanObject* brd = m_root->find(ad);
			if (brd) {
				m_tedit->append(QString("reseting (%1)...").arg(brd->name()));
				brd->reset();
			} else
				m_tedit->append(QString("device is not found at %1").arg(ad.toString()));
		}
	} else {
		m_tedit->append("invalid parameters");
	}
	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_config(const QStringList& cmdl)
{
	tm_start();
	if (cmdl.count() >= 2) {
		AnAddress addr(cmdl[1]);
		foreach (AnAddress ad, m_root->expand(addr)) {
			AnCanObject* brd = m_root->find(ad);
			if (brd) {
				m_tedit->append(QString("configuring (%1)...").arg(brd->name()));
				brd->config();
			} else
				m_tedit->append(QString("device is not found at %1").arg(ad.toString()));
		}
	} else {
		m_tedit->append("invalid parameters");
	}
	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_expand(const QStringList& cmdl)
{
	tm_start();
	if (cmdl.count() >= 2) {
		QStringList sl;
		foreach (AnAddress ad, m_root->expand(AnAddress(cmdl[1])))
			sl << ad.toString();
		m_tedit->append(sl.join("\n"));
	} else {
		m_tedit->append("invalid parameters");
	}
	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_show(const QStringList& cmdl)
{
	tm_start();
	if (cmdl.count() >= 2) {
		QStringList sl;
		foreach (AnAddress ad, m_root->expand(AnAddress(cmdl[1]))) {
			AnCanObject* brd = m_root->find(ad);
			if(brd)
				sl << brd->dump();
			else
				sl << "device is not found at " + ad.toString();
		}
		m_tedit->append(sl.join("\n\n") + "\n");
	} else {
		m_tedit->append("invalid parameters");
	}
	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_read(const QStringList& cmdl)
{
//	tm_start();

	int er = 0;

	if (cmdl.count() < 3) {
		++er;
		m_tedit->append("invalid parameters");
	}

	QList<AnAddress> alst;
	if (er == 0) {
		AnAddress addr(cmdl[1]);
		alst = m_root->expand(addr);
		if (alst.count() == 0) {
			m_tedit->append( QString(tr("invalid address: %1")).arg(addr.toString()) );
			++er;
		}
	}

	if (er == 0) {
		QList<quint8> dary;
		for (int i = 2; i < cmdl.count(); ++i)
			dary << parse_int( cmdl[i] );

		foreach (AnAddress ad, alst) {
			AnBoard* brd = dynamic_cast<AnBoard*>(m_root->find(ad));
			if (brd) {
				brd->msgr(dary);
			} else {
				m_tedit->append(QString("device is not found at %1")
					.arg(ad.toString()));
				++er;
			}
		}
	}

//	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_write(const QStringList& cmdl)
{
//	tm_start();

	int er = 0;

	if (cmdl.count() < 3) {
		++er;
		m_tedit->append("invalid parameters");
	}

	QList<AnAddress> alst;
	if (er == 0) {
		AnAddress addr(cmdl[1]);
		alst = m_root->expand(addr);
		if (alst.count() == 0) {
			m_tedit->append( QString(tr("invalid address: %1")).arg(addr.toString()) );
			++er;
		}
	}

	if (er == 0) {
		QList<quint8> dary;
		for (int i = 2; i < cmdl.count(); ++i)
			dary << parse_int( cmdl[i] );

		foreach (AnAddress ad, alst) {
			AnBoard* brd = dynamic_cast<AnBoard*>(m_root->find(ad));
			if (brd) {
				brd->msgw(dary);
			} else {
				m_tedit->append(QString("device is not found at %1")
					.arg(ad.toString()));
				++er;
			}
		}
	}

//	tm_stop();
	m_tedit->update();
}

//-----------------------------------------------------------------------------
void KConsole::cmd_sql(const QStringList& cmdl)
{
	tm_start();

	QStringList strlst;

	QSqlQuery qry;
	if (!qry.exec( cmdl.join(" ") )) {
		m_tedit->append(tr("SQL Error: ") + qry.lastError().databaseText());
		return;
	}

	QSqlRecord rec = qry.record();

	static QString sep("  ");
	static int mxx = 256;
	int fmx[rec.count()];
	int fty[rec.count()];

	// figure out maximum field length and type
	for (int i = 0; i < rec.count(); ++i) {
		fmx[i] = rec.fieldName(i).length();
		fty[i] = rec.field(i).type();
	}
	while (qry.next()) {
		for (int i = 0; i < rec.count(); ++i)
			if (fmx[i] < qry.value(i).toString().length())
				fmx[i] = qry.value(i).toString().length();
	}
	for(int i = 0; i < rec.count(); ++i) if (fmx[i] > mxx) fmx[i] = mxx;

	// print out headers
	for (int i = 0; i < rec.count(); ++i) {
		if (fty[i] == QVariant::String)
			strlst << rec.fieldName(i).leftJustified (fmx[i], ' ', true);
		else
			strlst << rec.fieldName(i).rightJustified(fmx[i], ' ', true);
	}
	m_tedit->append(strlst.join(sep));

	// print separators
	strlst.clear();
	for (int i = 0; i < rec.count(); ++i) {
		strlst << QString().leftJustified(fmx[i], '-');
	}
	m_tedit->append(strlst.join(sep));

	// print out records
	qry.first();
	do {
		strlst.clear();
		for (int i = 0; i < rec.count(); ++i) {
			if (fty[i] == QVariant::String)
				strlst << qry.value(i).toString().leftJustified (fmx[i], ' ', true);
			else
				strlst << qry.value(i).toString().rightJustified(fmx[i], ' ', true);
		}
		m_tedit->append(strlst.join(sep));
	} while(qry.next());

	tm_stop();
	m_tedit->update(); // update widget
}
//-----------------------------------------------------------------------------
void KConsole::cmd_about(const QStringList& cmdl)
{
	QStringList sl;
	sl << QString("%1 version %2")
		.arg(QApplication::applicationName())
		.arg(QApplication::applicationVersion());
	sl << QString(" - Copyright (C) 2008 Kohei Kajimoto. All rights reserved.");
	sl << QString(" - 'M' command syntax was originally designed by Dr. Schambach");
	sl << QString("   for his 'pcanloop' and 'pc' commands.");
	sl << QString(" - TCPU and TDIG information was obtained from 'TCPU Engineering");
	sl << QString("   User Manual', 'TDIG Engineering User Manual', 'HLP 3f Message");
	sl << QString("   Contents', and real devices by Blue Sky Electronics.");
	sl << QString(" - THUB information was obtained from 'HLP Messages for THUB' and");
	sl << QString("   private communication to Dr. Schambach.");

	m_tedit->setText(sl.join("\n"));
}
//-----------------------------------------------------------------------------
int KConsole::parse_int(QString str) const
{
	static const QRegExp rhex("^0x");
	static const QRegExp rbin("^0b");
	static const QRegExp roct("^0");

	int base = 10;
	if (str.contains(rhex)) {
		str.remove(rhex);
		base = 16;
	} else if (str.contains(rbin)) {
		str.remove(rbin);
		base = 2;
	} else if (str.contains(roct)) {
		base = 8;
	}
	return str.toInt(0, base);
}

void KConsole::tm_start() {
	m_start = QTime::currentTime();
}

void KConsole::tm_stop() {
	m_stop = QTime::currentTime();
	m_tedit->append( QString("-- start: %1, stop: %2, spent: %3 ms")
		.arg(m_start.toString())
		.arg(m_stop.toString())
		.arg(m_start.msecsTo(m_stop)) );
}
