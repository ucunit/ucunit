/*
 * uCUnit-xml.c
 *
 *  Created on: 17 Nov 2020
 *      Author: Zoltan Selmeczi
 */

/* ----- Includes ---------------------------------------------------------- */

#include "uCUnit-xml.h"
#include "uCUnit-v1.0.h"

#define XML_VERSION "1.0"
#define XML_ENCODING "utf-8"

/* ----- Functions --------------------------------------------------------- */
#ifdef UCUNIT_MODE_XML
static UCUNIT_XmlTestSuite staticTestSuite;


/* ----- Buffer size calculation for the xml string array ------------------ */
static size_t  getSizeOfHeader()
{
    size_t bufferSize = 0;

    bufferSize += 31;   // strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n") - 4
    bufferSize += strlen(XML_VERSION);
    bufferSize += strlen(XML_ENCODING);

    return bufferSize;
}

static size_t  getSizeOfProperties()
{
    size_t bufferSize = 0;

    bufferSize += 14;   // strlen("\t<properties>\n")
    bufferSize += 39;   // strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n") - 2
    bufferSize += 20;   // size of formatted date string
    bufferSize += 40;   // strlen("\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n") - 9
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_hour)
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_min)
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_min)
    bufferSize += 45;   // strlen("\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n") - 2
    bufferSize += strlen(staticTestSuite.ucunitVersion);
    bufferSize += 15;   // strlen("\t</properties>\n")

    return bufferSize;
}

static size_t getSizeOfTestsuiteBegin()
{
    size_t bufferSize = 0;

    bufferSize += 52; // strlen("<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n") - 6
    bufferSize += 4; // sizeof(ucunit_testcases_failed)
    bufferSize += strlen(staticTestSuite.testSuiteName);
    bufferSize += 4; // sizeof(staticTestSuite.numOfTestCases)

    return bufferSize;
}

static size_t  getSizeOfCheck(uint8_t i, uint8_t j, size_t fileNameLength, const char *result)
{
    size_t bufferSize = 0;

    bufferSize += 10;   // strlen("\t\t\t\t%s:%s %s(%s) %s\n") - 10
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].type);
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].arguments);
    bufferSize += strlen(staticTestSuite.testCases[i].checks[j].lineNumber);
    bufferSize += fileNameLength;
    bufferSize += strlen(result);

    return bufferSize;
}

static size_t getSizeOfSystemOut(uint8_t i, size_t fileNameLength)
{
    size_t bufferSize = 0;

    uint8_t j;
    for (j = 0; j < staticTestSuite.testCases[i].numOfChecks; ++j)
    {
        if ((staticTestSuite.testCases[i].checks[j].isPassed))
        {
            bufferSize += 1;   // strlen("\t")
            bufferSize += getSizeOfCheck(i,j,fileNameLength, "passed");
        }
    }
    return bufferSize;
}

static size_t getSizeOfFailures(uint8_t i, size_t fileNameLength)
{
    size_t bufferSize = 0;

    uint8_t j;
    for (j = 0; j < staticTestSuite.testCases[i].numOfChecks; ++j)
    {
        if (!(staticTestSuite.testCases[i].checks[j].isPassed))
        {
            bufferSize += getSizeOfCheck(i,j,fileNameLength, "failed");
        }
    }
    return bufferSize;
}

static size_t getSizeOfTestcases()
{
    size_t bufferSize = 0;
    size_t fileNameLength = strlen(staticTestSuite.filePath);

    uint8_t i;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        bufferSize += strlen(staticTestSuite.testCases[i].testCaseName);
        bufferSize += 21;        // strlen("\t\t<testcase name=\"%s\">\n") - 2
        bufferSize += 16;        // strlen("\t\t\t<system-out>\n")
        bufferSize += 14;        // strlen("\t\t\t\t<![CDATA[\n")
        bufferSize += getSizeOfSystemOut(i, fileNameLength);
        bufferSize += 25;        // strlen( "\t\t\t\t]]>\n\t\t\t</system-out>\n")
        if (!(staticTestSuite.testCases[i].isPassed))
        {
            bufferSize += 13;    // strlen("\t\t\t<failure>\n")
            bufferSize += getSizeOfFailures(i, fileNameLength);
            bufferSize += 14;    // strlen("\t\t\t</failure>\n")
        }
        bufferSize += 14;        // strlen("\t\t</testcase>\n")
    }
    return bufferSize;
}

size_t  UCUNIT_XML_GetSizeOfTestsuite()
{
    size_t bufferSize = 0;

    bufferSize += getSizeOfHeader();
    bufferSize += getSizeOfTestsuiteBegin();
    bufferSize += getSizeOfProperties();
    bufferSize += getSizeOfTestcases();
    bufferSize += 13; // strlen("</testsuite>\n")

    return bufferSize;
}

