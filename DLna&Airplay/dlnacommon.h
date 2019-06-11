#ifndef DLNACOMMON_H
#define DLNACOMMON_H

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

#include <QString>
#include "devstruct.h"

/*----------------------------XML----------------------*/
/*!
 * \brief Given a DOM node such as <Channel>11</Channel>, this routine
 * extracts the value (e.g., 11) from the node and returns it as
 * a string. The string must be freed by the caller using free.
 *
 * \return The DOM node as a string.
 */
char *getElementValue(
    /*! [in] The DOM node from which to extract the value. */
    IXML_Element *element);

/*!
 * \brief Given a DOM node representing a UPnP Device Description Document,
 * this routine parses the document and finds the Number service list
 * (i.e., the service list for the root device).  The service list
 * is returned as a DOM node list. The NodeList must be freed using
 * NodeList_free.
 *
 * \return The service list is returned as a DOM node list.
 */

IXML_NodeList *getFirstServiceList(/*! [in] The DOM node from which to extract the service list. */
    IXML_Document *doc);

/*!
 * \brief Given a document node, this routine searches for the first element
 * named by the input string item, and returns its value as a string.
 * String must be freed by caller using free.
 */
char *getFirstDocumentItem(
    /*! [in] The DOM document from which to extract the value. */
    IXML_Document *doc,
    /*! [in] The item to search for. */
    const char *item);

/*!
 * \brief Given a DOM element, this routine searches for the first element
 * named by the input string item, and returns its value as a string.
 * The string must be freed using free.
 */
char *getFirstElementItem(
    /*! [in] The DOM element from which to extract the value. */
    IXML_Element *element,
    /*! [in] The item to search for. */
    const char *item);

/*!
 * \brief This routine finds the first occurance of a service in a DOM
 * representation of a description document and parses it.  Note that this
 * function currently assumes that the eventURL and controlURL values in
 * the service definitions are full URLs.  Relative URLs are not handled here.
 */
int findAndParseService (/*! [in] The DOM description document. */
    IXML_Document *DescDoc,
    /*! [in] The location of the description document. */
    const char *location,
    /*! [in] The type of service to search for. */
    const char *serviceType,
    /*! [out] The service ID. */
    char **serviceId,
    /*! [out] The event URL for the service. */
    char **eventURL,
    /*! [out] The control URL for the service. */
    char **controlURL);

#endif // DLNACOMMON_H
