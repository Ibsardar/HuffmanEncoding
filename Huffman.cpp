/*	Huffman.cpp
-------------------------------
Ibrahim Sardar
CSCI 484
Question #6 Greedy Algorithms
*/

// acquire all necessary resources
#include <string>
using       std::string;
#include <iostream>
using       std::getline;
using       std::cout;
using       std::cin;
using       std::endl;
#include <fstream>
using       std::ifstream;
using       std::ofstream;
#include <sstream>
using       std::stringstream;
#include <list>
using       std::list;
#include <algorithm>
using       std::find;
#include <queue>
using       std::queue;
#include <utility>
using       std::pair;

// Node Structure
// every non-leaf node will have 2 children nodes and an empty character
// every leaf node will have no children and a non-empty character
// code will be 1 or 0 depending on if it is a left/right node of its parent
// root node will have an invalid code (-1)
struct Node {
    Node* left  = NULL;
    Node* right = NULL;
    string code = "";
    char character = ' ';
    int frequency = 0;

};

/// Prototypes of Functions:

/// --- HELPERS ---------------------------------------------------------------------------
int                         toInt               (string);
string                      toStr               (int);
int                         makeError           (string, string);
bool                        inList              (list<Node*>&, Node*);
void                        deleteList          (list<Node*>&);
void                        deleteTree          (Node*);

/// --- FILE I/O --------------------------------------------------------------------------
ifstream*                   readFile            (string);
list<Node*>                 parseFile           (ifstream&);
void                        writeFile           (list< pair<char,string> >&, string);

/// --- DEBUGGING -------------------------------------------------------------------------
void                        printList           (list<Node*>&, string);
void                        printList2          (list<char>&, string);
void                        printTree           (Node*, int);
void                        printTable          (list< pair<char,string> >&, string);
list<char>                  getChars            (list<Node*>&);

/// --- SORTING ---------------------------------------------------------------------------
bool                        compareNodes        (Node*, Node*);
bool                        comparePairs        (pair<char,string>&, pair<char,string>&);
void                        sortedInsert        (list<Node*>&, Node*);

/// --- HUFFMAN'S -------------------------------------------------------------------------
Node*                       encode              (list<Node*>&);
void                        dfsForCodes         (list< pair<char,string> >&, Node*);
list< pair<char,string> >   generateTable       (Node*);

/// --- PROCESSING ------------------------------------------------------------------------
list<Node*>                 getListFromFile     (string);
Node*                       getTreeFromList     (list<Node*>&);
list< pair<char,string> >   getTableFromTree    (Node*);
void                        writeTableToFile    (list< pair<char,string> >, string);

// converts string to int
int toInt(string str) {
	stringstream ss(str);
	int i;

	ss >> i;

	return i;
}

// converts int to string
string toStr(int i) {
	stringstream ss;
	string str;

	ss << i;
	ss >> str;

	return str;
}

// throws an exception with a message
// type = "warning" ---> print msg
// type = "fatal"   ---> print msg, stop program
// returns error code:
// 0 : ignorable
// 1 : fatal
int makeError(string type, string message) {
	if (type == "fatal") {
        try {
            throw -1;

        }
        catch(int e) {
            cout << endl << message << endl;
            return e;

        }
	}
	else if (type == "warning") {
		cout << endl << message << endl;
		return 0;

	}

	return 0;
}

// opens and reads a file
// returns an newly made input-file-stream
ifstream* readFile(string filename) {
	ifstream* file = new ifstream();

	file->open(filename);
	if (!file->is_open())
		makeError("fatal", "Error opening file.");

	return file;
}

// puts content from file into the list
list<Node*> parseFile(ifstream& file) {
	list<Node*> nodes;
	string line = "";
	string value = "";
	int freq = 0;
	int i = 0;
	stringstream ss;

    while (getline(file, line)) {
        if (i == 0) {
            if (line.empty() || line == " ") {
                makeError("warning", "Input file or first line is empty.");
                break;

            }

        }

        // parse line
        freq  = -1;
        value = "";
        ss.str(string());
        ss.clear();
        ss << line;
        getline(ss, value, ' ');
        ss >> freq;
        if (freq < 0) {
            makeError("fatal", "One or more lines have invalid input.");
            break;

        }
        if (value == "LF") {
            value = "\n";

        }
        if (value == "") {
            value = " ";

        }

        // create a Node on the heap
        // default constructor will set bits to 0 (no garbage refs)
        Node* n = new Node();
        n->character = value[0];
        n->frequency = freq;
        nodes.push_back(n);

        // increment counter
        i++;

    }

	cout << "Parsed " + toStr(i) + " lines from input file." << endl;

	return nodes;
}

