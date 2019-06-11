#include "dlnadevice.h"
#include "dlnacommon.h"

#include "controlmanager.h"
#include "devicemanager.h"
#include <QString>
#include <QApplication>
#include <QDebug>
#include "globalarg.h"

/*! Device handle supplied by UPnP SDK. */
UpnpDevice_Handle device_handle = -1;

/*! Mutex for protecting the global state table data
 * in a multi-threaded, asynchronous environment.
 * All functions should lock this mutex before reading
 * or writing the state table data. */
ithread_mutex_t AVDevMutex;

/*! Global arrays for storing Tv Control Service variable names, values,
 * and defaults. */
const char *av_rands_actName[] = { "SetVolume","GetVolume", "SetMute","GetMute"};
const char *av_trans_actName[] = { "SetAVTransportURI", "Play", "Pause","Stop",
                                 "Seek","GetPositionInfo","GetTransportInfo"};
const char *av_connect_actName[] = {"GetProtocolInfo"};
#define SET_RANDS_VOL   0
#define GET_RANDS_VOL   1
#define SET_RANDS_MUTE  2
#define GET_RANDS_MUTE  3

#define SET_TRANS_URL   0
#define SET_TRANS_Play  1
#define SET_TRANS_Pause 2
#define SET_TRANS_Stop  3
#define SET_TRANS_Seek  4
#define SET_TRANS_Pos   5
#define SET_TRANS_port  6

#define GET_CONNECT_PROINFO 0

char av_randsval[AV_RANDS_VARCOUNT][AV_MAX_VAL_LEN];
char av_transval[AV_TRANS_VARCOUNT][AV_MAX_VAL_LEN];
char av_connectval[AV_CONMANAG_VARCOUNT][AV_MAX_VAL_LEN];


/*! Global structure for storing the state table for this device. */
struct AvService av_service_table[SERVICE_SERVCOUNT];


int AvDeviceCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie)
{
    switch (EventType) {
        case UPNP_EVENT_SUBSCRIPTION_REQUEST:
            AvDeviceHandleSubscriptionRequest((struct
                               Upnp_Subscription_Request *)
                              Event);
            break;
        case UPNP_CONTROL_GET_VAR_REQUEST:
            AvDeviceHandleGetVarRequest((struct Upnp_State_Var_Request *)
                            Event);
            break;
        case UPNP_CONTROL_ACTION_REQUEST:
            AvDeviceHandleActionRequest((struct Upnp_Action_Request *)
                            Event);
            break;
            /* ignore these cases, since this is not a control point */
        case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
        case UPNP_DISCOVERY_SEARCH_RESULT:
        case UPNP_DISCOVERY_SEARCH_TIMEOUT:
        case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
        case UPNP_CONTROL_ACTION_COMPLETE:
        case UPNP_CONTROL_GET_VAR_COMPLETE:
        case UPNP_EVENT_RECEIVED:
        case UPNP_EVENT_RENEWAL_COMPLETE:
        case UPNP_EVENT_SUBSCRIBE_COMPLETE:
        case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
            break;
        default:
            qDebug() <<
                QString("Error in TvDeviceCallbackEventHandler: unknown event type %1\n")
                        .arg(EventType);
        }

        return 0;
        Cookie = Cookie;
}

int AvDeviceHandleSubscriptionRequest(Upnp_Subscription_Request *sr_event)
{
    unsigned int i = 0;
    int cmp1 = 0;
    int cmp2 = 0;
    const char *l_serviceId = NULL;
    const char *l_udn = NULL;
    const char *l_sid = NULL;

    /* lock state mutex */
    ithread_mutex_lock(&AVDevMutex);

    l_serviceId = sr_event->ServiceId;
    l_udn = sr_event->UDN;
    l_sid = sr_event->Sid;
    for (i = 0; i < SERVICE_SERVCOUNT; ++i) {
        cmp1 = strcmp(l_udn, av_service_table[i].UDN);
        cmp2 = strcmp(l_serviceId, av_service_table[i].ServiceId);
        if (cmp1 == 0 && cmp2 == 0) {
            UpnpAcceptSubscription(device_handle,
                           l_udn,
                           l_serviceId,
                           (const char **)
                           av_service_table[i].VariableName,
                           (const char **)
                           av_service_table
                           [i].VariableStrVal,
                           av_service_table[i].
                           VariableCount, l_sid);
        }
    }

    ithread_mutex_unlock(&AVDevMutex);

    return 1;
}

