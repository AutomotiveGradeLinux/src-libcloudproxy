/*
 * Copyright (C) 2020 MERA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <memory>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(double scale_factor, QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateStat(int confirm_good, int confirm_bad, int received, QVariant recvMsg);

private slots:
    void sendMsgButtonClick();
    void updateStat();

private:
    Ui::MainWindow *ui;
    int sendCount{0};
    int sendErrorCount{0};
    int confirmGood{0};
    int confirmBad{0};
    int receivedCount{0};
    QString lastSentMsg;
    QString lastReceivedMsg;
};

#endif // MAINWINDOW_H
