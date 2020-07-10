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
    QPushButton *sendMsgButton{nullptr};
    QLabel *appNameLabel{nullptr};
    QLabel *statLabel{nullptr};

    void setupUi(double scale_factor, QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1080*scale_factor, 1400*scale_factor);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));

        //QString style("color: white; font-size: 24px");
        QString style("color: white");

        sendMsgButton = new QPushButton(centralWidget);
        sendMsgButton->setObjectName(QString::fromUtf8("sendMsgButton"));
        sendMsgButton->setGeometry(QRect((540-242/2)*scale_factor, 100*scale_factor, 242*scale_factor, 64*scale_factor));
        sendMsgButton->setStyleSheet(style);

        appNameLabel = new QLabel(centralWidget);
        appNameLabel->setObjectName(QString::fromUtf8("label"));
        appNameLabel->setGeometry(QRect(40*scale_factor, 20*scale_factor, 1000*scale_factor, 32*scale_factor));
        appNameLabel->setStyleSheet(style);

        statLabel = new QLabel(centralWidget);
        statLabel->setObjectName(QString::fromUtf8("label"));
        statLabel->setGeometry(QRect(40*scale_factor, 180*scale_factor, 1000*scale_factor, 420*scale_factor));
        statLabel->setWordWrap(true);
        statLabel->setAlignment(Qt::AlignTop);
        statLabel->setStyleSheet(style);

        MainWindow->setCentralWidget(centralWidget);

        QObject::connect(sendMsgButton, SIGNAL(clicked()), MainWindow, SLOT(sendMsgButtonClick()));

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        sendMsgButton->setText(QApplication::translate("MainWindow", "Send message", nullptr));
        QString name = QString("Test application ") + APP_INSTANCE_NUM;
        appNameLabel->setText(QApplication::translate("MainWindow", name.toUtf8().constData(), nullptr));
    } // retranslateUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
