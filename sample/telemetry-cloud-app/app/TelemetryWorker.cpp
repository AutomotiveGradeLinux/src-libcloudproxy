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

#include "TelemetryWorker.h"
#include <QDebug>
#include <nlohmann/json.hpp>
#include <time.h>
#include <stdlib.h>

#include <libcloudproxy.h>

/*
left up: 56.327  43.981
right up:  56.327  44.03
right bottom:  56.3  44.03
left bottom:  56.3  43.981

start: 56.321674 44.006383
top: 56.329702 44.021352

diff -0.008028   -0.014969
coeff lon/lat = 1,8646


https://gps-coordinates.org/distance-between-coordinates.php
*/


static double drand(double min, double max)
{
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}


void TelemetryWorker::send()
{
    const double lat_top{56.329702};
    const double lon_lat_coef{1.8646};
    const double lat_step{0.001}, lon_step{lat_step*lon_lat_coef};

    if (direction == 0 && lat >= lat_top)
    {
        direction = 1;
        qDebug() << "Change direction to " << direction;
    }
    else if (direction == 1 && lat <= init_lat)
    {
        direction = 2;
        qDebug() << "Change direction to " << direction;
    }
    else if (direction == 2 && lat <= (init_lat - (lat_top - init_lat)))
    {
        direction = 3;
        qDebug() << "Change direction to " << direction;
    }
    else if (direction == 3 && lat >= init_lat)
    {
        direction = 0;
        qDebug() << "Change direction to " << direction;
    }


    if (direction == 0)
    {
        lat += lat_step;
        lon += lon_step;
    }
    else if (direction == 1)
    {
        lat -= lat_step;
        lon += lon_step;
    }
    else if (direction == 2)
    {
        lat -= lat_step;
        lon -= lon_step;
    }
    else if (direction == 3)
    {
        lat += lat_step;
        lon -= lon_step;
    }

    nlohmann::json jmsg{
        {"tele_ver", "1.0"},
        {"lat", lat},
        {"lon", lon},
        {"alt", (size_t)(alt + drand(0, 10))},
        {"speed", (size_t)(speed + drand(20, 30))},
        {"ts", time(nullptr)}
    };

    {
        //WARN: remove
        const auto& msg{jmsg.dump()};
        qDebug() << "TelemetryWorker: " << msg.c_str();
    }

    if (cloudproxyclient)
        qDebug() << "TelemetryWorker: cloud sendMessage result: " << cloudproxyclient->sendMessage(jmsg.dump());

}
