#include "dlnactrlpoint.h"
#include "dlnacommon.h"
#include "controlmanager.h"

#include <QDebug>

UpnpClient_Handle ctrlpt_handle = -1;
ithread_mutex_t DeviceListMutex;
int default_timeout = 1801;

void AvStateUpdate(const char *UDN, IXML_Document *ChangedVariables)
{
    IXML_NodeList *properties;
    IXML_NodeList *variables;
    IXML_Element *property;
    IXML_Element *variable;
    long unsigned int length;
    long unsigned int length1;
    long unsigned int i;
    char *tmpstate = NULL;

    /* Find all of the e:property tags in the document */
    properties = ixmlDocument_getElementsByTagName(ChangedVariables,"e:property");
    if (properties) {
        length = ixmlNodeList_length(properties);
        for (i = 0; i < length; i++) {
            /* Loop through each property change found */
            property = (IXML_Element *)ixmlNodeList_item(properties, i);
            /* For each variable name in the state table,
             * check if this is a corresponding property change */
            variables = ixmlElement_getElementsByTagName(
                property, "LastChange");
            /* If a match is found, extract
             * the value, and update the state table */
            if (variables) {
                length1 = ixmlNodeList_length(variables);
                if (length1) {
                    variable = (IXML_Element *)
                        ixmlNodeList_item(variables, 0);
                    tmpstate = getElementValue(variable);
                    if (strcmp(UDN,CONTROL->getSelUDN()) == 0 && tmpstate) {
                        CONTROL->analyDeviceLastChange(tmpstate);
                    }
                    ixmlNodeList_free(variables);
                    variables = NULL;
                }
            }
        }
        ixmlNodeList_free(properties);
    }
    return;
    UDN = UDN;
}

int AvCtrlPointDeleteNode(AvDevice devNode, bool isVerify)
{
    int rc, service;

    if (devNode.UDN[0] == '\0') {
        return AV_ERROR;
    }

    for (service = 0; service < SERVICE_SERVCOUNT; service++) {
        /*
           If we have a valid control SID, then unsubscribe
         */
        if (strcmp(devNode.ServiceList[service].SID, "") != 0) {
            rc = UpnpUnSubscribe(ctrlpt_handle,
                         devNode.ServiceList[service].
                         SID);
        }
    }

    CONTROL->delDevice(devNode.UDN,isVerify);
//    free(node);
//    node = NULL;

    return AV_SUCCESS;
}

int AvCtrlPointRemoveDevice(const char *UDN)
{
    ithread_mutex_lock(&DeviceListMutex);
    if (CONTROL->devInfoList.size() > 0) {
        for(int i=0;i<CONTROL->devInfoList.size();i++) {
            if(0 == strcmp(CONTROL->devInfoList.at(i).UDN, UDN)) {
                AvCtrlPointDeleteNode(CONTROL->devInfoList.at(i));
                CONTROL->devInfoList.removeAt(i);
                break;
            }
        }
    }

    ithread_mutex_unlock(&DeviceListMutex);
    return AV_SUCCESS;
}

int AvCtrlPointRemoveAll()
{
    ithread_mutex_lock(&DeviceListMutex);

    if (CONTROL->devInfoList.size() > 0) {
        for(int i=0;i<CONTROL->devInfoList.size();i++) {
            AvCtrlPointDeleteNode(CONTROL->devInfoList.at(i));
        }
        CONTROL->devInfoList.clear();
    }

    ithread_mutex_unlock(&DeviceListMutex);
    return AV_SUCCESS;
}

int AvCtrlPointSendActionArgs(const char *UDN, int service, const char *actionName, QList<actionKey_Value> ages)
{
    AvDevice devnode;
    IXML_Document *actionNode = NULL;
    int rc = AV_SUCCESS;

    ithread_mutex_lock(&DeviceListMutex);

    devnode = AvCtrlPointGetDevice(UDN);
    if (devnode.UDN[0] != '\0') {
        if (0 == ages.count()) {
            actionNode = UpnpMakeAction(actionName, CONTROL->AvServiceType[service],0, NULL);
        } else {
            for(int i=0;i<ages.count();i++) {
                if (UpnpAddToAction
                    (&actionNode, actionName,
                     CONTROL->AvServiceType[service], ages[i].key,
                     ages[i].value) != UPNP_E_SUCCESS) {
                }
            }
        }
        rc = UpnpSendActionAsync(ctrlpt_handle,
                     devnode.ServiceList[service].ControlURL,
                     CONTROL->AvServiceType[service], NULL,
                     actionNode,
                     AvCallbackEventHandler,NULL);

        if (rc != UPNP_E_SUCCESS) {
            rc = AV_ERROR;
        }

        if (actionNode)
            ixmlDocument_free(actionNode);
    }
    ithread_mutex_unlock(&DeviceListMutex);
    return rc;
}

