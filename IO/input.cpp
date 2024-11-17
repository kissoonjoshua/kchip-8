#include "UI/main_window.h"
#include "../UI/ui_main_window.h"
#include "IO/io.h"

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->isAutoRepeat()) {
      event->ignore();
      return;
    }
    switch(event->key()) {
    case Qt::Key_Escape: ui->action_Stop->trigger(); break;
    case Qt::Key_Space: ui->action_Pause->trigger(); break;
    case Qt::Key_Backspace: ui->action_Reset->trigger(); break;
    case Qt::Key_1: emuThread->key_press(SDL_SCANCODE_1, true); break;
    case Qt::Key_2: emuThread->key_press(SDL_SCANCODE_2, true); break;
    case Qt::Key_3: emuThread->key_press(SDL_SCANCODE_3, true); break;
    case Qt::Key_4: emuThread->key_press(SDL_SCANCODE_4, true); break;
    case Qt::Key_Q: emuThread->key_press(SDL_SCANCODE_Q, true); break;
    case Qt::Key_W: emuThread->key_press(SDL_SCANCODE_W, true); break;
    case Qt::Key_E: emuThread->key_press(SDL_SCANCODE_E, true); break;
    case Qt::Key_R: emuThread->key_press(SDL_SCANCODE_R, true); break;
    case Qt::Key_A: emuThread->key_press(SDL_SCANCODE_A, true); break;
    case Qt::Key_S: emuThread->key_press(SDL_SCANCODE_S, true); break;
    case Qt::Key_D: emuThread->key_press(SDL_SCANCODE_D, true); break;
    case Qt::Key_F: emuThread->key_press(SDL_SCANCODE_F, true); break;
    case Qt::Key_Z: emuThread->key_press(SDL_SCANCODE_Z, true); break;
    case Qt::Key_X: emuThread->key_press(SDL_SCANCODE_X, true); break;
    case Qt::Key_C: emuThread->key_press(SDL_SCANCODE_C, true); break;
    case Qt::Key_V: emuThread->key_press(SDL_SCANCODE_V, true); break;
    default: break;
    }
    QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if(event->isAutoRepeat()) {
      event->ignore();
      return;
    }
    switch(event->key()) {
    case Qt::Key_1: emuThread->key_press(SDL_SCANCODE_1, false); break;
    case Qt::Key_2: emuThread->key_press(SDL_SCANCODE_2, false); break;
    case Qt::Key_3: emuThread->key_press(SDL_SCANCODE_3, false); break;
    case Qt::Key_4: emuThread->key_press(SDL_SCANCODE_4, false); break;
    case Qt::Key_Q: emuThread->key_press(SDL_SCANCODE_Q, false); break;
    case Qt::Key_W: emuThread->key_press(SDL_SCANCODE_W, false); break;
    case Qt::Key_E: emuThread->key_press(SDL_SCANCODE_E, false); break;
    case Qt::Key_R: emuThread->key_press(SDL_SCANCODE_R, false); break;
    case Qt::Key_A: emuThread->key_press(SDL_SCANCODE_A, false); break;
    case Qt::Key_S: emuThread->key_press(SDL_SCANCODE_S, false); break;
    case Qt::Key_D: emuThread->key_press(SDL_SCANCODE_D, false); break;
    case Qt::Key_F: emuThread->key_press(SDL_SCANCODE_F, false); break;
    case Qt::Key_Z: emuThread->key_press(SDL_SCANCODE_Z, false); break;
    case Qt::Key_X: emuThread->key_press(SDL_SCANCODE_X, false); break;
    case Qt::Key_C: emuThread->key_press(SDL_SCANCODE_C, false); break;
    case Qt::Key_V: emuThread->key_press(SDL_SCANCODE_V, false); break;
    default: break;
    }
    QWidget::keyReleaseEvent(event);
}

void SDL::handle_input() {
  emuState->prevKeys = emuState->keys;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_KEYDOWN:
        switch(e.key.keysym.scancode) {
          case SDL_SCANCODE_1: emuState->keys |= 1 << 0x1; break;
          case SDL_SCANCODE_2: emuState->keys |= 1 << 0x2; break;
          case SDL_SCANCODE_3: emuState->keys |= 1 << 0x3; break;
          case SDL_SCANCODE_4: emuState->keys |= 1 << 0xC; break;
          case SDL_SCANCODE_Q: emuState->keys |= 1 << 0x4; break;
          case SDL_SCANCODE_W: emuState->keys |= 1 << 0x5; break;
          case SDL_SCANCODE_E: emuState->keys |= 1 << 0x6; break;
          case SDL_SCANCODE_R: emuState->keys |= 1 << 0xD; break;
          case SDL_SCANCODE_A: emuState->keys |= 1 << 0x7; break;
          case SDL_SCANCODE_S: emuState->keys |= 1 << 0x8; break;
          case SDL_SCANCODE_D: emuState->keys |= 1 << 0x9; break;
          case SDL_SCANCODE_F: emuState->keys |= 1 << 0xE; break;
          case SDL_SCANCODE_Z: emuState->keys |= 1 << 0xA; break;
          case SDL_SCANCODE_X: emuState->keys |= 1 << 0x0; break;
          case SDL_SCANCODE_C: emuState->keys |= 1 << 0xB; break;
          case SDL_SCANCODE_V: emuState->keys |= 1 << 0xF; break;
          default: break;
        };
        break;
      case SDL_KEYUP:
        switch(e.key.keysym.scancode) {
          case SDL_SCANCODE_1: emuState->keys &= ~(1 << 0x1); break;
          case SDL_SCANCODE_2: emuState->keys &= ~(1 << 0x2); break;
          case SDL_SCANCODE_3: emuState->keys &= ~(1 << 0x3); break;
          case SDL_SCANCODE_4: emuState->keys &= ~(1 << 0xC); break;
          case SDL_SCANCODE_Q: emuState->keys &= ~(1 << 0x4); break;
          case SDL_SCANCODE_W: emuState->keys &= ~(1 << 0x5); break;
          case SDL_SCANCODE_E: emuState->keys &= ~(1 << 0x6); break;
          case SDL_SCANCODE_R: emuState->keys &= ~(1 << 0xD); break;
          case SDL_SCANCODE_A: emuState->keys &= ~(1 << 0x7); break;
          case SDL_SCANCODE_S: emuState->keys &= ~(1 << 0x8); break;
          case SDL_SCANCODE_D: emuState->keys &= ~(1 << 0x9); break;
          case SDL_SCANCODE_F: emuState->keys &= ~(1 << 0xE); break;
          case SDL_SCANCODE_Z: emuState->keys &= ~(1 << 0xA); break;
          case SDL_SCANCODE_X: emuState->keys &= ~(1 << 0x0); break;
          case SDL_SCANCODE_C: emuState->keys &= ~(1 << 0xB); break;
          case SDL_SCANCODE_V: emuState->keys &= ~(1 << 0xF); break;
          default: break;
        };
        break;
      default: break;
    }
  }
}