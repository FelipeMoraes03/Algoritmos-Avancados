#include <iostream>

using namespace std;

class DinArray {
    private:
        uint32_t *vt;
        int size;
        int capacity;
        int numRemoves;

        //MÉTODOS
        //Aumenta o tamanho do array dinãmico
        void resize() {
            uint32_t *vtAux;
            vtAux = vt;

            capacity = 2 * capacity + 1;

            vt = new uint32_t[capacity];

            for (int i=0; i < size; i++) {
                vt[i] = vtAux[i + numRemoves];
            }
            delete vtAux;
            numRemoves = 0;
        }

    public:
        //CONSTRUCTOR
        DinArray() {
            vt = new uint32_t[3];
            size = 0;
            capacity = 3;
            numRemoves = 0;
        }

        //DESTRUCTOR
        ~DinArray() {
            delete vt;
        }

        //MÉTODOS
        //Insere um novo elemento no final do array
        void pushBack(uint32_t val) {
            if (capacity == size + numRemoves) {
                resize();
            }
            vt[size + numRemoves] = val;
            size++;
        }

        //Remove um elemento do final do array
        uint32_t popBack() {
            size--;
            return vt[size + numRemoves];
        }

        //Remove um elemento do início do array
        uint32_t popFront() {
            numRemoves++;
            size--;
            return vt[0];
        }

        //Verifica se o array está vazio
        bool empty() {
            if (getSize() == 0) {
                return true;
            }
            return false;
        }

        //GETTERS
        //Retorna o número de elementos do array
        int getSize() {
            return size;
        }

        //Retorna o elemento de index passado
        uint32_t getElmt(int index) {
            return vt[index + numRemoves];
        }

        //Retorna a capacidade do array
        int getCapacity() {
            return capacity;
        }

        //Retorna o número de elementos removidos ainda presentes no array (elementos "mortos")
        int getNumRemoves() {
            return numRemoves;
        }
};

class MinQueue {
    private:
        DinArray valueQueue;
        DinArray minIndexQueue;
        int numAbsoluteEnqueue=0, numAbsoluteDequeue=0;

    public:
        //Retira um elemento da Fila
        uint32_t dequeue() {
            uint32_t val = valueQueue.getElmt(0);
            if (val == getMinValue()) {
                minIndexQueue.popFront();
            }
            valueQueue.popFront();
            numAbsoluteDequeue++;

            return val;
        }

        //Insere um novo elemento na Fila
        void enqueue(uint32_t val) {
            numAbsoluteEnqueue++;
            valueQueue.pushBack(val);
            while(!minIndexQueue.empty() and valueQueue.getElmt(minIndexQueue.getElmt(minIndexQueue.getSize()-1) - numAbsoluteDequeue) > val) {
                minIndexQueue.popBack();
            }
            minIndexQueue.pushBack(numAbsoluteEnqueue - 1);
        }

        //GETTERS
        //Retorna o tamanho da Fila
        int getSize() {
            return valueQueue.getSize();
        }

        //Retorna o menor valor da Fila
        uint32_t getMinValue(){
            return valueQueue.getElmt(getMinIndex());
        }

        //Retorna o index do menor valor da Fila
        int getMinIndex() {
            return minIndexQueue.getElmt(0) - numAbsoluteDequeue;
        }

        //Retorna o tamanho da lista de possíveis mínimos
        int getMinSize() {
            return minIndexQueue.getSize();
        }
};

#define R 4294967296
#define A 1664525
#define C 1013904223

int main() {
    MinQueue minQ;

    uint32_t seed, burnIn, numOps, freqPushPop;
    cin >> seed >> burnIn >> numOps >> freqPushPop;

    //Insere o Burn In na Fila
    for (int i = 0; i < burnIn; i++) {
        minQ.enqueue(seed);
        seed = (A * seed + C) % R;
    }

    //Realiza as N operações na Fila após o Burn In
    for (int i = 0; i < numOps; i++) {
        if (seed % 10 < freqPushPop) {
            seed = (A * seed + C) % R;
            minQ.enqueue(seed);
        } else {
            minQ.dequeue();
        }

        cout << minQ.getMinSize() << " " << minQ.getMinIndex() << endl;

        seed = (A * seed + C) % R;
    }

    return 0;
}