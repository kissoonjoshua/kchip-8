#pragma once
#include "Common/common.h"

#include <QObject>
#include <QThread>

class EmuThread : public QThread {
  Q_OBJECT
public:
  EmuThread(Config *config, RenderCallback callback);
protected:
  void run() override;
private:
  State state{};
  Config *cfg;
  RenderCallback rcb;
};