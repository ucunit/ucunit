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

int ucunit_checks_failed = 0;
int ucunit_checks_passed = 0; /* Number of passed checks */
int ucunit_testcases_failed = 0; /* Number of failed test cases */
int ucunit_testcases_passed = 0; /* Number of passed test cases */
int ucunit_testcases_failed_checks = 0; /* Number of failed checks in a testcase */
int ucunit_checklist_failed_checks = 0; /* Number of failed checks in a checklist */
int ucunit_action = UCUNIT_ACTION_WARNING; /* Action to take if a check fails */
int ucunit_checkpoints[UCUNIT_MAX_TRACEPOINTS] = { 0 }; /* Max. number of tracepoints */
int ucunit_index = 0; /* Tracepoint index */

/* ----- Functions --------------------------------------------------------- */
#ifdef UCUNIT_MODE_XML
static UCUNIT_XmlTestSuite staticTestSuite;

/* ----- Buffer size calculation for the xml string array ------------------ */
static size_t getSizeOfHeader()
{
    size_t bufferSize = 0;

    bufferSize += 31;  // strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n") - 4
    bufferSize += strlen(XML_VERSION);
    bufferSize += strlen(XML_ENCODING);

    return bufferSize;
}

static size_t getSizeOfProperties()
{
    size_t bufferSize = 0;

    bufferSize += 14;   // strlen("\t<properties>\n")
    bufferSize += 39; // strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n") - 2
    bufferSize += 20;   // size of formatted date string
    bufferSize += 40; // strlen("\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n") - 9
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_hour)
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_min)
    bufferSize += 4;    // sizeof(staticTestSuite.time.tm_min)
    bufferSize += 45; // strlen("\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n") - 2
    bufferSize += strlen(staticTestSuite.ucunitVersion);
    bufferSize += 15;   // strlen("\t</properties>\n")

    return bufferSize;
}

static size_t getSizeOfTestsuiteBegin()
{
    size_t bufferSize = 0;

    bufferSize += 50; // strlen("<testsuite errors=\"%d\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n") - 8
    bufferSize += 4;  // sizeof(ucunit_testcases_failed)
    bufferSize += strlen(staticTestSuite.testSuiteName);
    bufferSize += 4;  // sizeof(staticTestSuite.numOfTestCases)
    bufferSize += 1;  //sizeof(uint8_t)

    return bufferSize;
}

static size_t getSizeOfCheck(uint8_t i, uint8_t j, size_t fileNameLength,
                             const char *result)
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
            bufferSize += getSizeOfCheck(i, j, fileNameLength, "passed");
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
            bufferSize += getSizeOfCheck(i, j, fileNameLength, "failed");
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
        bufferSize += 25;       // strlen( "\t\t\t\t]]>\n\t\t\t</system-out>\n")
        if (!(staticTestSuite.testCases[i].isPassed))
        {
            bufferSize += 13;    // strlen("\t\t\t<failure>\n")
            bufferSize += getSizeOfFailures(i, fileNameLength);
            bufferSize += 14;    // strlen("\t\t\t</failure>\n")
        }
        if (staticTestSuite.testCases[i].errorFlag)
        {
            bufferSize += 15;    //strlen("\t\t<system-err>\n")
            bufferSize += 65; //strlen("\t\t\tToo many checks.The allowed amount of checks per testcase is %d\n") - 2
            bufferSize += 4;     //sizeof(MAX_NUM_OF_CHECKS_PER_TESTCASE)
            bufferSize += 16;    //strlen("\t\t</system-err>\n")
        }
        bufferSize += 14;        // strlen("\t\t</testcase>\n")
    }
    return bufferSize;
}

void UCUNIT_XML_GetSizeOfTestsuite(size_t *bufferSize)
{

    *bufferSize += getSizeOfHeader();
    *bufferSize += getSizeOfTestsuiteBegin();
    *bufferSize += getSizeOfProperties();
    *bufferSize += getSizeOfTestcases();
    if (staticTestSuite.errorFlag)
    {
        *bufferSize += 15;    //strlen("\t\t<system-err>\n")
        *bufferSize += 71; // strlen("\t\tToo many testcases.The allowed amount of testcases per testsuite is %d\n") - 2
        *bufferSize += 4;     //sizeof(MAX_NUM_OF_TEST_CASES)
        *bufferSize += 16;    //strlen("\t\t</system-err>\n")
    }
    *bufferSize += 13; // strlen("</testsuite>\n")
}

/* ----- Setting up the test data structures -------------------------------------- */
static uint8_t getNumberOfErrors()
{
    uint8_t numberOfErrors = 0;
    if (staticTestSuite.errorFlag)
    {
        numberOfErrors++;
    }
    uint8_t i;
    for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
    {
        if (staticTestSuite.testCases[i].errorFlag)
        {
            numberOfErrors++;
        }
    }
    return numberOfErrors;
}

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
    if (staticTestSuite.errorFlag)
    {
        return;
    }
    UCUNIT_XmlTestCase testcase;
    testcase.errorFlag = false;
    testcase.testCaseName = testCaseName;
    testcase.numOfChecks = 0;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases] = testcase;
    staticTestSuite.numOfTestCases++;
    if (staticTestSuite.numOfTestCases == MAX_NUM_OF_TEST_CASES)
    {
        staticTestSuite.errorFlag = true;
    }
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].isPassed =
            isPassed;
    if ((isPassed))
    {
        ucunit_testcases_passed++;
    }
    else
    {
        ucunit_testcases_failed++;
    }
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char *type, char *arguments,
                              char *line)
{
    if (staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].errorFlag)
    {
        return;
    }
    if (staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks
                == MAX_NUM_OF_CHECKS_PER_TESTCASE-1)
    {
        staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].errorFlag =
                true;
    }
    UCUNIT_XmlCheck check;
    check.isPassed = isPassed;
    check.type = type;
    check.arguments = arguments;
    check.lineNumber = line;

    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].checks[staticTestSuite.testCases[staticTestSuite.numOfTestCases
            - 1].numOfChecks] = check;
    staticTestSuite.testCases[staticTestSuite.numOfTestCases - 1].numOfChecks++;
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
            "<testsuite errors=\"%d\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n",
            getNumberOfErrors(), ucunit_testcases_failed,
            staticTestSuite.testSuiteName, staticTestSuite.numOfTestCases);
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

    sprintf(tempBuffer,
            "\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n",
            staticTestSuite.time.tm_hour, staticTestSuite.time.tm_min,
            staticTestSuite.time.tm_sec);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n",
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
        UCUNIT_XML_GetTestcase(xmlString, i);
    }
}

