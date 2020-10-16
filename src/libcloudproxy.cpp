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

#include <thread>
#include <algorithm>
#include <condition_variable>

#include <string.h>

#include "libcloudproxy.h"
#include "hmi-debug.h"

static const char API_name[] = "cloudproxy";

const std::vector<std::string> CloudProxyClient::m_api_list {
    std::string("ping"),
    std::string("sendMessage")
};

const std::vector<std::string> CloudProxyClient::m_event_list {
    std::string("sendMessageConfirmation"),
    std::string("receivedMessage")
};


bool CloudType::isSupported(const char* type)
{
    return (type && (strcmp(Azure, type) == 0 || strcmp(Aws, type) == 0));
}

static void event_loop_run(sd_event* loop)
{
    sd_event_loop(loop);
    sd_event_unref(loop);
}

static void on_hangup(void *closure, afb_wsj1 *wsj)
{
    HMI_DEBUG("cloudproxyclient", "called");
    (void)closure;
    (void)wsj;
}

static void on_call(void *closure, const char *api, const char *verb, afb_wsj1_msg *msg)
{
    HMI_ERROR("cloudproxyclient", "this method should not be called");
    (void)closure;
    (void)api;
    (void)verb;
    (void)msg;
}

static void on_event(void* closure, const char* event, afb_wsj1_msg *msg)
{
    HMI_DEBUG("cloudproxyclient", "event [%s]", (event ? event: ""));
    (void)closure;

    static_cast<CloudProxyClient*>(closure)->on_event(nullptr, event, msg);
}

static void on_reply(void *closure, afb_wsj1_msg *msg)
{
    HMI_DEBUG("cloudproxyclient", "called");
    (void)closure;
    (void)msg;
}


CloudProxyClient::CloudProxyClient()
{
}

CloudProxyClient::~CloudProxyClient()
{
    if(m_websock)
    {
        afb_wsj1_unref(m_websock);
    }
    if(m_loop)
    {
        sd_event_exit(m_loop, 0);
    }
}

int CloudProxyClient::init(const int port, const std::string& token)
{
    int ret = 0;
    if(port <= 0 && token.size() == 0)
    {
        HMI_ERROR("cloudproxyclient","port and token should be > 0, Initial port and token uses.");
        return -1;
    }

    {
        m_loop = nullptr;
        int ret = sd_event_new(&m_loop);
        if(ret < 0)
        {
            HMI_ERROR("cloudproxyclient","Failed to create event loop");
            return -1;
        }

        {
            // enforce context to avoid initialization/goto error
            std::thread th(event_loop_run, m_loop);
            th.detach();
        }

        /* Initialize interface from websocket */
        m_itf.on_hangup = ::on_hangup;
        m_itf.on_call = ::on_call;
        m_itf.on_event = ::on_event;

        m_uri += "ws://localhost:" + std::to_string(port) + "/api?token=" + token;

        m_websock = afb_ws_client_connect_wsj1(m_loop, m_uri.c_str(), &m_itf, this);
        if(!m_websock)
        {
            HMI_ERROR("cloudproxyclient","Failed to create websocket connection");
            return -1;
        }
    }

    HMI_DEBUG("cloudproxyclient", "Initialized");

    return ret;
}


int CloudProxyClient::call_sync(const std::string& verb, json_object* arg)
{
    std::lock_guard<std::mutex> l(m_mutex);

    int ret{-1};
    if(!m_websock)
    {
        return -1;
    }

    if (verb.empty() || m_api_list.end() == std::find(m_api_list.begin(), m_api_list.end(), verb))
    {
        HMI_ERROR("cloudproxyclient","verb [%s] doesn't exit", verb.c_str());
        return -1;
    }

    std::condition_variable cv;
    std::mutex cv_m;
    bool signal{false};

    std::function<void(afb_wsj1_msg *msg)> local_on_reply = [&ret, &cv, &cv_m, &signal](afb_wsj1_msg *msg)->void
    {
        std::unique_lock<std::mutex> lock(cv_m);

        if (msg && afb_wsj1_msg_is_reply_ok(msg))
            ret = 0;
        else
            ret = -1;

        signal = true;
        cv.notify_all();
    };

    int call_ret = afb_wsj1_call_j(m_websock, API_name, verb.c_str(), arg,
                                   [](void *closure, afb_wsj1_msg *msg)->void
                                   {
                                       auto *cb = reinterpret_cast<std::function<void(afb_wsj1_msg *msg)> *>(closure);
                                       if (!cb || !(*cb))
                                       {
                                           HMI_ERROR("cloudproxyclient", "Can't process reply: invalid callback");
                                           return;
                                       }
                                       (*cb)(msg);
                                   },
                                   &local_on_reply);

    if (call_ret == 0)
    {
        // Wait a signal if not received yet:
        std::unique_lock<std::mutex> lock(cv_m);
        if (!signal)
            cv.wait(lock);
    }
    else
    {
        ret = call_ret;
    }

    if (ret != 0)
    {
        HMI_ERROR("cloudproxyclient", "Failed to call verb:%s", verb.c_str());
    }

    return ret;
}


