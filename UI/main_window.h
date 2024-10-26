#pragma once
#include "Emulator/emu_thread.h"
#include "Common/common.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public Q_SLOTS:
    void on_action_Load_ROM_triggered();
    void on_action_Open_Recent_triggered();
    void on_action_Configuration_triggered();
    void on_action_Video_Settings_triggered();
    void on_action_Audio_Settings_triggered();
    void on_action_Input_Settings_triggered();
    void on_action_Exit_triggered();
    void on_action_Pause_triggered();
    void on_action_Reset_triggered();
    void on_action_Save_Slot_1_triggered();
    void on_action_Save_Slot_2_triggered();
    void on_action_Save_Slot_3_triggered();
    void on_action_Load_Slot_1_triggered();
    void on_action_Load_Slot_2_triggered();
    void on_action_Load_Slot_3_triggered();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Config* get_config();

private:
    void update_window_size();

    Ui::MainWindow *ui;
    Config cfg{};
    EmuThread *emuThread;
};