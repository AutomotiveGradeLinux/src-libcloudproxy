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

#pragma once

#include <QTimer>

class CloudProxyClient;

class TelemetryWorker : public QTimer
{
    Q_OBJECT
public:
    TelemetryWorker(CloudProxyClient* cloudproxy, QObject* parent = nullptr)
        : QTimer(parent),
        cloudproxyclient{cloudproxy}
    {
    }

public slots:
    void send();

private:
    CloudProxyClient* cloudproxyclient{nullptr};

    int direction{0};
    const double init_lat{56.306};
    const double init_lon{44.070};
    double lat{init_lat};
    double lon{init_lon};
    double alt{300.0};
    double speed{500.0};
    uint32_t engine_rpm{3000};
    int32_t engine_temp{97};
};
