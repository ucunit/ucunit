/*
 * uCUnit-xml.c
 *
 *  Created on: 17 Nov 2020
 *      Author: Zoltan Selmeczi
 */

/* ----- Includes ---------------------------------------------------------- */

#include "uCUnit-xml.h"
#include "uCUnit-v1.0.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE   256

/* ----- Functions --------------------------------------------------------- */
#ifdef UCUNIT_MODE_XML
static UCUNIT_XmlTestSuite staticTestSuite;

void UCUNIT_XML_TestBegin(char *testSuiteName)
{
    staticTestSuite.testSuiteName = testSuiteName;
    time_t currentTime = time(NULL);
    staticTestSuite.time = *localtime(&currentTime);
    staticTestSuite.ucunitVersion = UCUNIT_VERSION;
}

void UCUNIT_XML_TestcaseBegin(char *testCaseName)
{
    /* TODO: implement */
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    /* TODO: implement */
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments)
{
    /* TODO implement */
}

void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite *testSuite)
{
    *testSuite = staticTestSuite;
}

void UCUNIT_XML_GetHeaderString(char* xmlString)
{
    /* TODO implement */
}

void UCUNIT_XML_GetPropertiesString(char *xmlString)
{
    strcat(xmlString, "<properties>\n");

    char tempBuffer[64] = { 0 };
    /* TODO: Change the output date format to e.g. "Nov 16 2020" */
    sprintf(tempBuffer,
            "\t<property name='compiled' value='%d-%02d-%02d'/>\n",
            staticTestSuite.time.tm_year + 1900,
            staticTestSuite.time.tm_mon + 1,
            staticTestSuite.time.tm_mday);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "\t<property name='time' value='%02d:%02d:%02d'/>\n",
            staticTestSuite.time.tm_hour,
            staticTestSuite.time.tm_min,
            staticTestSuite.time.tm_sec);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "\t<property name='ucunit-version' value='%s'/>\n",
            staticTestSuite.ucunitVersion);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));

    strcat(xmlString, "</properties>\n");
}

void UCUNIT_XML_GetTestcasesString(char* xmlString)
{
    /* TODO implement */
}
#else
void UCUNIT_XML_TestBegin(char* testSuitName)
{
    /* Empty implementation */
}

void UCUNIT_XML_TestcaseBegin(char* testCaseName)
{
    /* Empty implementation */
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    /* Empty implementation */
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments)
{
    /* Empty implementation */
}

void UCUNIT_XML_TestSummary(int testCasesFailed, int testCasesPassed, int checksFailed, int checksPassed)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite* test)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetHeaderString(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetPropertiesString(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetTestcasesString(char* xmlString)
{
    /* Empty implementation */
}
#endif
