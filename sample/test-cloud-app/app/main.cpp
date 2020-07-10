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

#include <QtCore/QUrlQuery>
#include <QtGui/QGuiApplication>
#include <QtCore/QCommandLineParser>
#include <QScopedPointer>

#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

#include "libcloudproxy.h"

CloudProxyClient* g_cloudproxyclient{nullptr};


int main(int argc, char *argv[])
{
    QString graphic_role = QString("cloudapp") + QString(APP_INSTANCE_NUM);

    QApplication app(argc, argv);
    app.setDesktopFileName(graphic_role);
    QScopedPointer<MainWindow> window;

    QCommandLineParser parser;
    parser.addPositionalArgument("port", app.translate("main", "port for binding"));
    parser.addPositionalArgument("secret", app.translate("main", "secret for binding"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
    QStringList positionalArguments = parser.positionalArguments();

    if (positionalArguments.length() == 2)
    {
        int port = positionalArguments.takeFirst().toInt();
        QString secret = positionalArguments.takeFirst();
        QUrl bindingAddress;
        bindingAddress.setScheme(QStringLiteral("ws"));
        bindingAddress.setHost(QStringLiteral("localhost"));
        bindingAddress.setPort(port);
        bindingAddress.setPath(QStringLiteral("/api"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("token"), secret);
        bindingAddress.setQuery(query);

        std::string token = secret.toStdString();
        window.reset(new MainWindow(1));

        g_cloudproxyclient = new CloudProxyClient();
        g_cloudproxyclient->init(port, token.c_str());

        static int i_confirm_good{0}, i_confirm_bad{0}, i_recv{0};
        g_cloudproxyclient->set_event_handler(CloudProxyClient::Event_SendMessageConfirmation, [&window](json_object* object){
                qDebug("CloudProxyClient::Event_SendMessageConfirmation: object ptr %p", object);

                const char* str = object ? json_object_to_json_string_ext(object, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) : "<obj_null>";
                if (!str)
                    str = "<empty>";
                qDebug("Event_SendMessageConfirmation: %s", str);

                json_object *j_result;
                if(!json_object_object_get_ex(object, "result", &j_result))
                {
                    qDebug("Can't read confirmation result");
                    return;
                }

                int result = (int)json_object_get_boolean(j_result);

                if (result)
                    ++i_confirm_good;
                else
                    ++i_confirm_bad;

                window->updateStat(i_confirm_good, i_confirm_bad, i_recv, QVariant());
                qDebug("Application received confirmation result [good: %d, bad: %d]: %d", i_confirm_good, i_confirm_bad, result);
            });
        g_cloudproxyclient->set_event_handler(CloudProxyClient::Event_ReceivedMessage, [&window](json_object* object){
                qDebug("CloudProxyClient::Event_ReceivedMessage: object ptr %p", object);

                const char* str = object ? json_object_to_json_string_ext(object, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY) : "<obj_null>";
                if (!str)
                    str = "<empty>";
                qDebug("Event_SendMessageConfirmation: %s", str);

                json_object *event_data;
                const char* data_str{nullptr};
                if(!json_object_object_get_ex(object, "data", &event_data) ||
                   (data_str = json_object_get_string(event_data)) == nullptr)
                {
                    qDebug("Can't read event data");
                    return;
                }

                ++i_recv;
                window->updateStat(i_confirm_good, i_confirm_bad, i_recv, QString(data_str));
                qDebug("Application received data [count: %d,  good: %d, bad %d]: %s", i_recv, i_confirm_good, i_confirm_bad, data_str);
            });


        window->show();
    }

    return app.exec();
}
