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
    case Qt::Key_1: emuThread->key_press(SDLK_1, true); break;
    case Qt::Key_2: emuThread->key_press(SDLK_2, true); break;
    case Qt::Key_3: emuThread->key_press(SDLK_3, true); break;
    case Qt::Key_4: emuThread->key_press(SDLK_4, true); break;
    case Qt::Key_Q: emuThread->key_press(SDLK_q, true); break;
    case Qt::Key_W: emuThread->key_press(SDLK_w, true); break;
    case Qt::Key_E: emuThread->key_press(SDLK_e, true); break;
    case Qt::Key_R: emuThread->key_press(SDLK_r, true); break;
    case Qt::Key_A: emuThread->key_press(SDLK_a, true); break;
    case Qt::Key_S: emuThread->key_press(SDLK_s, true); break;
    case Qt::Key_D: emuThread->key_press(SDLK_d, true); break;
    case Qt::Key_F: emuThread->key_press(SDLK_f, true); break;
    case Qt::Key_Z: emuThread->key_press(SDLK_z, true); break;
    case Qt::Key_X: emuThread->key_press(SDLK_x, true); break;
    case Qt::Key_C: emuThread->key_press(SDLK_c, true); break;
    case Qt::Key_V: emuThread->key_press(SDLK_v, true); break;
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
    case Qt::Key_1: emuThread->key_press(SDLK_1, false); break;
    case Qt::Key_2: emuThread->key_press(SDLK_2, false); break;
    case Qt::Key_3: emuThread->key_press(SDLK_3, false); break;
    case Qt::Key_4: emuThread->key_press(SDLK_4, false); break;
    case Qt::Key_Q: emuThread->key_press(SDLK_q, false); break;
    case Qt::Key_W: emuThread->key_press(SDLK_w, false); break;
    case Qt::Key_E: emuThread->key_press(SDLK_e, false); break;
    case Qt::Key_R: emuThread->key_press(SDLK_r, false); break;
    case Qt::Key_A: emuThread->key_press(SDLK_a, false); break;
    case Qt::Key_S: emuThread->key_press(SDLK_s, false); break;
    case Qt::Key_D: emuThread->key_press(SDLK_d, false); break;
    case Qt::Key_F: emuThread->key_press(SDLK_f, false); break;
    case Qt::Key_Z: emuThread->key_press(SDLK_z, false); break;
    case Qt::Key_X: emuThread->key_press(SDLK_x, false); break;
    case Qt::Key_C: emuThread->key_press(SDLK_c, false); break;
    case Qt::Key_V: emuThread->key_press(SDLK_v, false); break;
    default: break;
    }
    QWidget::keyReleaseEvent(event);
}

void SDL::handle_input() {
  emuState->prevKeys = emuState->keys;

  while(SDL_PollEvent(&e)) {
    switch(e.type) {
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_1: emuState->keys |= 1 << 0x1; break;
          case SDLK_2: emuState->keys |= 1 << 0x2; break;
          case SDLK_3: emuState->keys |= 1 << 0x3; break;
          case SDLK_4: emuState->keys |= 1 << 0xC; break;
          case SDLK_q: emuState->keys |= 1 << 0x4; break;
          case SDLK_w: emuState->keys |= 1 << 0x5; break;
          case SDLK_e: emuState->keys |= 1 << 0x6; break;
          case SDLK_r: emuState->keys |= 1 << 0xD; break;
          case SDLK_a: emuState->keys |= 1 << 0x7; break;
          case SDLK_s: emuState->keys |= 1 << 0x8; break;
          case SDLK_d: emuState->keys |= 1 << 0x9; break;
          case SDLK_f: emuState->keys |= 1 << 0xE; break;
          case SDLK_z: emuState->keys |= 1 << 0xA; break;
          case SDLK_x: emuState->keys |= 1 << 0x0; break;
          case SDLK_c: emuState->keys |= 1 << 0xB; break;
          case SDLK_v: emuState->keys |= 1 << 0xF; break;
          default: break;
        };
        break;
      case SDL_KEYUP:
        switch(e.key.keysym.sym) {
          case SDLK_1: emuState->keys &= ~(1 << 0x1); break;
          case SDLK_2: emuState->keys &= ~(1 << 0x2); break;
          case SDLK_3: emuState->keys &= ~(1 << 0x3); break;
          case SDLK_4: emuState->keys &= ~(1 << 0xC); break;
          case SDLK_q: emuState->keys &= ~(1 << 0x4); break;
          case SDLK_w: emuState->keys &= ~(1 << 0x5); break;
          case SDLK_e: emuState->keys &= ~(1 << 0x6); break;
          case SDLK_r: emuState->keys &= ~(1 << 0xD); break;
          case SDLK_a: emuState->keys &= ~(1 << 0x7); break;
          case SDLK_s: emuState->keys &= ~(1 << 0x8); break;
          case SDLK_d: emuState->keys &= ~(1 << 0x9); break;
          case SDLK_f: emuState->keys &= ~(1 << 0xE); break;
          case SDLK_z: emuState->keys &= ~(1 << 0xA); break;
          case SDLK_x: emuState->keys &= ~(1 << 0x0); break;
          case SDLK_c: emuState->keys &= ~(1 << 0xB); break;
          case SDLK_v: emuState->keys &= ~(1 << 0xF); break;
          default: break;
        };
        break;
      default: break;
    }
  }
}