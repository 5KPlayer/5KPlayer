#include "dlnacommon.h"

#include <cstdio>
#include <string>
#include "controlmanager.h"
#include <QDebug>

char *getElementValue(IXML_Element *element)
{
    IXML_Node *child = ixmlNode_getFirstChild((IXML_Node *)element);
    char *temp = NULL;

    if (child != 0 && ixmlNode_getNodeType(child) == eTEXT_NODE)
        temp = strdup(ixmlNode_getNodeValue(child));

    return temp;
}

IXML_NodeList *getFirstServiceList(IXML_Document *doc)
{
    IXML_NodeList *ServiceList = NULL;
    IXML_NodeList *servlistnodelist = NULL;
    IXML_Node *servlistnode = NULL;

    servlistnodelist =
        ixmlDocument_getElementsByTagName(doc, "serviceList");
    if (servlistnodelist && ixmlNodeList_length(servlistnodelist)) {
        /* we only care about the first service list, from the root
         * device */
        servlistnode = ixmlNodeList_item(servlistnodelist, 0);
        /* create as list of DOM nodes */
        ServiceList = ixmlElement_getElementsByTagName(
            (IXML_Element *)servlistnode, "service");
    }
    if (servlistnodelist)
        ixmlNodeList_free(servlistnodelist);

    return ServiceList;
}

char *getFirstDocumentItem(IXML_Document *doc, const char *item)
{
    IXML_NodeList *nodeList = NULL;
    IXML_Node *textNode = NULL;
    IXML_Node *tmpNode = NULL;
    char *ret = NULL;

    nodeList = ixmlDocument_getElementsByTagName(doc, (char *)item);
    if (nodeList) {
        tmpNode = ixmlNodeList_item(nodeList, 0);
        if (tmpNode) {
            textNode = ixmlNode_getFirstChild(tmpNode);
            if (!textNode) {
                ret = strdup("");
                goto epilogue;
            }
            ret = strdup(ixmlNode_getNodeValue(textNode));
            if (!ret) {
                ret = strdup("");
            }
        }
    }

epilogue:
    if (nodeList)
        ixmlNodeList_free(nodeList);

    return ret;
}

char *getFirstElementItem(IXML_Element *element, const char *item)
{
    IXML_NodeList *nodeList = NULL;
    IXML_Node *textNode = NULL;
    IXML_Node *tmpNode = NULL;
    char *ret = NULL;

    nodeList = ixmlElement_getElementsByTagName(element, (char *)item);
    if (nodeList == NULL) {
        return NULL;
    }
    tmpNode = ixmlNodeList_item(nodeList, 0);
    if (!tmpNode) {
        ixmlNodeList_free(nodeList);
        return NULL;
    }
    textNode = ixmlNode_getFirstChild(tmpNode);
    ret = strdup(ixmlNode_getNodeValue(textNode));
    if (!ret) {
        ixmlNodeList_free(nodeList);
        return NULL;
    }
    ixmlNodeList_free(nodeList);

    return ret;
}

int findAndParseService(IXML_Document *DescDoc, const char *location,
                        const char *serviceType, char **serviceId,
                        char **eventURL, char **controlURL)
{
    unsigned int i;
    unsigned long length;
    int found = 0;
    int ret;
    char *tempServiceType = NULL;
    char *baseURL = NULL;
    const char *base = NULL;
    char *relcontrolURL = NULL;
    char *releventURL = NULL;

    IXML_NodeList *serviceList = NULL;
    IXML_Element *service = NULL;

    baseURL = getFirstDocumentItem(DescDoc, "URLBase");
    if (baseURL)
        base = baseURL;
    else
        base = location;

    serviceList = getFirstServiceList(DescDoc);
    length = ixmlNodeList_length(serviceList);
    for (i = 0; i < length; i++) {
        service = (IXML_Element *)ixmlNodeList_item(serviceList, i);
        tempServiceType = getFirstElementItem((IXML_Element *)service, "serviceType");
        if (tempServiceType && strcmp(tempServiceType, serviceType) == 0) {
            *serviceId      = getFirstElementItem(service, "serviceId");
            relcontrolURL   = getFirstElementItem(service, "controlURL");
            releventURL     = getFirstElementItem(service, "eventSubURL");

            *controlURL     = (char*)malloc(strlen(base) + strlen(relcontrolURL) + 1);
            if (*controlURL) {
                ret = UpnpResolveURL(base, relcontrolURL, *controlURL);
                if (ret != UPNP_E_SUCCESS){
                }
//                    SampleUtil_Print("Error generating controlURL from %s + %s\n",
//                        base, relcontrolURL);
            }
            *eventURL = (char*)malloc(strlen(base) + strlen(releventURL) + 1);
            if (*eventURL) {
                ret = UpnpResolveURL(base, releventURL, *eventURL);
                if (ret != UPNP_E_SUCCESS) {
                }
//                    SampleUtil_Print("Error generating eventURL from %s + %s\n",
//                        base, releventURL);
            }

            if(relcontrolURL) {
                free(relcontrolURL);
                relcontrolURL = NULL;
            }
            if(releventURL) {
                free(releventURL);
                releventURL = NULL;
            }
            found = 1;
            break;
        }
        if(tempServiceType) {
            free(tempServiceType);
            tempServiceType = NULL;
        }
    }
    if(tempServiceType) {
        free(tempServiceType);
        tempServiceType = NULL;
    }
    if (serviceList) {
        ixmlNodeList_free(serviceList);
        serviceList = NULL;
    }
    if(baseURL) {
        free(baseURL);
        baseURL = NULL;
    }

    return found;
}

