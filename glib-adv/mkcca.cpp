#include "mkcca.h"


void TMKCCASemSpace::ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN,const double& Kapa, TFltV& CGpom1, TFltV& CGr, TFltV& CGp, TFltV& CGa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z,
        const int& CGMxIter, const double& RelErr, const TFltV& x0) {

    // prepare start vector
    x.Gen(Matrix.GetCols());
    if (x0.Empty()) { x.PutAll(0.0); }
    else { x = x0; }
	
	double rho, rho_new, alpha, pom3,resid;
	int niter = 0;
    
	CenterMultiply2(Matrix, x,CGpom1, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);


	TLinAlg::AddVec(-1.0, CGpom1, b, CGr);
    
	CGp = CGr;
	
	while (niter < CGMxIter){
	    rho = TLinAlg::DotProduct(CGr, CGr);

	    CenterMultiply2(Matrix, CGp,CGa, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);

        pom3 = (TLinAlg::DotProduct(CGa, CGp));
		alpha = rho/pom3;
	   
	   
	    TLinAlg::AddVec(alpha, CGp, x, x);
        TLinAlg::AddVec(-alpha, CGa, CGr, CGr);
       
        rho_new = TLinAlg::DotProduct(CGr, CGr);
        TLinAlg::AddVec(rho_new/rho, CGp, CGr,CGp);
		
		rho = rho_new;
        
		////residuum
		//CenterMultiply2(Matrix, x,CGpom1, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);
  //      TLinAlg::AddVec(-1.0, b, CGpom1, CGpom1);
		//
		//double r1 = TLinAlg::Norm(b);
		//double r0 = TLinAlg::Norm(CGpom1);
		//
		//resid = r0 / r1;

	    niter++;
	}

}

void TMKCCASemSpace::ConjugGrad2(const TMatrix& Matrix, const TFltV& b, TFltV& x, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN,const double& Kapa, TFltV& CGpom1, TFltV& CGr, TFltV& CGp, TFltV& CGa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z,
        const int& CGMxIter, const double& RelErr, const TFltV& x0) {

    // prepare start vector
    x.Gen(Matrix.GetCols());
    if (x0.Empty()) { x.PutAll(0.0); }
    else { x = x0; }
	
	double rho, rho_new, alpha, pom3,resid;
	int niter = 0;
    
	CenterMultiply3(Matrix, x,CGpom1, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);
	TLinAlg::AddVec(-1.0, CGpom1, b, CGr);
	CGp = CGr;
	while (niter < CGMxIter){
	    rho = TLinAlg::DotProduct(CGr, CGr);
	    CenterMultiply3(Matrix, CGp,CGa, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);
        pom3 = (TLinAlg::DotProduct(CGa, CGp));
		alpha = rho/pom3;
	    TLinAlg::AddVec(alpha, CGp, x, x);
        TLinAlg::AddVec(-alpha, CGa, CGr, CGr);
        rho_new = TLinAlg::DotProduct(CGr, CGr);
        TLinAlg::AddVec(rho_new/rho, CGp, CGr,CGp);
		rho = rho_new;
		////residuum
		//CenterMultiply3(Matrix, x,CGpom1, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);
  //      TLinAlg::AddVec(-1.0, b, CGpom1, CGpom1);
		//
		//double r1 = TLinAlg::Norm(b);
		//double r0 = TLinAlg::Norm(CGpom1);
		//
		//resid = r0 / r1;
	    niter++;
	}

}


void TMKCCASemSpace::CenterMultiply2(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN, const double& Kapa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z){
         
	    //TFltV z(Matrix.GetCols());
        
		//CenterMultiply(Matrix, x,z, Vekt, Koef, Ones,LangN, CMpom, CMpom2, CMy1);
	 //   CenterMultiply(Matrix, z,y, Vekt, Koef, Ones,LangN, CMpom, CMpom2, CMy1);
		//TLinAlg::LinComb((1-Kapa)*(1-Kapa) , y, 2*Kapa*(1-Kapa), z, y);
		//TLinAlg::AddVec(Kapa*Kapa, x, y, y);

   	CenterMultiply(Matrix, x,CM2z, Vekt, Koef, Ones,LangN, CMpom, CMpom2, CMy1);
    CenterMultiply(Matrix, CM2z,y, Vekt, Koef, Ones,LangN, CMpom, CMpom2, CMy1);
	TLinAlg::LinComb((1-Kapa)*(1-Kapa) , y, 2*Kapa*(1-Kapa), CM2z, y);
	TLinAlg::AddVec(Kapa*Kapa, x, y, y);



}


