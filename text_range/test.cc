#include "TextRange.h"

int main() {
	cout<<"---------- Query -------"<<endl;
	string r="Aaab";
	TextRange rng("[AaA-BaB)");
	cout<<"Range: "<<endl;
	rng.Print();
	if(rng.InRange(r))
		cout<<r<<" is in range."<<endl;
	else
		cout<<r<<" is NOT in range."<<endl;

	cout<<"--------- Addition --------"<<endl;
	TextRange rng1("[AaA-BaB]");
	TextRange rng2("[Aac-CaC]");
	TextRange rng3=rng1+rng2;
	cout<<rng1.ToString()<<" + "<<rng2.ToString()<<" = "<<rng3.ToString()<<endl;
	TextRange rng22("(Dd-Df]");
	TextRange rng33(rng3);
	rng33+=rng22;
	cout<<rng3.ToString()<<" + "<<rng22.ToString()<<" = "<<rng33.ToString()<<endl;

	cout<<"-------- Substraction---------"<<endl;
	TextRange rng4("[Aaab-BaB]");
	TextRange rng5=rng3-rng4;
	cout<<rng3.ToString()<<" - "<<rng4.ToString()<<" = "<<rng5.ToString()<<endl;
}
