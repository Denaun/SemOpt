#ifndef LABELLING_H_
#define LABELLING_H_

#include <map>
#include <string>
using namespace std;

class Argument;
class SetArguments;
#include "SetArguments.h"

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

#include "Argument.h"
#include "SetArguments.h"

#endif /* LABELLING_H_ */
