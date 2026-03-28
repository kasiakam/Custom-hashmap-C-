#include <iostream>
#include <string>
#include <utility>


template <typename Key, typename Value>
class HashMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;

        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    static const int DEFAULT_CAPACITY = 16;
    Node** table; // Tablica wskaŸników na wêz³y
    int capacity;
    int size;

  
    //funcja skrótu oparta na algorytmie DJBX33A
    int hash(const Key& key) const {
        int hashValue = 5381;
        for (char ch : key) {
            hashValue = ((hashValue << 5) + hashValue) + ch;
        }
        return hashValue % capacity; // modulo capacity!!! ¿eby by³ poprawny zakres indeksowania, który niewychodzi poza tablice
    }
    // zwolnienie dynamicznie alkowanej pamiêci 
    void freeTable() {
        for (int i = 0; i < capacity; ++i) {
            Node* current = table[i];
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

public:
    HashMap(int cap = DEFAULT_CAPACITY) : capacity(cap), size(0) {
        table = new Node * [capacity];
        for (int i = 0; i < capacity; ++i) {
            table[i] = nullptr;
        }
    }

    ~HashMap() {
        freeTable();
    }

    // dodawanie nowego klucza-wartoœci
    void insert(const Key& key, const Value& value) {
        int index = hash(key) % capacity;
        Node* current = table[index];

        //sprawdzanie czy podany klucz ju¿ istnieje
        while (current) {
            if (current->key == key) {
                current->value = value; // nadpisuje istniejaca wartosc
                return;
            }
            current = current->next;
        }

        // Wstaw nowy wêze³ na pocz¹tku listy
        Node* newNode = new Node(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        ++size;
    }

    //operator indeksowy []
    Value& operator[](const Key& key) {
        int index = hash(key);
        Node* current = table[index];

        //klucz istnieje, zwróæ wartoœæ
        while (current) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        // klucz nie istnieje, dodaj now¹ wartoœæ domyœln¹
        insert(key, Value());
        return table[index]->value; 
    }

    
    class Iterator {
    private:
        Node** table;
        int capacity;
        int currentBucket;
        Node* currentNode;

        void advance() {
            while (!currentNode && currentBucket < capacity) {
                ++currentBucket;
                if (currentBucket < capacity) {
                    currentNode = table[currentBucket];
                }
            }
        }
    public:
        Iterator(Node** t, int cap, int bucket, Node* node)
            : table(t), capacity(cap), currentBucket(bucket), currentNode(node) {
            advance();
        }

        std::pair<Key, Value> operator*() const {
            return { currentNode->key, currentNode->value };
        }

        Iterator& operator++() {
            if (currentNode) {
                currentNode = currentNode->next;
            }
            advance();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return currentNode != other.currentNode || currentBucket != other.currentBucket;
        }
    };

    Iterator begin() {
        return Iterator(table, capacity, 0, table[0]);
    }

    Iterator end() {
        return Iterator(table, capacity, capacity, nullptr);
    }

    int getSize() const {
        return size;
    }
};

int main() {
    HashMap<std::string, int> map;

    // Test 1: Dodawanie i nadpisywanie wartoœci
    std::cout << "Test 1: Dodawanie i nadpisywanie wartoœci" << std::endl;
    map.insert("one", 1);
    map.insert("two", 2);
    map.insert("three", 3);
    map.insert("duplicate", 10);
    map.insert("duplicate", 20); 
    std::cout << "Value for 'duplicate': " << map["duplicate"] << std::endl; // Oczekiwane: 20
    std::cout << std::endl;

    // Test 2: Dostêp do nieistniej¹cego klucza
    std::cout << "Test 2: Dostêp do nieistniej¹cego klucza" << std::endl;
    std::cout << "Value for non-existent key 'four': " << map["four"] << std::endl; // Oczekiwane: 0
    std::cout << std::endl;

    // Test 3: Kolizje
    std::cout << "Test 3: Kolizje" << std::endl;
    map.insert("abc", 100);
    map.insert("acb", 200); // mo¿liwa kolizja z "abc"
    std::cout << "Value for 'abc': " << map["abc"] << std::endl; // Oczekiwane: 100
    std::cout << "Value for 'acb': " << map["acb"] << std::endl; // Oczekiwane: 200
    std::cout << std::endl;

    // Test 4: Wielkoœæ mapy po wielu wstawieniach
    std::cout << "Test 4: Wielkoœæ mapy po wielu wstawieniach" << std::endl;
    std::cout << "Initial size of map: " << map.getSize() << std::endl;
    for (int i = 0; i < 1000; ++i) {
        map.insert("key" + std::to_string(i), i);
    }
    std::cout << "Size of map after 1000 insertions: " << map.getSize() << std::endl; // Oczekiwane: 1003
    std::cout << std::endl;

    // Test 5: Iteracja przez wszystkie elementy
    std::cout << "Test 5: Iteracja przez wszystkie elementy" << std::endl;
    for (auto it = map.begin(); it != map.end(); ++it) {
        auto [key, value] = *it;
        std::cout << key << ": " << value << std::endl;
    }
    std::cout << std::endl;


    // Test 6: Pusty HashMap
    std::cout << "Test 6: Pusty HashMap" << std::endl;
    HashMap<std::string, int> emptyMap;
    std::cout << "Size of empty map: " << emptyMap.getSize() << std::endl; // Oczekiwane: 0
    std::cout << "Value for non-existent key in empty map: " << emptyMap["nonexistent"] << std::endl; // Oczekiwane: 0
    std::cout << std::endl;
   

    return 0;
}