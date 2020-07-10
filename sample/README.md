# Test cloud applications #

## Description

The test-cloud-app and telemetry-cloud-app are examples of an agl-cloudproxy-service client.
They are used to demonstrate the possibilities of the cloudproxy service.

## Applications

* test-cloud-app: Simple application demonstrating the following possibilities:
    * Sending the messages to cloud over cloud proxy server
    * Receiving confirmations from cloud on the sent messages
    * Receiving messages from cloud


* telemetry-cloud-app: Application which sends the GPS coordinates to the cloud
    * Sending of the messages with telemetry to the cloud
    * Receiving of the messages from the cloud

Applications use CloudProxy client as high level interface for the messages sending and receiving

## CloudProxy client

CloudProxy client hides the communication with CloudProxy server. It incupsulates the following actions

* Connection to **cloudporxy** over wsj1 websocket
* Subscription to the event
* Event loop handling

**Note:** At the moment only two event types are supported
1. Event_SendMessageConfirmation
2. Event_ReceivedMessage

### Usage of a Cloudproxy client:

* Include the appropriate header to appliciation

 ```
 #include "cloudproxy-client/cloudproxyclient.h"
  ```

* Initialization of the CloudProxy client inside the application:

```
CloudProxyClient* g_cloudproxyclient{nullptr};

g_cloudproxyclient = new CloudProxyClient();
g_cloudproxyclient->init(port, token.c_str());

```

### Message handling from/to the CloudProxy server:


* Subsription to the Event_SendMessageConfirmation



```
g_cloudproxyclient->set_event_handler(CloudProxyClient::Event_SendMessageConfirmation, [](json_object* object){
                                json_object *j_result;
                                if(!json_object_object_get_ex(object, "result", &j_result))
                                {
                                        qDebug("Can't read confirmation result");
                                        return;
                                }

                                qDebug("Confirmation result %d", (int)json_object_get_boolean(j_result));
        });

   ```



* Subsription to the Event_SendMessageConfirmation

```
g_cloudproxyclient->set_event_handler(CloudProxyClient::Event_ReceivedMessage, [](json_object* object)
                                json_object *event_data;
                                const char* data_str{nullptr};
                                if(!json_object_object_get_ex(object, "data", &event_data) ||
                                    data_str = json_object_get_string(event_data)) == nullptr)
                                {
                                        qDebug("Can't read event data");
                                        return;
                                }

                                qDebug("Received data: %s", data_str);
        });
   ```


* Message sending:


```
qDebug() << "sendMessage result: " << g_cloudproxyclient->sendMessage("{\"app_key\": \"app_value1111\"}");
```