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
static UCUNIT_XmlCheck checks[MAX_NUM_OF_CHECKS];
static UCUNIT_XmlTestCase testcases[MAX_NUM_OF_TEST_CASES];

/* ----- Buffer size calculation for the xml string array ------------------ */
static size_t getSizeOfHeader()
{
    size_t bufferSize = 0u;

    bufferSize += 31u;  // strlen("<?xml version=\"%s\" encoding=\"%s\"?>\n") - 4
    bufferSize += strlen(XML_VERSION);
    bufferSize += strlen(XML_ENCODING);

    return bufferSize;
}

static size_t getSizeOfProperties()
{
    size_t bufferSize = 0u;

    bufferSize += 14u;   // strlen("\t<properties>\n")
    bufferSize += 39u; // strlen("\t\t<property name=\"compiled\" value=\"%s\"/>\n") - 2
    bufferSize += 20u;   // size of formatted date string
    bufferSize += 40u; // strlen("\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n") - 9
    bufferSize += sizeof(staticTestSuite.time.tm_hour);
    bufferSize += sizeof(staticTestSuite.time.tm_min);
    bufferSize += sizeof(staticTestSuite.time.tm_min);
    bufferSize += 45u; // strlen("\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n") - 2
    bufferSize += strlen(staticTestSuite.ucunitVersion);
    bufferSize += 15u;   // strlen("\t</properties>\n")

    return bufferSize;
}

static size_t getSizeOfTestsuiteBegin()
{
    size_t bufferSize = 0u;

    bufferSize += 50u; // strlen("<testsuite errors=\"%d\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n") - 8
    bufferSize += sizeof(ucunit_testcases_failed);
    bufferSize += strlen(staticTestSuite.testSuiteName);
    bufferSize += sizeof(staticTestSuite.numOfTestCases);
    bufferSize += sizeof(uint16_t);

    return bufferSize;
}

static size_t getSizeOfCheck(uint16_t checkIndex, size_t fileNameLength, const char *result)
{
    size_t bufferSize = 0u;

    bufferSize += 10u;   // strlen("\t\t\t\t%s:%s %s(%s) %s\n") - 10
    bufferSize += strlen(checks[checkIndex].type);
    bufferSize += strlen(checks[checkIndex].arguments);
    bufferSize += strlen(checks[checkIndex].lineNumber);
    bufferSize += fileNameLength;
    bufferSize += strlen(result);

    return bufferSize;
}

static size_t getSizeOfSystemOut(uint16_t testcaseIndex, uint16_t firstTestcaseCheckIndex, size_t fileNameLength)
{
    size_t bufferSize = 0u;

    uint16_t checkIndex;
    for (checkIndex = firstTestcaseCheckIndex;
            checkIndex < (firstTestcaseCheckIndex + testcases[testcaseIndex].numberOfChecks); ++checkIndex)
    {
        if ((checks[checkIndex].isPassed))
        {
            bufferSize += 1u;   // strlen("\t")
            bufferSize += getSizeOfCheck(checkIndex, fileNameLength, "passed");
        }
    }
    return bufferSize;
}

static size_t getSizeOfFailures(uint16_t testcaseIndex, uint16_t firstTestcaseCheckIndex, size_t fileNameLength)
{
    size_t bufferSize = 0u;

    uint16_t checkIndex;
    for (checkIndex = firstTestcaseCheckIndex;
            checkIndex < (firstTestcaseCheckIndex + testcases[testcaseIndex].numberOfChecks); ++checkIndex)
    {
        if (!(checks[checkIndex].isPassed))
        {
            bufferSize += getSizeOfCheck(checkIndex, fileNameLength, "failed");
        }
    }
    return bufferSize;
}

static size_t getSizeOfTestcases()
{
    size_t bufferSize = 0u;
    size_t fileNameLength = strlen(staticTestSuite.filePath);

    uint16_t testcaseIndex;
    uint16_t firstTestcaseCheckIndex = 0u;
    for (testcaseIndex = 0u; testcaseIndex < staticTestSuite.numOfTestCases; ++testcaseIndex)
    {
        bufferSize += strlen(testcases[testcaseIndex].testCaseName);
        bufferSize += 21;        // strlen("\t\t<testcase name=\"%s\">\n") - 2
        bufferSize += 16;        // strlen("\t\t\t<system-out>\n")
        bufferSize += 14;        // strlen("\t\t\t\t<![CDATA[\n")
        bufferSize += getSizeOfSystemOut(testcaseIndex, firstTestcaseCheckIndex, fileNameLength);
        bufferSize += 25;       // strlen( "\t\t\t\t]]>\n\t\t\t</system-out>\n")
        if (!(testcases[testcaseIndex].isPassed))
        {
            bufferSize += 13;    // strlen("\t\t\t<failure>\n")
            bufferSize += getSizeOfFailures(testcaseIndex, firstTestcaseCheckIndex, fileNameLength);
            bufferSize += 14;    // strlen("\t\t\t</failure>\n")
        }
        bufferSize += 14;        // strlen("\t\t</testcase>\n")
        firstTestcaseCheckIndex = (uint16_t) (testcases[testcaseIndex].numberOfChecks + firstTestcaseCheckIndex);
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
        *bufferSize += 10u;    //strlen("\t\t<error>\n")
        *bufferSize += 150u; // errormessage
        *bufferSize += sizeof(MAX_NUM_OF_TEST_CASES);
        *bufferSize += 11u;    //strlen("\t\t</error>\n")
    }
    *bufferSize += 13; // strlen("</testsuite>\n")
}

