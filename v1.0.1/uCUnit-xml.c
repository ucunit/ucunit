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
    char tempBuffer[getSizeOfTestcases()];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    strcat(tempBuffer, "\t<testcases>\n");

    unsigned int i;
    unsigned int j;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        sprintf(tempBuffer, "\t\t<testcase name=\"%s\">\n",
                staticTestSuite.testCases[i].testCaseName);
        strcat(tempBuffer, "\t\t\t<system-out>\n");
        strcat(tempBuffer, "\t\t\t\t![CDATA[\n");

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
        strncat(tempBuffer, systemOut, strlen(systemOut));
        memset(systemOut, 0, sizeof(systemOut));
        strcat(tempBuffer, "\t\t\t\t]]>\n\t\t\t</system-out>\n");

        if (!(staticTestSuite.testCases[i].isPassed))
        {
            strcat(xmlString, "\t\t\t<failure>\n");
            strncat(tempBuffer, failures, strlen(failures));
            memset(failures, 0, sizeof(failures));
            strcat(tempBuffer, "\t\t\t</failure>\n");
        }
        strcat(tempBuffer, "\t\t</testcase>\n");
    }
    strcat(tempBuffer, "\t</testcases>\n");

    strncat(xmlString, tempBuffer, strlen(tempBuffer));
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
    char tempBuffer[staticTestSuite.xmlBufferSize];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    UCUNIT_XML_GetXmlHeader(tempBuffer);
    UCUNIT_XML_GetTestsuiteBegin(tempBuffer);
    UCUNIT_XML_GetProperties(tempBuffer);
    UCUNIT_XML_GetTestcases(tempBuffer);
    UCUNIT_XML_GetTestsuiteClose(tempBuffer);

    strncat(xmlString, tempBuffer, strlen(tempBuffer));

//    UCUNIT_XML_GetXmlHeader(xmlString);
//    UCUNIT_XML_GetTestsuiteBegin(xmlString);
//    UCUNIT_XML_GetProperties(xmlString);
//    UCUNIT_XML_GetTestcases(xmlString);
//    UCUNIT_XML_GetTestsuiteClose(xmlString);
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
    staticTestSuite.xmlBufferSize += getSizeOfHeader();
    /*testsuite begin*/
    staticTestSuite.xmlBufferSize += strlen("<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n");
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(ucunit_testcases_failed));
    staticTestSuite.xmlBufferSize += strlen(staticTestSuite.testSuiteName);
    staticTestSuite.xmlBufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.numOfTestCases));

    staticTestSuite.xmlBufferSize += getSizeOfProperties();
    staticTestSuite.xmlBufferSize += getSizeOfTestcases();
    staticTestSuite.xmlBufferSize += strlen("</testsuite>\n");

    return staticTestSuite.xmlBufferSize;
}

unsigned int getSizeOfHeader()
{
    unsigned int bufferSize = 0;

    bufferSize = strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n");
    bufferSize = strlen(XML_VERSION);
    bufferSize = strlen(XML_ENCODING);

    return bufferSize;
}

unsigned int getSizeOfProperties()
{
    unsigned int bufferSize = 0;

    bufferSize += strlen("\t<properties>\n");
    bufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    bufferSize += 20; /*size of formatted date string*/
    bufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    bufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_hour));
    bufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_min));
    bufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.time.tm_sec));
    bufferSize += strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n");
    bufferSize += strlen(UCUNIT_DefineToString(staticTestSuite.ucunitVersion));
    bufferSize += strlen("\t</properties>\n");

    return bufferSize;
}

unsigned int getSizeOfTestcases()
{
    unsigned int bufferSize = 0;

    bufferSize += strlen("\t</testcases>\n");
    unsigned int i;
    unsigned int j;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        bufferSize += strlen(staticTestSuite.testCases[i].testCaseName);
        bufferSize += strlen("\t\t<testcase name=\"%s\">\n");

        bufferSize += getSizeOfSystemOut(i);
        if (!(staticTestSuite.testCases[i].isPassed))
        {
            bufferSize += strlen("\t\t\t<failure>\n");
            bufferSize += getSizeOfFailures(i);
            bufferSize += strlen("\t\t\t</failure>\n");
        }
        bufferSize += strlen("\t\t</testcase>\n");
    }
    bufferSize += strlen("\t</testcases>\n");

    return bufferSize;
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
