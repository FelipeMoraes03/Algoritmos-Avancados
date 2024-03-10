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

class Tree {
    public:
        int value;
        uint32_t priority;
        Tree *right, *left;

        // CONSTRUCTOR
        Tree(int _value, uint32_t _priority) {
            value = _value;
            priority = _priority;
            right = nullptr;
            left = nullptr;
        }

        // DESTRUCTOR
        ~Tree() {
            delete right;
            delete left;
        }

        // Busca um valor na BST
        // -> Retorna a a profundidade do no procurado ou -1 se o valor nao estiver presente
        int search(int val, int counter=0) {
            if (val == value) {
                return counter;
            } else if (val < value) {
                if (left) {
                    counter++;
                    return left->search(val, counter);
                } else {
                    return -1;
                }
            } else {
                if (right) {
                    counter++;
                    return right->search(val, counter);
                } else {
                    return -1;
                }
            }
        }
};

class Treap {
    private:
        Tree* root;
        uint32_t *rngSeed;

        // METODOS
        Tree* merge(Tree *x, Tree *y) {
            if (x == nullptr) {
                return y;
            } else if (y == nullptr) {
                return x;
            } else if (x->priority > y->priority) {
                x->right = merge(x->right, y);
                return x;
            } else {
                y->left = merge(x, y->left);
                return y;
            }
        }

        Tree* splitReturn[2];
        void split(Tree *root, int val) {
            if (root == nullptr) {
                splitReturn[0] = nullptr;
                splitReturn[1] = nullptr;
            } else if (val == root->value) {
                splitReturn[0] = root->left;
                splitReturn[1] = root->right;
            } else if (val < root->value) {
                split(root->left, val);
                root->left = splitReturn[1];
                splitReturn[1] = root;
            } else {
                split(root->right, val);
                root->right = splitReturn[0];
                splitReturn[0] = root;
            }
        }

        // Insere um valor ainda nao existente na Treap
        // Retorna o root atualizado da Treap
        Tree* insertNew(Tree *root, Tree *node, int &counter) {
            if (root == nullptr) {
                return node;
            } else if (node->priority > root->priority) {
                split(root, node->value);
                node->left = splitReturn[0];
                node->right = splitReturn[1];
                return node;
            } else if (node->value < root->value) {
                counter++;
                root->left = insertNew(root->left, node, counter);
                return root;
            } else {
                counter++;
                root->right = insertNew(root->right, node, counter);
                return root;
            }
        }

        bool removed = false;
        // Remove um valor da Treap
        // Retorna o root atualizado da Treap
        Tree* removeNew(Tree *root, int val, int &counter) {
            if (root == nullptr) {
                removed = true;
                return nullptr;
            } else if (val == root->value) {
                return merge(root->left, root->right);
            } else if (val < root->value) {
                counter++;
                root->left = removeNew(root->left, val, counter);
                return root;
            } else {
                counter++;
                root->right = removeNew(root->right, val, counter);
                return root;
            }
        }
    public:

        // CONSTRUTOR
        Treap(uint32_t *seed_) {
            root = nullptr;
            rngSeed = seed_;
        }
        // DESTRUTOR
        ~Treap() {
            delete root;
        }

        // Insere um novo valor na Treap caso o valor ainda nao esteja presente
        // Retorna a profundidade em que o valor foi inserido, ou -1 caso o valor ja estava presente
        int insert(int val) {
            int counter = 0;
            if (root == nullptr) {
                rng(rngSeed);
                root = new Tree(val, *rngSeed);
            } else if (root->search(val) == -1) {
                rng(rngSeed);
                Tree *node = new Tree(val, *rngSeed);
                root = insertNew(root, node, counter);
            } else {
                return -1;
            }
            return counter;
        }

        // Remove um valor da Treap caso o valor esteja presente
        // Retorna a profundidade em que o valor foi encontrado antes da remocao, ou -1 caso o valor nao estava presente
        int remove(int val) {
            int counter = 0;
            removed = false;
            root = removeNew(root, val, counter);
            if (!removed) {
                return counter;
            }
            return -1;
        }

        // Busca um valor na Treap
        // Retorna a profundidade em que o valor foi encontrado, ou -1 caso nao esteja presente
        int query(int val) {
            return root->search(val);
        }
};



int main() {
    uint32_t rngVal, sizeU, burnIn, nOps, freqInsert, freqRemove, freqSearch, freqPrint, aux; 
    uint32_t *rngPt = &rngVal;
    int val;

    cin >> rngVal >> sizeU >> burnIn >> nOps >> freqInsert >> freqRemove >> freqSearch >> freqPrint;

    Treap tp(rngPt);

    // Insere os valores do burnIn
    for (int i = 0; i < burnIn; i++) {
        val = rngVal % sizeU;
        tp.insert(val);
        rng(rngPt);
    }

    // Simula as operacoes apos o burnIn
    int returnOp;
    for (int i = 0; i < nOps; i++) {
        aux = rngVal % (freqSearch + freqRemove + freqInsert);
        rng(rngPt);
        val = rngVal % sizeU;

        if (aux < freqInsert) { // INS
            returnOp = tp.insert(val);
            if (i % freqPrint == 0) {
                cout << "I " << val << " " << returnOp << endl;
            }
        } else if (aux < freqInsert + freqRemove) { // DEL
            returnOp = tp.remove(val);
            if (i % freqPrint == 0) {
                cout << "D " << val << " " << returnOp << endl;
            }
        } else { // QRY
            returnOp = tp.query(val);
            if (i % freqPrint == 0) {
                cout << "Q " << val << " " << returnOp << endl;
            }            
        }

        rng(rngPt);
    }

    return 0;
}