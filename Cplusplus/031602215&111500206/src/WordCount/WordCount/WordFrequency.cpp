#include "WordFrequency.h"

static unordered_map<string, int> hash_table;
static unordered_map<string, int>::iterator hash_iter;

bool MySort(const pair<int, string>& word1, const pair<int, string>& word2)
{
	if (word1.first == word2.first) { // if words have the same frequency, output in dictionary order
		return word1.second < word2.second;
	}
	else  {  // Sort word with frequency, high to low
		return word1.first > word2.first;
	}
}

int TransitionStoreWord(int state, char input, string & word, int TitleorAbstract)
{
	switch (state)
	{
	case OUTWORD:
		if (Separator(input)) return OUTWORD;
		if (isalpha(input)) { word += input; return P1; }
		if (IsNum(input)) return NotAWord;

	case NotAWord:
		if (Separator(input)) return OUTWORD;
		else return NotAWord;

	case P1:
		if (IsNum(input)) { word.clear(); return NotAWord; }
		if (isalpha(input)) { word += input; return  P2; }
		else { word.clear(); return OUTWORD; }

	case P2:
		if (IsNum(input)) { word.clear(); return NotAWord; }
		if (isalpha(input)) { word += input; return  P3; }
		else { word.clear(); return OUTWORD; }

	case P3:
		if (IsNum(input)) { word.clear(); return NotAWord; }
		if (isalpha(input)) { word += input; return VALIDWORD; }
		else { word.clear(); return OUTWORD; }

	case VALIDWORD:
		if (isalnum(input)) { word += input; return VALIDWORD; }
		else {
			InsertToHashTable(word, TitleorAbstract);
			//cout << word << endl;
			word.clear();
			return OUTWORD;
		}
	}
	return ERRORSTATE;
}

void InsertToHashTable(string & word, int TitleorAbstract)
{
	if ((hash_iter = hash_table.find(word)) == hash_table.end()) { // a new word
		if (TitleorAbstract == TITLE) // 开启权重词频功能后，title行的单词frequency权重算为10
		{
			pair<string, int> newWord = pair<string, int>(word, 10);
			hash_table.insert(newWord);
		}
		else if (TitleorAbstract == ABSTRACT)// abstract行的单词frequency权重算为1
		{
			pair<string, int> newWord = pair<string, int>(word, 1);
			hash_table.insert(newWord);
		}
		else if (TitleorAbstract == NONWEIGHT) // 没有开启权重词频功能，frequency统统算为1
		{
			pair<string, int> newWord = pair<string, int>(word, 1);
			hash_table.insert(newWord);
		}
	}
	else { // A word previously appeared
		if (TitleorAbstract == TITLE) // title行的单词frequency权重增加10
			hash_iter->second += 10;
		else if (TitleorAbstract == ABSTRACT)
			hash_iter->second++;
		else if (TitleorAbstract == NONWEIGHT)
			hash_iter->second++;
	}
}

void WordFrequency(string line, int TitleorAbstract)
{
	string word;
	char c;
	int state = OUTWORD;
	int i = 0;
	while ((c = line[i]) != '\0' && state != ERRORSTATE) {
		if (!(c >= 1 && c <= 255)) //non - ascii character
		{
			state = OUTWORD;
			i++;
			continue;
		}
		c = tolower(c);
		state = TransitionStoreWord(state, c, word, TitleorAbstract);
		i++;
	}
	if (state == VALIDWORD) {
		InsertToHashTable(word, TitleorAbstract);
	}
}

vector<pair<int, string>> TopWords()
{
	vector<pair<int, string>> TopWords;

	for (hash_iter = hash_table.begin(); hash_iter != hash_table.end(); hash_iter++) {
		pair<int, string> currentWord = make_pair(hash_iter->second, hash_iter->first);
		TopWords.push_back(currentWord);
	}

	sort(TopWords.begin(), TopWords.end(), MySort);
	hash_table.clear();
	return TopWords;
}

extern std::string outputFileName;
extern int topNWords;

int OutputToFile(vector<pair<int, string>>& TopWords)
{
	if (TopWords.size() == 0) {
		return -1;
	}

	fstream file;
	file.open(outputFileName.c_str(), ios::app);
	if (!file) {
		printf("Failed to create output file.\n");
		return -1;
	}
	int size = TopWords.size();
	for (int i = 0;i < size && i < topNWords; i++) {
		const char *word = (TopWords[i]).second.c_str();
		file << "<";
		file << word;
		file << ">: ";
		file << (TopWords[i]).first;
		file << endl;
	}
	printf("Top 10 words have been stored in output file\n");
	return 0;
}

int OutputToFile(int numChar, int numWords, int numLines)
{
	fstream file;
	file.open(outputFileName.c_str(), ios::out);
	if (!file) {
		printf("Failed to create output file.\n");
		return -1;
	}
	file << "characters: " << numChar << endl;
	file << "words: " << numWords << endl;
	file << "lines: " << numLines << endl;
	file.close();

	return 0;
}

int OutputToFile(int argc, char ** argv)
{
	fstream file;
	file.open(outputFileName.c_str(), ios::app);
	if (!file) {
		printf("Failed to create output file.\n");
		return -1;
	}
	for (int i = 0; i < argc; i++)
	{
		file << argv[i] << " ";
	}
	file << endl;
	return 0;
}
