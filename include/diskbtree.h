/*
    Trabalho final - Técnicas de programação    2022.2
                Árvore B em disco
    Alunos:
        Luiz Augusto da Silva - 0048504
        Sandy Cristina Barros - 0049777

*/

#ifndef DISKBTREE_H_INCLUDED
#define DISKBTREE_H_INCLUDED
#include<iostream>
#include <string>
#include <vector>
#include <queue>
#include "typedFile.h"
#include "node.h"
#include "record.h"

using namespace std;


template <class T,const unsigned int MIN_DEGREE>
class diskbtree: private typedFile<T, MIN_DEGREE>{
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");
public:
    diskbtree();
    diskbtree(const string name, const string type, const unsigned int version);
    ~diskbtree();
    bool insertKey(T key);
    node<T,MIN_DEGREE> getRoot(){return this->root;};
    bool searchKey(node <T, MIN_DEGREE> r,T key);
    void printTree(node <T, MIN_DEGREE> r); //Usado somente para testes.
    void print();
    bool removeKey(T key);
private:

    node<T,MIN_DEGREE> root;

    bool insertInNodeNonFull(node<T,MIN_DEGREE>& x,unsigned long long int index,T key, int iX);
    void splitChild(node<T,MIN_DEGREE>& x,int i, int iX);

    bool writeNode(node<T,MIN_DEGREE> x, unsigned long long int i);
    node<T,MIN_DEGREE> readNode(unsigned long long int i);

    //Auxiliar do removeKey();
    bool removeAux(node <T, MIN_DEGREE> r,T key, unsigned long long int index);

    //Auxiliar do print();
    void printAux(node<T, MIN_DEGREE> x, vector<string> &v, unsigned int lvl);

    static const unsigned int MAX = 2 * MIN_DEGREE - 1;
    static const unsigned int MIN = MIN_DEGREE - 1;
    static const unsigned int NOT_FOUND = -1;
};

template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::~diskbtree(){
    typedFile<T, MIN_DEGREE>::close();
}

//Construtor default não faz nada.
template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::diskbtree():typedFile<T, MIN_DEGREE>::typedFile() {}


//Construtor parametrizado cria um arquivo tipado e determina uma raiz vazia.
template <class T, const unsigned int MIN_DEGREE>
diskbtree<T, MIN_DEGREE>::diskbtree(const string name, const string type, const unsigned int version):typedFile<T, MIN_DEGREE>::typedFile(name, type, version){
    if(typedFile<T, MIN_DEGREE>::isOpen()){
        unsigned int i;
        i = typedFile<T, MIN_DEGREE>::getFirstValid();
        if(i == 0){
            node<T, MIN_DEGREE> n;
            n.setLeaf(true);
            n.setSize(0);
            this->root = n;

            record<T, MIN_DEGREE> r;
            r.setData(n);
            typedFile<T, MIN_DEGREE>::insertRecord(r);
        }
        else{
            this->root = this->readNode(i);
        }
    }
    else{
        cout<<endl<<"Error"<<endl;
    }
}

//Função para inserção de uma chave a partir da raiz
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::insertKey(T key){
    node<T, MIN_DEGREE>r;
    unsigned long long int i;
    i = typedFile<T, MIN_DEGREE>::getFirstValid();
    r = this->readNode(i);
    if (r.getSize() == MAX){
        node<T, MIN_DEGREE> s;
        s.setLeaf(false);
        s.setSize(0);
        s.setChildren(0,i);

        record<T, MIN_DEGREE> r1;
        r1.setData(s);
        typedFile<T, MIN_DEGREE>::insertRecord(r1);     //Inserindo a raiz no arquivo tipado
        i = typedFile<T, MIN_DEGREE>::getFirstValid();
        this->splitChild(s,0,i);
        this->insertInNodeNonFull(s,i,key,i);
        this->root = s;
    }
    else{
        this->insertInNodeNonFull(r,i,key,i);
        this->root = r;
    }
}

//Função de remoção de chave a partir da raiz
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::removeKey(T key){
    node<T, MIN_DEGREE>r;
    unsigned long long int i;
    i = typedFile<T, MIN_DEGREE>::getFirstValid();
    r = this->readNode(i);
    this->removeAux(r,key,i);
}

