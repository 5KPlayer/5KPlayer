#ifndef DEVSTRUCT_H
#define DEVSTRUCT_H

#include "upnp.h"
// 更新状态
#define SERVICE_SERVCOUNT 3
#define AV_MAXVARS 10
#define AV_MAXACTIONS 10
#define DESC_URL_SIZE 200

#define AV_RANDS_VARCOUNT       4
#define AV_TRANS_VARCOUNT       7
#define AV_CONMANAG_VARCOUNT    1
#define AV_MAX_VAL_LEN      20

#define SERVICE_RenderingControl 0
#define SERVICE_AVTransport 1
#define SERVICE_ConnectionManager 2


#define AV_SUCCESS		0
#define AV_ERROR		(-1)
#define AV_WARNING		1

typedef enum {
    STATE_UPDATE = 0,
    DEVICE_ADDED = 1,
    DEVICE_REMOVED = 2,
    GET_VAR_COMPLETE = 3
} eventType;

struct actionKey_Value {
    char key[NAME_SIZE];
    char value[10000];
};

struct service {
    char ServiceId[NAME_SIZE];
    char ServiceType[NAME_SIZE];
    char EventURL[NAME_SIZE];
    char ControlURL[NAME_SIZE];
    char ScpdURL[NAME_SIZE];
    char *VariableStrVal[AV_MAXVARS];
    char SID[NAME_SIZE];
    service() {
        memset(ServiceId,0,NAME_SIZE);
        memset(ServiceType,0,NAME_SIZE);
        memset(EventURL,0,NAME_SIZE);
        memset(ControlURL,0,NAME_SIZE);
        memset(ScpdURL,0,NAME_SIZE);
        memset(SID,0,NAME_SIZE);
    }
};

struct AvDevice {
    char UDN[250];
    char DescDocURL[250];
    char FriendlyName[250];
    char DeviceType[250];
    char PresURL[250];
    int  AdvrTimeOut;
    struct service ServiceList[SERVICE_SERVCOUNT];  // 现在只关注3种服务
    AvDevice() {
        memset(UDN,0,250);
        memset(DescDocURL,0,250);
        memset(FriendlyName,0,250);
        memset(DeviceType,0,250);
        memset(PresURL,0,250);
        AdvrTimeOut = 0;
    }
};

/*----------------------------------------------------*/
/*!
 * \brief Prototype for all actions. For each action that a service
 * implements, there is a corresponding function with this prototype.
 *
 * Pointers to these functions, along with action names, are stored
 * in the service table. When an action request comes in the action
 * name is matched, and the appropriate function is called.
 * Each function returns UPNP_E_SUCCESS, on success, and a nonzero
 * error code on failure.
 */
typedef int (*upnp_action)(
    /*! [in] Document of action request. */
    IXML_Document *request,
    /*! [out] Action result. */
    IXML_Document **out,
    /*! [out] Error string in case action was unsuccessful. */
    const char **errorString);

/*! Structure for storing Tv Service identifiers and state table. */
struct AvService {
    /*! Universally Unique Device Name. */
    char UDN[NAME_SIZE];
    /*! . */
    char ServiceId[NAME_SIZE];
    /*! . */
    char ServiceType[NAME_SIZE];
    /*! . */
    const char *VariableName[AV_MAXVARS];
    /*! . */
    char *VariableStrVal[AV_MAXVARS];
    /*! . */
    const char *ActionNames[AV_MAXACTIONS];
    /*! . */
    upnp_action actions[AV_MAXACTIONS];
    /*! . */
    int VariableCount;
};

#include <QList>
struct deviceInfo {
    char udn[250];
    char friendlyName[250];
    char baseHttp[250];
    int  deviceType;     // 1: video 2: music
    QStringList deviceVideoFormats;
    QStringList deviceAudioFormats;
    deviceInfo() {
        deviceType = -1;
        deviceVideoFormats.clear();
        deviceAudioFormats.clear();
    }
};

/*----------------------------------------------------*/
/*! Device type for MediaRenderer device. */
const char AvDeviceType[] = "urn:schemas-upnp-org:device:MediaRenderer:1";


#endif // DEVSTRUCT_H
