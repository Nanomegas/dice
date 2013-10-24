///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "DevsXmlFile.h"

#include "curl/curl.h"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlreader.h>
#include <libxml2/libxml/tree.h>
#include <libxml2/libxml/xmlversion.h>
#include <sys/time.h>
#include <vector>
#include <string.h>

#define CURL_STATICLIB

struct MemoryStruct {
    char *memory;
    size_t size;
};

static struct MemoryStruct ohciDb;

static size_t writer (void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */ 
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';
    
    return realsize;
}

static long gLastLoadTime = 0L;

int getDBCacheAge(void)
{
	timeval curTime;
	long lPrevTime, lCurTime = 0;
	gettimeofday (&curTime, NULL);
	lCurTime = curTime.tv_sec;
	lPrevTime = gLastLoadTime;
	
	return (lCurTime - lPrevTime);
	
}

static std::vector<fwi::OHCI1394_DEV_INFO> vendorsFromXML;

// get attributes of a "controller" XML_ELEMENT_NODE
void parseController(xmlDocPtr doc, xmlNodePtr cur)
{
	if (0 == cur)
	{	
		return;
	}
		
	if (XML_ELEMENT_NODE != cur->type)
	{
		return;
	}
	
	if (xmlStrcmp(cur->name, (const xmlChar *)"controller"))
	{
		return;
	}
	
//	printf("\nnew controller (%lu):\n", vendorsFromXML.size());

	fwi::OHCI1394_DEV_INFO vInfo = { 0 };

	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"vendorid")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.vendorId, (char *)key, NUM_STR_SIZE-1);
				}
//				printf(" vendorId: %s\n", vInfo.vendorId);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"deviceid")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.deviceId, (char *)key, NUM_STR_SIZE-1);
				}
//				printf(" deviceId: %s\n", vInfo.deviceId);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"vendorname")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.vendorName, (char *)key, VENDOR_SIZE-1);
				}
//				printf(" vendorName: %s\n", vInfo.vendorName);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"chipset")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.chipset, (char *)key, CHIPSET_SIZE-1);
				}
				printf(" chipset: %s\n", vInfo.chipset);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"maxtx")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.maxTx, (char *)key, NUM_STR_SIZE-1);
				}
//				printf(" maxTx: %s\n", vInfo.maxTx);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"maxrx")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.maxRx, (char *)key, NUM_STR_SIZE-1);
				}
//				printf(" maxRx: %s\n", vInfo.maxRx);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"notes")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				if (strlen((const char *)key) > 0)
				{
					strncpy((char *)vInfo.notes, (char *)key, NOTES_SIZE-1);
				}
//				printf(" notes: %s\n", vInfo.notes);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"maxspeed")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				sscanf((char *)key, "%d", &vInfo.maxspeed);
//				printf(" maxspeed: %d\n", vInfo.maxspeed);
				xmlFree(key);
			}
		}
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"support")))
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
				sscanf((char *)key, "%d", &vInfo.support);
//				printf(" support: %d\n", vInfo.support);
				xmlFree(key);
			}
		}
		else
		{
			if (cur->xmlChildrenNode)
			{
				xmlChar *key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
//				printf("unknown key %s\n", key);
				xmlFree(key);
			}
		}
		cur = cur->next;
	}

	vendorsFromXML.push_back(vInfo);
}

void parseOhciDB(void)
{
	vendorsFromXML.clear();
	
	xmlDocPtr doc = 0;
	xmlNodePtr cur = 0;
	
	doc = xmlParseDoc((const xmlChar *)ohciDb.memory);
	if (doc == NULL )
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return;
	}
	
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL)
	{
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "ohci1394devices"))
	{
		fprintf(stderr,"document of the wrong type");
		xmlFreeDoc(doc);
		return;
	}
	
	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"controller")))
		{
			parseController (doc, cur);
		}
		
		cur = cur->next;
	}

	xmlFreeDoc(doc);

    return;
}

void devsXMLInitialize(void)
{
	// initialize only if 10 minutes have passed since last init
	long lMinutes = getDBCacheAge()/60;
	
	if ((lMinutes < 0) || (lMinutes > 10))
	{
		CURL *curl = 0;
		CURLcode res;
    
		ohciDb.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */ 
		ohciDb.size = 0;    /* no data at this point */ 
        
		curl_global_init(CURL_GLOBAL_ALL);
		curl = curl_easy_init();
    
		if (curl)
		{
			curl_easy_setopt(curl, CURLOPT_URL, "http://www.tctechnologies.tc/appdata/ohci1394db.xml/");
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_ALL);
			curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // the http URL redirects to an https URL
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&ohciDb);
			res = curl_easy_perform(curl);
			if (CURLE_OK != res)
			{
				printf ("curl error %d \n", (int)res);
			}
//			printf("%s", ohciDb.memory);
			
			curl_easy_cleanup(curl);
		}
		
		timeval curTime;
		gettimeofday (&curTime, NULL);
		gLastLoadTime = curTime.tv_sec;
		
		parseOhciDB();
	}
}

void devsXMLUninitialize(void)
{
    if(ohciDb.memory)
        free(ohciDb.memory);

	vendorsFromXML.clear();
    
    curl_global_cleanup();
}

long findControllerInfoXML(fwi::OHCI1394_DEV_INFO * devInfo)
{
	devsXMLInitialize();
	
	for (size_t index=0; index<vendorsFromXML.size(); index++)
	{	
		if ( (0 == strncmp(vendorsFromXML[index].vendorId, devInfo->vendorId, NUM_STR_SIZE))
		  && (0 == strncmp(vendorsFromXML[index].deviceId, devInfo->deviceId, NUM_STR_SIZE)) )
		{
			strncpy(devInfo->vendorName, vendorsFromXML[index].vendorName, VENDOR_SIZE);
			strncpy(devInfo->chipset, vendorsFromXML[index].chipset, CHIPSET_SIZE);
			strncpy(devInfo->maxTx, vendorsFromXML[index].maxTx, NUM_STR_SIZE);
			strncpy(devInfo->maxRx, vendorsFromXML[index].maxRx, NUM_STR_SIZE);
			devInfo->maxspeed = vendorsFromXML[index].maxspeed;
			strncpy(devInfo->notes, vendorsFromXML[index].notes, NOTES_SIZE);
			devInfo->support = vendorsFromXML[index].support;
			devInfo->bValid = true;
			devInfo->bFound = true;
			return 0;
		}
	}
	
    return -1;
}