//Auxiliar da função removeKey
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::removeAux(node <T, MIN_DEGREE> r,T key, unsigned long long int index){
    int j = 0;
    node <T, MIN_DEGREE> c1;
    node <T, MIN_DEGREE> c2;
    node <T, MIN_DEGREE> c3;
    record<T, MIN_DEGREE> r1;

    while (j < r.getSize() && key > r.getkey(j)){
        j++;
    }
    if(j < r.getSize() &&  key == r.getkey(j)){
        if (r.isleaf()){    //Caso 1
            for (int i = j; i < r.getSize(); i++)
                {
                    r.setKey(i,r.getkey(i+1));
                }
                r.setSize(r.getSize()-1);
                r1.setData(r);
                typedFile<T, MIN_DEGREE>::writeRecord(r1,index);

                cout <<"Chave "<<key<<" excluída"<<endl;
                return true;
        }else{  //Caso 2
            c1 = this->readNode(r.getChildren(j));
            if(c1.getSize() >= MIN_DEGREE){ //Caso 2a
                    r.setKey(j,c1.getkey(c1.getSize()-1));
                    c1.setSize(c1.getSize()-1);

                    record<T, MIN_DEGREE> r1;

                    r1.setData(c1);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,r.getChildren(j));

                    r1.setData(r);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,index);
                    return true;
            }
            c2 = this->readNode(r.getChildren(j+1));
            if(c2.getSize() >= MIN_DEGREE){ //Caso 2b
                    r.setKey(j,c2.getkey(0));

                    for (int i = 0 ; i < c2.getSize(); i++){
                        c2.setKey(i,c2.getkey(i+1));
                    }
                    c2.setSize(c2.getSize()-1);

                    record<T, MIN_DEGREE> r1;

                    r1.setData(c2);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,r.getChildren(j+1));

                    r1.setData(r);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,index);
                    return true;
            }else{ //Caso 2c
                for (int i = 0; i < c1.getSize(); i++){
                    c3.setKey(i,c1.getkey(i));
                }
                c3.setSize(c1.getSize());


                c3.setKey(MIN_DEGREE-1, r.getkey(j));
                c3.setSize(c3.getSize()+1);


                for (int i = 0 ; i< c2.getSize(); i++){
                   c3.setKey(i+MIN_DEGREE,c2.getkey(i));
                }
                c3.setSize(c3.getSize()+c2.getSize());

                if(!c1.isleaf()){
                    for (int i = 0 ; i <= c1.getSize(); i++){
                        c3.setChildren(i,c1.getChildren(i));
                    }
                    for (int i = 0 ; i <= c2.getSize(); i++){
                        c3.setChildren(i+MIN_DEGREE,c2.getChildren(i));
                    }
                    c3.setLeaf(false);
                }

                for (int i = j; i < r.getSize(); i++)
                {
                    r.setKey(i,r.getkey(i+1));
                }

                if(!r.isleaf()){
                    for (int i = j; i < r.getSize(); i++){
                        r.setChildren(i,r.getChildren(i+1));
                    }
                }
                r.setSize(r.getSize()-1);

                if(r.getSize() == 0){
                    this->removeAux(c3,key,index);
                }else{
                    unsigned long long int p = typedFile<T, MIN_DEGREE>::alocateNextPosition();
                    r1.setData(c3);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,p);
                    this->removeAux(c3,key,p);

                    r.setChildren(j,p);
                    r1.setData(r);
                    typedFile<T, MIN_DEGREE>::writeRecord(r1,index);
                }
                return true;
            }
        }
    }else{
         if(r.isleaf()){
            cout << "Chave "<<key<<" não encontrada"<<endl;
            return true;
         }else{ //Caso 3
             c1 = this->readNode(r.getChildren(j));
             if(c1.getSize() >= MIN_DEGREE){
                removeAux(c1,key,r.getChildren(j));
                return true;
             }else{ //Caso 3a
                if(r.getSize() == MAX){
                    c1 = this->readNode(r.getChildren(j-1));
                    c2 = this->readNode(r.getChildren(j));
                }else{
                    c2 = this->readNode(r.getChildren(j+1));
                }
                if(c2.getSize() >= MIN_DEGREE){

                    c1.setKey(c1.getSize(), r.getkey(j));
                    c1.setSize(c1.getSize()+1);

                    r.setKey(j,c2.getkey(0));
                    r1.setData(r);
                    typedFile<T,MIN_DEGREE>::writeRecord(r1,index);

                    for (int i = 0 ; i< c2.getSize(); i++){
                        c2.setKey(i,c2.getkey(i+1));
                    }
                    c2.setSize(c2.getSize()-1);
                    r1.setData(c2);
                    typedFile<T,MIN_DEGREE>::writeRecord(r1,r.getChildren(j+1));

                    removeAux(c1,key,r.getChildren(j));
                    return true;
                }else{ //Caso 3b
                         for (int i = 0; i < c1.getSize(); i++){
                            c3.setKey(i,c1.getkey(i));
                         }
                        c3.setSize(c1.getSize());

                        c3.setKey(MIN_DEGREE-1, r.getkey(j));
                        c3.setSize(c3.getSize()+1);


                        for (int i = 0 ; i< c2.getSize(); i++){
                           c3.setKey(i+MIN_DEGREE,c2.getkey(i));
                        }
                        c3.setSize(c3.getSize()+c2.getSize());

                        if(!c1.isleaf()){
                            for (int i = 0 ; i <= c1.getSize(); i++){
                                c3.setChildren(i,c1.getChildren(i));
                            }
                            for (int i = 0 ; i <= c2.getSize(); i++){
                                c3.setChildren(i+MIN_DEGREE,c2.getChildren(i));
                            }
                            c3.setLeaf(false);
                        }

                        for (int i = j; i < r.getSize(); i++)
                        {
                            r.setKey(i,r.getkey(i+1));
                        }

                        if(!r.isleaf()){
                            for (int i = j; i < r.getSize(); i++){
                                r.setChildren(i,r.getChildren(i+1));
                            }
                        }
                        if(r.getSize() == 1){
                            c3.setKey(MIN_DEGREE-1,r.getkey(0));
                        }
                        r.setSize(r.getSize()-1);

                        if(r.getSize() == 0){
                            this->removeAux(c3,key,index);
                        }else{
                            unsigned long long int p = typedFile<T, MIN_DEGREE>::alocateNextPosition();
                            r1.setData(c3);
                            typedFile<T, MIN_DEGREE>::writeRecord(r1,p);

                            this->removeAux(c3,key,p);

                            r.setChildren(j,p);

                            r1.setData(r);
                            typedFile<T, MIN_DEGREE>::writeRecord(r1,index);
                        }
                }
             }
         }

    }
}

