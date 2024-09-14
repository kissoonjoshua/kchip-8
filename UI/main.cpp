#include "Common/common.h"
#include "Emulator/emu_core.h"
#include "IO/io.h"
#include "main_window.h"
#include <QApplication> 
#include <QCommandLineParser>

using namespace Qt::StringLiterals;

int main(int argc, char **argv) {
  // Create Qt application
  QApplication app(argc, argv);
  QCoreApplication::setApplicationName("kchip-8");
  QCommandLineParser parser;

  // Commandline interface
  parser.setApplicationDescription(QObject::tr("C++ Chip-8 Emulator"));
  parser.addHelpOption();
  parser.addPositionalArgument("File"_L1, "Chip-8 rom to open");
  parser.process(app);

  // Get commandline args
  const QStringList &args = parser.positionalArguments();
  const QString &romName = args.count() > 0 ? args.at(0) : QString();

  // Initialize emulator state and set rom location
  Config cfg{};
  State state{};
  SDL sdl(state, cfg);  
  KChip8 emu(state, cfg, sdl);

  // Create main window
  // MainWindow window;
  // window.show();

  // If rom name is valid, run it 
  if(!romName.isEmpty()) {
    emu.set_rom(romName.toStdString());
    emu.run();
  }

  return 0;//app.exec();
}