/*#include "hashtab.h"

class func
{
public:
	inline unsigned int operator ()(const int p) const { return p;}

};

void test()
{
	abase::hashtab<int,int,func> ht(10,func());
	ht.put(10,10);
	ht.put(10,10);
	ht.get(10);
	ht.resize(100);
	ht.clear();
	ht.erase(10);
}

*/