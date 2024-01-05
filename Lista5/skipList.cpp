#include <iostream>
#include <vector>

using namespace std;

#define A 1664525
#define C 1013904223
#define R 4294967296
#define P 2147483647

void rng(uint32_t *seed) {
    *seed = (A * (*seed) + C) % R;
}

class Node {
    public:
    int value;
    uint32_t height;
    vector<Node*> next;

    // CONTRUCTOR
    Node(int _value, uint32_t _height) {
        value = _value;
        height = _height;
        next.resize(height, nullptr);
    }
};

class SkipList {
    private:
        Node *head;
        uint32_t height;
        vector<Node*> lastPrecursor;
        uint32_t *rngSeed;
    public:

        // CONSTRUCTOR
        SkipList(uint32_t *seed_) {
            height = 1;
            head = new Node(-1, 1);

            rngSeed = seed_;
        }

        // DESTRUCTOR
        ~SkipList() {
            Node* cur = head;
            while (cur != nullptr) {
                Node* nextNode = cur->next[0]; // Como no nivel 0 todos os nos estao presentes, iteramos por ele
                delete cur;
                cur = nextNode;
            }
        }

        // METODOS
        // Verifica o caminho (precursores de cada nivel) para o valor buscado
        // -> Retorna o numero de nos percorridos
        // -> Armazena o caminho (precursores) no vector lastPrecursor
        int precursor(int val) {
            int counter = 1;
            lastPrecursor.resize(height, nullptr);
            Node *cur = head;
            for (int l = height-1; l >= 0; l--) {
                while (cur->next[l] != nullptr and cur->next[l]->value < val) {
                    cur = cur->next[l];
                    counter++;
                }
                lastPrecursor[l] = cur;
            }
            return counter;
        }

        // Verifica se um elemento esta presente na SkipList
        // -> Retorna uma string indicando o numero de nos percorridos e se o valor foi encontrado
        string search(int val) {
            int counter = precursor(val);
            if (lastPrecursor[0]->next[0] != nullptr and lastPrecursor[0]->next[0]->value == val) {
                return ("F " + to_string(counter) + " " + to_string(lastPrecursor[0]->next[0]->height)); // Valor encontrado
            }
            return ("F " + to_string(counter) + " 0"); // Valor nao encontrado
        }

        // Aleatoriza a altura de um novo no inserido na SkipList
        uint32_t random_height() {
            uint32_t h=1, max_h=height+1;
            rng(rngSeed);
            while ((*rngSeed % 100) < 50 and h < max_h) {
                h++;
                rng(rngSeed);
            }
            return h;
        }

        // Insere um novo valor na SkipList (Caso ainda nao esteja presente)
        // -> Retorna uma string indicando o numero de nos percorridos e se o valor foi inserido (ou ja estava presente)
        string insert(int val) {
            precursor(val);
            if (lastPrecursor[0]->next[0] != nullptr and lastPrecursor[0]->next[0]->value == val) {
                return "I 0"; // Valor ja presente
            }
            uint32_t h = random_height();
            if (h > height) {
                head->next.resize(h, nullptr);
                for (int i = height; i < h; i++) {
                    lastPrecursor.push_back(head);
                }
                height = h;
                head->height = h;                
            }
            Node *n = new Node(val, h);
            for (int l = 0; l < h; l++) {
                n->next[l] = lastPrecursor[l]->next[l];
                lastPrecursor[l]->next[l] = n;
            }
            return "I 1"; // Valor inserido
        }

        // Remove um valor da SkipList (Caso esteja presente)
        // -> Retorna uma string indicando o numero de nos percorridos e se o valor foi removido (ou nao estava presente)
        string remove(int val) {
            precursor(val);
            if (lastPrecursor[0]->next[0] == nullptr or lastPrecursor[0]->next[0]->value != val) {
                return "D 0"; // Valor nao presente antes da remocao
            }
            Node *N = lastPrecursor[0]->next[0];
            for (int l = 0; l < N->height; l++) {
                lastPrecursor[l]->next[l] = N->next[l];
            }
            while (height > 1 and head->next[height-1] == nullptr) {
                height--;
            }
            head->height = height;
            head->next.resize(height);
            delete N;
            return "D 1"; // Valor removido
        }

};

int main() {
    uint32_t rngVal, sizeU, burnIn, nOps, freqSearch, freqInsert, freqRemove, freqPrint, aux; 
    uint32_t *rngPt = &rngVal;
    int val;

    cin >> rngVal >> sizeU >> burnIn >> nOps >> freqSearch >> freqInsert >> freqRemove >> freqPrint;

    SkipList skL(rngPt);

    for (int i = 0; i < burnIn; i++) {
        val = rngVal % sizeU;
        skL.insert(val);
        rng(rngPt);
    }

    string returnOp;
    for (int i = 0; i < nOps; i++) {
        aux = rngVal % (freqSearch + freqInsert + freqRemove);
        rng(rngPt);
        val = rngVal % sizeU;

        if (aux < freqSearch) { //FND
            returnOp = skL.search(val);
        } else if (aux < (freqSearch + freqInsert)) { //INS
            returnOp = skL.insert(val);
        } else { //DEL
            returnOp = skL.remove(val);
        }

        if (i % freqPrint == 0) {
            cout << returnOp << endl;
        }
        
        rng(rngPt);
    }
    
    return 0;
}