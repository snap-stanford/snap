void TCCAR::Calc(const TFltV& Query, const TMatrix& SourceL,  const TMatrix& TargetL, TFltV& Result,
		  const double& Regparam, const int& Cgsiter){

	TFltV Ones(SourceL.GetCols());
	Ones.PutAll(1.0);

	TFltV CenterS(SourceL.GetRows());
	SourceL.Multiply(Ones, CenterS);
	TLinAlg::MultiplyScalar(1.0 / SourceL.GetCols(), CenterS, CenterS);
	
	TFltV CenterT(TargetL.GetRows());
    TargetL.Multiply(Ones, CenterT);
	TLinAlg::MultiplyScalar(1.0 / SourceL.GetCols(), CenterT, CenterT);

	TFltV CM2p(SourceL.GetCols()); 		
	TFltV CGpom1(TargetL.GetRows()); 
	TFltV CGr(TargetL.GetRows()); 
	TFltV CGp(TargetL.GetRows()); 
	TFltV CGa(TargetL.GetRows());	

	TFltV pom(SourceL.GetRows());
	TFltV cq1(SourceL.GetCols());
	TFltV cq(TargetL.GetRows());
	TLinAlg::LinComb(1, Query, -1, CenterS, pom);
	CenterMultiply(SourceL, pom ,cq1, CenterS, true,  Ones);
    CenterMultiply(TargetL, cq1, cq, CenterT, false, Ones);
    
	TFltV pqy(TargetL.GetRows());
	ConjugGrad(TargetL, cq, pqy, CenterT, Ones, Regparam, Cgsiter , CM2p, CGpom1, CGr, CGp, CGa); 
    //pqy = irKy(cq);
	double lam = sqrt(2.0 / (double)TLinAlg::DotProduct(pqy, cq));
    //lam = sqrt(2.0 /(pqy' * cq));
	TLinAlg::LinComb(lam, pqy, 1, CenterT, Result);
    //qy = lam * pqy + cy;
	// plujs = (qy >0);
   // qy = qy.* plujs;
}

void TCCAR::CenterMultiply(const TMatrix& Matrix, const TFltV& x,TFltV& y, 
        const TFltV& Center, const bool& transposed,  const TFltV& Ones){

	if (transposed){
		Matrix.MultiplyT(x, y);
		TLinAlg::LinComb(1, y, -1*( TLinAlg::DotProduct(Center, x)), Ones, y);         
	}
	else{
        Matrix.Multiply(x, y);
		TLinAlg::LinComb(1, y, -1*( TLinAlg::DotProduct(Ones, x)), Center, y); 
	}
}

void TCCAR::CenterMultiply2(const TMatrix& Matrix, const TFltV& x,TFltV& y, 
        const TFltV& Center, const TFltV& Ones, const double& Regparam, TFltV& CM2p){

	CenterMultiply(Matrix, x, CM2p, Center, true, Ones);
	CenterMultiply(Matrix, CM2p, y, Center, false, Ones);
    TLinAlg::LinComb(1- Regparam, y, Regparam, x , y); 
}

void TCCAR::ConjugGrad(const TMatrix& Matrix, const TFltV& b, TFltV& x, const TFltV& Center, 
        const TFltV& Ones, const double& Regparam, const double& Cgsiter ,TFltV& CM2p, TFltV& CGpom1, 
        TFltV& CGr, TFltV& CGp, TFltV& CGa, const double& RelErr, const TFltV& x0) {

	x.Gen(Matrix.GetRows());
    if (x0.Empty()) { x.PutAll(0.0); }
    else { x = x0; }
	
	double rho, rho_new, alpha, pom3/*, resid*/;
	int niter = 0;
    
	CenterMultiply2(Matrix, x,CGpom1, Center, Ones, Regparam, CM2p);

	TLinAlg::AddVec(-1.0, CGpom1, b, CGr);
    
	CGp = CGr;
	
	while (niter < Cgsiter){
	    rho = TLinAlg::DotProduct(CGr, CGr);

	    CenterMultiply2(Matrix, CGp,CGa, Center, Ones, Regparam, CM2p);

        pom3 = (TLinAlg::DotProduct(CGa, CGp));
		alpha = rho/pom3;
	   
	    TLinAlg::AddVec(alpha, CGp, x, x);
        TLinAlg::AddVec(-alpha, CGa, CGr, CGr);
       
        rho_new = TLinAlg::DotProduct(CGr, CGr);
        TLinAlg::AddVec(rho_new/rho, CGp, CGr,CGp);
		
		rho = rho_new;
        
		////residuum
		//CenterMultiply2(Matrix, x,CGpom1, Vekt, Koef, Ones,LangN, Kapa, CMpom, CMpom2, CMy1, CM2z);
        //TLinAlg::AddVec(-1.0, b, CGpom1, CGpom1);
		//
		//double r1 = TLinAlg::Norm(b);
		//double r0 = TLinAlg::Norm(CGpom1);
		//
		//resid = r0 / r1;

	    niter++;
	}
}
