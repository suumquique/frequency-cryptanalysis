// ��������� ����������� �� ������ ��� �������� �������� ��������, ������������� �������� scanf
#pragma warning( disable : 6031)

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>

/* ���������� ����� � ������� ��������� (������� ��������� ����������� ������ ��������).
* �������� - 35, ��� ��� �� ������ ���������� ����� �� ������ ��������� �������� �������� */
#define STEPS 35
// ������������ ����� ����� � ������
#define MAX_WORD_LENGTH 30
// ������������ ���������� ���� � ������
#define MAX_WORD_COUNT 200
// ������������ ����� ������ � ��������
#define MAX_FILE_LENGTH 10000
// �����������, �� ������� ����������� ����������� �� ��������� �����
#define delimeters " ,-.:;\n"
// "��������" ��������, �� ���� ���������� ��������� ���� � ���
#define alphabetCardinality 33
// ��������� ������� ��� ����������� ����������� ���������
#define RUS_ENCODING 1251

// ��������� ��� �������� ������� ������������ ����� � ������. letter - �����, count - ������� ��� ��� ����������� � ������
typedef struct {
	char letter;
	size_t count;
} Counter;
typedef Counter* CounterPtr;

// ����� �������� ��������, ������������� � ��������� ������� (�����, ������������� � ������� ����� ����, ���� �������)
const char* frequency = "���������������������������������";
// ������� ��������, � ������ ���������� �������� ������� �������� ������ ����� �� ������� ������
char history[STEPS][MAX_FILE_LENGTH];
// ������� "���" �������, �� ���� ������ ����������� ������ � ������� history
size_t currentStep = 0;

int compareCounter(CounterPtr c1, CounterPtr c2);
void addOrUpdate(CounterPtr counter, char letter);
bool isLetter(char symbol);
void printHelpMessage();
void analyseText();
void groupByWordLength();
void groupByEncryptedLettersNumber();
void showCurrentCryptogramState();
void replaceLetters(char* replaceString);
void goBack(size_t steps);
void replaceByFrequencyAlgorithm();

void main(void) {
	size_t i = 0;
	SetConsoleCP(RUS_ENCODING);
	SetConsoleOutputCP(RUS_ENCODING);

	char inputFileName[100], outputFileName[100];
	puts("������� ���� � ����� � �������� �������:");
	gets(inputFileName);
	puts("������� ���� � �����, ���� ����� ������� ���������: ");
	gets(outputFileName);
	FILE* inputFile = fopen(inputFileName, "r"), * outputFile = fopen(outputFileName, "w+");
	for (i = 0; (history[currentStep][i] = fgetc(inputFile)) != EOF; i++);
	history[currentStep][i] = '\0';

	enum  {stop, frequencyAnalysis, maxLetters, maxEncrypted, showCurrent, replacing, changeHistory, autoreplacement, helpInfo} variant;
	printHelpMessage();

	scanf("%u", &variant);
	getchar();
	while (variant != stop) {
		switch (variant) {
			case helpInfo:
				printHelpMessage();
				break;
			case frequencyAnalysis:
				analyseText();
				break;
			case  maxLetters:
				groupByWordLength();
				break;
			case maxEncrypted:
				groupByEncryptedLettersNumber();
				break;
			case showCurrent:
				showCurrentCryptogramState();
				break;
			case replacing:
				replaceLetters(NULL);
				break;
			case changeHistory:
				goBack(0);
				break;
			case autoreplacement:
				replaceByFrequencyAlgorithm();
				break;
			default:
				printf("������� �������������� ��������, ����� �������� �������, ������� 8");
				break;
		}

		printf("\n\n������� ��������� ��������:\n");
		scanf("%u", &variant);
		getchar();
	}

	for (i = 0; history[currentStep][i]; i++) fputc(history[currentStep][i], outputFile);
	puts("������ ���������. ������� ��������� ������� � �������� ����.");
}

void printHelpMessage(void) {
	printf("��������� ��������: \n0 - ��������� ������\
		\n1 - ��������� ������ � ����� ��������� �����\
		\n2 - ����� ����, ��������������� �� ���������� ����\
		\n3 - ����� ����, ��������������� �� ���������� ��������������� ����\
		\n4 - ����� �������� ��������� ������������\
		\n5 - �������� ����� � ������� ����� ��������� ������������. ������ ����������� � ������� \"���������� �����=���������� �����\", ���� ��������� �������� ����� ��������� - ����� ������\
		\n6 - ����� ������� ����������� � ������������� ��������� (�� ������������ ���������� �����)\
		\n7 - ���������� ���� � ������� ���������� �������\
		\n8 -  ������� ��� ��������� �����\n\n");
}

bool isLetter(char symbol) {
	if ((symbol >= '�' && symbol <= '�') || (symbol >= '�' && symbol <= '�') || symbol == '�' || symbol == '�') return true;
	else return false;
}
void showCurrentCryptogramState() {
	printf("������� ��������� �������� �������������� ������������ (�������� ����� - �������������, ��������� - ��� �������������):\n");
	printf(history[currentStep]);
}

int compareByLength(const char* s1, const char* s2) {
	return strlen(s2) - strlen(s1);
}

