#include <QtGui/QApplication>
#include "KMainWindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  KMainWindow kmw;
  kmw.show();	
  return app.exec();
}