int AvDeviceHandleGetVarRequest(Upnp_State_Var_Request *cgv_event)
{
    unsigned int i = 0;
    int j = 0;
    int getvar_succeeded = 0;

    cgv_event->CurrentVal = NULL;

    ithread_mutex_lock(&AVDevMutex);

    for (i = 0; i < SERVICE_SERVCOUNT; i++) {
        /* check udn and service id */
        const char *devUDN      = cgv_event->DevUDN;
        const char *serviceID   = cgv_event->ServiceID;
        if (strcmp(devUDN, av_service_table[i].UDN) == 0 &&
            strcmp(serviceID, av_service_table[i].ServiceId) == 0) {
            /* check variable name */
            for (j = 0; j < av_service_table[i].VariableCount; j++) {
                const char *stateVarName = cgv_event->StateVarName;
                if (strcmp(stateVarName,av_service_table[i].VariableName[j]) == 0) {
                    getvar_succeeded = 1;
                    cgv_event->CurrentVal = ixmlCloneDOMString(
                                av_service_table[i].VariableStrVal[j]);
                    break;
                }
            }
        }
    }
    if (getvar_succeeded) {
        cgv_event->ErrCode = UPNP_E_SUCCESS;
    } else {
        cgv_event->ErrCode = 404;
        strcpy(cgv_event->ErrStr, "Invalid Variable");
    }

    ithread_mutex_unlock(&AVDevMutex);

    return cgv_event->ErrCode == UPNP_E_SUCCESS;
}
int AvDeviceHandleActionRequest(Upnp_Action_Request *ca_event)
{
    /* Defaults if action not found. */
    int action_found = 0;
    int i = 0;
    int service = -1;
    int retCode = 0;
    const char *errorString = NULL;
    const char *devUDN = NULL;
    const char *serviceID = NULL;
    const char *actionName = NULL;

    ca_event->ErrCode = 0;
    ca_event->ActionResult = NULL;

    devUDN     = ca_event->DevUDN;
    serviceID  = ca_event->ServiceID;
    actionName = ca_event->ActionName;
    //qDebug() << "AvDeviceHandleActionRequest:" << actionName;
    for(int i=0;i<SERVICE_SERVCOUNT;i++) {
        if (strcmp(devUDN,    av_service_table[i].UDN) == 0 &&
            strcmp(serviceID, av_service_table[i].ServiceId) == 0) {
            /* Request for action in the TvDevice Control Service. */
            service = i;
            break;
        }
    }
    if(service == -1) {
        action_found = 0;
    } else {
        /* Find and call appropriate procedure based on action name.
         * Each action name has an associated procedure stored in the
         * service table. These are set at initialization. */
        for (i = 0;
             i < AV_MAXACTIONS && av_service_table[service].ActionNames[i] != NULL;
             i++) {
            if (!strcmp(actionName, av_service_table[service].ActionNames[i])) {
                retCode = av_service_table[service].actions[i](
                    ca_event->ActionRequest,
                    &ca_event->ActionResult,
                    &errorString);
                action_found = 1;
                break;
            }
        }
    }

    if (!action_found) {
        ca_event->ActionResult = NULL;
        strcpy(ca_event->ErrStr, "Invalid Action");
        ca_event->ErrCode = 401;
    } else {
        if (retCode == UPNP_E_SUCCESS) {
            ca_event->ErrCode = UPNP_E_SUCCESS;
        } else {
            /* copy the error string */
            strcpy(ca_event->ErrStr, errorString);
            switch (retCode) {
            case UPNP_E_INVALID_PARAM:
                ca_event->ErrCode = 402;
                break;
            case UPNP_E_INTERNAL_ERROR:
            default:
                ca_event->ErrCode = 501;
                break;
            }
        }
    }

    return ca_event->ErrCode;
}

void deviceInit()
{
    ithread_mutex_init(&AVDevMutex, NULL);
}

void stopDevice()
{
    UpnpUnRegisterRootDevice(device_handle);
    ithread_mutex_destroy(&AVDevMutex);
}

/*!
 * \brief Initializes the service table for the specified service.
 */
