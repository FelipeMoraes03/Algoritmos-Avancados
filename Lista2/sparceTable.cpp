#include <iostream>
#include <cmath>

using namespace std;

#define A 1664525
#define C 1013904223
#define R 4294967296

uint32_t minimun(uint32_t a, uint32_t b=4294967295) {
    return min(a, b);
}

uint32_t maximum(uint32_t a, uint32_t b=0) {
    return max(a, b);
}

uint32_t summatory(uint32_t a, uint32_t b=0) {
    return a + b;
}

uint32_t rng(uint32_t seed) {
    uint32_t value = (A * seed + C) % R;
    return value;
}

class SparceTable {
    private:
        uint32_t seed;
        int size, lgSize;
        uint32_t (*function)(uint32_t, uint32_t);
        uint32_t (*rQuerry)(int, int);
        uint32_t **table;
        uint32_t neutralElement;

        void buildTable() {
            table = new uint32_t *[lgSize];
            for (int i = 0; i < lgSize; i++) {
                table[i] = new uint32_t [size];
            }
            preProcessing();
        }

        void preProcessing() {
            for (int j = 0; j < size; j++) {
                table[0][j] = seed%(4*size);
                seed = rng(seed);
            }
            int k = 1;
            for (int i = 1; i < lgSize; i++) {
                for (int j = 0; j < size; j++) {
                    if (j+k <= size) {
                        table[i][j] = function(table[i-1][j], table[i-1][j+k]);
                    } else {
                        table[i][j] = function(table[i-1][j], neutralElement);
                    }
                }
                k = 2*k;
            }
        }

    public:
        //CONSTRUCTOR
        SparceTable(uint32_t _seed, int _size, string _function) {
            seed = _seed;
            size = _size;
            lgSize = log2(size) + 1;

            if (_function == "MIN") {
                function = minimun;
                //rQuerry = rangeQuerryIdempotente;
                neutralElement = 4294967295;
            } else if (_function == "MAX") {
                function = maximum;
                //rQuerry = rangeQuerryIdempotente;
                neutralElement = 0;
            } else if (_function == "SUM") {
                function = summatory;
                //rQuerry = rangeQuerry;               
                neutralElement = 0;
            }    

            buildTable();
        }

        //DESTRUCTOR
        ~SparceTable() {
            for (int i = 0; i < lgSize; i++) {
                delete table[i];
            }
            delete table;
        }

        //MÉTODOS
        uint32_t rangeQuerry(int left, int right) {
            uint32_t answer = neutralElement;
            int index;
            while (left < right) {
                index = log2(right - left);
                answer = function(answer, table[index][left]);
                left = left + pow(2, index);
            }
            return answer;
        }

        uint32_t rangeQuerryIdempotente(int left, int right) {
            int indexLeft = log2(right - left);
            int indexRight = right - pow(2, indexLeft);

            return function(table[indexLeft][left], table[indexLeft][indexRight]);
        }

        void update(uint16_t value, int index) {
            table[0][index] = value;
            int k = 1;
            for (int i = 1; i < lgSize; i++) {
                for (int l = max(0, index-2*k+1); l <= index; l++) {
                    if (l+k <= size) {
                        table[i][l] = function(table[i-1][l], table[i-1][l+k]);
                    } else {
                        table[i][l] = function(table[i-1][l], neutralElement);
                    }   
                }
                k = 2*k;
            }
        }

        //GETTERS
        uint32_t getSeed() {
            return seed;
        }
};

int main() {
    uint32_t seed, result;
    int size, numOps, freqRangeQuery, freqUpdate, numCase;
    string function;

    numCase = 0;
    while (cin >> seed >> size >> function >> numOps >> freqRangeQuery >> freqUpdate) {
        cout << "caso " << numCase << endl;

        SparceTable sTb(seed, size, function);

        seed = sTb.getSeed();

        for (size_t i = 0; i < numOps; i++) {
            if (seed % (freqRangeQuery + freqUpdate) < freqRangeQuery) {
                seed = rng(seed);
                int leftInterval, rightInterval;
                leftInterval = seed % size;
                seed = rng(seed);
                rightInterval = leftInterval + 1 + (seed % (size - leftInterval));
                seed = rng(seed);
                if (function[0] == 'M') {
                    result = sTb.rangeQuerryIdempotente(leftInterval, rightInterval);
                } else {
                    result = sTb.rangeQuerry(leftInterval, rightInterval);
                }
            } else {
                seed = rng(seed);
                int index = seed % size;
                seed = rng(seed);
                uint32_t value = seed % (4*size);
                seed = rng(seed);
                sTb.update(value, index);
                if (function[0] == 'M') {
                    result = sTb.rangeQuerryIdempotente(index, size);
                } else {
                    result = sTb.rangeQuerry(index, size);
                }
            }
            cout << result << endl;
        }
        numCase++;
        cout << endl;
    }
    return 0;
}