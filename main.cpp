#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

// trie node, which children lead to non-NUll nodes shows which letters
// are valid continuations
typedef struct trienode{
bool isWord; //=can we stop now and have a word?
trienode** children;
} trienode;

// tree showing where possible spaces are
typedef struct treenode{
int pos; //location of the space
vector<treenode*> children; 
} treenode;

// put a given word into a trie
void loadWord(trienode* trie, const char* word, int wordLength){
	trienode* child;
	if(trie->children[word[0]]==NULL){
		trie->children[word[0]] = (trienode*)malloc(sizeof(trienode));
		trie->children[word[0]]->children = (trienode**)malloc(sizeof(trienode*)*26);
		for(int i = 0; i < 26; i++){
			trie->children[word[0]]->children[i] = NULL;
		}
		trie->children[word[0]]->isWord = false;
	}
	if(wordLength == 1){
		trie->children[word[0]]->isWord = true;
		return;
	}
	loadWord(trie->children[word[0]], word+1, wordLength-1);
	return;
}

// given a list of words, load them into a trie
// this was written before I decided to use vector<T>
trienode* loadTrie(string words[], int numWords){
	trienode* trie = (trienode*)malloc(sizeof(trienode));
	trie->children = (trienode**)malloc(sizeof(trienode*)*26);
	trie->isWord = false;
	for(int i = 0; i < 26; i++){
		trie->children[i] = NULL;
	}
	printf("here\n");
	for(int i = 0; i < numWords; i++){
		loadWord(trie, words[i].data(), words[i].length());
	}
	return trie;
}

// read in a dictionary of newline separated words
vector<string> readDict(FILE* dict){
	vector<string> words;
	vector<char> word;
	char c;
	while((c = fgetc(dict)) != EOF){
		if((int)c == 13 || (int)c == 39){ //skip apostrophes and carriage returns
			continue;
		}
		if(c == '\n'){
			string s(word.begin(), word.end());
			for(int i = 0; i < s.length(); i++){
				s[i] = tolower(s[i]) - 97;
			}
			words.push_back(s);
			word.clear();
			continue;
		}
		word.push_back(c);
		if((c < 'a' || c > 'z') && c != '\n'){
			printf("bad character %i\n", (int)c);
		}
	}
	return words; // C++ guarantees contiguous storage, so this works
}

// check if something is a word in the dictionary (not used)
bool isWord(string word, trienode* trie){
	for(int i = 0; i < word.length(); i++){
		if(trie->children[word[i]] == NULL){
			return false;
		}
		trie = trie->children[word[i]];
	}
	return trie->isWord;
}

//find all possible words starting from a given letter in the text
vector<int> findNextWords(string text, int pos, trienode* trie){
	vector<int> nextWords;
	int i = pos;
	while(i < text.length() && trie->children[text[i]] != NULL){
		trie = trie->children[text[i]];
		i++;
		if(trie->isWord){
			nextWords.push_back(i);
		}
	}
	return nextWords;
}

//recursive function to find all interpretations of a text
//starting from pos given dictionary trie
treenode* parseText(string text, int pos, trienode* trie){
	treenode* tree = (treenode*)malloc(sizeof(treenode));
	tree->pos = pos;
	tree->children = vector<treenode*>();
	vector<int> nextWords = findNextWords(text, pos, trie);
	for(int i = 0; i < nextWords.size(); i++){
		//spawn a child
		treenode* child = parseText(text, nextWords[i], trie);
		if(child != NULL){
			tree->children.push_back(child);
		}
	}
	if(tree->children.size() == 0 && pos < text.length()){
		free(tree);
		return NULL;
	}
	return tree;
}

// read in the file while eliminating everything but a-z (including spaces)
// split by periods since the inerpretations must agree there, no need to branch more
vector<string> getSentencesNoSpaces(FILE* textfile){
	vector<string> sentences;
	vector<char> sentence;
	char c;
	while((c = fgetc(textfile)) != EOF){
		char tlc = tolower(c);
		if(c == '.'){
			string s(sentence.begin(), sentence.end());
			sentences.push_back(s);
			sentence.clear();
		}
		else if(tlc >= 'a' && tlc <= 'z'){
			sentence.push_back(tlc);
		}
	}
	return sentences;
}

//print a path with the spaces properly inserted
void printPath(string text, vector<int> path){
	int pathcount = 0;
	for(int i = 0; i < text.length(); i++){
		if(pathcount < path.size() && i == path[pathcount]){
			printf(" ");
			pathcount++;
		}
		printf("%c", text[i]);
	}
}

//print all root to leaf paths in the tree
void printTree(string text, treenode* tree, vector<int> path){
	if(tree->children.size() == 0){
		printPath(text, path);
		printf("\n");
		return;
	}
	for(int i = 0; i < tree->children.size(); i++){
		vector<int> newPath = path;
		newPath.push_back(tree->children[i]->pos);
		printTree(text, tree->children[i], newPath);
	}
	return;
}


// read in the dictionary and text
// print the text by sentences without spaces
// list all space-interpretations of the text
int main () {
  FILE* dict = fopen("./wordsEn.txt", "r");
  vector<string> words = readDict(dict);
  trienode* trie = loadTrie(&words[0], words.size());
  vector<string> sentences = getSentencesNoSpaces(fopen("./lincoln-gettysburg-122.txt", "r"));

  printf("text is:\n");
  for(int i = 0; i < sentences.size(); i++){
  	printf("sentence %i\n",i);
  	printf("%s\n",sentences[i].c_str());
  }
  for(int i = 0; i < sentences.size(); i++){
  	for(int j = 0; j < sentences[i].length(); j++){ //convert to 0='a'
  		sentences[i][j] -= 97;
  	}
  }
  for(int i = 0; i < sentences.size(); i++){
  	treenode* tree = parseText(sentences[i], 0, trie);
  	printf("parsed sentence %i:\n", i);
  	for(int j = 0; j < sentences[i].length(); j++){ //convert back to ascii
  		sentences[i][j] += 97;
  	}
  	printf("%s\n",sentences[i].c_str()); //original sentence

  	if(tree == NULL){
  		printf("no interpretation found\n");
  	}
  	else{
  		vector<int> path;
  		printTree(sentences[i], tree, path);
  	}
  }
  return 0;
}