static int SetServiceTable(
    /*! [in] one of TV_SERVICE_CONTROL or, TV_SERVICE_PICTURE. */
    int serviceType,
    /*! [in] UDN of device containing service. */
    const char *UDN,
    /*! [in] serviceId of service. */
    const char *serviceId,
    /*! [in] service type (as specified in Description Document) . */
    const char *serviceTypeS,
    /*! [in,out] service containing table to be set. */
    struct AvService *out)
{
    int i = 0;

    strcpy(out->UDN, UDN);
    strcpy(out->ServiceId, serviceId);
    strcpy(out->ServiceType, serviceTypeS);

    switch (serviceType) {
    case SERVICE_RenderingControl:
        out->VariableCount = AV_RANDS_VARCOUNT;
        for (i = 0;
             i < av_service_table[SERVICE_RenderingControl].VariableCount;
             i++) {
            av_service_table[SERVICE_RenderingControl].VariableName[i]
                = av_rands_actName[i];
            av_service_table[SERVICE_RenderingControl].VariableStrVal[i]
                = av_randsval[i];
            strcpy(av_service_table[SERVICE_RenderingControl].
                VariableStrVal[i], "0");
        }
        break;
    case SERVICE_AVTransport:
        out->VariableCount = AV_TRANS_VARCOUNT;
        for (i = 0;
             i < av_service_table[SERVICE_AVTransport].VariableCount;
             i++) {
            av_service_table[SERVICE_AVTransport].VariableName[i] =
                av_trans_actName[i];
            av_service_table[SERVICE_AVTransport].VariableStrVal[i] =
                av_transval[i];
            strcpy(av_service_table[SERVICE_AVTransport].
                VariableStrVal[i], "0");
        }
        break;
    case SERVICE_ConnectionManager:
        out->VariableCount = AV_CONMANAG_VARCOUNT;
        for (i = 0;
             i < av_service_table[SERVICE_ConnectionManager].VariableCount;
             i++) {
            av_service_table[SERVICE_ConnectionManager].VariableName[i] =
                av_connect_actName[i];
            av_service_table[SERVICE_ConnectionManager].VariableStrVal[i] =
                av_connectval[i];
            strcpy(av_service_table[SERVICE_ConnectionManager].
                VariableStrVal[i], "0");
        }
        break;
    default:
        assert(0);
    }

    return SetActionTable(serviceType, out);
}

int AvDeviceStateTableInit(char *descDocURL)
{
    IXML_Document *DescDoc = NULL;
    int ret = UPNP_E_SUCCESS;
    char *servid_ctrl = NULL;
    char *evnturl_ctrl = NULL;
    char *ctrlurl_ctrl = NULL;
    char *servid_trans = NULL;
    char *evnturl_trans = NULL;
    char *ctrlurl_trans = NULL;
    char *udn = NULL;

    /*Download description document */
    if (UpnpDownloadXmlDoc(descDocURL, &DescDoc) != UPNP_E_SUCCESS) {
        ret = UPNP_E_INVALID_DESC;
        goto error_handler;
    }
    udn = getFirstDocumentItem(DescDoc, "UDN");
    /* Find the SERVICE_RenderingControl Service identifiers */
    if (!findAndParseService(DescDoc, descDocURL,
                        CONTROL->AvServiceType[SERVICE_RenderingControl],
                        &servid_ctrl, &evnturl_ctrl,
                        &ctrlurl_ctrl)) {
        ret = UPNP_E_INVALID_DESC;
        goto error_handler;
    }
    /* set control service table */
    SetServiceTable(SERVICE_RenderingControl, udn, servid_ctrl,
            CONTROL->AvServiceType[SERVICE_RenderingControl],
            &av_service_table[SERVICE_RenderingControl]);

    /* Find the AVTrans Service identifiers */
    if (!findAndParseService(DescDoc, descDocURL,
                        CONTROL->AvServiceType[SERVICE_AVTransport],
                        &servid_trans, &evnturl_trans,
                        &ctrlurl_trans)) {
        ret = UPNP_E_INVALID_DESC;
        goto error_handler;
    }
    /* set AVTrans service table */
    SetServiceTable(SERVICE_AVTransport, udn, servid_trans,
            CONTROL->AvServiceType[SERVICE_AVTransport],
            &av_service_table[SERVICE_AVTransport]);

    /* Find the AVConnect Service identifiers */
    if (!findAndParseService(DescDoc, descDocURL,
                        CONTROL->AvServiceType[SERVICE_ConnectionManager],
                        &servid_trans, &evnturl_trans,
                        &ctrlurl_trans)) {
        ret = UPNP_E_INVALID_DESC;
        goto error_handler;
    }
    /* set AVTrans service table */
    SetServiceTable(SERVICE_ConnectionManager, udn, servid_trans,
            CONTROL->AvServiceType[SERVICE_ConnectionManager],
            &av_service_table[SERVICE_ConnectionManager]);
error_handler:
    /* clean up */
    if (udn) {
        free(udn);
        udn = NULL;
    }
    if (servid_ctrl) {
        free(servid_ctrl);
        servid_ctrl = NULL;
    }
    if (evnturl_ctrl) {
        free(evnturl_ctrl);
        evnturl_ctrl = NULL;
    }
    if (ctrlurl_ctrl) {
        free(ctrlurl_ctrl);
        ctrlurl_ctrl = NULL;
    }
    if (servid_trans) {
        free(servid_trans);
        servid_trans = NULL;
    }
    if (evnturl_trans) {
        free(evnturl_trans);
        evnturl_trans = NULL;
    }
    if (ctrlurl_trans) {
        free(ctrlurl_trans);
        ctrlurl_trans = NULL;
    }
    if (DescDoc)
        ixmlDocument_free(DescDoc);

    return (ret);
}

