#include <iostream>
#include <cmath>

using namespace std;

#define A 1664525
#define C 1013904223
#define R 4294967296

// Funcoes combinaveis que serao utilizadas nos testes
uint32_t minimun(uint32_t a, uint32_t b=4294967295) {
    return min(a, b);
}
uint32_t maximum(uint32_t a, uint32_t b=0) {
    return max(a, b);
}
uint32_t summatory(uint32_t a, uint32_t b=0) {
    return a + b;
}

// Gerador de valores pseudoaleatorios
void rng(uint32_t *seed) {
    *seed = (A * (*seed) + C) % R;
}

class segTree {
    private:
        uint32_t *rngSeed, *tree;
        uint32_t (*function)(uint32_t, uint32_t);
        uint32_t neutralElement;
        int size;

        // Cria uma nova SegmentTree com valores pseudoaleatorios
        void build() {
            uint32_t val;
            tree = new uint32_t [2*size];
            for (int i = 0; i < size; i++) {
                val = *rngSeed % (4*size);
                tree[i+size] = function(val, neutralElement);
                rng(rngSeed);
            }
            for (int i = size-1; i > 0; i--) {
                tree[i] = function(tree[2*i], tree[2*i+1]);
            }
            
        }
    public:

        // CONSTRUCTOR
        segTree(uint32_t *_seed, int _size, string _function) {
            rngSeed = _seed;
            size = _size;

            if (_function == "MIN") {
                function = minimun;
                neutralElement = 4294967295;
            } else if (_function == "MAX") {
                function = maximum;
                neutralElement = 0;
            } else if (_function == "SUM") {
                function = summatory;
                neutralElement = 0;
            }

            build();
        }

        // DESTRUCTOR
        ~segTree() {
            delete tree;
        }

        // METODOS
        // Realiza a querry em um intervalo da SegmentTree
        // -> Retorna o resultado da aplicacao da funcao combinavel sobre o intervalo passado
        uint32_t rangeQuerry(int left, int right) {
            int leftAux, rightAux;
            uint32_t answer = neutralElement;
            leftAux = size + left;
            rightAux = size + right;
            while (leftAux < rightAux) {
                if (leftAux % 2 == 1) { // leftAux = filho a direita
                    answer = function(answer, tree[leftAux]);
                    leftAux++;
                }
                if (rightAux % 2 == 1) { // rightAux = filho a esquerda
                    rightAux--;
                    answer = function(answer, tree[rightAux]);
                }
                leftAux = floor(leftAux/2);
                rightAux = floor(rightAux/2);
            }
            return answer;
        }

        // Atualiza o valor de um elemento da SementTree
        void update(uint32_t value, int index) {
            index = size + index;
            tree[index] = function(value, neutralElement);
            index = floor(index/2);
            while (index >= 1) {
                tree[index] = function(tree[2*index], tree[2*index+1]);
                index = floor(index/2);
            }
        }
};



int main() {
    uint32_t rngVal, returnOp, value;
    uint32_t *rngPt = &rngVal;
    int size, numOps, freqRangeQuery, freqUpdate, freqPrint, numCase, leftInterval, rightInterval, index;
    string function;

    numCase = 0;
    while (cin >> rngVal >> size >> function >> numOps >> freqRangeQuery >> freqUpdate >> freqPrint) {
        cout << "caso " << numCase << endl;

        segTree sT(rngPt, size, function);

        for (size_t i = 0; i < numOps; i++) {
            if (rngVal % (freqRangeQuery + freqUpdate) < freqRangeQuery) { // QRY
                rng(rngPt);
                leftInterval = rngVal % size;
                rng(rngPt);
                rightInterval = leftInterval + 1 + (rngVal % (size - leftInterval));
                returnOp = sT.rangeQuerry(leftInterval, rightInterval);
            } else { // UPD
                rng(rngPt);
                index = rngVal % size;
                rng(rngPt);
                value = rngVal % (4*size);
                sT.update(value, index);
                returnOp = sT.rangeQuerry(index, size);
            }
            if (i % freqPrint == 0) {
                cout << returnOp << endl;
            }
            rng(rngPt);
        }
        numCase++;
        cout << endl;
    }
    return 0;
}