void TMKCCASemSpace::CenterMultiply3(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN, const double& Kapa, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z){
         
   	CenterMultiply(Matrix, x,CM2z, Vekt, Koef, Ones,LangN, CMpom, CMpom2, CMy1);
   	TLinAlg::LinComb((1-Kapa), CM2z, Kapa, x, y);
}

void TMKCCASemSpace::CenterMultiply(const TMatrix& Matrix, const TFltV& x,TFltV& y, const TFltVV& Vekt, const TFltV& Koef, const TFltV& Ones, const int& LangN, TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1){
 
	//int Docs = Matrix.GetCols();

	//TFltV pom(Matrix.GetRows());
	//TFltV y1(Matrix.GetCols());
	//
	//Matrix.Multiply(x, pom); 
	//Matrix.MultiplyT(pom, y1); 
 //   

	//double y2 = TLinAlg::SumVec(y1) /Docs;
	//double y3 = TLinAlg::SumVec(x);
 //   
	//TFltV pom2(Matrix.GetCols());
	//Vekt.GetCol(LangN, pom2);

	//TLinAlg::AddVec(-y3, pom2, y1, y1);
	//TLinAlg::AddVec(Docs*(Koef[LangN] * y3 - y2), Ones, y1, y );

	int Docs = Matrix.GetCols();

	//TFltV pom(Matrix.GetRows());
	//TFltV y1(Matrix.GetCols());
	
	Matrix.Multiply(x, CMpom[LangN]); 
	Matrix.MultiplyT(CMpom[LangN], CMy1); 
    

	double y2 = TLinAlg::SumVec(CMy1) /Docs;
	double y3 = TLinAlg::SumVec(x);
    
	//TFltV pom2(Matrix.GetCols());
	Vekt.GetCol(LangN, CMpom2);

	TLinAlg::AddVec(-y3, CMpom2, CMy1, CMy1);
	TLinAlg::AddVec(Docs*(Koef[LangN] * y3 - y2), Ones, CMy1, y );
}

