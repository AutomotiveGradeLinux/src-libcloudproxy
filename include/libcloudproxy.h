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

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <json-c/json.h>
#include <systemd/sd-event.h>

extern "C"
{
#include <afb/afb-wsj1.h>
#include <afb/afb-ws-client.h>
}

class CloudProxyClient
{
public:
    using handler_func = std::function<void(json_object*)>;

    enum EventType {
        Event_Min,
        Event_SendMessageConfirmation = 1,
        Event_ReceivedMessage,
        Event_Max
    };


    CloudProxyClient();
    ~CloudProxyClient();

    CloudProxyClient(const CloudProxyClient &) = delete;
    CloudProxyClient &operator=(const CloudProxyClient &) = delete;

    int init(const int port, const std::string& token);

    int sendMessage(const std::string& data);

    void set_event_handler(enum EventType et, handler_func f);

    void on_event(void *closure, const char *event, struct afb_wsj1_msg *msg);

    int subscribe(const std::string& event_name);
    int unsubscribe(const std::string& event_name);

private:
    int call(const std::string& verb, struct json_object* arg);

private:
    static const std::vector<std::string> m_api_list;
    static const std::vector<std::string> m_event_list;

    std::map<EventType, handler_func> handlers;
    struct afb_wsj1_itf m_itf;
    struct afb_wsj1* m_websock{nullptr};
    sd_event* m_loop{nullptr};
    std::string m_uri;
};
