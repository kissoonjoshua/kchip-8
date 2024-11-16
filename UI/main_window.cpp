#include "UI/main_window.h"
#include "UI/ui_main_window.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("KChip-8");
    setWindowIcon(QIcon(":/images/kchip-8.ico"));
    setCentralWidget(ui->openGLWidget);
    update_window_size();
    // Disable resize on drag
    statusBar()->setSizeGripEnabled(false);
    // Create render callback
    auto renderCallback = [&](const uint8_t *buffer) { ui->openGLWidget->update_buffer(buffer); };
    // Start emulation thread
    emuThread = new EmuThread(&cfg, renderCallback);
    emuThread->start();
}

MainWindow::~MainWindow()
{
    // wait for emulator thread to finish
    if(emuThread->isRunning()) {
        emuThread->terminate();
        emuThread->wait();
    }

    delete emuThread;
    delete ui;
}

void MainWindow::on_action_Load_ROM_triggered() {
    auto filepath = QFileDialog::getOpenFileName(this, tr("Open Chip-8 File"), "", tr("ch8 Files (*.ch8)"));
    if(!filepath.isEmpty()) {
        emuThread->set_rom(filepath.toStdString());
        if(emuThread->stopped()) {
            emuThread->start_emu();
        } else {
            on_action_Reset_triggered();
        }
    }
}
void MainWindow::on_action_Open_Recent_triggered() {}
void MainWindow::on_action_Configuration_triggered() {}
void MainWindow::on_action_Video_Settings_triggered() {}
void MainWindow::on_action_Audio_Settings_triggered() {}
void MainWindow::on_action_Input_Settings_triggered() {}
void MainWindow::on_action_Exit_triggered() { QApplication::quit(); }
void MainWindow::on_action_Pause_triggered() {
    if(emuThread->stopped()) {
        reset_pause_action();
        return;
    }
    emuThread->toggle_pause(ui->action_Pause->isChecked());
    ui->action_Pause->setText(ui->action_Pause->isChecked() ? "Unpause" : "Pause");
}
void MainWindow::on_action_Stop_triggered() {
    if(emuThread->stopped()) return;
    emuThread->stop();
    reset_pause_action();
}
void MainWindow::on_action_Reset_triggered() { 
    if(emuThread->stopped()) return;
    emuThread->reset(); 
    reset_pause_action();
}
void MainWindow::on_action_Save_Slot_1_triggered() {}
void MainWindow::on_action_Save_Slot_2_triggered() {}
void MainWindow::on_action_Save_Slot_3_triggered() {}
void MainWindow::on_action_Load_Slot_1_triggered() {}
void MainWindow::on_action_Load_Slot_2_triggered() {}
void MainWindow::on_action_Load_Slot_3_triggered() {}

// Update and constrain OpenGL window and main window size using config height and width
void MainWindow::update_window_size() {
    ui->openGLWidget->setFixedSize(QSize(cfg.displayWidth * cfg.scale, cfg.displayHeight * cfg.scale));
    setFixedSize(QSize(width(), height()));
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void MainWindow::reset_pause_action() {
    if(ui->action_Pause->isChecked()) {
        ui->action_Pause->setChecked(false);
        ui->action_Pause->setText("Pause");
    }
}

Config* MainWindow::get_config() { return &cfg; }