void TMKCCASemSpace::CalcKCCA(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TVec<TFltV> >& AlphaVV) {

    // some basic constants
    const int Langs = BowMatrixV.Len();
    IAssert(Langs > 1);
    const int Docs = BowMatrixV[0].GetCols();
    // reservate space on the output
    AlphaVV.Gen(Langs);
    for (int LangN = 0; LangN < Langs; LangN++) {
        IAssert(BowMatrixV[LangN].GetCols() == Docs);
        AlphaVV[LangN].Gen(Dims);
		for (int DimN = 0 ; DimN < Dims; DimN++){
             AlphaVV[LangN][DimN].Gen(Docs);
			 TLAMisc::FillRnd(AlphaVV[LangN][DimN], TRnd(0));
			 //AlphaVV[LangN][DimN].PutAll(0.0);
		}        
    }
    
	TFltVV Vekt(Docs, Langs);
	TFltV Koef(Langs);
	TFltV Ones(Docs);
	Ones.PutAll(1.0 / Docs);
    for (int LangN = 0; LangN < Langs; LangN++) {
		TFltV pom(BowMatrixV[LangN].GetRows());	    
		TFltV pom2(Docs);
		BowMatrixV[LangN].Multiply(Ones, pom); 
	    BowMatrixV[LangN].MultiplyT(pom, pom2); 
		for (int DocN = 0; DocN < Docs; DocN++){
		    Vekt(DocN, LangN) = pom2[DocN];             //kopiranje
		}				
	}
	TLinAlg::MultiplyT(Vekt, Ones, Koef);


	//CG variables
	TFltV CGpom1(Docs);
	TFltV CGr(Docs);
	TFltV CGp(Docs);
	TFltV CGa(Docs);
 	
	//CM
	TVec<TFltV> CMpom;
	CMpom.Gen(Langs);
	for (int LangN = 0; LangN< Langs; ++LangN){
	    CMpom[LangN].Gen(BowMatrixV[LangN].GetRows());
	}	
	TFltV CMpom2(Docs);
	TFltV CMy1(Docs);
	
	//CM2
	TFltV CM2z(Docs);

	//TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z
	//CMpom, CMpom2, CMy1, CM2z


	
	TFltV bb(Docs);
	TFltV pombb(Docs);
	TFltV pombb2(Docs);
	TFltV x(Docs);

    printf("Start loop\n");
	printf("%d Docs \n", Docs);
	// MAIN LOOP
	for (int DimN = 0; DimN < Dims; DimN++){
		printf("%d \n", DimN);
		for (int HorstIterN = 0; HorstIterN < HorstMxIter; ++HorstIterN){

			pombb.PutAll(0.0);
			int ind = HorstIterN % Langs;
			for (int LangN = 0; LangN < Langs; LangN++){
				if (ind == LangN){continue;}
			    //b(:, per(j)) = A(alpha{per(j)}(:, l)) ;
				CenterMultiply(BowMatrixV[LangN], AlphaVV[LangN][DimN], bb,Vekt, Koef, Ones, LangN, CMpom, CMpom2, CMy1);
				TLinAlg::AddVec(1.0, bb, pombb, pombb);
			}
			CenterMultiply(BowMatrixV[ind], pombb, bb,Vekt, Koef, Ones, ind, CMpom, CMpom2, CMy1);
            ConjugGrad(BowMatrixV[ind], bb, x, Vekt, Koef, Ones, ind ,Kapa, CGpom1, CGr, CGp, CGa, CMpom, CMpom2, CMy1, CM2z , CGMxIter);
			//odstej stare alfe
			//alpha{ per(1)}(:, 1:l-1) * (alpha{ per(1)}(:, 1:l-1)' *  A(A(f)))
			
			if (DimN > 0){
				CenterMultiply(BowMatrixV[ind], x, pombb,Vekt, Koef, Ones, ind, CMpom, CMpom2, CMy1);

				CenterMultiply(BowMatrixV[ind], pombb, pombb2,Vekt, Koef, Ones, ind, CMpom, CMpom2, CMy1);
	             			
				
				//MNOZENJE
				
				//printf(" heja %d ", ind);
				TFltV pom5(DimN);
				
				for (int i = 0; i < DimN; i++){
					pom5[i] = TLinAlg::DotProduct(pombb2, AlphaVV[ind][i]);
				}
				pombb.PutAll(0.0);
				for (int i = 0; i < DimN; i++){
					TLinAlg::AddVec(pom5[i], AlphaVV[ind][i], pombb, pombb); 
				}
				TLinAlg::AddVec(-1.0, pombb, x, AlphaVV[ind][DimN]);
			}
			
			else{
                AlphaVV[ind][DimN] = x;
			}

			//normiraj
			CenterMultiply(BowMatrixV[ind], AlphaVV[ind][DimN], x,Vekt, Koef, Ones, ind, CMpom, CMpom2, CMy1);
			double norm = TLinAlg::Norm(x);
			TLinAlg::MultiplyScalar(1.0/norm, AlphaVV[ind][DimN], AlphaVV[ind][DimN]);
			
			//////centermulti alfa[ind][0]   *  centermulti alfa[ind][1]
   //         TFltV T1(Docs);
			//TFltV T2(Docs);
   //         CenterMultiply(BowMatrixV[ind], AlphaVV[ind][0], T1,Vekt, Koef, Ones, ind);
			//CenterMultiply(BowMatrixV[ind], AlphaVV[ind][1], T2,Vekt, Koef, Ones, ind);
			//printf(" Hjec : %f ", TLinAlg::DotProduct(T1,T2));

		}

  //      //printf(" D %f D", Koef[0]);
		////printf(" D %f D", Koef[1]);
		////TLAMisc::SaveMatlabTFltVV(Vekt,  "vekt.dat");
		////TLAMisc::SaveMatlabTFltV(Koef, "koef.dat");
        
		
		//cov
		double cov = 0;
		for (int ii = 0; ii < Langs; ii++)
		{
			TFltV tX(Docs);
			CenterMultiply(BowMatrixV[ii], AlphaVV[ii][DimN], tX,Vekt, Koef, Ones, ii, CMpom, CMpom2, CMy1);
			for (int jj = ii+1; jj < Langs; ++jj)
			{
				TFltV tY(Docs);		
			    CenterMultiply(BowMatrixV[jj], AlphaVV[jj][DimN], tY,Vekt, Koef, Ones, jj, CMpom, CMpom2, CMy1);
			    cov = cov + (TLinAlg::DotProduct(tX,tY));
			}
			

		}
		cov = 2* cov/(Langs * Langs - Langs);
		printf("Cov %f \n", cov);

	}

	//for(int i=0; i < Docs; ++i){
	//	
	//	TStr str = "alpha";
	//    
	//	TStr str2 = TInt::GetStr(i);
 //       str += str2;
	//	str += ".dat";
 //       TLAMisc::SaveMatlabTFltVV(AlphaVV[i],  str);
	//}

	//TLAMisc::SaveMatlabTFltV(AlphaVV[0][0],  "s1.dat");
 //   TLAMisc::SaveMatlabTFltV(AlphaVV[1][0],  "s2.dat");
 //   TLAMisc::SaveMatlabTFltV(AlphaVV[0][1],  "s3.dat");
 //   TLAMisc::SaveMatlabTFltV(AlphaVV[1][1],  "s4.dat");
	////TLAMisc::SaveMatlabTFltVV(jedro, "sranje.dat");


	//TFltV pom3(BowMatrixV[0].GetCols());
	//TFltV pom4(BowMatrixV[0].GetRows());
	//pom3.PutAll(0.0);
	//pom3[0] = 1.0;
	//BowMatrixV[0].Multiply(pom3, pom4);
	//TLAMisc::SaveMatlabTFltV(pom4, "sranje.dat");
	//printf(" %f ", TLinAlg::Norm(pom4));


 //   // par vektorjev
 //   TFltV x(Docs);  // vektor dolzine docs
 //   TLAMisc::FillRnd(x, TRnd(1)); // napolnemo z random elementi
 //   TLinAlg::Normalize(x); // normiramo vektor
 //   TFltV y(BowMatrixV[1].GetRows()); // nek vektor
 //   // spreminanje elemnta v matriki
 //   //AlphaVV[1](5,4) = 4.43;
 //   //// skalarni produkt 5 stolpca matrike alphaVV[1] z vektorjem x
 //   //TLinAlg::DotProduct(AlphaVV[1], 5, x);
 //   //// pristeje 0.4 * prvi stolpec drugemu stoplcu
 //   //TLinAlg::AddVec(0.4, AlphaVV[1], 0, AlphaVV[1], 1);
 //   //// pristeje 0.4 * prvi stoplec vektorju x
 //   //TLinAlg::AddVec(0.4, AlphaVV[1], 0, x);
 //   //// naracunamo normo petega stoplca
 //   //double Norm = TLinAlg::Norm(AlphaVV[1], 5);
 //   //// preberemo cetrto vrstico matrike
 //   //TFltV z; AlphaVV[1].GetRow(4, z);
 //   //// preberemo cetrti stoplec matrike
 //   //TFltV zz; AlphaVV[1].GetCol(4, zz);
 //   //// mnozimo vektor x z matriko BowMatrixV[1]
 //   //BowMatrixV[1].Multiply(x, y);
 //   //BowMatrixV[1].MultiplyT(y, x);
 //   //BowMatrixV[1].Multiply(AlphaVV[1], 0, y);
	//printf("%d \n", BowMatrixV[1].GetCols());
 //   printf("%d \n", BowMatrixV[1].GetRows());
 //   
	//TFltV b(Docs);  // vektor dolzine docs
 //   TLAMisc::FillRnd(b, TRnd(1)); // napolnemo z random elementi
 //   

	//CenterMultiply2(BowMatrixV[0], Ones,x, Vekt, Koef, Ones,0, 0.5);

	//printf(" %f ", TLinAlg::Norm(x));

	////ConjugGrad(BowMatrixV[1], b, x, Vekt, Koef, Ones, 1,Kapa , 20);

	//printf("Konji \n");

}


