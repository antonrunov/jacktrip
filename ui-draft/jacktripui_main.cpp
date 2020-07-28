#include "JTMainWindow.h"

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <memory>

int main( int argc, char* argv[] )
{
  QApplication a( argc, argv );
  setlocale( LC_NUMERIC, "C" );
  setlocale( LC_TIME, "C" );
  QPixmap logo("audionavt.png");
  a.setWindowIcon(logo);

  std::unique_ptr<JTMainWindow> w(new JTMainWindow());
  w->show();
  a.exec();
  return 0;
}
