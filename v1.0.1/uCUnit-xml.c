/*
 * uCUnit-xml.c
 *
 *  Created on: 17 Nov 2020
 *      Author: Zoltan Selmeczi
 */

/* ----- Includes ---------------------------------------------------------- */

#include "uCUnit-xml.h"
#include "uCUnit-v1.0.h"

#define BUFFER_SIZE   256
#define XML_VERSION "1.0"
#define XML_ENCODING "utf-8"

/* ----- Functions --------------------------------------------------------- */
#ifdef UCUNIT_MODE_XML
static UCUNIT_XmlTestSuite staticTestSuite;

void UCUNIT_XML_TestBegin(char *testSuiteName)
{
    staticTestSuite.testSuiteName = testSuiteName;
    time_t currentTime = time(NULL);
    staticTestSuite.time = *localtime(&currentTime);
    staticTestSuite.ucunitVersion = UCUNIT_VERSION;
    staticTestSuite.numOfTestCases = 0;
}

void UCUNIT_XML_TestcaseBegin(char *testCaseName)
{
    UCUNIT_XmlTestCase testcase;
    testcase.testCaseName = testCaseName;
    testcase.numOfChecks = 0;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases] = testcase;
    staticTestSuite.numOfTestCases += 1;
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].isPassed = isPassed;
    if ((isPassed))
    {
        ucunit_testcases_passed++;
    }
    else
    {
        ucunit_testcases_failed++;
    }

}

void UCUNIT_XML_CheckExecuted(bool isPassed, char *type, char *arguments,char *file, char *line)
{
    UCUNIT_XmlCheck check;
    check.isPassed = isPassed;
    check.type = type;
    check.arguments = arguments;
    check.filePath = file;
    check.lineNumber = line;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].checks[staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks] = check;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks += 1;
}

void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite *testSuite)
{
    //testSuite = staticTestSuite;
}

void UCUNIT_XML_GetXmlHeader(char *xmlString)
{
    unsigned int bufferSize = strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n");
    bufferSize += strlen(XML_VERSION);
    bufferSize += strlen(XML_ENCODING);
    char tempBuffer[bufferSize];
    sprintf(tempBuffer, "<?xml version=\"%s\" encoding=\"%s\"?>\n",
    XML_VERSION,
            XML_ENCODING);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));
}

void UCUNIT_XML_GetTestsuiteBegin(char *xmlString)
{
    unsigned int bufferSize = strlen("<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n");
    bufferSize += strlen(UCUNIT_DefineToString(ucunit_testcases_failed));
    bufferSize += strlen(staticTestSuite.testSuiteName);
    bufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.numOfTestCases));
    char tempBuffer[bufferSize];

    sprintf(tempBuffer,
            "<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n",
            ucunit_testcases_failed, staticTestSuite.testSuiteName,
            staticTestSuite.numOfTestCases);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

}

void UCUNIT_XML_GetProperties(char *xmlString)
{
    strcat(xmlString, "\t<properties>\n");

    char tempBuffer[64] = { 0 };

    char formatted_date[20];
    strftime(formatted_date, 40, "%B %d %Y", &staticTestSuite.time);
    sprintf(tempBuffer, "\t\t<property name=\"compiled\" value=\"%s\"/>\n",
            formatted_date);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n",
            staticTestSuite.time.tm_hour, staticTestSuite.time.tm_min,
            staticTestSuite.time.tm_sec);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n",
            staticTestSuite.ucunitVersion);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    strcat(xmlString, "\t</properties>\n");
}

void UCUNIT_XML_GetTestcases(char *xmlString)
{
    strcat(xmlString, "\t<testcases>\n");

    unsigned int i;
    unsigned int j;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        unsigned int bufferSize = strlen(staticTestSuite.testCases[i].testCaseName);
        bufferSize += strlen("\t\t<testcase name=\"%s\">\n");
        char tempBuffer[bufferSize];
        memset(tempBuffer, 0, sizeof(tempBuffer));

        sprintf(tempBuffer, "\t\t<testcase name=\"%s\">\n",
                staticTestSuite.testCases[i].testCaseName);
        strncat(xmlString, tempBuffer, strlen(tempBuffer));

        strcat(xmlString, "\t\t\t<system-out>\n");
        strcat(xmlString, "\t\t\t\t![CDATA[\n");

        char systemOut[getSizeOfSystemOut(i)];
        char failures[getSizeOfSystemOut(i)];

        memset(systemOut, 0, sizeof(systemOut));
        memset(failures, 0, sizeof(failures));

        for (j = 0; j < staticTestSuite.testCases[i].numOfChecks; ++j)
        {
            if ((staticTestSuite.testCases[i].checks[j].isPassed))
            {
                strcat(systemOut, "\t");
                UCUNIT_XML_GetChecks(systemOut,i,j,"passed");
            }else
            {
                UCUNIT_XML_GetChecks(failures,i,j,"failed");
            }
        }
        strncat(xmlString, systemOut, strlen(systemOut));
        memset(systemOut, 0, sizeof(systemOut));
        strcat(xmlString, "\t\t\t\t]]>\n\t\t\t</system-out>\n");

        if (!(staticTestSuite.testCases[i].isPassed))
        {
            strcat(xmlString, "\t\t\t<failure>\n");
            strncat(xmlString, failures, strlen(failures));
            memset(failures, 0, sizeof(failures));
            strcat(xmlString, "\t\t\t</failure>\n");
        }
        strcat(xmlString, "\t\t</testcase>\n");
    }
    strcat(xmlString, "\t</testcases>\n");
}

