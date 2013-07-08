
#include <map>
#include <string>
using namespace std;

#include "Argument.h"
#include "SetArguments.h"

#ifndef LABELLING_H_
#define LABELLING_H_

typedef string Label;

class Labelling
{
	map<Argument *, Label> labelling;
	SetArguments in;
	SetArguments out;
	SetArguments undec;

public:
	const static Label lab_in;
	const static Label lab_out;
	const static Label lab_undec;
	Labelling();
	virtual ~Labelling();
	void add_label(Argument *, Label);
	bool exists_argument_labelling(Argument *);
	Label get_label(Argument *);
	SetArguments *inargs();
	SetArguments *outargs();
	SetArguments *undecargs();
	bool empty();
	void clone(Labelling *);
};

#endif /* LABELLING_H_ */
