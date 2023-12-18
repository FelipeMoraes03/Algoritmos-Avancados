#include <iostream>
#include <cmath>

using namespace std;

#define A 1664525
#define C 1013904223
#define R 4294967296

uint32_t rng(uint32_t seed) {
    uint32_t value = (A * seed + C) % R;
    return value;
}

class VanEmdeBoas {
    private:
        VanEmdeBoas *summary = nullptr;
        VanEmdeBoas **clusters = nullptr;
        int size, sqrSize;
        int minValue, maxValue;

        int high(int value) {
            return floor(value / sqrSize);
        }

        int low(int value) {
            int md = sqrt(size);
            return value % md;
        }

        int index(int h, int l) {
            return h * sqrSize + l;
        }
    public:
        //CONSTRUCTOR
        VanEmdeBoas(int _size) {
            size = _size;
            minValue = size;
            maxValue = -1;
            sqrSize = ceil(sqrt(size));

            //Caso Recursivo
            if (size > 2) {
                summary = new VanEmdeBoas(sqrSize);
                clusters = new VanEmdeBoas *[sqrSize];

                for (int i = 0; i < sqrSize; i++) { 
                    clusters[i] = new VanEmdeBoas(sqrSize);
                }
            }
        }

        //DESTRUCTOR
        ~VanEmdeBoas() {
            if (size == 2) {
                return;
            }
            for (int i = 0; i < sqrSize; i++) {
                delete clusters[i];
            }
            delete[] clusters;
            delete summary;
        }

        //MÉTODOS
        int insert(int value) {
            int nivel=1, nextNivel;
            int h, l;
            if (value == minValue or value == maxValue) {
                return 0;
            }
            if (size == 2) {
                minValue = min(minValue, value);
                maxValue = max(maxValue, value);
                return nivel;
            }
            if (minValue == size) {
                minValue = value;
                maxValue = value;
                return nivel;
            }
            if (value < minValue) {
                int aux = minValue;
                minValue = value;
                value = aux;
            }
            maxValue = max(maxValue, value);
            h = high(value);
            l = low(value);
            if (clusters[h]->minValue == clusters[h]->size) {
                summary->insert(h);
            }
            nextNivel = clusters[h]->insert(l);
            if (nextNivel == 0) {
                return 0;
            }
            nivel = nivel + nextNivel;
            return nivel;
        }

        int remove(int value) {
            int i, h, l;
            int nivel=1, nextNivel;

            if (size == 2) {
                if (value != minValue and value != maxValue) {
                    return 0;
                }   
                if (value == minValue) {
                    if (maxValue != minValue) {
                        minValue = maxValue;
                    } else {
                        minValue = 2; //size
                    }
                }
                if (value == maxValue) {
                    if (minValue != size) {
                        maxValue = minValue;
                    } else {
                        maxValue = -1;
                    }
                }
                return nivel;
            }
            if (value == minValue) {
                i = summary->minValue;
                if (i == summary->size) {
                    minValue = size;
                    maxValue = -1;
                    return nivel;
                } else {
                    minValue = index(i, clusters[i]->minValue);
                    value = minValue;
                }
            }
            h = high(value);
            l = low(value);
            nextNivel = clusters[h]->remove(l);
            
            if (nextNivel == 0) {
                return 0;
            }

            nivel = nivel + nextNivel;
            if (clusters[h]->minValue == clusters[h]->size) {
                summary->remove(h);
            }
            if (value == maxValue) {
                i = summary->maxValue;
                if (i == -1) {
                    maxValue = minValue;
                } else {
                    maxValue = index(i, clusters[i]->maxValue);
                }
            }
            return nivel;
        }

        int successor(int value) {
            int h, l, s;
            if (value < minValue) {
                return minValue;
            }
            if (value > maxValue) {
                return size;
            }
            if (size == 2) {
                if (value == 0 and maxValue == 1) {
                    return 1;
                } else {
                    return 2;
                }
            }
            h = high(value);
            l = low(value);
            if (l < clusters[h]->maxValue) {
                s = clusters[h]->successor(l);
                return index(h, s);
            } else {
                h = summary->successor(h);
                if (h == summary->size) {
                    return size;
                } else {
                    s = clusters[h]->minValue;
                    return index(h, s);
                }
            }
        }

        //GETTERS
        int getSize() {
            return size;
        }
};

int main() {
    uint32_t seed, M, burnIn, numOps, freqInsert, freqSucc, freqRemv, freqPrint, sizeU;
    int value, op, aux, nivel;

    cin >> seed >> M >> burnIn >> numOps >> freqInsert >> freqSucc >> freqRemv >> freqPrint;
    sizeU = pow(2, pow(2, M));

    VanEmdeBoas deBoas(sizeU);

    for (int i = 0; i < burnIn; i++) {
        value = seed % sizeU;
        deBoas.insert(value);
        seed = rng(seed);
    }

    for (int i = 0; i < numOps; i++) {
        op = seed % (freqInsert + freqSucc + freqRemv);

        seed = rng(seed);
        value = seed % sizeU;
        if (op < freqInsert) {
            nivel = deBoas.insert(value);

            if (i % freqPrint == 0) {
                cout << "I " << nivel << endl;
            }
            
        } else if (freqInsert <= op and op < (freqInsert + freqSucc)) {
            aux = deBoas.successor(value);

            if (i % freqPrint == 0) {
                cout << "S " << aux << endl;
            }

        } else if ((freqInsert + freqSucc) <= op) {
            aux = deBoas.successor(value);
            if (aux != sizeU) {
                value = aux;
            }
            aux = deBoas.remove(value);

            if (i % freqPrint == 0) {
                cout << "D " << aux << endl;
            }
        }

        seed = rng(seed);
    }
    return 0;
}