void UCUNIT_XML_GetChecks(char *xmlString, int i, int j, const char *result)
{
    char tempBuffer[getSizeOfCheck(i, j, result)];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t\t\t%s:%s %s(%s) %s\n",
    staticTestSuite.testCases[i].checks[j].filePath,
    staticTestSuite.testCases[i].checks[j].lineNumber,
    staticTestSuite.testCases[i].checks[j].type,
    staticTestSuite.testCases[i].checks[j].arguments,
    result);

    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

void UCUNIT_XML_GetTestsuiteClose(char *xmlString)
{
    strcat(xmlString, "</testsuite>\n");
}

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
//    char tempBuffer[staticTestSuite.xmlBufferSize];
//    memset(tempBuffer, 0, sizeof(tempBuffer));

    UCUNIT_XML_GetXmlHeader(xmlString);
    UCUNIT_XML_GetTestsuiteBegin(xmlString);
    UCUNIT_XML_GetProperties(xmlString);
    UCUNIT_XML_GetTestcases(xmlString);
    UCUNIT_XML_GetTestsuiteClose(xmlString);

//    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

unsigned int getSizeOfSystemOut(int i)
{
    unsigned int bufferSize = 0;

    unsigned int j;
    for (j = 0; j < staticTestSuite.testCases[i].numOfChecks; ++j)
    {
        if ((staticTestSuite.testCases[i].checks[j].isPassed))
        {
            bufferSize += getSizeOfCheck(i,j, "passed");
        }
    }
    return bufferSize;
}

unsigned int getSizeOfFailures(int i)
{
    unsigned int bufferSize = 0;

    unsigned int j;
    for (j = 0; j < staticTestSuite.testCases[i].numOfChecks; ++j)
    {
        if (!(staticTestSuite.testCases[i].checks[j].isPassed))
        {
            bufferSize += getSizeOfCheck(i,j, "failed");
        }
    }
    return bufferSize;
}

unsigned int getSizeOfCheck(int i, int j, const char *result)
{
    unsigned int bufferSize = 0;

    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].type);
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].arguments);
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].lineNumber);
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].filePath);
    bufferSize += strlen(result);
    bufferSize += 10;

    return bufferSize;
}

unsigned int getSizeOfTestsuite()
{
    /*xml_header*/
    staticTestSuite.xmlBufferSize = strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n");
    staticTestSuite.xmlBufferSize = strlen(XML_VERSION);
    staticTestSuite.xmlBufferSize = strlen(XML_ENCODING);
    /*properties*/
    staticTestSuite.xmlBufferSize += strlen("\t<properties>\n");
    staticTestSuite.xmlBufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    staticTestSuite.xmlBufferSize += 20;
    staticTestSuite.xmlBufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_hour));
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_min));
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_sec));
    staticTestSuite.xmlBufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.ucunitVersion));
    staticTestSuite.xmlBufferSize += strlen("\t</properties>\n");
    /*testcases*/
    unsigned int i;
    unsigned int j;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        staticTestSuite.xmlBufferSize += strlen(staticTestSuite.testCases[i].testCaseName);
        staticTestSuite.xmlBufferSize += strlen("\t\t<testcase name=\"%s\">\n");

        staticTestSuite.xmlBufferSize += getSizeOfSystemOut(i);
        if (!(staticTestSuite.testCases[i].isPassed))
        {
            staticTestSuite.xmlBufferSize += strlen("\t\t\t<failure>\n");
            staticTestSuite.xmlBufferSize += getSizeOfFailures(i);
            staticTestSuite.xmlBufferSize += strlen("\t\t\t</failure>\n");
        }
        staticTestSuite.xmlBufferSize += strlen("\t\t</testcase>\n");
    }
    staticTestSuite.xmlBufferSize += strlen("\t</testcases>\n");
    staticTestSuite.xmlBufferSize += strlen("</testsuite>\n");

    return staticTestSuite.xmlBufferSize;
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

void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments, char* file, char* line)
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

void UCUNIT_XML_GetXmlHeader(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetTestsuiteBegin(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetProperties(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetTestcases(char* xmlString)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetTestsuiteClose(char *xmlString)
{
    /* Empty implementation */
}
void UCUNIT_XML_GetXmlObject(char *xmlString)
{
	/* Empty implementation */
}
#endif
