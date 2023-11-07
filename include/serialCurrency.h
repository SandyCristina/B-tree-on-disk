/*
    Trabalho final - Técnicas de programação    2022.2
                Árvore B em disco
    Alunos:
        Luiz Augusto da Silva - 0048504
        Sandy Cristina Barros - 0049777

*/

#ifndef SERIALCURRENCY_H
#define SERIALCURRENCY_H
#include <iostream>

#include "serializable.h"

using namespace std;

class serialCurrency : public serializable {
    friend ostream &operator << (ostream &output, const serialCurrency &s){
        if(s.getY() < 0){
            int aux = s.getY()*(-1);
            if(aux < 10){
                output <<s.getX()<<".0"<<aux;
                return output;
            }
            else{
                output <<s.getX()<<"."<<aux;
                return output;
            }
        }else{
            if(s.getY() < 10){
                output <<s.getX()<<".0"<<s.getY();
                return output;
            }
            else{
                output <<s.getX()<<"."<<s.getY();
                return output;
            }
        }
    }
    friend istream &operator >> (istream &input,serialCurrency &s){
        double w;
        input >> w;
        s.setValue(w);
        return input;
    }
   public:
      serialCurrency();
      serialCurrency(double n);
      serialCurrency(const serialCurrency &other);
      virtual ~serialCurrency();
      serialCurrency operator=(const serialCurrency &other);
      bool operator==(const serialCurrency &other) const;
      bool operator<(const serialCurrency &other) const;
      bool operator<=(const serialCurrency &other) const;
      bool operator>(const serialCurrency &other) const;
      bool operator>=(const serialCurrency &other) const;
      bool operator!=(const serialCurrency &other) const;
      serialCurrency operator+(const serialCurrency &other) const;
      serialCurrency operator-(const serialCurrency &other) const;
      serialCurrency operator*(const serialCurrency &other) const;
      serialCurrency operator/(const serialCurrency &other) const;
      serialCurrency operator++();
      serialCurrency operator--();
      void setX(int x);
      int getX() const;
      void setY(int y);
      int getY() const;
      void setValue(double n);
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toXML(){return "";}
      virtual void fromXML(string repr){}
      virtual string toCSV(){return "";}
      virtual void fromCSV(string repr){}
      virtual string toJSON(){return "";}
      virtual void fromJSON(string repr){}
      virtual unsigned long long int size() const;
   protected:
      int x;
      int y;
};

#endif // SERIALCURRENCY_H
