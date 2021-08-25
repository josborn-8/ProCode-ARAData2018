
using namespace std;

#include<TTree.h>
#include<TMath.h>



TTree *sortTreeEntries(TTree *testTree)
{
	//testTree->Scan();

	Int_t nentries = (Int_t)testTree->GetEntries();
	
	testTree->Draw("qualityParameter","","goff");
	Int_t *index = new Int_t[nentries];
	
	TMath::Sort(nentries,testTree->GetV1(),index);
						
	TTree *tsorted = (TTree*)testTree->CloneTree(0);
		for (Int_t i=0;i<nentries;i++) {
		testTree->GetEntry(index[i]);
		tsorted->Fill();
	}

//	tsorted->Print();
//	tsorted->Scan();

	delete [] index;
	return tsorted;
}
