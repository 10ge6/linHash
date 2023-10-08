#include <iostream>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <cmath>

using namespace std;

int accessCount = 0;

struct Block {
    vector<int> Keys;
    Block *next;
    int maxSize;

    // constructor
    Block() {
        next = NULL;
        Keys.clear();
    }

    void add(int key) {
        if(Keys.size() < maxSize) {
            Keys.push_back(key);
            return;
        } else if(next == NULL) {
            next = new Block();
        }
        next->add(key);
    }

    bool checkKey(int key) {
        for(int i = 0; i < Keys.size(); i++) {
            count++;
            if(Keys[i] == key) return true;
        };
        
        if(next = NULL) {
            return false;
        } else {
            return next->checkKey(key);
        }
    }

    void print() {

    }
}

struct Table {
    vector<Block *> Buckets;
    int l, N, m;
    int threshold;
    int keysTotal;

    // constructor
    Table() {
        l = 0;
        N = 0;
        keysTotal = 0;
        // Buckets.clear();
        Buckets.push_back(new Block());
        Buckets.push_back(new Block());
    }

    unsigned int hash(int k, int l/*, int m*/) {
        // int m = Buckets.size();
        int m = 2;
        return k % (pow(2,l) * m);
    }

    void insert(int k, int l) {
        unsigned int h = hash(k, l);
        if(h < N) h = hash(k, l+1);
        Buckets[h].add(k);
        keysTotal++;
        if(keysTotal / (Buckets.size() * Buckets[0].maxSize) > threshold) {
            Bucket = new Bucket();
            vector<int> tmpKeys; // recalc hash
            Block tmp = Buckets[n];
            
            do {
                for(int i = 0; i < tmp.Keys.size()) {
                    tmpKeys.push_back(Buckets[n].Keys[i]);
                }
                tmp = tmp.next;
            } while(tmp.next != NULL);
            
            Buckets[N].clear();

            for(int i = 0; i < tmpKeys.size(); i++) {
                h = hash(tmpKeys[i], l+1);
                Buckets[h].add(tmpKeys[i]);
            }

            N++;
        }
        if(N > pow(2,l)) {
            N = 0;
            L++;
        }
    }

    bool checkBlock() {

    }

    void print() {

    }
}

int main(int argc, char **argv) {
    cout << "Uso: qtd paginas, fator de carga, no. chaves e chaves" << endl;

    int keyQty, p, aMax; // m = 2
    cin >> p >> aMax >> keyQty;
    int inputKeys[keyQty];
    
    for(int i = 0; i< keyQty; i++) {
        cin >> inputKeys[i];
    }

    Table t = new Table();
    t.threshold = aMax;
    t.Buckets[0].maxSize = p;
    t.Buckets[1].maxSize = p;
}

// Table t;
// Block b = completarBlock(inputMax);
// t.Buckets.push_back(b);

// completarBlock(int maxSizeInput) {
//     new Block();
//     b.maxSize = maxSizeInput;
//     return b;
// } analogo pra table : m (loop para inicializar m Blocks em Buckets), threshold