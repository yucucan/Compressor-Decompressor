#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// Dictionary item (string and code pair)
class DictItem {
private:
    string key;
    int code;

public:
    DictItem(const string& k = "", int c = -1) : key(k), code(c) {}

    bool operator==(const DictItem& other) const {
        return key == other.key;
    }

    bool operator!=(const DictItem& other) const {
        return key != other.key;
    }

    const string& getKey() const {
        return key;
    }

    int getCode() const {
        return code;
    }

    void setKey(const string& k) {
        key = k;
    }

    void setCode(int c) {
        code = c;
    }
};

// Hash function for strings (using a simple polynomial rolling hash)
// Found from google
unsigned int hashString(const string& str, int tableSize) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = (hash * 31 + c) % tableSize;
    }
    return hash;
}

// Hash table implementation with linear probing
// Very similar to the one in the lecture slides
template <class HashedObj>
class HashTable {
    public:
        enum EntryType { ACTIVE, EMPTY, DELETED };
    private:
        struct HashEntry {
            HashedObj element;
            EntryType info;

            HashEntry(const HashedObj& e = HashedObj(), EntryType i = EMPTY)
                : element(e), info(i) {}
        };

        vector<HashEntry> array;
        int currentSize;
        const HashedObj ITEM_NOT_FOUND;

        // From lecture slides
        bool isActive( int currentPos ) const {
            return array[currentPos].info == ACTIVE;
        }

        // Find the position where item should be located
        int findPos(const HashedObj& item) const {
            int hash = hashString(dynamic_cast<const DictItem&>(item).getKey(), array.size());
            int pos = hash % array.size();

            while (array[pos].info != EMPTY && array[pos].element != item) {
                pos = (pos + 1) % array.size();  // Linear probing
            }

            return pos;
        }

        // Rehash the table when it gets too full. 
        // Very similar to the one in the lecture slides
        void rehash() {
            vector<HashEntry> oldArray = array;

            // Create new double-sized, empty table
            array.resize(nextPrime(2 * oldArray.size()));
            for (auto& entry : array)
                entry.info = EMPTY;

            // Copy table over
            currentSize = 0;
            for (const auto& entry : oldArray) {
                if (entry.info == ACTIVE) {
                    insert(entry.element);
                }
            }
        }

        // From lecture slides
        bool isPrime(int n)
        {
            if (n == 2 || n == 3)
                return true;

            if (n == 1 || n % 2 == 0)
                return false;

            for (int i = 3; i * i <= n; i += 2)
                if (n % i == 0)
                    return false;

            return true;
        }

        // From lecture slides
        int nextPrime(int n)
        {
            if (n % 2 == 0)
                n++;

            for (; !isPrime(n); n += 2)
                ;

            return n;
        }
    public:
        // Constructor. 
        // Very similar to the one in the lecture slides
        explicit HashTable(const HashedObj& notFound, int size = 101) :
            ITEM_NOT_FOUND(notFound), currentSize(0) {
            array.resize(nextPrime(size));
            for (auto& entry : array)
                entry.info = EMPTY;
        }

        // Insert an item into the hash table. 
        // Very similar to the one in the lecture slides
        void insert(const HashedObj& item) {
            int pos = findPos(item);
            if (isActive(pos)) {
                return;
            }

            array[pos].element = item;
            array[pos].info = ACTIVE;

            // Rehash if table is at least half full
            if (++currentSize > array.size() / 2) {
                rehash();
            }
        }

        // Find an item in the hash table. From lecture slides
        const HashedObj& find(const HashedObj& item) const {
            int pos = findPos(item);
            if (array[pos].info == ACTIVE) {
                return array[pos].element;
            }
            return ITEM_NOT_FOUND;
        }

        // Remove an item from the hash table. 
        // Very similar to the one in the lecture slides
        void remove(const HashedObj& item) {
            int pos = findPos(item);
            if (array[pos].info == ACTIVE) {
                array[pos].info = DELETED;
                currentSize--;
            }
        }
        
        // For deep copying
        const HashTable & operator=( const HashTable & rhs ) {
            if (this != &rhs)
            {
                array = rhs.array;
                currentSize = rhs.currentSize;
            }
            return *this;
        }
};

// Compress a string using the Ziv-Lempel algorithm
string compress(const string& input) {
    // Initialize hash table with ASCII characters
    HashTable<DictItem> dictionary(DictItem("", -1), 4096);

    for (int i = 0; i < 256; i++) {
        string s(1, char(i));
        dictionary.insert(DictItem(s, i));
    }

    string current = "";
    string compressed = "";
    int nextCode = 256;

    for (char ch : input) {
        string next = current + ch;
        DictItem found = dictionary.find(DictItem(next, -1));

        if (found.getCode() != -1) {
            current = next; 
        } else {
            if (compressed.length() > 0) {
                compressed += " ";
            }
            compressed += to_string(dictionary.find(DictItem(current, -1)).getCode());
            if (nextCode < 4096) {
                dictionary.insert(DictItem(next, nextCode++));
            }
            current = string(1, ch);
        }
    }

    if (!current.empty()) {
        if (compressed.length() > 0) {
            compressed += " ";
        }
        compressed += to_string(dictionary.find(DictItem(current, -1)).getCode());
    }

    // One space at the end of the file after the last integer code written
    compressed += " ";
    return compressed;
}

// Decompress a string using the Ziv-Lempel algorithm
string decompress(const string& input) {
    string decompressed = "";
    istringstream iss(input);
    vector<int> codes;
    int code;

    while (iss >> code) {
        codes.push_back(code);
    }
    
    if (codes.empty()) {
        return decompressed; 
    }

    // string is better than DictItem for this part
    vector<string> stringTable(4096);
    for (int i = 0; i < 256; i++) {
        stringTable[i] = string(1, char(i));
    }

    int nextCode = 256;
    decompressed = stringTable[codes[0]];

    string previous = decompressed;

    for (size_t i = 1; i < codes.size(); i++) {
        int currentCode = codes[i];
        string current;

        if (currentCode < nextCode && !stringTable[currentCode].empty()) {
            // Case 1: The code is already in the table
            current = stringTable[currentCode];
        }
        else if (currentCode == nextCode) {
            // Case 2: The code is not in the table
            current = previous + previous[0];
        }
        else {
            // Case 3: Invalid code encountered
            cerr << "Error: Invalid code encountered during decompression." << endl;
            return "";
        }

        decompressed += current;
        if (nextCode < 4096) {
            stringTable[nextCode++] = previous + current[0];
        }

        previous = current;
    }

    return decompressed;
}

int main() {
    int choice;
    string input;

    cout << "To compress a file, press 1. To decompress a file, press 2: ";
    cin >> choice;
    cin.ignore(); // Ignore the newline character

    if (choice == 1) {
        cout << "Enter the input string: ";
        getline(cin, input);
        string compressed = compress(input);
        cout << "Compressed output: " << compressed << endl;
    }
    else if (choice == 2) {
        cout << "Enter the compressed string: ";
        getline(cin, input);
        string decompressed = decompress(input);
        cout << "Decompressed string: " << decompressed << endl;
    }
    else {
        cout << "Invalid choice." << endl;
    }

    return 0;
}