/*
 * $Id$
 *
 *  Created on: Nov 28, 2008
 *      Author: koheik
 */

#ifndef KConsole_H_
#define KConsole_H_

#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QMutex>

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>

#include "AnRoot.h"


class KConsole : public QDialog
{
	Q_OBJECT

public:
	KConsole(AnRoot *root, QWidget *parnt = 0);

public slots:
	void textChanged(const QString &text);
	void clicked(bool b);
	void debug_send(AnRdMsg);
	void debug_recv(AnRdMsg);

private:
	void cmd_debug (const QStringList &cmdl);
	
	void cmd_sync  (const QStringList &cmdl);
	void cmd_reset (const QStringList &cmdl);
	void cmd_config(const QStringList &cmdl);

	void cmd_expand(const QStringList &cmdl);

	void cmd_show  (const QStringList &cmdl);
	void cmd_read  (const QStringList &cmdl);
	void cmd_write (const QStringList &cmdl);

	void cmd_msg   (const QStringList &cmdl);

	void cmd_sql   (const QStringList &cmdl);
	void cmd_about (const QStringList &cmdl);

	int  parse_int(QString str) const;
	void tm_start();
	void tm_stop();

	QFont        m_font;

	QPushButton *m_button;
	QPushButton *m_clearb;
	QLineEdit   *m_ledit;
	QTextEdit   *m_tedit;
	QString      m_text;

	AnRoot      *m_root;

	QTime        m_start;
	QTime        m_stop;
	QMutex       m_mutex;
	int          m_nmsgs;
};
#endif /* KConsole_H_ */
