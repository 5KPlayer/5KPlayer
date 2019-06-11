#ifndef DLNADEVICE_H
#define DLNADEVICE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ithread.h"
#include "ixml.h" /* for IXML_Document, IXML_Element */
#include "upnp.h" /* for Upnp_EventType */
#include "upnptools.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "devstruct.h"
#include <QList>

/*! Device handle returned from sdk */
extern UpnpDevice_Handle device_handle;

/*! Mutex for protecting the global state table data
 * in a multi-threaded, asynchronous environment.
 * All functions should lock this mutex before reading
 * or writing the state table data. */
extern ithread_mutex_t AVDevMutex;

/*! Array of service structures */
extern struct AvService av_service_table[];

/*!
 * \brief The callback handler registered with the SDK while registering
 * root device.
 *
 * Dispatches the request to the appropriate procedure
 * based on the value of EventType. The four requests handled by the
 * device are:
 *	\li 1) Event Subscription requests.
 *	\li 2) Get Variable requests.
 *	\li 3) Action requests.
 */
int AvDeviceCallbackEventHandler(
    /*! [in] The type of callback event. */
    Upnp_EventType,
    /*! [in] Data structure containing event data. */
    void *Event,
    /*! [in] Optional data specified during callback registration. */
    void *Cookie);

/*!
 * \brief Called during a subscription request callback.
 *
 * If the subscription request is for this device and either its
 * control service or picture service, then accept it.
 */
int AvDeviceHandleSubscriptionRequest(
    /*! [in] The subscription request event structure. */
    struct Upnp_Subscription_Request *sr_event);

/*!
 * \brief Called during a get variable request callback.
 *
 * If the request is for this device and either its control service or
 * picture service, then respond with the variable value.
 */
int AvDeviceHandleGetVarRequest(
    /*! [in,out] The control get variable request event structure. */
    struct Upnp_State_Var_Request *cgv_event);

/*!
 * \brief Called during an action request callback.
 *
 * If the request is for this device and either its control service
 * or picture service, then perform the action and respond.
 */
int AvDeviceHandleActionRequest(
    /*! [in,out] The control action request event structure. */
    struct Upnp_Action_Request *ca_event);

void    deviceInit();
void    stopDevice();

int AvDeviceStateTableInit(char *descDocURL);
int SetActionTable(int serviceType,struct AvService *out);
int AvDeviceSetServiceTableVar(unsigned int service,int variable,char *value);
int AvDeviceLastChange(unsigned int service, char *actionName, QList<actionKey_Value> args);
int AvDeviceActionResponse(IXML_Document **out,unsigned int service, char *actionName, QList<actionKey_Value> args);
/*----------------------------------------------*/
int AvDeviceSetVolume(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceSetMute(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceGetVolume(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceGetMute(IXML_Document *in,IXML_Document **out,const char **error);
/*-------------------------------------------------*/
int AvDeviceSetAVTransportURI(IXML_Document *in,IXML_Document **out,const char **error);
int AvDevicePlay(IXML_Document *in,IXML_Document **out,const char **error);
int AvDevicePause(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceStop(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceSeek(IXML_Document *in,IXML_Document **out,const char **error);

int AvDeviceGetPositionInfo(IXML_Document *in,IXML_Document **out,const char **error);
int AvDeviceGetTransportState(IXML_Document *in,IXML_Document **out,const char **error);

int AvDeviceGetProtocolInfo(IXML_Document *in,IXML_Document **out,const char **error);
#endif // DLNADEVICE_H
