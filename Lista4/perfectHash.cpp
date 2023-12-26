#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

#define A 1664525
#define C 1013904223
#define R 4294967296
#define P 2147483647

void rng(uint32_t *seed) {
    *seed = (A * (*seed) + C) % R;
}

class OpenHashTable {
    private:
        uint32_t size, capacity, funcParamA, funcParamB;
        uint32_t *rngSeed;

        void reHash() {
            vector<int> *aux;
            aux = table;
            uint32_t auxCapacity = capacity;

            // Nova tabela
            capacity = (2 * capacity) + 1;
            size = 0;
            table = new vector<int>[capacity];

            // Nova funcao de dispercao
            rng(rngSeed);
            funcParamA = 1 + (*rngSeed)%(P-1);
            rng(rngSeed);
            funcParamB = (*rngSeed)%P;

            for (int i = 0; i < auxCapacity; i++) {
                for (int j = 0; j < aux[i].size(); j++) {
                    insert(aux[i][j]);
                }
            }

            delete[] aux;
        }

    public:
        vector<int> *table;

        //CONSTRUCTOR
        OpenHashTable(uint32_t capacity_, uint32_t *seed_) {
            capacity = capacity_;
            rngSeed = seed_;
            size = 0;
            
            // Gera a funcao de dispercao
            funcParamA = 1 + (*rngSeed)%(P-1);
            rng(rngSeed);
            funcParamB = (*rngSeed)%P;
            rng(rngSeed);

            table = new vector<int>[capacity];
        }

        // DESTRUTOR
        ~OpenHashTable() {
            delete[] table;
        }

        //METODOS
        // Insere um novo valor na Hashtable
        // -> Retorna posicao do valor na HashTable
        string insert(int val) {
            string position = querry(val);
            if (position != "-1") {
                return position;
            }
            if (2 * size > capacity) {
                reHash();
            }
            uint32_t index = ((funcParamA * val + funcParamB) % P) % capacity;
            table[index].push_back(val);
            size++;

            return to_string(index) + " " + to_string(table[index].size()-1);
        }

        // Verifica se o valor ja esta inserido na Hashtable
        // -> Retorna posicao do valor na HashTable OU "-1" se nao esta na presente
        string querry(int val) {
            uint32_t index = ((funcParamA * val + funcParamB) % P) % capacity;
            for (int i = 0; i < table[index].size(); i++) {
                if (table[index].at(i) == val) {
                    return to_string(index) + " " + to_string(i);
                }
            }
            return "-1";
        }

        //GETTERS
        uint32_t getCapacity() {
            return capacity;
        }

        uint32_t getSize() {
            return size;
        }

        uint32_t getVtSize(uint32_t index) {
            return table[index].size();
        }

        int getValue(uint32_t l, uint32_t c) {
            return table[l][c];
        }
};

class SecondLevelTable {
    public:
        uint32_t size, capacity, funcParamA, funcParamB;
        int *table;
        uint32_t *rngSeed;

        SecondLevelTable(uint32_t size_, uint32_t *seed_) {
            size = size_;
            capacity = pow(size, 2) + 1;
            rngSeed = seed_;

            // Gera a funcao de dispercao
            funcParamA = 1 + (*rngSeed)%(P-1);
            rng(rngSeed);
            funcParamB = (*rngSeed)%P;
            rng(rngSeed);

            table = new int[capacity];
            for (int i = 0; i < capacity; i++) {
                table[i] = -1;
            }
        }

        ~SecondLevelTable() {
            delete[] table;
        }

        //METODOS
        // Insere um novo valor na Hashtable
        // -> Retorna booleano se ocorreu colisao
        bool insert(uint32_t val) {
            uint32_t index = ((funcParamA * val + funcParamB) % P) % capacity;
            if (table[index] == -1) {
                table[index] = val;
                return false;
            }
            return true;
        }
};

class PerfectHashTable {
    private:
        uint32_t size, funcParamA, funcParamB;
        SecondLevelTable **table;
        uint32_t *rngSeed;
        vector<int> *buckets;

        void buildTable(OpenHashTable *auxTable) {
            // Cria os buckets
            uint32_t auxVal, index;
            uint32_t totalCapacity;
            bool totalCapacityBool = true;
            while (totalCapacityBool) {
                // Gera nova funcao de dispercao
                funcParamA = 1 + (*rngSeed)%(P-1);
                rng(rngSeed);
                funcParamB = (*rngSeed)%P;
                rng(rngSeed);

                buckets = new vector<int>[size];
                for (int i = 0; i < auxTable->getCapacity(); i++) {
                    for (int j = 0; j < auxTable->getVtSize(i); j++) {
                        auxVal = auxTable->getValue(i, j);
                        index = ((funcParamA * auxVal + funcParamB) % P) % size;
                        buckets[index].push_back(auxVal);
                    }
                }

                // Verifica o somatorio das capacidades dos buckets
                totalCapacity = 0;
                for (int i = 0; i < size; i++) {
                    totalCapacity += pow(buckets[i].size(), 2);
                }
                if (totalCapacity <= 4*size) {
                    totalCapacityBool = false;
                } else {
                    delete[] buckets;
                }
            }

            // Cria a tabela
            table = new SecondLevelTable *[size];
            bool collisions;
            for (int i = 0; i < size; i++) {
                collisions = true;
                if (buckets[i].size() > 0) {
                    while (collisions) {
                        table[i] = new SecondLevelTable(buckets[i].size(), rngSeed);
                        for (int j = 0; j < buckets[i].size(); j++) {
                            collisions = table[i]->insert(buckets[i][j]);
                            if (collisions) {
                                delete table[i];
                                break;
                            }
                        }
                    }
                }
            }
        }

    public:
        PerfectHashTable(int size_, uint32_t *seed_, OpenHashTable *auxTable) {
            size = 2 * floor(size_/2) + 1;
            rngSeed = seed_;

            buildTable(auxTable);

            delete[] buckets;
        }

        ~PerfectHashTable() {
            delete[] table;
        }

        // Verifica se o valor esta presente na Hashtable
        // -> Retorna posicao do valor na HashTable OU "-1 -1" se nao esta na presente
        string querry(int val) {
            int i, j;
            i = ((funcParamA * val + funcParamB) % P) % size;
            if (table[i] != nullptr) {
                j = ((table[i]->funcParamA * val + table[i]->funcParamB) % P) % table[i]->capacity;
                if (table[i]->table[j] == val) {
                    return to_string(i) + " " + to_string(j);
                }
            }
            return "-1 -1";
        }
};

int main() {
    uint32_t rngVal, sizeU, initialCap, nOpsInsert, nOpsQuerry, freqPrintInsert, freqPrintQuerry; 
    uint32_t *rngPt = &rngVal;

    cin >> rngVal >> sizeU >> initialCap >> nOpsInsert >> nOpsQuerry >> freqPrintInsert >> freqPrintQuerry;

    OpenHashTable openTb(initialCap, rngPt);

    string position;
    int val;
    for (int i = 0; i < nOpsInsert; i++) {
        val = rngVal%sizeU;
        position = openTb.insert(val);

        if (i%freqPrintInsert == 0) {
            cout << "I " << val << " " << position << endl;
        }
        rng(rngPt);
    }

    PerfectHashTable perfTb(openTb.getSize(), rngPt, &openTb);
    
    for (int i = 0; i < nOpsQuerry; i++) {
        val = rngVal%sizeU;
        position = perfTb.querry(val);

        if (i%freqPrintQuerry == 0) {
            cout << "Q " << val << " " << position << endl;
        }
        
        rng(rngPt);
    }

    return 0;
}