int SetActionTable(int serviceType, AvService *out)
{
    if (serviceType == SERVICE_RenderingControl) {
        out->ActionNames[SET_RANDS_VOL] = "SetVolume";
        out->actions[SET_RANDS_VOL] = AvDeviceSetVolume;
        out->ActionNames[GET_RANDS_VOL] = "GetVolume";
        out->actions[GET_RANDS_VOL] = AvDeviceGetVolume;
        out->ActionNames[SET_RANDS_MUTE] = "SetMute";
        out->actions[SET_RANDS_MUTE] = AvDeviceSetMute;
        out->ActionNames[GET_RANDS_MUTE] = "GetMute";
        out->actions[GET_RANDS_MUTE] = AvDeviceGetMute;
        return 1;
    } else if (serviceType == SERVICE_AVTransport) {
        out->ActionNames[SET_TRANS_URL] = "SetAVTransportURI";
        out->actions[SET_TRANS_URL] = AvDeviceSetAVTransportURI;
        out->ActionNames[SET_TRANS_Play] = "Play";
        out->actions[SET_TRANS_Play] = AvDevicePlay;
        out->ActionNames[SET_TRANS_Pause] = "Pause";
        out->actions[SET_TRANS_Pause] = AvDevicePause;
        out->ActionNames[SET_TRANS_Stop] = "Stop";
        out->actions[SET_TRANS_Stop] = AvDeviceStop;
        out->ActionNames[SET_TRANS_Seek] = "Seek";
        out->actions[SET_TRANS_Seek] = AvDeviceSeek;
        out->ActionNames[SET_TRANS_Pos] = "GetPositionInfo";
        out->actions[SET_TRANS_Pos] = AvDeviceGetPositionInfo;
        out->ActionNames[SET_TRANS_port] = "GetTransportInfo";
        out->actions[SET_TRANS_port] = AvDeviceGetTransportState;
        return 1;
    } else if(serviceType == SERVICE_ConnectionManager) {
        out->ActionNames[GET_CONNECT_PROINFO] = "GetProtocolInfo";
        out->actions[GET_CONNECT_PROINFO] = AvDeviceGetProtocolInfo;
        return 1;
    }

    return 0;
}

int AvDeviceSetServiceTableVar(unsigned int service, int variable, char *value)
{
    /* IXML_Document  *PropSet= NULL; */
    if (service >= SERVICE_SERVCOUNT ||
        strlen(value) >= AV_MAX_VAL_LEN)
        return (0);

    ithread_mutex_lock(&AVDevMutex);

    strcpy(av_service_table[service].VariableStrVal[variable], value);
    UpnpNotify(device_handle,
        av_service_table[service].UDN,
        av_service_table[service].ServiceId,
        (const char **)&av_service_table[service].VariableName[variable],
        (const char **)&av_service_table[service].VariableStrVal[variable], 1);

    ithread_mutex_unlock(&AVDevMutex);

    return 1;
}