void TMKCCASemSpace::CalcKCCA2(const TVec<TBowMatrix>& BowMatrixV, const int& Dims, 
        const double& Kapa, const int& CGMxIter, const int& HorstMxIter, 
        TVec<TVec<TFltV> >& AlphaVV) {
    // some basic constants
    const int Langs = BowMatrixV.Len();
    IAssert(Langs > 1);
    const int Docs = BowMatrixV[0].GetCols();
    // reservate space on the output
    AlphaVV.Gen(Langs);
    for (int LangN = 0; LangN < Langs; LangN++) {
        IAssert(BowMatrixV[LangN].GetCols() == Docs);
        AlphaVV[LangN].Gen(Dims);
		for (int DimN = 0 ; DimN < Dims; DimN++){
             AlphaVV[LangN][DimN].Gen(Docs);
			 TLAMisc::FillRnd(AlphaVV[LangN][DimN], TRnd(0));
			 //AlphaVV[LangN][DimN].PutAll(0.0);
		}        
    }
    
	TFltVV Vekt(Docs, Langs);
	TFltV Koef(Langs);
	TFltV Ones(Docs);
	Ones.PutAll(1.0 / Docs);
    for (int LangN = 0; LangN < Langs; LangN++) {
		TFltV pom(BowMatrixV[LangN].GetRows());	    
		TFltV pom2(Docs);
		BowMatrixV[LangN].Multiply(Ones, pom); 
	    BowMatrixV[LangN].MultiplyT(pom, pom2); 
		for (int DocN = 0; DocN < Docs; DocN++){
		    Vekt(DocN, LangN) = pom2[DocN];             //kopiranje
		}				
	}
	TLinAlg::MultiplyT(Vekt, Ones, Koef);


	//CG variables
	TFltV CGpom1(Docs);
	TFltV CGr(Docs);
	TFltV CGp(Docs);
	TFltV CGa(Docs);
 	
	//CM
	TVec<TFltV> CMpom;
	CMpom.Gen(Langs);
	for (int LangN = 0; LangN< Langs; ++LangN){
	    CMpom[LangN].Gen(BowMatrixV[LangN].GetRows());
	}	
	TFltV CMpom2(Docs);
	TFltV CMy1(Docs);
	
	//CM2
	TFltV CM2z(Docs);

	//TVec<TFltV>& CMpom, TFltV& CMpom2, TFltV& CMy1, TFltV& CM2z
	//CMpom, CMpom2, CMy1, CM2z


	TVec<TFltV> u;
	TVec<TFltV> u2p;
	u.Gen(Langs);
	u2p.Gen(Langs);
    for (int LangN = 0; LangN < Langs; LangN++) {
		u[LangN].Gen(Docs);
		u2p[LangN].Gen(Docs);
	}

	TFltV bb(Docs);
	TFltV pombb(Docs);
	TFltV pombb2(Docs);
	TFltV x(Docs);

	TFltV h(Docs);
	TFltV h2(Docs);
	TFltV h3(Docs);

    printf("Start loop\n");
	printf("%d Docs \n", Docs);
	// MAIN LOOP
	for (int DimN = 0; DimN < Dims; DimN++){
		printf("%d \n", DimN);
		
		if (DimN > 0){
			for (int LangN = 0; LangN < Langs; ++LangN){     


				CenterMultiply3(BowMatrixV[LangN], AlphaVV[LangN][DimN], pombb,Vekt, Koef, Ones, LangN, Kapa,CMpom, CMpom2, CMy1, CM2z);

	            
				TFltV pom5(DimN);
				
				for (int i = 0; i < DimN; i++){
					pom5[i] = TLinAlg::DotProduct(pombb, AlphaVV[LangN][i]);
				}
				pombb.PutAll(0.0);
				for (int i = 0; i < DimN; i++){
					TLinAlg::AddVec(pom5[i], AlphaVV[LangN][i], pombb, pombb); 
				}
				

				CenterMultiply3(BowMatrixV[LangN], pombb,  x ,Vekt, Koef, Ones, LangN,Kapa, CMpom, CMpom2, CMy1, CM2z);
	            
				TLinAlg::AddVec(-1.0, x, AlphaVV[LangN][DimN], AlphaVV[LangN][DimN]);
			}
		}

		for (int LangN = 0; LangN < Langs; ++LangN){
			ConjugGrad2(BowMatrixV[LangN], AlphaVV[LangN][DimN], u[LangN], Vekt, Koef, Ones, LangN ,Kapa, CGpom1, CGr, CGp, CGa, CMpom, CMpom2, CMy1, CM2z , CGMxIter);

			CenterMultiply(BowMatrixV[LangN], u[LangN], u[LangN],Vekt, Koef, Ones, LangN, CMpom, CMpom2, CMy1);

		}
		
		for (int HorstIterN = 0; HorstIterN < HorstMxIter; ++HorstIterN){

			for (int LangN = 0; LangN < Langs; ++LangN){
				//h
				pombb.PutAll(0.0);
				for (int i = 0; i < Langs; i++){
					if (i != LangN){
						TLinAlg::AddVec(1, u[i], pombb, pombb); 
					}
				}
			    CenterMultiply(BowMatrixV[LangN], pombb, pombb,Vekt, Koef, Ones, LangN, CMpom, CMpom2, CMy1);
				ConjugGrad2(BowMatrixV[LangN], pombb, h, Vekt, Koef, Ones, LangN ,Kapa, CGpom1, CGr, CGp, CGa, CMpom, CMpom2, CMy1, CM2z , CGMxIter);

				//h2
				h2.PutAll(0.0);
				if (DimN > 0){
					CenterMultiply3(BowMatrixV[LangN], h, pombb,Vekt, Koef, Ones, LangN, Kapa,CMpom, CMpom2, CMy1, CM2z);
					TFltV pom5(DimN);
					for (int i = 0; i < DimN; i++){
						pom5[i] = TLinAlg::DotProduct(pombb, AlphaVV[LangN][i]);
					}
					pombb.PutAll(0.0);
					for (int i = 0; i < DimN; i++){
						TLinAlg::AddVec(pom5[i], AlphaVV[LangN][i], pombb, pombb); 
					}
					CenterMultiply3(BowMatrixV[LangN], pombb,  h2 ,Vekt, Koef, Ones, LangN,Kapa, CMpom, CMpom2, CMy1, CM2z);
				}

                //h3
				TLinAlg::MultiplyScalar(1/((1- Kapa)*(1- Kapa)) + 0.000001 , AlphaVV[LangN][DimN], h3);


				TLinAlg::AddVec(-1, h2, h, AlphaVV[LangN][DimN]);
				
				TLinAlg::AddVec(1, h3, AlphaVV[LangN][DimN], AlphaVV[LangN][DimN]);

				double norm = TLinAlg::Norm(AlphaVV[LangN][DimN]);
				TLinAlg::MultiplyScalar(1.0/norm, AlphaVV[LangN][DimN], AlphaVV[LangN][DimN]);
				ConjugGrad2(BowMatrixV[LangN], AlphaVV[LangN][DimN], u2p[LangN], Vekt, Koef, Ones, LangN ,Kapa, CGpom1, CGr, CGp, CGa, CMpom, CMpom2, CMy1, CM2z , CGMxIter);
				

			}
			for (int LangN = 0; LangN < Langs; ++LangN){
                if (HorstIterN < HorstMxIter - 1){
				     CenterMultiply(BowMatrixV[LangN], u2p[LangN], u[LangN],Vekt, Koef, Ones, LangN, CMpom, CMpom2, CMy1);
				}
			}
			
		}
		for (int LangN = 0; LangN < Langs; ++LangN){
             AlphaVV[LangN][DimN] = u2p[LangN];
		}
		
		//cov
		double cov = 0;
		for (int ii = 0; ii < Langs; ii++)
		{
			TFltV tX(Docs);
			CenterMultiply(BowMatrixV[ii], AlphaVV[ii][DimN], tX,Vekt, Koef, Ones, ii, CMpom, CMpom2, CMy1);
			double norm = TLinAlg::Norm(tX);
		    TLinAlg::MultiplyScalar(1.0/norm, tX, tX);
			for (int jj = ii+1; jj < Langs; ++jj)
			{
				TFltV tY(Docs);		
			    CenterMultiply(BowMatrixV[jj], AlphaVV[jj][DimN], tY,Vekt, Koef, Ones, jj, CMpom, CMpom2, CMy1);
			    double norm2 = TLinAlg::Norm(tY);
				TLinAlg::MultiplyScalar(1.0/norm, tY, tY);
				cov = cov + (TLinAlg::DotProduct(tX,tY));
			}
			

		}
		cov = 2* cov/(Langs * Langs - Langs);
		printf("Cov %f \n", cov);

	}
   //         TFltV T1(Docs);
			//TFltV T2(Docs);
   //         int dre = 0;
			//CenterMultiply3(BowMatrixV[dre], AlphaVV[dre][0], T1,Vekt, Koef, Ones, dre, Kapa,  CMpom, CMpom2, CMy1,CM2z);
			//CenterMultiply3(BowMatrixV[dre], AlphaVV[dre][0], T2,Vekt, Koef, Ones, dre , Kapa,  CMpom, CMpom2, CMy1,CM2z);
			//printf(" Hjec : %f ", TLinAlg::DotProduct(T1,T2));

}

