# scriptio-continua
Orthographic word finder for texts without spaces

This program (written in C++ solely for convenience of vector\<T>) will take in an English text that is written without spaces and provide all the different ways to place the spaces given its provided list of words.

It splits the text by periods, since all interpretations must agree on them, and eliminates all non alphabetic characters for simplicity.

I wrote this quickly, so it currently is entirely contained in main.cpp. My apologies. The code is manageable though since it is small. Tested with g++ -std=c++11 -O2.

**How it works:**

The dictionary is loaded into a trie for quick as-we-read indexing. The different ways to add spaces are represented in a tree, in which the position of the space is stored. The program will find all possible next spaces from a given point, and for each point it will repeat this process recursively. If it finds leftover text before the period that it cannot make into a word, it will declare the last space invalid and delete it. That is, if ever it is found there is no valid next space, the current space is deleted (and this deletion will be back propagated naturally by the recursion). The resulting tree encodes all possible spacings for the sentence.

**What is wrong with it:**

I am looking for find a more sparse dictionary that still includes word inflections. At the moment, too many interpretations are considered valid. For example, "th" is considered a valid orthographic word, probably since it is the chemical symbol for thorium, but I doubt Lincoln mentioned it in his Gettysburg address. If one finds/produces a better dictionary, this program could be a nice way to find ambiguities that arise when spaces are omitted. Also, the tree-like nature of the problem means that it might "hang" on larger inputs.

The current dictionary (included) is from sil.org, and the Gettysburg Adress (included) is from textfiles.com.

Anyone is free to modify/distribute/do whatever with this. No guarantees of anything.
