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
#include <nlohmann/json.hpp>


#include <libcloudproxy.h>
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

void MainWindow::updateState(QVariant recvMsg)
{
    if (!recvMsg.isValid())
        return;

    // expected format: {"message": "bla-bla"}
    std::string cloud_msg;
    try
    {
        const std::string& msg = recvMsg.toString().toStdString();
        if (msg.empty())
        {
            qDebug() << "Received message is empty";
        }
        else
        {
            const nlohmann::json& jmsg = nlohmann::json::parse(msg);
            cloud_msg = jmsg["message"].get<std::string>();
        }
    }
    catch (std::exception& ex)
    {
        qDebug() << "Can't parse incomming message: " << ex.what();
    }

    QString str = QString::asprintf("Cloud info: %s", cloud_msg.c_str());
    ui->stateLabel->setText(str);
}

void MainWindow::startMsgButtonClick()
{
    qDebug() << "MainWindow::startMsgButtonClick()";

    if (started)
        ui->startMsgButton->setText("Start");
    else
        ui->startMsgButton->setText("Stop");
    started = !started;

    if (worker)
    {
        worker->stop();
        worker.reset();
        qDebug() << "Worker destroyed";
    }

    if (started)
    {
        worker.reset(new TelemetryWorker(g_cloudproxyclient, this));
        connect(worker.get(), SIGNAL(timeout()), worker.get(), SLOT(send()));
        worker->start(1001);
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}
