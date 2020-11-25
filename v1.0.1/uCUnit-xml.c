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
	staticTestSuite.testCases[staticTestSuite.numOfTestCases-1].isPassed = isPassed;
}

void UCUNIT_XML_CheckExecuted(bool isPassed, char* type, char* arguments)
{
	UCUNIT_XmlCheck check;
	check.isPassed = isPassed;
	check.type = type;
	check.arguments = arguments;
	check.filePath =__FILE__;
	check.lineNumber = UCUNIT_DefineToString(__LINE__);
	staticTestSuite.testCases[staticTestSuite.numOfTestCases-1].
	checks[staticTestSuite.testCases[staticTestSuite.numOfTestCases-1].numOfChecks]=check;
	staticTestSuite.testCases[staticTestSuite.numOfTestCases-1].numOfChecks += 1;
}

void UCUNIT_XML_GetTestsuite(UCUNIT_XmlTestSuite *testSuite)
{

}

void UCUNIT_XML_GetXmlHeader(char* xmlString)
{
	char tempBuffer[64] = { 0 };
	sprintf(tempBuffer, "<?xml version=\"%s\" encoding=\"%s\"?>\n",
			XML_VERSION,
			XML_ENCODING);
	strncat(xmlString, tempBuffer, strlen(tempBuffer));
	memset(tempBuffer, 0, sizeof(tempBuffer));
}

void UCUNIT_XML_GetTestsuiteBegin(char* xmlString)
{
	char tempBuffer[64] = { 0 };
	sprintf(tempBuffer, "<testsuite errors=\"0\" failures=\"%d\" name=\"%s\" tests=\"%d\">\n",
			ucunit_checks_failed,
			staticTestSuite.testSuiteName,
			staticTestSuite.numOfTestCases);
	strncat(xmlString, tempBuffer, strlen(tempBuffer));
	memset(tempBuffer, 0, sizeof(tempBuffer));
}

void UCUNIT_XML_GetProperties(char* xmlString)
{
    strcat(xmlString, "\t<properties>\n");

    char tempBuffer[64] = { 0 };

    char formatted_date [40];
    strftime( formatted_date, 40, "%B %d %Y", &staticTestSuite.time );

    sprintf(tempBuffer,"\t\t<property name='compiled' value='%s'/>\n",formatted_date);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "\t\t<property name='time' value='%02d:%02d:%02d'/>\n",
			staticTestSuite.time.tm_hour,
			staticTestSuite.time.tm_min,
			staticTestSuite.time.tm_sec);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    memset(tempBuffer, 0, sizeof(tempBuffer));

    sprintf(tempBuffer,
            "\t\t<property name='ucunit-version' value='%s'/>\n",
			staticTestSuite.ucunitVersion);
    strncat(xmlString, tempBuffer, strlen(tempBuffer));
    strcat(xmlString, "\t</properties>\n");
}

void UCUNIT_XML_GetTestcases(char* xmlString)
{
	strcat(xmlString, "\t<testcases>\n");
	char tempBuffer[64] = { 0 };

	int i;
	int j;
	for (i = 0; i < staticTestSuite.numOfTestCases; ++i)
	{
		sprintf(tempBuffer, "\t\t<testcase name=\"%s\">\n",staticTestSuite.testCases[i].testCaseName );
		strncat(xmlString, tempBuffer, strlen(tempBuffer));
		memset(tempBuffer, 0, sizeof(tempBuffer));

		strcat(xmlString, "\t\t\t<system-out>\n");
		strcat(xmlString, "\t\t\t\t![CDATA[\n");

		for (j = 0; j <staticTestSuite.testCases[i].numOfChecks; ++j )
		{
			sprintf(tempBuffer, "\t\t\t\t\t%s:%d %s(%s)",
					__FILE__,
					__LINE__,
					staticTestSuite.testCases[i].checks[j].type,
					staticTestSuite.testCases[i].checks[j].arguments);
			/* TODO correct error */
			strncat(xmlString, tempBuffer, strlen(tempBuffer));
			memset(tempBuffer, 0, sizeof(tempBuffer));
			if ( (staticTestSuite.testCases[i].checks[j].isPassed) ) { strcat(xmlString, " passed\n"); } else { strcat(xmlString, " failed\n"); }
		}
		strcat(xmlString, "\t\t\t\t]]>\n\t\t\t</system-out>\n\t\t</testcase>\n");
	}
	strcat(xmlString, "\t</testcases>\n");
}

void UCUNIT_XML_GetTestsuiteClose(char *xmlString)
{
	strcat(xmlString, "</testsuite>");
}

void UCUNIT_XML_GetXmlObject(char* xmlString)
{
	UCUNIT_XML_GetXmlHeader(xmlString);
	UCUNIT_XML_GetTestsuiteBegin(xmlString);
	UCUNIT_XML_GetProperties(xmlString);
	//UCUNIT_XML_GetTestcases(xmlString);
	//UCUNIT_XML_GetTestsuiteClose(xmlString);
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
#endif
