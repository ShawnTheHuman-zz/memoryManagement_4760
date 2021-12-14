/*
	Shawn Brown
	project 6 - CS4760
	
	bitvector.h

	defines bitvector class
*/
#ifndef BITVECTOR_H
#define BITVECTOR_H


class bitvector
{
	
	private:
		int _size;
		unsigned char* _usageArray;

	public:
	
		bitvector(int size);
		~bitvector();


		void set_bit(int, bool);
		bool get_bit(int);
	


};





#endif
