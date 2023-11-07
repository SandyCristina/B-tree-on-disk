/*
    Trabalho final - Técnicas de programação    2022.2
                Árvore B em disco
    Alunos:
        Luiz Augusto da Silva - 0048504
        Sandy Cristina Barros - 0049777

*/

//Esta classe está praticamente igual ao material didático, mundando somente de T data para node<T, MIN_DEGREE> data
#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "serializable.h"
#include "node.h"

using namespace std;

template <class T, const unsigned int MIN_DEGREE>
class record : public serializable {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:
      record();
      record(node<T, MIN_DEGREE> d);
      record(const record<T, MIN_DEGREE> &other);
      virtual ~record();
      record<T, MIN_DEGREE> operator=(const record<T, MIN_DEGREE> &other);
      bool operator==(const record<T, MIN_DEGREE> &other);
      node<T, MIN_DEGREE> getData() const;
      void setData(node<T, MIN_DEGREE> d);
      unsigned long long int getNext() const;
      void setNext(unsigned long long int n);
      bool isDeleted() const;
      void del();
      void undel();
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toCSV() { return ""; }
      virtual void fromCSV(string repr) {}
      virtual string toJSON() { return ""; }
      virtual void fromJSON(string repr) {}
      virtual string toXML() { return ""; }
      virtual void fromXML(string repr) {}
      virtual unsigned long long int size() const;
   protected:
      node<T, MIN_DEGREE> data;
      bool deleted;
      unsigned long long int next;
};

template <class T, const unsigned int MIN_DEGREE>
record<T, MIN_DEGREE>::record() : serializable() {
    node<T, MIN_DEGREE> d_;
    this->data = d_;
    this->deleted = false;
    this->next = 0;
}

template <class T, const unsigned int MIN_DEGREE>
record<T, MIN_DEGREE>::record(node<T, MIN_DEGREE> d) : serializable() {
    this->data = d;
    this->deleted = false;
    this->next = 0;
}

template <class T, const unsigned int MIN_DEGREE>
record<T, MIN_DEGREE>::record(const record<T, MIN_DEGREE> &other) {
    this->data = other.getData();
    this->deleted = other.isDeleted();
    this->next = other.getNext();
}

template <class T, const unsigned int MIN_DEGREE>
record<T, MIN_DEGREE>::~record() {}

template <class T, const unsigned int MIN_DEGREE>
record<T, MIN_DEGREE> record<T, MIN_DEGREE>::operator=(const record<T, MIN_DEGREE> &other) {
    record<T, MIN_DEGREE> aux(other);
    this->data = other.getData();
    this->deleted = other.isDeleted();
    this->next = other.getNext();
    return aux;
}

template <class T, const unsigned int MIN_DEGREE>
bool record<T, MIN_DEGREE>::operator==(const record<T, MIN_DEGREE> &other) {
    return ((this->data == other.getData()) && (this->deleted == other.isDeleted()) && (this->next == other.getNext()));
}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE> record<T, MIN_DEGREE>::getData() const {
    return this->data;
}

template <class T, const unsigned int MIN_DEGREE>
void record<T, MIN_DEGREE>::setData(node<T, MIN_DEGREE> d) {
    this->data = d;
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int record<T, MIN_DEGREE>::getNext() const {
    return this->next;
}

template <class T, const unsigned int MIN_DEGREE>
void record<T, MIN_DEGREE>::setNext(unsigned long long int n) {
    this->next = n;
}

template <class T, const unsigned int MIN_DEGREE>
bool record<T, MIN_DEGREE>::isDeleted() const {
    return this->deleted;
}

template <class T, const unsigned int MIN_DEGREE>
void record<T, MIN_DEGREE>::del() {
    this->deleted = true;
}

template <class T, const unsigned int MIN_DEGREE>
void record<T, MIN_DEGREE>::undel() {
    this->deleted = false;
}

template <class T, const unsigned int MIN_DEGREE>
string record<T, MIN_DEGREE>::toString() {
    string repr = "";
    repr += this->data.toString();
    repr += string(reinterpret_cast<char*>(&deleted), sizeof(deleted));
    repr += string(reinterpret_cast<char*>(&next), sizeof(next));
    return repr;
}

template <class T, const unsigned int MIN_DEGREE>
void record<T, MIN_DEGREE>::fromString(string repr) {
    int pos = 0;
    string data_string = repr.substr(pos,data.size());
    this->data.fromString(data_string);
    pos += this->data.size();
    repr.copy(reinterpret_cast<char*>(&this->deleted), sizeof(this->deleted), pos);
    pos += sizeof(this->deleted);
    repr.copy(reinterpret_cast<char*>(&this->next), sizeof(this->next), pos);
    pos += sizeof(this->next);
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int record<T, MIN_DEGREE>::size() const {
    return ((this->data.size())+(sizeof(this->deleted))+(sizeof(this->next)));
}

#endif // RECORD_H