int AvDeviceLastChange(unsigned int service, char* actionName, QList<actionKey_Value> args)
{
    IXML_Document  *PropSet= NULL;
    if (service >= SERVICE_SERVCOUNT)
        return (0);

    UpnpAddToPropertySet(&PropSet,
                         "LastChange",
                         Device->createEventLastChange(service,actionName,args));
    ithread_mutex_lock(&AVDevMutex);

    UpnpNotifyExt(device_handle,av_service_table[service].UDN,
                  av_service_table[service].ServiceId,
                  PropSet);

    ithread_mutex_unlock(&AVDevMutex);

    return 1;
}

int AvDeviceActionResponse(IXML_Document **out, unsigned int service, char *actionName, QList<actionKey_Value> args)
{
    int ret = UPNP_E_SUCCESS;

    ithread_mutex_lock(&AVDevMutex);
    if(args.count() == 0) {
        ret = UpnpAddToActionResponse(out, actionName,
                                CONTROL->AvServiceType[service],
                                NULL,
                                NULL);
    } else {
        for(int i=0;i<args.count();i++) {
            ret = UpnpAddToActionResponse(out, actionName,
                                    CONTROL->AvServiceType[service],
                                    args.at(i).key,
                                    args.at(i).value);
            if(ret != UPNP_E_SUCCESS) {
                ret = UPNP_E_INTERNAL_ERROR;
                break;
            }
        }
    }

    ithread_mutex_unlock(&AVDevMutex);
    return ret;
}

int AvDeviceSetVolume(IXML_Document *in, IXML_Document **out, const char **error)
{
    char *value = NULL;
    int volume = 0;
    int ret = UPNP_E_SUCCESS;
    (*out) = NULL;
    (*error) = NULL;
    if (!(value = getFirstDocumentItem(in, "DesiredVolume"))) {
        (*error) = "Invalid Volume";
        return UPNP_E_INVALID_PARAM;
    }
    volume = atoi(value);
    if (volume < 0 || volume > 100) {
        (*error) = "Invalid Volume";
        return UPNP_E_INVALID_PARAM;
    }
    /* Vendor-specific code to set the volume goes here. */
    if (AvDeviceSetServiceTableVar(SERVICE_RenderingControl,
                       SET_RANDS_VOL, value)) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_RenderingControl,"SetVolume",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            ret = UPNP_E_INTERNAL_ERROR;
        }
        CONTROL->sendCtrlVolumeChange(volume);
    } else {
        (*error) = "Internal Error";
        ret = UPNP_E_INTERNAL_ERROR;
    }
    //free(value);
    return ret;
}