void UCUNIT_XML_GetTestcase(char *xmlString, uint8_t i)
{
    char tempBuffer[100] = { 0 };

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
            UCUNIT_XML_GetChecks(systemOut, i, j, "passed");
        }
        else
        {
            UCUNIT_XML_GetChecks(failures, i, j, "failed");
        }
    }

    strncat(xmlString, systemOut, strlen(systemOut));
    strcat(xmlString, "\t\t\t]]>\n\t\t</system-out>\n");
    if (!(staticTestSuite.testCases[i].isPassed)
            && getSizeOfFailures(i, strlen(staticTestSuite.filePath) != 0))
    {
        strcat(xmlString, "\t\t<failure>\n");
        strncat(xmlString, failures, strlen(failures));
        strcat(xmlString, "\t\t</failure>\n");
    }

    if (staticTestSuite.testCases[i].errorFlag)
    {
        strcat(xmlString, "\t\t<system-err>\n");
        sprintf(tempBuffer,
                "\t\t\tToo many checks within this testcase.The allowed amount of checks per testcase is %d\n",
                MAX_NUM_OF_CHECKS_PER_TESTCASE);
        strncat(xmlString, tempBuffer, strlen(tempBuffer));
        memset(tempBuffer, 0, sizeof(tempBuffer));
        strcat(xmlString, "\t\t</system-err>\n");
    }
    strcat(xmlString, "\t</testcase>\n");
}

void UCUNIT_XML_GetChecks(char *xmlString, uint8_t i, uint8_t j,
                          const char *result)
{
    char tempBuffer[getSizeOfCheck(i, j, strlen(staticTestSuite.filePath),
                                   result)];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t\t%s:%s %s(%s) %s\n", staticTestSuite.filePath,
            staticTestSuite.testCases[i].checks[j].lineNumber,
            staticTestSuite.testCases[i].checks[j].type,
            staticTestSuite.testCases[i].checks[j].arguments, result);

    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
    UCUNIT_XML_GetXmlHeader(xmlString);
    UCUNIT_XML_GetTestsuiteBegin(xmlString);
    UCUNIT_XML_GetProperties(xmlString);
    UCUNIT_XML_GetTestcases(xmlString);
    if (staticTestSuite.errorFlag)
    {
        UCUNIT_XML_GetTestsuiteSystemErr(xmlString);
    }
    strcat(xmlString, "</testsuite>\n");
}

void UCUNIT_XML_GetTestsuiteSystemErr(char *xmlString)
{
    char tempBuffer[100] = { 0 };

    strcat(xmlString, "\t<system-err>\n");
    sprintf(tempBuffer,
            "\t\tToo many testcases.The allowed amount of testcases is %d\n",
            MAX_NUM_OF_TEST_CASES);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));
    strcat(xmlString, "\t</system-err>\n");
}

#else
void UCUNIT_XML_TestBegin(char* testSuiteName,  char *file)
{
    UNUSED(testSuiteName);
    UNUSED(file);
}

void UCUNIT_XML_TestcaseBegin(char* testCaseName)
{
    UNUSED(testCaseName);
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    UNUSED(isPassed);
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments, char* line)
{
    UNUSED(isPassed);
    UNUSED(type);
    UNUSED(arguments);
    UNUSED(line);
}

void UCUNIT_XML_GetXmlHeader(char* xmlString)
{
    UNUSED(xmlString);
}

void UCUNIT_XML_GetTestsuiteBegin(char* xmlString)
{
    UNUSED(xmlString);
}

void UCUNIT_XML_GetProperties(char* xmlString)
{
    UNUSED(xmlString);
}

void UCUNIT_XML_GetTestcases(char* xmlString)
{
    UNUSED(xmlString);
}

void UCUNIT_XML_GetTestsuiteClose(char *xmlString)
{
    UNUSED(xmlString);
}
void UCUNIT_XML_GetTestcase(char *xmlString, uint8_t i)
{
    UNUSED(xmlString);
    UNUSED(i);
}

void UCUNIT_XML_GetChecks(char *xmlString, uint8_t i, uint8_t j,const char *result)
{
    UNUSED(xmlString);
    UNUSED(i);
    UNUSED(j);
    UNUSED(result);
}

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
    UNUSED(xmlString);
}

void UCUNIT_XML_GetSizeOfTestsuite(size_t *bufferSize)
{
    UNUSED(bufferSize);
}

char* UCUNIT_XML_GetTestFileName(void)
{
    char *ret = 0;
    return ret;
}

void UCUNIT_XML_GetTestsuiteSystemErr(char *xmlString)
{
    UNUSED(xmlString);
}
#endif