AvDevice AvCtrlPointGetDevice(const char *UDN)
{
    AvDevice deviceInfo;
    if(CONTROL->devInfoList.size() > 0) {
        for(int i=0;i<CONTROL->devInfoList.count();i++) {
            if(strcmp(CONTROL->devInfoList.at(i).UDN,UDN) == 0){
                deviceInfo = CONTROL->devInfoList.at(i);
                break;
            }
        }
    }
    return deviceInfo;
}

void AvCtrlPointHandleEvent(const char *sid,
                            int evntkey,
                            IXML_Document *changes)
{
    int service;

    ithread_mutex_lock(&DeviceListMutex);

    for(int i=0;i<CONTROL->devInfoList.size();i++) {
        for(service = 0; service < SERVICE_SERVCOUNT; ++service) {
            if (strcmp(CONTROL->devInfoList.at(i).ServiceList[service].SID, sid) ==  0) {
                AvStateUpdate(CONTROL->devInfoList.at(i).UDN,changes);
                break;
            }
        }
    }

    ithread_mutex_unlock(&DeviceListMutex);
}

void AvCtrlPointHandleSubscribeUpdate(const char *eventURL,
                                      const Upnp_SID sid,
                                      int timeout)
{
    int service;

    ithread_mutex_lock(&DeviceListMutex);

    for(int i=0;i<CONTROL->devInfoList.size();i++) {
        for (service = 0; service < SERVICE_SERVCOUNT; service++) {
            if (strcmp
                (CONTROL->devInfoList.at(i).ServiceList[service].EventURL,eventURL) == 0) {
                AvDevice devInfo = CONTROL->devInfoList.at(i);
                strcpy(devInfo.ServiceList[service].SID, sid);
                CONTROL->devInfoList.replace(i,devInfo);
                break;
            }
        }
    }

    ithread_mutex_unlock(&DeviceListMutex);

    return;
    timeout = timeout;
}

int AvCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
    switch ( EventType ) {
    /* SSDP Stuff */
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
    case UPNP_DISCOVERY_SEARCH_RESULT: {
        struct Upnp_Discovery *d_event = (struct Upnp_Discovery *)Event;
        IXML_Document *DescDoc = NULL;
        int ret;

        if (d_event->ErrCode == UPNP_E_SUCCESS) {
            ret = UpnpDownloadXmlDoc(d_event->Location, &DescDoc);
            if (ret == UPNP_E_SUCCESS) {
                AvCtrlPointAddDevice(DescDoc, d_event->Location, d_event->Expires);
            }
            if (DescDoc) {
                ixmlDocument_free(DescDoc);
                DescDoc = NULL;
            }
        }
        // 加入列表
        break;
    }
    case UPNP_DISCOVERY_SEARCH_TIMEOUT:
        /* Nothing to do here... */
        break;
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {
        struct Upnp_Discovery *d_event = (struct Upnp_Discovery *)Event;
        if (d_event->ErrCode == UPNP_E_SUCCESS) {
            AvCtrlPointRemoveDevice(d_event->DeviceId);
        }
        break;
    }
    /* SOAP Stuff */
    case UPNP_CONTROL_ACTION_COMPLETE: {
        struct Upnp_Action_Complete *a_event = (struct Upnp_Action_Complete *)Event;
        if (a_event->ErrCode == UPNP_E_SUCCESS) {
            CONTROL->analyCtrlActionComplete(a_event->ActionResult,a_event->CtrlUrl);
//            qDebug() << "UPNP_CONTROL_ACTION_COMPLETE:" << ixmlDocumenttoString(a_event->ActionResult);
//            qDebug() << "UPNP_CONTROL_ACTION_COMPLETE:" << ixmlDocumenttoString(a_event->ActionRequest);
        } else {
//            qDebug() << a_event->ErrCode;
//            qDebug() << "UPNP_CONTROL_ACTION_COMPLETE ERROR:" << ixmlDocumenttoString(a_event->ActionResult);
//            qDebug() << "UPNP_CONTROL_ACTION_COMPLETE ERROR:" << ixmlDocumenttoString(a_event->ActionRequest);
        }
        /* No need for any processing here, just print out results.
         * Service state table updates are handled by events. */
        break;
    }
    case UPNP_CONTROL_GET_VAR_COMPLETE: {
        struct Upnp_State_Var_Complete *sv_event = (struct Upnp_State_Var_Complete *)Event;

        if (sv_event->ErrCode != UPNP_E_SUCCESS) {
        } else {
            qDebug() << sv_event->CtrlUrl
                     << sv_event->StateVarName
                     << sv_event->CurrentVal;
//            TvCtrlPointHandleGetVar(
//                sv_event->CtrlUrl,
//                sv_event->StateVarName,
//                sv_event->CurrentVal);
        }
        break;
    }
    /* GENA Stuff */
    case UPNP_EVENT_RECEIVED: {
        struct Upnp_Event *e_event = (struct Upnp_Event *)Event;

        AvCtrlPointHandleEvent(
            e_event->Sid,
            e_event->EventKey,
            e_event->ChangedVariables);
        break;
    }
    case UPNP_EVENT_SUBSCRIBE_COMPLETE:
    case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
    case UPNP_EVENT_RENEWAL_COMPLETE: {
        struct Upnp_Event_Subscribe *es_event = (struct Upnp_Event_Subscribe *)Event;

        if (es_event->ErrCode == UPNP_E_SUCCESS) {
            AvCtrlPointHandleSubscribeUpdate(
                es_event->PublisherUrl,
                es_event->Sid,
                es_event->TimeOut);
        }
        break;
    }
    case UPNP_EVENT_AUTORENEWAL_FAILED:
    case UPNP_EVENT_SUBSCRIPTION_EXPIRED: {
        struct Upnp_Event_Subscribe *es_event = (struct Upnp_Event_Subscribe *)Event;
        int TimeOut = 100;
        Upnp_SID newSID;
        int ret;
        ret = UpnpSubscribe(ctrlpt_handle,es_event->PublisherUrl,
                            &TimeOut,newSID);
        if (ret == UPNP_E_SUCCESS) {
            AvCtrlPointHandleSubscribeUpdate(es_event->PublisherUrl,newSID,TimeOut);
        }
        break;
    }
    /* ignore these cases, since this is not a device */
    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
    case UPNP_CONTROL_GET_VAR_REQUEST:
    case UPNP_CONTROL_ACTION_REQUEST:
        break;
    }

    return 0;
    Cookie = Cookie;
}