/* ----- Setting up the test data structures -------------------------------------- */
static uint16_t getNumberOfErrors()
{
    uint16_t numberOfErrors = 0u;
    if (staticTestSuite.errorFlag)
    {
        numberOfErrors++;
    }
    return numberOfErrors;
}

static void getTestsuiteError(char *xmlString)
{
    char tempBuffer[150] = { 0 };

    strcat(xmlString, "\t<error>\n");

    switch (staticTestSuite.errorType)
    {
        case UCUNIT_ErrorTypeTooManyTestCases:
            sprintf(tempBuffer, "\t\tToo many testcases.The allowed amount of testcases is %d\n",
            MAX_NUM_OF_TEST_CASES);
            break;

        case UCUNIT_ErrorTypeTooManyChecks:
            sprintf(tempBuffer, "\t\tToo many checks.The allowed amount of checks is %d\n",
            MAX_NUM_OF_CHECKS);
    }
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    strcat(xmlString, "\t</error>\n");
}

void UCUNIT_XML_TestBegin(char *testSuiteName, char *file)
{
    staticTestSuite.testSuiteName = testSuiteName;
    staticTestSuite.filePath = file;
    time_t currentTime = time(NULL);
    staticTestSuite.time = *localtime(&currentTime);
    staticTestSuite.ucunitVersion = UCUNIT_VERSION;
    staticTestSuite.numOfTestCases = 0u;
}

void UCUNIT_XML_TestcaseBegin(char *testCaseName)
{
    if (staticTestSuite.errorFlag)
    {
        return;
    }
    if (staticTestSuite.numOfTestCases == MAX_NUM_OF_TEST_CASES)
    {
        staticTestSuite.errorFlag = true;
        staticTestSuite.errorType = UCUNIT_ErrorTypeTooManyTestCases;
        return;
    }
    UCUNIT_XmlTestCase testcase;
    testcase.testCaseName = testCaseName;
    testcase.numberOfChecks = 0u;
    testcases[staticTestSuite.numOfTestCases] = testcase;
    staticTestSuite.numOfTestCases++;
}

void UCUNIT_XML_TestcaseEnd(bool isPassed)
{
    if ((ucunit_testcases_failed + ucunit_testcases_passed) < MAX_NUM_OF_TEST_CASES)
    {
        testcases[staticTestSuite.numOfTestCases - 1].isPassed = isPassed;
        if ((isPassed))
        {
            ucunit_testcases_passed++;
        }
        else
        {
            ucunit_testcases_failed++;
        }
    }
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char *type, char *arguments, char *line)
{
    if (staticTestSuite.errorFlag)
    {
        return;
    }
    if (staticTestSuite.numOfChecks == MAX_NUM_OF_CHECKS)
    {
        staticTestSuite.errorFlag = true;
        staticTestSuite.errorType = UCUNIT_ErrorTypeTooManyChecks;
        return;
    }
    UCUNIT_XmlCheck check;
    check.isPassed = isPassed;
    check.type = type;
    check.arguments = arguments;
    check.lineNumber = line;

    checks[staticTestSuite.numOfChecks] = check;
    staticTestSuite.numOfChecks++;
    testcases[staticTestSuite.numOfTestCases - 1].numberOfChecks++;
}

/* ----- Assembling the xml object into a string array -------------------------------------- */

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
    UCUNIT_XML_GetXmlHeader(xmlString);
    UCUNIT_XML_GetTestsuiteBegin(xmlString);
    UCUNIT_XML_GetProperties(xmlString);
    UCUNIT_XML_GetTestcases(xmlString);
    if (staticTestSuite.errorFlag)
    {
        getTestsuiteError(xmlString);
    }
    strcat(xmlString, "</testsuite>\n");
}

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

    sprintf(tempBuffer, "<testsuite errors=\"%d\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n", getNumberOfErrors(),
            ucunit_testcases_failed, staticTestSuite.testSuiteName, staticTestSuite.numOfTestCases);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

