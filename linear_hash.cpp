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

    double alphaMedio()
    {
        int a = blockscriados;
        return (double)(keysTotal) / (((double)(blockscriados) + (double)(Buckets.size())) * (double)(pStore));
    }

    double pAsterisco()
    {
        return ((double)(blockscriados) + (double)(Buckets.size())) / (double)(Buckets.size());
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
    double alphaMaxpVec[40][2], alphaMedioVec[40] = {0}, pAsteriscoVec[40] = {0};
    int forIter = 0;
    for (int pageSize : pageSizes)
    {
        for (float alphaMax : alphaMaxValues)
        {
            double alphaMedioIterTotal = 0.0, pAsteriscoIterTotal = 0.0;
            // cout << "p=" << pageSize << ",alphaMax=" << alphaMax << endl;
            alphaMaxpVec[forIter][0] = alphaMax;
            alphaMaxpVec[forIter][1] = pageSize;
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
            // cout << "avg_alphaMedio=" << alphaMedioIterTotal / repetitions << endl;
            alphaMedioVec[forIter] = alphaMedioIterTotal / repetitions;
            // cout << "avg_pAsterisco=" << pAsteriscoIterTotal / repetitions << endl;
            pAsteriscoVec[forIter] = pAsteriscoIterTotal / repetitions;
            forIter++;
        }
    }
    cout << '[';
    for (int i = 0; i < 40; i++)
    {
        cout << "\'(" << alphaMaxpVec[i][0] << ',' << alphaMaxpVec[i][1] << "\')";
        if (i < 39)
            cout << ',';
    }
    cout << "],[";
    for (int i = 0; i < 40; i++)
    {
        cout << alphaMedioVec[i];
        if (i < 39)
            cout << ',';
    }
    cout << ']' << endl << endl << '[';
    for (int i = 0; i < 40; i++)
    {
        cout << "\'(" << alphaMaxpVec[i][0] << ',' << alphaMaxpVec[i][1] << ")\'";
        if (i < 39)
            cout << ',';
    }
    cout << "],[";
    for (int i = 0; i < 40; i++)
    {
        cout << pAsteriscoVec[i];
        if (i < 39)
            cout << ',';
    }
    cout << ']' << endl;

    cout << "====================================" << endl;

    // desempenho quanto ao numero medio de acessos
    vector<vector<float>> x_axis = {};
    double mediaC[40] = {0}, mediaS[40] = {0};
    int forIter1;
    for (int pageSize : pageSizes)
    {
        for (float alphaMax : alphaMaxValues)
        {
            // double Ctotal = 0, Stotal = 0;
            for (int repeat = 0; repeat < repetitions; repeat++)
            {

                Table *t = new Table(2, pageSize, alphaMax);
                int n = 1000 * pageSize;
                vector<int> inputs = {};
                for (int i = 0; i < n; i++)
                {
                    int input = generateRandomKey(0, 16383); // 14 bits
                    t->insert(input);
                    inputs.push_back(input);
                }

                // vector<double> alfaMedio = {};
                // vector<double> pEstrela = {};
                // alfaMedio.push_back(t->alphaMedio());
                // pEstrela.push_back(t->pAsterisco());

                int KC = 0;
                for (int i = 0; i < ceil(0.2 * n); i++)
                {
                    // int min_input = *std::min_element(inputs.begin(), inputs.end());
                    // int max_input = *std::max_element(inputs.begin(), inputs.end());
                    int index = generateRandomKey(0, (1000 * pageSize) - 1);
                    t->checkBlock(inputs[index]);
                    KC += accessCount;
                    accessCount = 0;
                }

                int KS = 0;
                for (int i = 0; i < ceil(0.2 * n); i++)
                {
                    int k = generateRandomKey(16384, 32767);
                    t->checkBlock(k);
                    KS += accessCount;
                    accessCount = 0;
                }

                // Ctotal += KC / ceil(0.2 * n);
                mediaC[forIter1] += KC / ceil(0.2 * n);
                // cout << KC / ceil(0.2 * n) << endl;
                // Stotal += KS / ceil(0.2 * n);
                mediaS[forIter1] += KS / ceil(0.2 * n);
                // cout << KS / ceil(0.2 * n) << endl;
                delete t;
            }
            forIter1++;
            // cout << "avg_C=" << Ctotal / repetitions << endl;
            // cout << "avg_S=" << Stotal / repetitions << endl;
        }
    }
    for(int i = 0; i < 40; i++)
    {
        mediaC[i] /= repetitions;
    }
    for(int i = 0; i < 40; i++)
    {
        mediaS[i] /= repetitions;
    }
    cout << '[';
    for(int i = 0; i < 8; i++)
    {
        cout << alphaMaxValues[i];
        if(i < 7) cout << ',';
    }
    cout << "],[";
    for(int i = 0; i < 40; i++)
    {
        cout << mediaC[i];
        if(i < 39) cout << ',';
    }
    cout << ']' << endl << endl <<'[';
    for(int i = 0; i < 8; i++)
    {
        cout << pageSizes[i];
        if(i < 7) cout << ',';
    }
    cout << "],[";
    for(int i = 0; i < 40; i++)
    {
        cout << mediaC[i];
        if(i < 39) cout << ',';
    }
    cout << ']' << endl << endl << '[';
    for(int i = 0; i < 8; i++)
    {
        cout << alphaMaxValues[i];
        if(i < 7) cout << ',';
    }
    cout << "],[";
    for(int i = 0; i < 40; i++)
    {
        cout << mediaS[i];
        if(i < 39) cout << ',';
    }
    cout << ']' << endl << endl << '[';
    for(int i = 0; i < 8; i++)
    {
        cout << pageSizes[i];
        if(i < 7) cout << ',';
    }
    cout << "],[";
    for(int i = 0; i < 40; i++)
    {
        cout << mediaS[i];
        if(i < 39) cout << ',';
    }
    cout << ']' << endl;

    cout << "====================================" << endl;

    // desempenho durante a inclusao dos n registros
    int pageSize = 10;
    float alphaMax = 0.85;
    double alphaMedioiVec[20] = {0}, pAsteriscoiVec[20] = {0}, LmaxVec[20] = {0};
    for (int repeat = 0; repeat < repetitions; repeat++)
    {
        // cout << "iter " << repeat + 1 << endl;
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
                // cout << "section " << i / 500 << "..." << endl;
                // cout << "avg_alphaMedio_i=" << alphaMedio_iIterTotal / i << endl;
                alphaMedioiVec[(i / 500) - 1] += alphaMedio_iIterTotal / i;
                // cout << "avg_pAsterisco_i=" << pAsterisco_iIterTotal / i << endl;
                pAsteriscoiVec[(i / 500) - 1] += pAsterisco_iIterTotal / i;
                // cout << "avg_Lmax_i=" << Lmax_iIterTotal / i << endl;
                LmaxVec[(i / 500) - 1] += Lmax_iIterTotal / i;
            }
        }
        blockscriados = 0;
        delete t;
    }
    cout << '[';
    for (int i = 1; i <= 20; i++)
    {
        cout << i * 500;
        if (i < 20)
            cout << ',';
    }
    cout << "],[";
    for (int i = 0; i < 20; i++)
    {
        alphaMedioiVec[i] /= repetitions;
        cout << alphaMedioiVec[i];
        if (i < 19)
            cout << ',';
    }
    cout << "],[";
    for (int i = 1; i <= 20; i++)
    {
        cout << i * 500;
        if (i < 20)
            cout << ',';
    }
    cout << "],[";
    for (int i = 0; i < 20; i++)
    {
        pAsteriscoiVec[i] /= repetitions;
        cout << pAsteriscoiVec[i];
        if (i < 19)
            cout << ',';
    }
    cout << "],[";
    for (int i = 1; i <= 20; i++)
    {
        cout << i * 500;
        if (i < 20)
            cout << ',';
    }
    cout << "],[";
    for (int i = 0; i < 20; i++)
    {
        LmaxVec[i] /= repetitions;
        cout << LmaxVec[i];
        if (i < 19)
            cout << ',';
    }
    cout << ']' << endl;

    return EXIT_SUCCESS;
}