/* ----- Setting up the test data structures -------------------------------------- */
void UCUNIT_XML_TestBegin(char *testSuiteName, char *file)
{
    staticTestSuite.testSuiteName = testSuiteName;
    staticTestSuite.filePath = file;
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
    if (staticTestSuite.numOfTestCases > MAX_NUM_OF_TEST_CASES)
    {

    }
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

void UCUNIT_XML_CheckExecuted(bool isPassed, char *type, char *arguments, char *line)
{
    UCUNIT_XmlCheck check;
    check.isPassed = isPassed;
    check.type = type;
    check.arguments = arguments;
    check.lineNumber = line;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].checks[staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks] = check;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks += 1;
    if (staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks > MAX_NUM_OF_CHECKS_PER_TESTCASE)
    {

    }
}

/* ----- Assembling the xml object into a string array -------------------------------------- */
void UCUNIT_XML_GetXmlHeader(char *xmlString)
{
    char tempBuffer[getSizeOfHeader()];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "<?xml version=\"%s\" encoding=\"%s\"?>\n",
    XML_VERSION,
            XML_ENCODING);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

void UCUNIT_XML_GetTestsuiteBegin(char *xmlString)
{
    char tempBuffer[getSizeOfTestsuiteBegin()];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n",
            ucunit_testcases_failed,
            staticTestSuite.testSuiteName,
            staticTestSuite.numOfTestCases);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
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
    memset(tempBuffer, 0, sizeof(tempBuffer));

    strcat(xmlString, "\t</properties>\n");
}

void UCUNIT_XML_GetTestcases(char *xmlString)
{
    uint8_t i;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        UCUNIT_XML_GetTestcase(xmlString,i);
    }
}

void UCUNIT_XML_GetTestcase(char *xmlString, uint8_t i)
{
    char tempBuffer[64] = { 0 };

    sprintf(tempBuffer, "\t<testcase name=\"%s\">\n",
            staticTestSuite.testCases[i].testCaseName);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));
    strcat(xmlString, "\t\t<system-out>\n");
    strcat(xmlString, "\t\t\t<![CDATA[\n");

    char systemOut[getSizeOfSystemOut(i, strlen(staticTestSuite.filePath))];
    char failures[getSizeOfFailures(i, strlen(staticTestSuite.filePath))];
    memset(systemOut, 0, sizeof(systemOut));
    memset(failures, 0, sizeof(failures));

    uint8_t j;
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
    strcat(xmlString, "\t\t\t]]>\n\t\t</system-out>\n");
    if (!(staticTestSuite.testCases[i].isPassed))
    {
        strcat(xmlString, "\t\t<failure>\n");
        strncat(xmlString, failures, strlen(failures));
        strcat(xmlString, "\t\t</failure>\n");
    }
    strcat(xmlString, "\t</testcase>\n");
}

void UCUNIT_XML_GetChecks(char *xmlString, uint8_t i, uint8_t j, const char *result)
{
    char tempBuffer[getSizeOfCheck(i, j,strlen(staticTestSuite.filePath), result)];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t\t%s:%s %s(%s) %s\n",
    staticTestSuite.filePath,
    staticTestSuite.testCases[i].checks[j].lineNumber,
    staticTestSuite.testCases[i].checks[j].type,
    staticTestSuite.testCases[i].checks[j].arguments,
    result);

    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
    UCUNIT_XML_GetXmlHeader(xmlString);
    UCUNIT_XML_GetTestsuiteBegin(xmlString);
    UCUNIT_XML_GetProperties(xmlString);
    UCUNIT_XML_GetTestcases(xmlString);
    strcat(xmlString, "</testsuite>\n");
}

void UCUNIT_XML_WriteXmlObjectToFile(char *xmlString)
{
    char outputfile[strlen(staticTestSuite.filePath)+3];
    memset(outputfile, 0, sizeof(outputfile));
    strncat(outputfile, &staticTestSuite.filePath[0], strlen(staticTestSuite.filePath)-1);
    strcat(outputfile, "xml");

    FILE *fp;

    fp = fopen(outputfile, "w+");
    fprintf(fp, xmlString);
    fclose(fp);
}

char* UCUNIT_XML_GetTestFileName(void)
{
    return staticTestSuite.filePath;
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
void UCUNIT_XML_GetTestcase(char *xmlString, int i)
{

}

void UCUNIT_XML_GetChecks(char *xmlString, int i, int j, const char *result)
{
    /* Empty implementation */
}

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
	/* Empty implementation */
}
void UCUNIT_XML_WriteXmlObjectToFile(char *xmlString)
{
    /* Empty implementation */
}

unsigned int UCUNIT_XML_GetSizeOfTestsuite()
{
    return 1;
}

#endif
