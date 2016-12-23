#ifndef _TYPES_H
#define _TYPES_H

#include <complex>
#include <vector>
#include<iostream>
#include <math.h>

using namespace std;

typedef vector<complex <double> > ComplexVector;

/* template <class T> class ComplexVector */
/* { */
/*   private: */
/*     vector<complex<T> > localV; */
/*   public: */
/*     ComplexVector(); */
/*     ComplexVector(vector<T>&); */
/*     vector<T> get_real(); */
/*     vector<T> get_complex(); */
/*     vector<T> get_abs(); */
/*     vector<T> get_phase(); */
/*     void push_back(complex<T>); */
/*     complex<T> at(int); */
/*     void set(int,complex<T>); */
/*     int get_size(); */
/*     void reserve(int); */
/* }; */

// Global variable
const complex<double> i(0, 1);
# define PI 3.14159265358979323846
# define DEBUG(X) cout << X << "\n"

class SpeechBuffer
{
private:
	vector<vector <double> > bufferMatrix;
	int bufferLength; //# of coloumns
	int overlap;
	int length;//# of buffered rows
	int currentRow;
	int nextOut;
public:
	SpeechBuffer(int rows, int bufferLength, int overlap);//samples
	void initializeBuffer(vector<double>);
	vector<double> get_next();
	void add_to_end(vector<double>);
	int getLength();
};


// Define here as templates are tricky to compile when shared

/* template <class T> */
/* ComplexVector<T>::ComplexVector() */
/* {} */

/* template <class T> */
/* ComplexVector<T>::ComplexVector(vector<T>& data_in) */
/* { */
/*     this->localV.resize(2*data_in.capacity()); */
/*     DEBUG(data_in.capacity()); */
/*     DEBUG(this->localV.capacity()); */
/*     DEBUG(this->localV.size()); */
/*     for(int index=0; index<data_in.capacity(); index ++) */
/*     { */
/*         this->localV[index] = complex<T>(data_in.at(index),0); */
/*     } */
/* } */


/* template <class T> */
/* vector<T> ComplexVector<T>::get_real() */
/* { */
/*     vector<T> real_items; */
/*     for(int index=0; index<this->localV.size()/2; index ++) */
/*     { */
/*         real_items.push_back(real(this->localV.at(index))); */
/*     } */
/*     return(real_items); */
/* } */

/* template <class T> */
/* vector<T> ComplexVector<T>::get_complex() */
/* { */
/*     vector<T> complex_items; */
/*     for(int index=1; index<this->localV.size()/2; index ++) */
/*     { */
/*         complex_items.push_back(imag(this->localV.at(index))); */
/*     } */
/*     return(complex_items); */
/* } */

/* template <class T> */
/* vector<T> ComplexVector<T>::get_abs() */
/* { */
/*     vector<T> abs_items; */
/*     for(int index=1; index<this->localV.size()/2; index ++) */
/*     { */
/*         abs_items.push_back(abs(this->localV.at(index))); */
/*     } */
/*     return(abs_items); */
/* } */

/* template <class T> */
/* vector<T> ComplexVector<T>::get_phase() */
/* { */
/*     vector<T> phase_items; */
/*     for(int index=1; index<this->localV.size()/2; index ++) */
/*     { */
/*         phase_items.push_back(arg(this->localV.at(index))); */
/*     } */
/*     return(phase_items); */
/* } */

/* template <class T> */
/* void ComplexVector<T>::push_back(complex<T> entry) */
/* { */
/*     DEBUG("Front and Back"); */
/*     this->localV.push_back(entry); */
/*     DEBUG(this->localV.front()); */
/*     DEBUG(this->localV.back()); */
/*     DEBUG("Size"); */
/*     DEBUG(this->localV.capacity()); */
/*     DEBUG(this->localV.size()); */
/* }  */

/* template <class T> */
/* complex<T> ComplexVector<T>::at(int index) */
/* { */
/*     return(this->localV.at(index)); */
/* } */

/* template <class T> */
/* void ComplexVector<T>::set(int index,complex<T> val) */
/* { */
/*     this->localV.at(index) = val; */
/* } */

/* template <class T> */
/* int ComplexVector<T>::get_size() */
/* { */
/*     return(this->localV.capacity()); */
/* }  */

/* template <class T> */
/* void ComplexVector<T>::reserve(int Size) */
/* { */
/*     this->localV.resize(Size); */
/*     DEBUG(this->localV.capacity()); */
/* } */

#endif
