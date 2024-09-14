#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->openGLWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_Load_ROM_triggered() {}
void MainWindow::on_action_Open_Recent_triggered() {}
void MainWindow::on_action_Configuration_triggered() {}
void MainWindow::on_action_Video_Settings_triggered() {}
void MainWindow::on_action_Audio_Settings_triggered() {}
void MainWindow::on_action_Input_Settings_triggered() {}
void MainWindow::on_action_Exit_triggered() { QApplication::quit(); }
void MainWindow::on_action_Pause_triggered() {}
void MainWindow::on_action_Reset_triggered() {}
void MainWindow::on_action_Save_Slot_1_triggered() {}
void MainWindow::on_action_Save_Slot_2_triggered() {}
void MainWindow::on_action_Save_Slot_3_triggered() {}
void MainWindow::on_action_Load_Slot_1_triggered() {}
void MainWindow::on_action_Load_Slot_2_triggered() {}
void MainWindow::on_action_Load_Slot_3_triggered() {}