void AvCtrlPointAddDevice(IXML_Document *DescDoc,const char *location,int expires)
{
    char *deviceType = NULL;
    char *friendlyName = NULL;
    char presURL[200];
    char *baseURL = NULL;
    char *relURL = NULL;
    char *UDN = NULL;
    char *serviceId[SERVICE_SERVCOUNT]  = { NULL, NULL, NULL };
    char *eventURL[SERVICE_SERVCOUNT]   = { NULL, NULL, NULL };
    char *controlURL[SERVICE_SERVCOUNT] = { NULL, NULL, NULL };

    Upnp_SID eventSID[SERVICE_SERVCOUNT];
    int TimeOut[SERVICE_SERVCOUNT] = {
        default_timeout,
        default_timeout,
        default_timeout
    };

    int     ret     = 1;
    bool    found   = false;

    /* Read key elements from description document */
    UDN             = getFirstDocumentItem(DescDoc, "UDN");
    deviceType      = getFirstDocumentItem(DescDoc, "deviceType");
    friendlyName    = getFirstDocumentItem(DescDoc, "friendlyName");
    baseURL         = getFirstDocumentItem(DescDoc, "URLBase");
    relURL          = getFirstDocumentItem(DescDoc, "presentationURL");

    QString uuid(UDN);
    QRegExp ex("^uuid:([0-9a-zA-Z]{8}(-[0-9a-zA-Z]{4}){3}-[a-zA-Z0-9]{12})$");
    if(!ex.exactMatch(uuid)) {
        return;
    }

    ret = UpnpResolveURL((baseURL ? baseURL : location), relURL, presURL);

//    if (UPNP_E_SUCCESS != ret)
//        SampleUtil_Print("Error generating presURL from %s + %s\n",
//                 baseURL, relURL);

    // 只找MediaRenderer设备
    ithread_mutex_lock(&DeviceListMutex);
    if (strcmp(deviceType, AvDeviceType) == 0) {
        /* Check if this device is already in the list */
        for(int i=0;i<CONTROL->devInfoList.size();i++) {
            if(strcmp(CONTROL->devInfoList.at(i).UDN,UDN) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            for (int serCount = 0; serCount < SERVICE_SERVCOUNT;serCount++) {
                if (findAndParseService(DescDoc, location, CONTROL->AvServiceType[serCount],
                    &serviceId[serCount], &eventURL[serCount],&controlURL[serCount])) {
                    ret = UpnpSubscribe(ctrlpt_handle,
                              eventURL[serCount],
                              &TimeOut[serCount],
                              eventSID[serCount]);
                    if (ret != UPNP_E_SUCCESS) {
                        strcpy(eventSID[serCount], "");
                    }
                }
            }
            /* Create a new device node */
            AvDevice devInfo;
            strcpy(devInfo.UDN,          UDN);
            strcpy(devInfo.DescDocURL,   location);
            strcpy(devInfo.FriendlyName, friendlyName);
            strcpy(devInfo.PresURL,      presURL);
            devInfo.AdvrTimeOut = expires;
            for (int serCount = 0; serCount < SERVICE_SERVCOUNT;serCount++) {
                if (serviceId[serCount] == NULL) {
                    /* not found */
                    continue;
                }
                strcpy(devInfo.ServiceList[serCount].
                       ServiceId, serviceId[serCount]);
                strcpy(devInfo.ServiceList[serCount].
                       ServiceType, CONTROL->AvServiceType[serCount]);
                strcpy(devInfo.ServiceList[serCount].
                       ControlURL, controlURL[serCount]);
                strcpy(devInfo.ServiceList[serCount].
                       EventURL, eventURL[serCount]);

                strcpy(devInfo.ServiceList[serCount].
                       SID, eventSID[serCount]);

            }
            /* Insert the new device node in the list */
            deviceInfo deviceIn;
            strcpy(deviceIn.udn,UDN);
            strcpy(deviceIn.friendlyName,friendlyName);
            strcpy(deviceIn.baseHttp,CONTROL->getBaseHttp(location));
            CONTROL->addDevice(deviceIn);
            CONTROL->devInfoList.append(devInfo);
        }
    }
    ithread_mutex_unlock(&DeviceListMutex);
    if (deviceType) {
        free(deviceType);
        deviceType = NULL;
    }
    if (friendlyName) {
        free(friendlyName);
        friendlyName = NULL;
    }
    if (UDN) {
        free(UDN);
        UDN = NULL;
    }
    if (baseURL) {
        free(baseURL);
        baseURL = NULL;
    }
    if (relURL) {
        free(relURL);
        relURL = NULL;
    }
    for (int serCount = 0; serCount < SERVICE_SERVCOUNT; serCount++) {
        if (serviceId[serCount]) {
            free(serviceId[serCount]);
            serviceId[serCount] = NULL;
        }
        if (controlURL[serCount]) {
            free(controlURL[serCount]);
            controlURL[serCount] = NULL;
        }
        if (eventURL[serCount]) {
            free(eventURL[serCount]);
            eventURL[serCount] = NULL;
        }
    }
}

int AvCtrlPointSendGetVolume(char *UDN)
{
    QList<actionKey_Value> volumeList;
    volumeList.clear();
    actionKey_Value instanceID;
    actionKey_Value channel;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(channel.key,         "Channel");
    strcpy(channel.value,       "Master");

    volumeList.append(instanceID);
    volumeList.append(channel);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_RenderingControl,"GetVolume",
                                     volumeList);
}

int AvCtrlPointSendSetVolume(char *UDN, int volume)
{
    QList<actionKey_Value> volumeList;
    volumeList.clear();
    actionKey_Value instanceID;
    actionKey_Value channel;
    actionKey_Value desiredVolume;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(channel.key,         "Channel");
    strcpy(channel.value,       "Master");
    strcpy(desiredVolume.key,   "DesiredVolume");
    strcpy(desiredVolume.value, CONTROL->QString2char(QString::number(volume)));

    volumeList.append(instanceID);
    volumeList.append(channel);
    volumeList.append(desiredVolume);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_RenderingControl,"SetVolume",
                                     volumeList);
}