void groupByWordLength() {
	size_t wordCount, i;
	char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];
	char currentString[MAX_FILE_LENGTH];
	strcpy(currentString, history[currentStep]);

	char* word = strtok(currentString, delimeters);
	for (wordCount = 0; word != NULL; word = strtok(NULL, delimeters), wordCount++) {
		strcpy(words[wordCount], word);
	}

	qsort(words, wordCount, sizeof(char) * MAX_WORD_LENGTH, compareByLength);
	
	size_t maxLen = strlen(words[0]), currentLen;
	for (size_t i = 0; i < wordCount; i++) {
		currentLen = strlen(words[i]);
		printf(words[i]);
		if (currentLen < maxLen) {
			maxLen = currentLen;
			printf("\n\n");
		}
		else printf(" ");
	}

	printf("\n");
}
size_t getEncryptedLettersCount(char* s) {
	for (size_t i = 0, count = 0; ; i++) {
		if (!s[i]) return count;
		if (s[i] > '�' && s[i] < '�' || s[i] == '�') count++;
	}
}
int compareByEncryptedLettersCount(char* s1, char* s2) {
	return getEncryptedLettersCount(s2) - getEncryptedLettersCount(s1);
}

void groupByEncryptedLettersNumber() {
	size_t wordCount, i;
	char words[MAX_WORD_COUNT][MAX_WORD_LENGTH];
	char currentString[MAX_FILE_LENGTH];
	strcpy(currentString, history[currentStep]);

	char* word = strtok(currentString, delimeters);
	for (wordCount = 0; word != NULL; word = strtok(NULL, delimeters), wordCount++) {
		strcpy(words[wordCount], word);
	}

	qsort(words, wordCount, sizeof(char) * MAX_WORD_LENGTH, compareByEncryptedLettersCount);

	size_t maxLen = getEncryptedLettersCount(words[0]), currentLen;
	for (size_t i = 0; i < wordCount; i++) {
		currentLen = getEncryptedLettersCount(words[i]);
		printf(words[i]);
		if (currentLen < maxLen) {
			maxLen = currentLen;
			printf("\n\n");
		}
		else printf(" ");
	}

	printf("\n");
}

void goBack(size_t steps) {
	if (!steps) {
		printf("������� ��������� ��������� �������?\n");
		scanf("%u", &steps);
		getchar();
	}
	if (steps >= currentStep) currentStep = 0;
	else currentStep -= steps;

	printf("������� ���: %u\n", currentStep);
}

void replaceLetters(char* replaceString) {

	if (replaceString == NULL) {
		char* s[alphabetCardinality * 5]; replaceString = s;
		printf("������� ������ � ������� \"���������� �����=�����, �� ������� ��������\", ���� ���� �������� ��������� ����, ���� ������� ����� ������� ��� �������:\n");
		gets(replaceString);
	}

	strcpy(history[currentStep + 1], history[currentStep]);
	char* newCryptogram = history[++currentStep];
	char* replacement = strtok(replaceString, delimeters);
	
	while (replacement != NULL) {
		char replaceable = replacement[0], replacing = replacement[2];
		if (strlen(replacement) != 3 || replacement[1] != '=' || !isLetter(replaceable) || !isLetter(replacing)) {
			printf("������������ ������ ����� ��� ������ �����, ���������� ��� ���");
			break;
		}

		for (size_t i = 0; newCryptogram[i]; i++) {
			if (newCryptogram[i] == replaceable) newCryptogram[i] = replacing;
		}

		replacement = strtok(NULL, delimeters);
	}

	showCurrentCryptogramState();
}

void addOrUpdate(CounterPtr counter, char letter) {
	while (counter->count) {
		if (counter->letter == letter) {
			counter->count++;
			return;
		}
		counter++;
	}
	counter->letter = letter;
	counter->count = 1;
}

int compareCounter(CounterPtr c1, CounterPtr c2) {
	return c2->count - c1->count;
}

void analyseText() {
	Counter counter[alphabetCardinality * 2];
	for (size_t i = 0; i < alphabetCardinality * 2; i++) counter[i].count = 0;
	for (size_t i = 0; history[currentStep][i]; i++) if(isLetter(history[currentStep][i])) addOrUpdate(&counter, history[currentStep][i]);

	qsort(counter, alphabetCardinality * 2, sizeof(Counter), compareCounter);

	printf("\n��������� ������������� � ������������      ��������� ������������� � ������� �����\n\n");
	for (size_t i = 0; counter[i].count; i++) {
		printf("%20c%40c\n", counter[i].letter, frequency[i]);
		for (size_t i = 0; i < 80; i++) printf("-");
		printf("\n");
	}
}

void replaceByFrequencyAlgorithm() {
	Counter counter[alphabetCardinality * 2];
	for (size_t i = 0; i < alphabetCardinality * 2; i++) counter[i].count = 0;
	for (size_t i = 0; history[currentStep][i]; i++) if (isLetter(history[currentStep][i])) addOrUpdate(&counter, history[currentStep][i]);

	qsort(counter, alphabetCardinality * 2, sizeof(Counter), compareCounter);

	char replaceString[alphabetCardinality * 5], *replaceStringPtr = replaceString;

	for (size_t i = 0; counter[i].count; i++) {
		*replaceStringPtr++ = counter[i].letter;
		*replaceStringPtr++ = '=';
		*replaceStringPtr++ = frequency[i];
		if (counter[i + 1].count) *replaceStringPtr++ = ' ';
	}
	*replaceStringPtr = '\0';

	replaceLetters(replaceString);
}