//Função padrão de acordo com o material didático
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::insertInNodeNonFull(node<T,MIN_DEGREE>& x,unsigned long long int index,T key, int iX){
    int i = x.getSize() - 1;

    if(x.isleaf()){
        while(i >= 0 && key < x.getkey(i)){
            x.setKey(i + 1, x.getkey(i));
            --i;
        }
        x.setKey(i+1, key);
        x.setSize(x.getSize()+1);
        return this->writeNode(x,index);
    }
    else{
        while(i >= 0 && key < x.getkey(i)){
            i--;
        }
        i++;
        node<T, MIN_DEGREE> n;
        n = this->readNode(x.getChildren(i));
        if(n.getSize() == MAX){
            this->splitChild(x,i,iX);
            if (key > x.getkey(i)){
                i++;
            }
            n = this->readNode(x.getChildren(i));
        }
        this->insertInNodeNonFull(n,x.getChildren(i),key,x.getChildren(i));
        return true;
    }
}

//Função padrão de acordo com o material didático
template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::splitChild(node<T,MIN_DEGREE>& x, int i, int iX){
    node <T, MIN_DEGREE> y,z;
    y = this->readNode(x.getChildren(i));
    z.setLeaf(y.isleaf());
    z.setSize(MIN);

    for (unsigned int j = 0; j < MIN; j++){
        z.setKey(j,y.getkey(j+MIN_DEGREE));
    }

    if(!y.isleaf()){
        for (unsigned int j = 0; j < MIN_DEGREE; j++){
            z.setChildren(j, y.getChildren(j + MIN_DEGREE));
        }
    }

    y.setSize(MIN);

//  for (int j = x.getSize()-1; j > i  ; j--){  //Inserção de 100 a 1
    for (int j = x.getSize(); j > i  ; j--){
        x.setChildren(j+1, x.getChildren(j));
    }

    record<T, MIN_DEGREE> r1;
    r1.setData(z);
    unsigned long long int p = typedFile<T, MIN_DEGREE>::alocateNextPosition();
    typedFile<T, MIN_DEGREE>::writeRecord(r1,p);
    x.setChildren(i+1,p);

//  for (int j = x.getSize()-1; j > i; j--){  //Inserção de 100 a 1
    for (int j = x.getSize()-1; j >= i; j--){
        x.setKey(j+1, x.getkey(j));
    }

    x.setKey(i, y.getkey(MIN_DEGREE-1));
    x.setSize(x.getSize() + 1);

    record<T, MIN_DEGREE> r2;
    r2.setData(y);
    typedFile<T, MIN_DEGREE>::writeRecord(r2,x.getChildren(i));

    record<T, MIN_DEGREE> r3;
    r3.setData(x);
    typedFile<T, MIN_DEGREE>::writeRecord(r3,iX);
}

