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

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QComboBox>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget{nullptr};
    QPushButton *startMsgButton{nullptr};
    QLabel *sendStatusLabel{nullptr};
    QLabel *stateLabel{nullptr};

    void setupUi(double scale_factor, QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1080*scale_factor, 1400*scale_factor);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));

        //QString style("color: white; font-size: 24px");
        QString style("color: white");

        startMsgButton = new QPushButton(centralWidget);
        startMsgButton->setObjectName(QString::fromUtf8("startMsgButton"));
        startMsgButton->setGeometry(QRect((540-242/2)*scale_factor, 20*scale_factor, 242*scale_factor, 64*scale_factor));
        startMsgButton->setStyleSheet(style);

        sendStatusLabel = new QLabel(centralWidget);
        sendStatusLabel->setObjectName(QString::fromUtf8("sendStatusLabel"));
        sendStatusLabel->setGeometry(QRect(40*scale_factor, 180*scale_factor, 1000*scale_factor, 80*scale_factor));
        sendStatusLabel->setWordWrap(false);
        sendStatusLabel->setAlignment(Qt::AlignTop);
        sendStatusLabel->setStyleSheet(style);

        stateLabel = new QLabel(centralWidget);
        stateLabel->setObjectName(QString::fromUtf8("stateLabel"));
        stateLabel->setGeometry(QRect(40*scale_factor, 280*scale_factor, 1000*scale_factor, 420*scale_factor));
        stateLabel->setWordWrap(true);
        stateLabel->setAlignment(Qt::AlignTop);
        stateLabel->setStyleSheet(style);

        MainWindow->setCentralWidget(centralWidget);

        QObject::connect(startMsgButton, SIGNAL(clicked()), MainWindow, SLOT(startMsgButtonClick()));

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        startMsgButton->setText(QApplication::translate("MainWindow", "Start", nullptr));
        sendStatusLabel->setText(QApplication::translate("MainWindow", "Connection status: None", nullptr));
        stateLabel->setText(QApplication::translate("MainWindow", "Cloud message:", nullptr));
    } // retranslateUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