void UCUNIT_XML_GetProperties(char *xmlString)
{
    strcat(xmlString, "\t<properties>\n");
    char tempBuffer[64] = { 0 };

    char formatted_date[20];
    strftime(formatted_date, 40, "%B %d %Y", &staticTestSuite.time);

    sprintf(tempBuffer, "\t\t<property name=\"compiled\" value=\"%s\"/>\n", formatted_date);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t<property name=\"time\" value=\"%02d:%02d:%02d\"/>\n", staticTestSuite.time.tm_hour,
            staticTestSuite.time.tm_min, staticTestSuite.time.tm_sec);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t<property name=\"ucunit-version\" value=\"%s\"/>\n", staticTestSuite.ucunitVersion);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    strcat(xmlString, "\t</properties>\n");
}

void UCUNIT_XML_GetTestcases(char *xmlString)
{
    uint16_t testcaseIndex;
    uint16_t firstTestcaseCheckIndex = 0u;
    for (testcaseIndex = 0u; testcaseIndex < staticTestSuite.numOfTestCases; ++testcaseIndex)
    {
        if (testcases[testcaseIndex].numberOfChecks != 0)
        {
            UCUNIT_XML_GetTestcase(xmlString, testcaseIndex, firstTestcaseCheckIndex);
            firstTestcaseCheckIndex = (uint16_t) (testcases[testcaseIndex].numberOfChecks + firstTestcaseCheckIndex);
        }
    }
}

void UCUNIT_XML_GetTestcase(char *xmlString, uint16_t testcaseIndex, uint16_t firstTestcaseCheckIndex)
{
    char tempBuffer[100] = { 0 };

    sprintf(tempBuffer, "\t<testcase name=\"%s\">\n", testcases[testcaseIndex].testCaseName);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));
    strcat(xmlString, "\t\t<system-out>\n");
    strcat(xmlString, "\t\t\t<![CDATA[\n");

    char systemOut[getSizeOfSystemOut(testcaseIndex, firstTestcaseCheckIndex, strlen(staticTestSuite.filePath))];
    char failures[getSizeOfFailures(testcaseIndex, firstTestcaseCheckIndex, strlen(staticTestSuite.filePath))];
    memset(systemOut, 0, sizeof(systemOut));
    memset(failures, 0, sizeof(failures));

    uint16_t checkIndex;
    for (checkIndex = firstTestcaseCheckIndex;
            checkIndex < firstTestcaseCheckIndex + testcases[testcaseIndex].numberOfChecks; ++checkIndex)
    {
        if ((checks[checkIndex].isPassed))
        {
            strcat(systemOut, "\t");
            UCUNIT_XML_GetCheck(systemOut, checkIndex, "passed");
        }
        else
        {
            UCUNIT_XML_GetCheck(failures, checkIndex, "failed");
        }
    }

    strncat(xmlString, systemOut, strlen(systemOut));
    strcat(xmlString, "\t\t\t]]>\n\t\t</system-out>\n");
    if (!(testcases[testcaseIndex].isPassed)
            && (getSizeOfFailures(testcaseIndex, firstTestcaseCheckIndex, strlen(staticTestSuite.filePath) != 0)))
    {
        strcat(xmlString, "\t\t<failure>\n");
        strncat(xmlString, failures, strlen(failures));
        strcat(xmlString, "\t\t</failure>\n");
    }
    strcat(xmlString, "\t</testcase>\n");
}

void UCUNIT_XML_GetCheck(char *xmlString, uint16_t checkIndex, const char *result)
{
    char tempBuffer[getSizeOfCheck(checkIndex, strlen(staticTestSuite.filePath), result)];
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer, "\t\t\t%s:%s %s(%s) %s\n", staticTestSuite.filePath, checks[checkIndex].lineNumber,
            checks[checkIndex].type, checks[checkIndex].arguments, result);

    strncat(xmlString, tempBuffer, strlen(tempBuffer));
}

bool UCUNIT_XML_IsTestSuiteResultPassed(void)
{
    if ((ucunit_testcases_failed != 0) || staticTestSuite.errorFlag)
    {
        return false;
    }
    return true;
}

#else

void UCUNIT_XML_GetSizeOfTestsuite(size_t *bufferSize)
{
    UNUSED(bufferSize);
}

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

void UCUNIT_XML_GetXmlObject(char *xmlString)
{
    UNUSED(xmlString);
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

void UCUNIT_XML_GetTestcase(char *xmlString, uint16_t testcaseIndex, uint16_t firstTestcaseCheckIndex)
{
    UNUSED(xmlString);
    UNUSED(testcaseIndex);
    UNUSED(firstTestcaseCheckIndex);
}

void UCUNIT_XML_GetCheck(char *xmlString, uint16_t checkIndex,
                          const char *result)
{
    UNUSED(xmlString);
    UNUSED(checkIndex);
    UNUSED(result);
}

bool UCUNIT_XML_IsTestSuiteResultPassed(void)
{
    return false;
}

#endif
