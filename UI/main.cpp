#include "main_window.h"

#include <QApplication> 
#include <QCommandLineParser>

using namespace Qt::StringLiterals;

int main(int argc, char **argv) {
  // Create Qt application
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("kchip-8");
  QCommandLineParser parser;

  // CLI
  parser.setApplicationDescription("C++ Chip-8 Emulator"_L1);
  parser.addHelpOption();
  parser.addPositionalArgument("File"_L1, "Chip-8 rom to open");
  parser.process(app);

  // Get CLI args
  const QStringList &args = parser.positionalArguments();
  const QString &romName = args.count() > 0 ? args.at(0) : QString();

  // Create main window
  MainWindow window;
  window.show();

  // If rom name is valid, run it 
  // if(!romName.isEmpty()) {
  //   emu.set_rom(romName.toStdString());
  //   emu.run();
  // }

  return app.exec();
}