void writeFile(list< pair<char,string> >& tbl, string filename) {
    ofstream f;
    f.open(filename);
	if (!f.is_open())
		makeError("fatal", "Error opening output file.");

    list< pair<char,string> >::iterator element;
    for (element=tbl.begin(); element!=tbl.end(); ++element) {
        switch ( (*element).first ) {

            case '\n':
                f << "\\n";
                break;

            case '\r':
                f << "\\r";
                break;

            case '\t':
                f << "\\t";
                break;

            default:
                f << (*element).first;

        }

        f << " " << (*element).second << endl;

    }

    f.close();
    return;
}

// prints some content of every Node in a list
void printList(list<Node*>& lst, string label="List content:") {
    cout << endl << label << endl;
    list<Node*>::iterator element;
    for (element=lst.begin(); element!=lst.end(); ++element) {
        switch ( (*element)->character ) {

            case '\n':
                cout << "Value: \\n, ";
                break;

            case '\r':
                cout << "Value: \\r, ";
                break;

            case '\t':
                cout << "Value: \\t, ";
                break;

            default:
                cout << "Value: " << (*element)->character << ", ";

        }

        cout << "Frequency: " << (*element)->frequency << endl;

    }

    cout << endl;

}

// prints every character from a list of characters
void printList2(list<char>& lst, string label="List content:") {
    cout << endl << label << endl;
    list<char>::iterator element;
    for (element=lst.begin(); element!=lst.end(); ++element) {
        switch ( (*element) ) {

            case '\n':
                cout << "\\n";
                break;

            case '\r':
                cout << "\\r";
                break;

            case '\t':
                cout << "\\t";
                break;

            default:
                cout << (*element);

        }

        cout << " ";

    }

    cout << endl << endl;

}

// prints some content of every Node in a tree
void printTree(Node* n, int lvl=0) {
    if (n==NULL) {

        return;
    }

    switch ( n->character ) {

        case '\n':
            cout << "(V: \\n, ";
            break;

        case '\r':
            cout << "(V: \\r, ";
            break;

        case '\t':
            cout << "(V: \\t, ";
            break;

        default:
            cout << "(V: " << n->character << ",\t";

    }

    cout << "F: " << n->frequency << ",\tLv: " << lvl << ",\tCode: " << n->code << ")";
    if (n->left == NULL && n->right == NULL) {
        cout << "\t~Leaf Node" << endl;
    } else {
        cout << "\t~Non-Leaf Node" << endl;
    }

    printTree(n->left, lvl+1);
    printTree(n->right, lvl+1);

}

// prints some content of every Node in a list
void printTable(list< pair<char,string> >& tbl, string label="Table content:") {
    cout << endl << label << endl;
    list< pair<char,string> >::iterator element;
    for (element=tbl.begin(); element!=tbl.end(); ++element) {
        switch ( std::get<0>(*element) ) {

            case '\n':
                cout << "Char: \\n, ";
                break;

            case '\r':
                cout << "Char: \\r, ";
                break;

            case '\t':
                cout << "Char: \\t, ";
                break;

            default:
                cout << "Char: " << std::get<0>(*element) << ", ";

        }

        cout << "Encoding: " << std::get<1>(*element) << endl;

    }

    cout << endl;

}

// frees all allocated Nodes from a Node-list
void deleteList(list<Node*>& lst) {
    while(!lst.empty()) {
        delete lst.front();
        lst.pop_front();

    }

}

// frees all allocated Nodes from a Node-tree
void deleteTree(Node* root) {
    if (root == NULL) {

        return;
    }

    if (root->left == NULL && root->right == NULL) {
        delete root;
        root = NULL;

        return;
    }

    deleteTree(root->left);
    deleteTree(root->right);

}

// Node comparing function for sorting
bool compareNodes(Node* a, Node* b) {
    if (a->frequency < b->frequency) {

        return true;
    }

    return false;
}

// Pair comparing function for sorting by ASCII
bool comparePairs(pair<char,string>& a, pair<char,string>& b) {
    if (a.first < b.first) {

        return true;
    }

    return false;
}

// insert a Node in a Node-list while keeping the Node-list sorted (by frequency)
void sortedInsert(list<Node*>& lst, Node* n) {
    list<Node*>::iterator element;
    for (element=lst.begin(); element!=lst.end(); ++element) {
        if ( compareNodes(n,(*element)) ) {

            lst.insert( element,n );
            return;
        }

    }

    lst.insert(element,n);

}