int AvDeviceSetMute(IXML_Document *in, IXML_Document **out, const char **error)
{
    char *mute = NULL;
    (*out) = NULL;
    (*error) = NULL;
    if (!(mute = getFirstDocumentItem(in, "DesiredMute"))) {
        (*error) = "Invalid Volume";
        return UPNP_E_INVALID_PARAM;
    }
    Device->deviceMute();
    if (AvDeviceSetServiceTableVar(SERVICE_RenderingControl,
                       SET_RANDS_MUTE, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_RenderingControl,"SetMute",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceGetVolume(IXML_Document *in, IXML_Document **out, const char **error)
{
    int volume = Global->volume();

    (*out) = NULL;
    (*error) = NULL;
    QString str = QString::number(volume);

    QByteArray byte = str.toUtf8();
    char* value =  byte.data();

    if (volume < 0 || volume > 144) {
        (*error) = "Invalid Volume";
        return UPNP_E_INVALID_PARAM;
    }

    /* Vendor-specific code to set the volume goes here. */
    if (AvDeviceSetServiceTableVar(SERVICE_RenderingControl,
                       GET_RANDS_VOL, value)) {
        QList<actionKey_Value> args;
        args.clear();
        actionKey_Value curVolume;
        strcpy(curVolume.key,   "CurrentVolume");
        strcpy(curVolume.value, value);
        args.append(curVolume);
        if (AvDeviceActionResponse(out,SERVICE_RenderingControl,"GetVolume",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceGetMute(IXML_Document *in, IXML_Document **out, const char **error)
{
    bool mute = Global->_mute;

    (*out) = NULL;
    (*error) = NULL;

    if (AvDeviceSetServiceTableVar(SERVICE_RenderingControl,
                       GET_RANDS_MUTE, "")) {
        QList<actionKey_Value> args;
        args.clear();
        actionKey_Value curMute;
        strcpy(curMute.key,   "CurrentMute");
        strcpy(curMute.value, mute?"true":"false");
        args.append(curMute);
        if (AvDeviceActionResponse(out,SERVICE_RenderingControl,"GetMute",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceGetPositionInfo(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_Pos, "")) {
        QList<actionKey_Value> args;
        args.clear();
        actionKey_Value track;
        actionKey_Value duration;
        actionKey_Value relTime;
        actionKey_Value absTime;
        strcpy(track.key,       "Track");
        strcpy(track.value,     "1");
        strcpy(duration.key,    "TrackDuration");
        strcpy(duration.value,  CONTROL->QString2char(Global->_playTotalTime));
        strcpy(relTime.key,     "RelTime");
        strcpy(relTime.value,   CONTROL->QString2char(Global->_playCurTime));
        strcpy(absTime.key,     "AbsTime");
        strcpy(absTime.value,   CONTROL->QString2char(Global->_playCurTime));

        args.append(track);
        args.append(duration);
        args.append(relTime);
        args.append(absTime);

        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"GetPositionInfo",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceGetTransportState(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_port, "")) {
        QString state = Device->getDevState();
        QList<actionKey_Value> args;
        args.clear();
        actionKey_Value curState;
        actionKey_Value curStates;
        actionKey_Value playSpeed;

        strcpy(curState.key,    "CurrentTransportState");
        strcpy(curState.value,  CONTROL->QString2char(state));
        strcpy(curStates.key,   "CurrentTransportStatus");
        strcpy(curStates.value, "OK");
        strcpy(playSpeed.key,   "TransportPlaySpeed");
        strcpy(playSpeed.value, "1");

        args.append(curState);
        args.append(curStates);
        args.append(playSpeed);

        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"GetTransportInfo",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceSetAVTransportURI(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;

    Device->setTransing(QString(getFirstDocumentItem(in, "CurrentURI")));
    Device->deviceTransportState("TRANSITIONING");
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_URL, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"SetAVTransportURI",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDevicePlay(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    CONTROL->clearSelDeviceInfo();
    Device->deviceTransportState("PLAYING");
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_Play, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"Play",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDevicePause(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    Device->deviceTransportState("PAUSED_PLAYBACK");
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_Pause, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"Pause",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息

        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceStop(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    Device->deviceTransportState("STOPPED");
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_Stop, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"Stop",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceSeek(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    Device->deviceSeek(QString(getFirstDocumentItem(in, "Target")));
    if (AvDeviceSetServiceTableVar(SERVICE_AVTransport,
                       SET_TRANS_Seek, "")) {
        QList<actionKey_Value> args;
        args.clear();
        if (AvDeviceActionResponse(out,SERVICE_AVTransport,"Seek",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}

int AvDeviceGetProtocolInfo(IXML_Document *in, IXML_Document **out, const char **error)
{
    (*out) = NULL;
    (*error) = NULL;
    if (AvDeviceSetServiceTableVar(SERVICE_ConnectionManager,
                       GET_CONNECT_PROINFO, "")) {
        QList<actionKey_Value> args;
        args.clear();
        actionKey_Value sourc;
        actionKey_Value sink;
        strcpy(sourc.key,       "Source");
        strcpy(sourc.value,     "");
        strcpy(sink.key,        "Sink");
        strcpy(sink.value,      CONTROL->getDeviceSink());
        args.append(sourc);
        args.append(sink);

        if (AvDeviceActionResponse(out,SERVICE_ConnectionManager,"GetProtocolInfo",args)
            != UPNP_E_SUCCESS) {
            (*out) = NULL;
            (*error) = "Internal Error";
            return UPNP_E_INTERNAL_ERROR;
        }// out 信息
        return UPNP_E_SUCCESS;
    } else {
        (*error) = "Internal Error";
        return UPNP_E_INTERNAL_ERROR;
    }
}
