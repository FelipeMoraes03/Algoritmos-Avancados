#include <iostream>

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
        Tree *right, *left, *par;

        // CONSTRUCTOR
        Tree(int _value, Tree *_par) {
            value = _value;
            right = nullptr;
            left = nullptr;
            par = _par;
        }

        // DESTRUCTOR
        ~Tree() {
            delete right;
            delete left;
        }

        // METODOS
        // Insere um novo valor na BST
        // -> Retorna a profundidade em que o novo no foi inserido (ou -1 o valor ja estava presente ns BST)
        int insert(int val, Tree *ptPar) {
            int counter = 0;
            if (val == value) {
                return -1;
            } else if (val < value) {
                if (left) {
                    counter = left->insert(val, left);
                } else {
                    left = new Tree(val, ptPar);
                }
            } else {
                if (right) {
                    counter = right->insert(val, right);
                } else {
                    right = new Tree(val, ptPar);
                }
            }
            counter++;
            if (counter == 0) {
                counter = -1;
            }
            return counter;
        }

        // Busca um valor na BST
        // -> Retorna um ponteiro para o no com o valor buscado (ou NULL se o valor nao estiver presente na BST)
        // -> Armazena a profundidade do no com valor encontrado em uma variavel passada
        Tree* search(int val, Tree *pointer, int &counter) {
            if (val == value) {
                return pointer;
            } else if (val < value) {
                if (left) {
                    counter++;
                    return left->search(val, left, counter);
                } else {
                    return nullptr;
                }
            } else {
                if (right) {
                    counter++;
                    return right->search(val, right, counter);
                } else {
                    return nullptr;
                }
            }
        }
};

class SplayTree {
    private:
        Tree *root;

        // Realiza uma rotacao a esquerda na subarvore passada
        // -> Retorna a nova raiz da subarvore apos a rotacao
        Tree* rotateLeft(Tree *pointer) {
            Tree *parTree, *rightTree, *rigthLeftTree;
            parTree = pointer->par;
            rightTree = pointer->right;
            rigthLeftTree = rightTree->left;
            pointer->right = rigthLeftTree;
            if (rigthLeftTree) {
                rigthLeftTree->par = pointer;
            }
            rightTree->left = pointer;
            pointer->par = rightTree;
            rightTree->par = parTree;
            if (parTree and pointer == parTree->left) {
                parTree->left = rightTree;
            } else if (parTree and pointer == parTree->right) {
                parTree->right = rightTree;
            }
            return rightTree;
        }

        // Realiza uma rotacao a direita na subarvore passada
        // -> Retorna a nova raiz da subarvore apos a rotacao
        Tree* rotateRight(Tree *pointer) {
            Tree *parTree, *leftTree, *leftRightTree;
            parTree = pointer->par;
            leftTree = pointer->left;
            leftRightTree = leftTree->right;
            pointer->left = leftRightTree;
            if (leftRightTree) {
                leftRightTree->par = pointer;
            }
            leftTree->right = pointer;
            pointer->par = leftTree;
            leftTree->par = parTree;
            if (parTree and pointer == parTree->right) {
                parTree->right = leftTree;
            } else if (parTree and pointer == parTree->left) {
                parTree->left = leftTree;
            }
            return leftTree;
        }

        // Realiza uma unica rotacao (direita ou esquerda) para subir o no buscado para a raiz da subarvore
        // -> Retorna a nova raiz da subarvore apos a rotacao
        Tree* zig(Tree *pointer) {
            Tree *parTree;
            parTree = pointer->par;
            if (pointer == parTree->left) {
                return rotateRight(parTree);
            } else {
                return rotateLeft(parTree);
            }
        }

        // Realiza duas rotacoes para subir o no buscado para a raiz da subarvore
        // -> Retorna a nova raiz da subarvore apos as rotacoes
        Tree* zigZag(Tree *pointer) {
            Tree *parTree, *gTree;
            parTree = pointer->par;
            gTree = parTree->par;
            if (parTree == gTree->left) {
                if (pointer == parTree->left) {  // LEFT-LEFT
                    rotateRight(gTree);
                    return rotateRight(parTree);
                } else {                       // LEFT-RIGHT
                    rotateLeft(parTree);
                    return rotateRight(gTree);
                }
            } else {
                if (pointer == parTree->right) { // RIGHT-RIGHT
                    rotateLeft(gTree);
                    return rotateLeft(parTree);
                } else {                       // RIGHT-LEFT
                    rotateRight(parTree);
                    return rotateLeft(gTree);
                }
            }
        }

        // Sobe com o no buscado para a raiz da arvore principal
        // -> Retorna a nova raiz da arvore principal todas as rotacoes
        Tree* splay(Tree *pointer) {
            Tree *gTree;
            while (pointer->par) {
                gTree = pointer->par->par;
                if (!gTree) {
                    zig(pointer);
                } else {
                    zigZag(pointer);
                }
            }
            return pointer;
        }

    public:
        // CONSTRUCTOR
        SplayTree() {
            root = nullptr;
        }

        // DESTRUCTOR
        ~SplayTree() {
            delete root;
        }

        // METODOS
        // Insere um novo valor na SplayTree
        // -> Retorna a profundidade em que o novo no foi inserido (ou -1 o valor ja estava presente ns SplayTree) 
        int insert(int val) {
            if (!root) {
                root = new Tree(val, nullptr);
                return 0;
            } else {
                return root->insert(val, root);
            }
        }

        // Busca um valor na SplayTree
        // -> Retorna a profundidade em que valor foi encontrado (ou -1 o valor nao estava presente ns SplayTree) 
        int splayTreeFound(int val) {
            Tree *pointer;
            int height = -1, counter = 0;
            if (root) {
                pointer = root->search(val, root, counter);
                if (pointer) {
                    root = splay(pointer);
                    height = counter;
                }
            }
            return height; 
        }
};

int main() {
    uint32_t rngVal, sizeU, burnIn, nOps, freqInsert, freqSearch, freqPrint, aux; 
    uint32_t *rngPt = &rngVal;
    int val;

    cin >> rngVal >> sizeU >> burnIn >> nOps >> freqInsert >> freqSearch >> freqPrint;

    SplayTree spT;

    // Insere os valores do burnIn
    for (int i = 0; i < burnIn; i++) {
        val = rngVal % sizeU;
        spT.insert(val);
        rng(rngPt);
    }

    // Simula as operacoes apos o burnIn
    int returnOp;
    for (int i = 0; i < nOps; i++) {
        aux = rngVal % (freqSearch + freqInsert);
        rng(rngPt);
        val = rngVal % sizeU;

        if (aux < freqInsert) { //INS
            returnOp = spT.insert(val);
            if (i % freqPrint == 0) {
                cout << "I " << val << " " << returnOp << endl;
            }
        } else { //QRY
            returnOp = spT.splayTreeFound(val);
            if (i % freqPrint == 0) {
                cout << "Q " << val << " " << returnOp << endl;
            }            
        }

        rng(rngPt);
    }

    return 0;
}