//Função padrão de acordo com o material didático.
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::searchKey(node <T, MIN_DEGREE> r,T key){
    int i = 0;
    while(i <= r.getSize()-1 && key > r.getkey(i)){
        i++;
    }
    if(i <= r.getSize()-1 &&  key == r.getkey(i)){
        cout << "Nó da chave: ";
        r.printNode();
        cout << ", chave encontrada na posicao "<<i<<endl;
        return true;
    }
    if(r.isleaf()){
        cout << "Chave não encontrada"<<endl;
        return true;
    }
    else{
        r = this->readNode(r.getChildren(i));
        searchKey(r,key);
    }
}

//Função para testes
template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::printTree(node <T, MIN_DEGREE> r){
    queue <node <T, MIN_DEGREE>> print;
    print.push(r);
    int i = 0;
    while(print.front().getSize() > 0){
        print.front().printNode();
        for(int i = 0; i <= print.front().getSize(); i++){
            if (print.front().getChildren(i)!= NOT_FOUND){
                print.push(readNode(print.front().getChildren(i)));
            }
        }
        print.pop();
        i++;
    }
}

//Função necessária para utilizar a árvore B junto com o arquivo tipado
template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE> diskbtree<T, MIN_DEGREE>::readNode(unsigned long long int i){
    record<T, MIN_DEGREE> r;
    typedFile<T, MIN_DEGREE>::readRecord(r, i);
    return r.getData();
}

//Função necessária para utilizar a árvore B junto com o arquivo tipado
template <class T, const unsigned int MIN_DEGREE>
bool diskbtree<T, MIN_DEGREE>::writeNode(node<T,MIN_DEGREE> x, unsigned long long int i){
    record<T, MIN_DEGREE> r(x);
    return typedFile<T, MIN_DEGREE>::writeRecord(r,i);
}

//Função padrão de acordo com o material didático (memTree)
template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::print(){
    int lvl = 0;
    vector<string> levels(1);
    printAux(this->readNode(typedFile<T,MIN_DEGREE>::getFirstValid()), levels, lvl);

    for (string s : levels){
        cout << s << endl;
    }
}

//Função padrão de acordo com o material didático (memTree)
template <class T, const unsigned int MIN_DEGREE>
void diskbtree<T, MIN_DEGREE>::printAux(node<T, MIN_DEGREE> x, vector<string> &v, unsigned int lvl){
    string aux = "[";
    int i = 0;

    if (v.size() < lvl + 1){
        v.resize(lvl + 1);
    }

    if (!x.isleaf()){
        for (i = 0; i <= x.getSize(); i++){
            if (x.getChildren(i) != NOT_FOUND){
                node<T, MIN_DEGREE> n1 = this->readNode(x.getChildren(i));
                printAux(n1, v, lvl + 1);
            }
        }
    }

    for (i = 0; i < x.getSize(); i++){
        if(x.getkey(i).getY() < 0){
            int aux2 = x.getkey(i).getY()*-1;
            if(aux2 < 10 && aux2 >= 0){
                aux += to_string(x.getkey(i).getX()) + ".0"+to_string(aux2)+", ";
            }
            else{
                aux += to_string(x.getkey(i).getX()) + "."+to_string(aux2)+", ";
            }
        }else{
        if(x.getkey(i).getY() < 10 && x.getkey(i).getY() >= 0){
            aux += to_string(x.getkey(i).getX()) + ".0"+to_string(x.getkey(i).getY())+", ";
        }
        else{
            aux += to_string(x.getkey(i).getX()) + "."+to_string(x.getkey(i).getY())+", ";
        }
        }
    }

    if (aux.length() > 1){
        aux += "\b\b] ";
    }
    else{
        aux += "] ";
    }

    v[lvl] += aux;
}

#endif // DISKBTREE_H_INCLUDED