void TMKCCASemSpace::Calc(const TVec<PBowDocBs>& BowDocBsV, TVec<PSemSpace>& SemSpaceV, const TStrV& InBowFNmV, const int& trainsize,
        const int& Dims, const double& Kapa, const int& CGMxIter, const int& HorstMxIter) {

    // prepare matrix with TFIDF vectors for each language
    TVec<TBowMatrix> BowMatrixV;
    TIntV dedek(trainsize);
	for (int dedekN = 0; dedekN < dedek.Len(); dedekN++){
        dedek[dedekN] = dedekN;
	}
	
	TVec<PBowDocWgtBs> BowDocWgtBsV;

	for (int BowDocBsN = 0; BowDocBsN < BowDocBsV.Len(); BowDocBsN++) {
        
		PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(
            BowDocBsV[BowDocBsN], bwwtNrmTFIDF);
        BowMatrixV.Add(TBowMatrix(BowDocWgtBs, dedek));               //CCA!!!!!!!!!!!!!!! 
        //BowDocWgtBsV.Add(BowDocWgtBs);                      //LSI
        //BowMatrixV.Add(TBowMatrix(BowDocWgtBs, dedek));     //LSI
				
    }
    // calculate
    TVec<TVec<TFltV> > AlphaVV;

    
	//K-means
	//PBowDocWgtBs BowDocWgtBs = TBowDocWgtBs::New(BowDocBsV[0], bwwtNrmTFIDF);
	//PBowDocPart particije = TBowClust::GetKMeansPartForDocWgtBs(TNotify::StdNotify,BowDocWgtBs, BowDocBsV[0], TBowSim::New(bstEucl), TRnd(0), 3, 4, 0.1, 10);


	//zlepiti BowDocBsV[i] 


	//TBowMatrix zlepek(BowDocWgtBsV, dedek);
	//printf("%d \n", zlepek.GetCols());
	//printf("%d \n", zlepek.GetRows());


	//for (int i = 0; i < BowDocBsV.Len()- 1; ++i){
	//    BowDocWgtBsV.DelLast();
	//}

	////LSI
	//int numdim = 10;
	//TFltV SgnValV(numdim);
	//TFltVV LeftSgnVecVV(zlepek.GetRows(), numdim);
	//TFltVV RightSgnVecVV(zlepek.GetRows(), numdim);
	////ssotNoOrto, ssotSelective, ssotFull
	//TSparseSVD::LanczosSVD(zlepek, numdim, 3*numdim, ssotSelective, SgnValV, LeftSgnVecVV, RightSgnVecVV, false);
	//
	//TLAMisc::SaveMatlabTFltV(SgnValV,  "S.dat");
	//TLAMisc::SaveMatlabTFltVV(LeftSgnVecVV,  "U.dat");
	//TLAMisc::SaveMatlabTFltVV(RightSgnVecVV,  "V.dat");
 //   

    //CalcKCCA(BowMatrixV, Dims, Kapa, CGMxIter, BowDocBsV.Len() * HorstMxIter, AlphaVV);
	CalcKCCA2(BowMatrixV, Dims, Kapa, CGMxIter,HorstMxIter, AlphaVV);
	
	for(int i = 0; i < InBowFNmV.Len(); i++)
	{
		TStr st = InBowFNmV[i];
		st += ".dat";
 
        TLAMisc::SaveMatlabTFltVV(AlphaVV[i],  st);
	}



}

//void TMKCCASemSpace::SaveAlpha(const TStrV& InBowFNmV){
//
//	for(int i = 0; i < InBowFNmV.Len(); i++)
//	{
//		TStr st = InBowFNmV[i];
//		st += ".dat";
//        
//		SemSpaceV.
//
//        TLAMisc::SaveMatlabTFltVV(AlphaV[i],  st);
//	}
//	
//
//}