#ifndef DLNACTRLPOINT_H
#define DLNACTRLPOINT_H
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
#include <QMap>
#include <QString>

extern UpnpClient_Handle ctrlpt_handle;
extern ithread_mutex_t DeviceListMutex;

/*!
 * \brief Update a Tv state table. Called when an event is received.
 *
 * Note: this function is NOT thread save. It must be called from another
 * function that has locked the global device list.
 **/
void AvStateUpdate(/*! [in] The UDN of the parent device. */
    const char *UDN,
    /*! [out] DOM document representing the XML received with the event. */
    IXML_Document *ChangedVariables);

void	AvCtrlPointHandleEvent(const char *sid, int evntkey, IXML_Document *changes);
void	AvCtrlPointHandleSubscribeUpdate(const char *eventURL, const Upnp_SID sid, int timeout);
int		AvCallbackEventHandler(Upnp_EventType EventType, void *Event, void *Cookie);

void	AvCtrlPointAddDevice(IXML_Document *DescDoc, const char *location, int expires);
int		AvCtrlPointDeleteNode(AvDevice devNode,bool isVerify = false);
int		AvCtrlPointRemoveDevice(const char *UDN);
int		AvCtrlPointRemoveAll(void);

bool    setActionNode(IXML_Document **actionNode, const char *actionname, int service, const char *nodeName, char *nodeVal);

void    ctrlPointInit();

int		AvCtrlPointSendActionArgs(const char *UDN, int service, const char *actionName, QList<actionKey_Value> ages);

AvDevice AvCtrlPointGetDevice(const char *UDN);

int		AvCtrlPointSendSetVolume(char* UDN, int volume);
int		AvCtrlPointSendGetVolume(char* UDN);
int     AvCtrlPointSendSetMute(char* UDN, bool mute);
int     AvCtrlPointSendGetMute(char* UDN);
int     AvCtrlPointSendSetTransportURI(char* UDN,char* url);
int     AvCtrlPointSendPlay(char* UDN);
int     AvCtrlPointSendPause(char* UDN);
int     AvCtrlPointSendStop(char* UDN);
int     AvCtrlPointSendGetPoint(char* UDN);
int     AvCtrlPointSendGetTransportInfo(char* UDN);
int     AvCtrlPointSendSeek(char* UDN,char* seekTime);
int     AvCtrlPointSendGetProtocolInfo(const char *UDN);

void    stopCtrlPoint();
#endif // DLNACTRLPOINT_H