// returns a list of characters from a Node-list
list<char> getChars(list<Node*>& lst) {
    list<char> characters;
    list<Node*>::iterator element;
    for (element=lst.begin(); element!=lst.end(); ++element) {
        char curr = (*element)->character;
        characters.push_back(curr);

    }

    return characters;
}

// encodes a Node-list via Huffman's encoding
Node* encode(list<Node*>& lst) {
    // base case : empty
    if (lst.empty()) {
        makeError("fatal","Something went wrong during the Huffman encoding process.");

        return NULL;
    }

    // base case : 1 element
    if (lst.size() == 1) {
        Node* popped = lst.front();
        lst.pop_front();

        return popped;
    }

    // other cases : 2+ elements
    Node* a = lst.front();
    lst.pop_front();
    Node* b = lst.front();
    lst.pop_front();
    Node* c = new Node();
    c->frequency = a->frequency + b->frequency;
    c->left  = a;
    c->right = b;
    a->code  = "0";
    b->code  = "1";
    sortedInsert(lst, c);
    return encode(lst);

}

// checks if Node is in a list
bool inList(list<Node*>& lst, Node* n) {
    list<Node*>::iterator runner;
    runner = find(lst.begin(),lst.end(),n);
    if (runner != lst.end()) {
        return true;
    }

    return false;
}

// perform depth first search to add all character-code pairs to table
void dfsForCodes(list< pair<char,string> >& table, Node* curr) {
    // base case : empty
    if (curr == NULL) {

        return;
    }

    // base case : outer (leaf)
    if ( curr->right==NULL && curr->left==NULL ) {
        pair<char,string> match;
        string code = curr->code;
        char character = curr->character;

        match = make_pair(character,code);
        table.push_back(match);

        return;
    }

    // case : inner (non-leaf)
    if ( curr->right!=NULL && curr->left!=NULL ) {
        string thisCode;
        string nextCode;

        // right
        thisCode = curr->code;
        nextCode = curr->right->code;
        curr->right->code = thisCode + nextCode;
        dfsForCodes(table, curr->right);

        // left
        thisCode = curr->code;
        nextCode = curr->left->code;
        curr->left->code = thisCode + nextCode;
        dfsForCodes(table, curr->left);

        return;
    }

    // case : corrupt (exactly one child is NULL)
    makeError("fatal","A corrupt Node was found in the tree.");

    return;
}

// generates a list of pairs of the format: character,binary-code
list< pair<char,string> > generateTable(Node* tree) {
    list< pair<char,string> > table;
    dfsForCodes(table,tree);

    return table;
}

// reads file, parses file, creates list
list<Node*> getListFromFile(string filename) {
    cout << "File I/O in progress......." << endl;
	ifstream* f = readFile("freq.txt");
	list<Node*> l = parseFile(*(f));
    printList(l, "Parsed Input:");

    f->close();
    delete f;

    return l;
}

// sorts list, creates Huffman encoding tree
Node* getTreeFromList(list<Node*>& l) {
    cout << "Sorting.......";
    l.sort(compareNodes);
    cout << "done!" << endl;
    printList(l, "Sorted Input:");

    list<char> c = getChars(l);
    printList2(c, "Characters to encode:");

    Node* tree = encode(l);
    cout << "Huffman's Encoding Tree:" << endl;
    printTree(tree);

    return tree;
}

// creates Huffman encoding table
list< pair<char,string> > getTableFromTree(Node* tree) {
    cout << "Generating code table......." << endl;
    list< pair<char,string> > table;
    table = generateTable(tree);
    table.sort(comparePairs);
    printTable(table, "Huffman's Encoding Table:");

    return table;
}

// writes each character-code pair to text file
void writeTableToFile(list< pair<char,string> > tbl, string filename) {
    cout << "File I/O in progress......." << endl;
    writeFile(tbl, filename);
    cout << "Huffman Encodings Complete." << endl;
    cin.ignore();

}

// runs all necessary processing functions,
// cleans up any allocated data
int main() {
    list<Node*> l;
	Node* tree;
	list< pair<char,string> > tbl;

    l = getListFromFile("freq.txt");
    tree = getTreeFromList(l);
    tbl = getTableFromTree(tree);
    writeTableToFile(tbl,"codetable.txt");

	deleteTree(tree);
}
