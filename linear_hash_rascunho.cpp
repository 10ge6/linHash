#include <iostream>
#include <queue>
#include <cmath>
#include <numeric>
#include <algorithm> // Para std::find

using namespace std;

int blockscriados = 0;
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
            blockscriados++;
        }
        next->add(key);
    }

    bool checkKey(int key)
    {
        for (int i = 0; i < Keys.size(); i++)
        {
            if (Keys[i] == key)
                return true;
        };

        if (next == NULL)
        {
            return false;
        }
        else
        {
            accessCount++;
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

    int depth(int count)
    {
        count++;
        if (next != NULL)
            count += next->depth(count);
        return count;
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
        m = mInicial;
    }

    unsigned int hash(int k, int l)
    {
        int valorHash = k % (int)(pow(2, l) * m);
        return valorHash;
    }

    void insert(int k)
    {
        unsigned int h = hash(k, l);
        if (h < N)
            h = hash(k, l + 1);
        Buckets[h]->add(k);
        keysTotal++;
        if (keysTotal / ((blockscriados + Buckets.size()) * pStore) > threshold)
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
        if (N >= (int)(pow(2, l) * m))
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
            accessCount++;
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

    int Lmax()
    {
        int Lmax = 0;
        for (int i = 0; i < Buckets.size(); i++)
        {
            int count = Buckets[i]->depth(0);
            if (count > Lmax)
                Lmax = count;
        }
        return Lmax;
    }

    double alphaMedio()
    {
        return (double)(keysTotal) / (((double)(blockscriados) + (double)(Buckets.size())) * (double)(pStore));
    }

    double pAsterisco()
    {
        return ((double)(blockscriados) + (double)(Buckets.size())) / (double)(Buckets.size());
    }
};

int generateRandomKey(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int main(int argc, char **argv)
{

    vector<int> pageSizes = {1, 5, 10, 20, 50};
    vector<float> alphaMaxValues = {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
    const int repetitions = 10;

    srand(static_cast<unsigned>(time(nullptr)));

    // desempenho quanto ao espaco
    for (int pageSize : pageSizes)
    {
        for (float alphaMax : alphaMaxValues)
        {
            double alphaMedioIterTotal = 0.0, pAsteriscoIterTotal = 0.0;
            cout << "p=" << pageSize << ",alphaMax=" << alphaMax << endl;
            for (int repeat = 0; repeat < repetitions; repeat++)
            {
                Table *t = new Table(2, pageSize, alphaMax);
                int n = 1000 * pageSize;
                for (int i = 0; i < n; i++)
                {
                    int input = generateRandomKey(0, 16383); // 14 bits
                    t->insert(input);
                }
                alphaMedioIterTotal += t->alphaMedio();
                pAsteriscoIterTotal += t->pAsterisco();
                blockscriados = 0;
                delete t;
            }
            cout << "avg_alphaMedio=" << alphaMedioIterTotal / repetitions << endl;
            cout << "avg_pAsterisco=" << pAsteriscoIterTotal / repetitions << endl;
        }
    }

    // desempenho quanto ao numero medio de acessos
    vector<vector<float>> x_axis = {};
    for (int pageSize : pageSizes)
    {
        for (float alphaMax : alphaMaxValues)
        {
            x_axis.push_back({float(pageSize), alphaMax});
            int Ctotal = 0, Stotal = 0;
            for (int repeat = 0; repeat < repetitions; repeat++)
            {

                Table *t = new Table(2, pageSize, alphaMax);
                int n = 1000 * pageSize;
                vector<int> inputs = {};
                for (int i = 0; i < n; i++)
                {
                    int input = generateRandomKey(1, 10000);
                    t->insert(input);
                    inputs.push_back(input);
                }

                // vector<double> alfaMedio = {};
                // vector<double> pEstrela = {};
                // alfaMedio.push_back(t->alphaMedio());
                // pEstrela.push_back(t->pAsterisco());

                vector<int> KC = {};
                for (int i = 0; i < ceil(0.2 * n); i++)
                {
                    int min_input = *std::min_element(inputs.begin(), inputs.end());
                    int max_input = *std::max_element(inputs.begin(), inputs.end());
                    int teste = generateRandomKey(min_input, max_input);
                    t->checkBlock(teste);
                    KC.push_back(accessCount);
                    accessCount = 0;
                }

                vector<int> KS = {};
                for (int i = 0; i < ceil(0.2 * n); i++)
                {
                    int k = generateRandomKey(1, 10000);
                    if (std::find(inputs.begin(), inputs.end(), k) == inputs.end())
                    {
                        t->checkBlock(k);
                        KS.push_back(accessCount);
                        accessCount = 0;
                    }
                }

                Ctotal += std::accumulate(KC.begin(), KC.end(), 0) / ceil(0.2 * n);
                Stotal += std::accumulate(KS.begin(), KS.end(), 0) / ceil(0.2 * n);

                delete t;
            }
            cout << "avg_C=" << Ctotal / repetitions << endl;
            cout << "avg_S=" << Stotal / repetitions << endl;
        }
    }

    // desempenho durante a inclusao dos n registros
    int pageSize = 10;
    float alphaMax = 0.85;
    double alphaMedioVec[20] = { 0 }, pAsteriscoVec[20] = { 0 }, LmaxVec[20] = { 0 };
    for (int repeat = 0; repeat < repetitions; repeat++)
    {
        cout << "iter " << repeat + 1 << endl;
        double alphaMedio_iIterTotal = 0.0, pAsterisco_iIterTotal = 0.0, Lmax_iIterTotal = 0.0;
        Table *t = new Table(2, pageSize, alphaMax);
        int n = 1000 * pageSize;
        for (int i = 1; i <= n; i++)
        {
            int input = generateRandomKey(0, 16383); // 14 bits
            t->insert(input);
            alphaMedio_iIterTotal += t->alphaMedio();
            pAsterisco_iIterTotal += t->pAsterisco();
            Lmax_iIterTotal += t->Lmax();
            if (i % 500 == 0)
            {
                cout << "section " << i / 500 << "..." << endl;
                // cout << "avg_alphaMedio_i=" << alphaMedio_iIterTotal / i << endl;
                alphaMedioVec[(i / 500) - 1] += alphaMedio_iIterTotal / i;
                // cout << "avg_pAsterisco_i=" << pAsterisco_iIterTotal / i << endl;
                pAsteriscoVec[(i / 500) - 1] += pAsterisco_iIterTotal / i;
                // cout << "avg_Lmax_i=" << Lmax_iIterTotal / i << endl;
                LmaxVec[(i / 500) - 1] += Lmax_iIterTotal / i;
            }
        }
        blockscriados = 0;
        delete t;
    }
    for(int i = 0; i < 20; i++)
    {
        alphaMedioVec[i] /= repetitions;
        pAsteriscoVec[i] /= repetitions;
        LmaxVec[i] /= repetitions;
    }

    return EXIT_SUCCESS;
};
