#include "main_window.h"

#include <QApplication> 

using namespace Qt::StringLiterals;

int main(int argc, char **argv) {
  // Create Qt application
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("kchip-8");

  // Create main window
  MainWindow window;
  window.show();

  return app.exec();
}