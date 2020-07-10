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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include <unistd.h>
#include <sys/stat.h>


#include "libcloudproxy.h"
extern CloudProxyClient* g_cloudproxyclient;

MainWindow::MainWindow(double scale_factor, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(scale_factor, this);

    //AGL style
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    this->setStyleSheet(
        "background-image:url(:/images/AGL_HMI_Blue_Background_NoCar-01.png) 0 0 0 0 stretch stretch; background: transparent;" );
}

void MainWindow::updateStat(int confirm_good, int confirm_bad, int received, QVariant recvMsg)
{
    confirmGood = confirm_good;
    confirmBad = confirm_bad;
    receivedCount = received;

    if (recvMsg.isValid())
        lastReceivedMsg = recvMsg.toString();

    updateStat();
}

void MainWindow::updateStat()
{
    QString str = QString::asprintf("Sent: OK=%d, NOK=%d\nConfirmed: OK=%d, NOK=%d\nReceived=%d\n\n",
                                    sendCount, sendErrorCount, confirmGood, confirmBad, receivedCount);
    str.append("Last sent message:\n");
    str.append(lastSentMsg);
    str.append("\n\nLast received message:\n");
    str.append(lastReceivedMsg);
    ui->statLabel->setText(str);
    //QApplication::processEvents();
}

void MainWindow::sendMsgButtonClick()
{
    qDebug() << "MainWindow::sendMegButtonClick()";

    std::string msg = std::string{"{\"app_key\": \"app_value_"} + std::to_string(sendCount + sendErrorCount) + "\"}";
    int res = g_cloudproxyclient->sendMessage(msg);
    if (res == 0)
        ++sendCount;
    else
        ++sendErrorCount;

    lastSentMsg = msg.c_str();
    qDebug() << "cloud sendMessage result: " << res;
}


MainWindow::~MainWindow()
{
    delete ui;
}
