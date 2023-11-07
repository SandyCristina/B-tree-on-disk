/*
    Trabalho final - Técnicas de programação    2022.2
                Árvore B em disco
    Alunos:
        Luiz Augusto da Silva - 0048504
        Sandy Cristina Barros - 0049777

*/

//Arquivo header.cpp padrão de acordo com o material disponibilizado.

#include "header.h"

header::header() : serializable() {
   type = "***";
   firstDeleted = 0;
   firstValid = 0;
   version = 0;
}

header::header(const string t, unsigned int v) : serializable() {
   type = t.substr(0, 3);
   firstDeleted = 0;
   firstValid = 0;
   version = v;
}

header::header(const header &h) :serializable(){
   type = h.getType();
   firstDeleted = h.getFirstDeleted();
   firstValid = h.getFirstValid();
   version = h.getVersion();
}

header::~header() {}

header header::operator=(const header &h) {
   header aux(h);
   if (this == &h)
      return *this;

   type = h.getType();
   firstDeleted = h.getFirstDeleted();
   firstValid = h.getFirstValid();
   version = h.getVersion();

   return aux;
}

bool header::operator==(const header &h) {
   return type == h.getType() && version == h.getVersion() &&
          firstDeleted == h.getFirstDeleted() &&
          firstValid == h.getFirstValid();
}

unsigned long long int header::getFirstDeleted() const {
   return firstDeleted;
}

void header::setFirstDeleted(unsigned long long int r) {
   firstDeleted = r;
}

unsigned long long int header::getFirstValid() const {
   return firstValid;
}

void header::setFirstValid(unsigned long long int r) {
   firstValid = r;
}

string header::getType() const {
   return type;
}

void header::setType(string t) {
   type = t.substr(0, 3);
}

unsigned int header::getVersion() const {
   return this->version;
}

void header::setVersion(unsigned int v) {
   this->version = v;
}

string header::toString() {
   string aux = "";
   aux += type;
   aux += string(reinterpret_cast<char*>(&version), sizeof(version));
   aux += string(reinterpret_cast<char*>(&firstValid), sizeof(firstValid));
   aux += string(reinterpret_cast<char*>(&firstDeleted), sizeof(firstDeleted));
   return aux;
}

void header::fromString(string repr) {
   int pos = 0;
   type = repr.substr(pos, 3);
   pos += type.length();
   repr.copy(reinterpret_cast<char*>(&version), sizeof(version), pos);
   pos += sizeof(version);
   repr.copy(reinterpret_cast<char*>(&firstValid), sizeof(firstValid), pos);
   pos += sizeof(firstValid);
   repr.copy(reinterpret_cast<char*>(&firstDeleted), sizeof(firstDeleted), pos);
}

unsigned long long int header::size() const {
   return type.length() + sizeof(version) + sizeof(firstValid) + sizeof(firstDeleted);
}