int CloudProxyClient::sendMessage(const std::string& cloud_type, const std::string& data)
{
    if(!m_websock || !CloudType::isSupported(cloud_type.c_str()))
        return -1;

    json_object* j_obj = json_object_new_object();
    json_object_object_add(j_obj, "cloud_type", json_object_new_string(cloud_type.c_str()));
    json_object_object_add(j_obj, "data", json_object_new_string(data.c_str()));

    return this->call_sync("sendMessage", j_obj);
}

void CloudProxyClient::set_event_handler(enum EventType et, handler_func f)
{
    if (et > Event_Min && et < Event_Max)
    {
        switch (et)
        {
            case Event_SendMessageConfirmation:
                this->subscribe(CloudProxyClient::m_event_list[0]);
                break;
            case Event_ReceivedMessage:
                this->subscribe(CloudProxyClient::m_event_list[1]);
                break;
            default:
                break;
        }

        this->m_handlers[et] = std::move(f);
    }
}

int CloudProxyClient::subscribe(const std::string& event_name)
{
    if(!m_websock)
        return -1;

    json_object* j_obj = json_object_new_object();
    json_object_object_add(j_obj, "event", json_object_new_string(event_name.c_str()));

    int ret = afb_wsj1_call_j(m_websock, API_name, "subscribe", j_obj, ::on_reply, this);
    if (ret < 0)
        HMI_ERROR("cloudproxyclient", "subscribe filed for '%s'", event_name.c_str());

    return ret;
}

int CloudProxyClient::unsubscribe(const std::string& event_name)
{
    if(!m_websock)
        return -1;

    json_object* j_obj = json_object_new_object();
    json_object_object_add(j_obj, "event", json_object_new_string(event_name.c_str()));

    int ret = afb_wsj1_call_j(m_websock, API_name, "unsubscribe", j_obj, ::on_reply, this);
    if (ret < 0)
        HMI_ERROR("cloudproxyclient", "unsubscribe failed for '%s'", event_name.c_str());

    return ret;
}

void CloudProxyClient::on_event(void *closure, const char *event, afb_wsj1_msg *msg)
{
    HMI_DEBUG("cloudproxyclient", "event: (%s) msg: (%s).", event, afb_wsj1_msg_object_s(msg));
    (void) closure;

    if (strstr(event, API_name) == nullptr)
        return;


    json_object* ev_contents = afb_wsj1_msg_object_j(msg);
    json_object *json_data;
    if(!json_object_object_get_ex(ev_contents, "data", &json_data))
    {
        HMI_ERROR("cloudproxyclient", "got ev_contents error.");
        return;
    }

    const char* event_type = nullptr;
    json_object *json_event_type;
    if(!json_object_object_get_ex(json_data, "type", &json_event_type) ||
       (event_type = json_object_get_string(json_event_type)) == nullptr)
    {
        HMI_ERROR("cloudproxyclient", "event_type is null.");
        return;
    }

    const std::string et{event_type};
    if (CloudProxyClient::m_event_list[0] == et)
    {
        auto i = this->m_handlers.find(Event_SendMessageConfirmation);
        if (i != this->m_handlers.end())
            i->second(json_data);
    }
    else if (CloudProxyClient::m_event_list[1] == et)
    {
        auto i = this->m_handlers.find(Event_ReceivedMessage);
        if (i != this->m_handlers.end())
            i->second(json_data);
    }
}
