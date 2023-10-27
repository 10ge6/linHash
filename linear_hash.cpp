#include <iostream>
#include <cstdlib>
#include <fstream>
#include <queue>
#include <cmath>

using namespace std;

int accessCount = 0;

struct Block
{
    vector<int> Keys;
    Block *next;
    int maxSize;

    // constructor
    Block(int maxSizeInicial)
    {
        next = NULL;
        Keys.clear();
        maxSize = maxSizeInicial;
    }

    void add(int key)
    {
        if (Keys.size() < maxSize)
        {
            Keys.push_back(key);
            return;
        }
        else if (next == NULL)
        {
            next = new Block(maxSize);
        }
        next->add(key);
    }

    bool checkKey(int key)
    {
        for (int i = 0; i < Keys.size(); i++)
        {
            accessCount++;
            if (Keys[i] == key)
                return true;
        };

        if (next = NULL)
        {
            return false;
        }
        else
        {
            return next->checkKey(key);
        }
    }

    void print()
    {
        for (int i = 0; i < Keys.size(); i++)
        {
            cout << ' ' << Keys[i] << endl;
            if (i == Keys.size() - 1)
                cout << "----" << endl;
        }
        if (next != NULL)
            next->print();
    }
};

struct Table
{
    vector<Block *> Buckets;
    int l, N, m, pStore;
    float threshold;
    int keysTotal;

    // constructor
    Table(int mInicial, int p, float aMaxInicial)
    {
        l = 0;
        N = 0;
        keysTotal = 0;
        threshold = aMaxInicial;
        Buckets.clear();
        for (int i = 0; i < mInicial; i++)
        {
            Buckets.push_back(new Block(p));
        }
        pStore = p;
        m = Buckets.size();
    }

    unsigned int hash(int k, int l)
    {
        int valorHash = k % (int)(pow(2, l) * pStore);
        return valorHash;
    }

    void insert(int k)
    {
        unsigned int h = hash(k, l);
        if (h < N)
            h = hash(k, l + 1);
        Buckets[h]->add(k);
        keysTotal++;
        if (keysTotal / (Buckets.size() * pStore) > threshold)
        {
            vector<int> tmpKeys; // recalc hash
            tmpKeys.clear();
            Block *tmpBlock = new Block(pStore);
            tmpBlock = Buckets[N];

            do
            {
                for (int i = 0; i < tmpBlock->Keys.size(); i++)
                {
                    tmpKeys.push_back(tmpBlock->Keys[i]);
                }
                tmpBlock = tmpBlock->next;
            } while (tmpBlock != NULL);

            tmpBlock = Buckets[N];

            do
            {
                tmpBlock->Keys.clear();
                tmpBlock = tmpBlock->next;

            } while (tmpBlock != NULL);

            Buckets[N]->next = NULL;
            Buckets.push_back(new Block(pStore));

            for (int i = 0; i < tmpKeys.size(); i++)
            {
                h = hash(tmpKeys[i], l + 1);
                Buckets[h]->add(tmpKeys[i]);
            }

            N++;
        }
        if (N >= (int)(pow(2, l) * pStore))
        {
            N = 0;
            l++;
        }
    }

    bool checkBlock(int key)
    {
        {
            int i = hash(key, l);
            if (i < N)
                i = hash(key, l + 1);
            return Buckets[i]->checkKey(key);
        }
    }

    void print()
    {
        for (int i = 0; i < Buckets.size(); i++)
        {
            cout << " L" << i << endl
                 << "----" << endl;
            Buckets[i]->print();
            if (i != Buckets.size() - 1)
                cout << endl;
        }
    }
};

int main(int argc, char **argv)
{
    cout << "Uso: qtd inicial de paginas, tamanho de pagina, fator de carga, no. chaves e chaves" << endl;

    int keyQty, m, p;
    float aMax;
    cin >> m >> p >> aMax >> keyQty;
    Table *t = new Table(m, p, aMax);

    for (int i = 0; i < keyQty; i++)
    {
        int input;
        cin >> input;
        t->insert(input);
    }

    t->print();

    return EXIT_SUCCESS;
}