int AvCtrlPointSendSetMute(char *UDN, bool mute)
{
    QList<actionKey_Value> muteList;
    muteList.clear();
    actionKey_Value instanceID;
    actionKey_Value channel;
    actionKey_Value desiredMute;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(channel.key,         "Channel");
    strcpy(channel.value,       "Master");
    strcpy(desiredMute.key,     "DesiredMute");
    strcpy(desiredMute.value,   mute? "1" : "0");

    muteList.append(instanceID);
    muteList.append(channel);
    muteList.append(desiredMute);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_RenderingControl,"SetMute",
                                     muteList);
}

int AvCtrlPointSendGetMute(char *UDN)
{
    QList<actionKey_Value> muteList;
    muteList.clear();
    actionKey_Value instanceID;
    actionKey_Value channel;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(channel.key,         "Channel");
    strcpy(channel.value,       "Master");

    muteList.append(instanceID);
    muteList.append(channel);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_RenderingControl,"GetMute",
                                     muteList);
}

int AvCtrlPointSendSetTransportURI(char *UDN, char *url)
{
    QList<actionKey_Value> setTransList;
    setTransList.clear();
    actionKey_Value instanceID;
    actionKey_Value curUrl;
    actionKey_Value urlMetaData;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(curUrl.key,          "CurrentURI");
    strcpy(curUrl.value,        url);
    strcpy(urlMetaData.key,     "CurrentURIMetaData");
    strcpy(urlMetaData.value,   "");

    setTransList.append(instanceID);
    setTransList.append(curUrl);
    setTransList.append(urlMetaData);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"SetAVTransportURI",
                                     setTransList);
}

int AvCtrlPointSendPlay(char *UDN)
{
    QList<actionKey_Value> setPlayList;
    setPlayList.clear();
    actionKey_Value instanceID;
    actionKey_Value speed;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(speed.key,          "Speed");
    strcpy(speed.value,        "1");

    setPlayList.append(instanceID);
    setPlayList.append(speed);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"Play",
                                     setPlayList);
}

int AvCtrlPointSendPause(char *UDN)
{
    QList<actionKey_Value> setPauseList;
    setPauseList.clear();
    actionKey_Value instanceID;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");

    setPauseList.append(instanceID);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"Pause",
                                     setPauseList);
}

int AvCtrlPointSendStop(char *UDN)
{
    QList<actionKey_Value> setStopList;
    setStopList.clear();
    actionKey_Value instanceID;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");

    setStopList.append(instanceID);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"Stop",
                                     setStopList);
}

int AvCtrlPointSendGetPoint(char *UDN)
{
    QList<actionKey_Value> setGetPointList;
    setGetPointList.clear();
    actionKey_Value instanceID;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");

    setGetPointList.append(instanceID);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"GetPositionInfo",
                                     setGetPointList);
}

int AvCtrlPointSendGetTransportInfo(char *UDN)
{
    QList<actionKey_Value> setGetTransportInfo;
    setGetTransportInfo.clear();
    actionKey_Value instanceID;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");

    setGetTransportInfo.append(instanceID);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"GetTransportInfo",
                                     setGetTransportInfo);
}

int AvCtrlPointSendSeek(char *UDN, char *seekTime)
{
    QList<actionKey_Value> setSeekList;
    setSeekList.clear();
    actionKey_Value instanceID;
    actionKey_Value seekMode;   // seekMode:ABS_COUNT|TRACK_NR|REL_TIME
    actionKey_Value target;
    strcpy(instanceID.key,      "InstanceID");
    strcpy(instanceID.value,    "0");
    strcpy(seekMode.key,        "Unit");
    strcpy(seekMode.value,      "REL_TIME");
    strcpy(target.key,          "Target");
    strcpy(target.value,        seekTime);

    setSeekList.append(instanceID);
    setSeekList.append(seekMode);
    setSeekList.append(target);

    return AvCtrlPointSendActionArgs(UDN,SERVICE_AVTransport,"Seek",
                                     setSeekList);
}

int AvCtrlPointSendGetProtocolInfo(const char *UDN)
{
    QList<actionKey_Value> ProtocolInfoList;
    ProtocolInfoList.clear();

    return AvCtrlPointSendActionArgs(UDN,SERVICE_ConnectionManager,"GetProtocolInfo",
                                     ProtocolInfoList);
}

void ctrlPointInit()
{
    ithread_mutex_init(&DeviceListMutex, 0);
}

void stopCtrlPoint()
{
    AvCtrlPointRemoveAll();
    UpnpUnRegisterClient( ctrlpt_handle );
    // 这里不需要
    //UpnpFinish();
}
