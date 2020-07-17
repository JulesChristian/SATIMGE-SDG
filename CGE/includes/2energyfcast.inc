*---------------------------------------------------------------
*ESTIMATE DEMAND FORECAST
*---------------------------------------------------------------

* Previous: SFORE(FS,XC,T) = SUM((A,RD)$MFSA(FS,A), PVA0(A,RD)*QVAX(A,RD,XC,T,TT));
*
 SFORE(FS,XC,T) = SUM(A$MFSA(FS,A), QVAX(A,'nat',XC,T,TT));
* SFORE(FS,XC,T) = SUM(A$MFSA(FS,A), QAX(A,XC,T,TT));

 HFORE(FH,XC,TC)$SUM(H$MFHH(FH,H), HPOPX(H,XC,TC,TT)) = SUM(H$MFHH(FH,H), YIX(H,XC,TC,TT)/HPOPX(H,XC,TC,TT))/CPIX(XC,TC,TT);
 PFORE(FH,XC,TC) = SUM(H$MFHH(FH,H), HPOPX(H,XC,TC,TT));

 sumSFORE = SUM(FS,SFORE(FS,XC,'2012'));
*INCOME CALCULATIONS

*Calculate mean (assume uniform population distribution within each group)
 FHMM(FH,'MEAN')$FHMM(FH,'MAX') = (FHMM(FH,'MAX') + FHMM(FH,'MIN'))/2;

*Forecast mean incomes based on TIMES data
 FHMEAN(FH,T) = FHMM(FH,'MEAN') * HFORE(FH,XC,T);

*POPULATION CALCULATIONS

 FHMMB(FH,'MIN',TC)$SUM(T1, HFORE(FH,XC,T1)) = FHMM(FH,'MIN') * HFORE(FH,XC,TC)/SUM(T1, HFORE(FH,XC,T1));
 FHMMB(FH,'MAX',TC)$SUM(T1, HFORE(FH,XC,T1)) = FHMM(FH,'MAX') * HFORE(FH,XC,TC)/SUM(T1, HFORE(FH,XC,T1));

*Lowest group to highest group
 FHMOVE('HHD-LE','HHD-HE',TC)
         = (FHMMB('HHD-LE','MAX',TC) - FHMM('HHD-HE','MIN'))
         / (FHMMB('HHD-LE','MAX',TC) - FHMMB('HHD-LE','MIN',TC));
 FHMOVE('HHD-LE','HHD-HE',TC)$(FHMOVE('HHD-LE','HHD-HE',TC) LT 0) = 0;

*Lowest group to middle group
 FHMOVE('HHD-LE','HHD-ME',TC)
         = (FHMMB('HHD-LE','MAX',TC) - FHMM('HHD-ME','MIN'))
         / (FHMMB('HHD-LE','MAX',TC) - FHMMB('HHD-LE','MIN',TC))
         - FHMOVE('HHD-LE','HHD-HE',TC);

*Middle group to highest group
 FHMOVE('HHD-ME','HHD-HE',TC)$(FHMMB('HHD-ME','MIN',TC) LT FHMM('HHD-HE','MIN'))
         = (FHMMB('HHD-ME','MAX',TC) - FHMM('HHD-HE','MIN'))
         / (FHMMB('HHD-ME','MAX',TC) - FHMMB('HHD-ME','MIN',TC));
 FHMOVE('HHD-ME','HHD-HE',TC)$(FHMMB('HHD-ME','MIN',TC) GT FHMM('HHD-HE','MIN')) = 1;
 FHMOVE('HHD-ME','HHD-HE',TC)$(FHMOVE('HHD-ME','HHD-HE',TC) LT 0) = 0;

*Final population numbers
 TFHPOP('HHD-LE',XC,TC) = PFORE('HHD-LE',XC,TC) *
         (1 - FHMOVE('HHD-LE','HHD-ME',TC) - FHMOVE('HHD-LE','HHD-HE',TC));

 TFHPOP('HHD-ME',XC,TC) = PFORE('HHD-ME',XC,TC) *
         (1 - FHMOVE('HHD-ME','HHD-HE',TC))
         + FHMOVE('HHD-LE','HHD-ME',TC)*PFORE('HHD-LE',XC,TC);

 TFHPOP('HHD-HE',XC,TC) = PFORE('HHD-HE',XC,TC)
         + FHMOVE('HHD-LE','HHD-HE',TC)*PFORE('HHD-LE',XC,TC)
         + FHMOVE('HHD-ME','HHD-HE',TC)*PFORE('HHD-ME',XC,TC);

*INCOME CALCULATIONS (CONT)

loop (H,
 popval(H,XC,TC) =  (sum(FH,TFHPOP(FH,XC,TC))/10)*ORD(H)*popvalfact(H,XC,TC);
 );

*Reverse mapping
 MFHHT(FH,H,TC)=no;
 MFHHT('hhd-le',H,TC)$((popval(H,XC,TC) LE TFHPOP('hhd-le',XC,TC)))=YES;
 MFHHT('hhd-me',H,TC)$(((popval(H,XC,TC) GT TFHPOP('hhd-le',XC,TC)) and (popval(H,XC,TC) LE (TFHPOP('hhd-le',XC,TC)+TFHPOP('hhd-me',XC,TC)))))=YES;
 MFHHT('hhd-he',H,TC)$((popval(H,XC,TC) GT (TFHPOP('hhd-me',XC,TC)+TFHPOP('hhd-le',XC,TC))))=YES;

 HFORE(FH,XC,TC)$SUM(H$MFHHT(FH,H,TC), HPOPX(H,XC,TC,TT)) = SUM(H$MFHHT(FH,H,TC), YIX(H,XC,TC,TT)/HPOPX(H,XC,TC,TT))/CPIX(XC,TC,TT);



* remember to update HFORE with new mappings
*    HFORE(FH,XC,MYPTC) = HFORE(FH,XC,'2040');

    SFORERUN(FS,TC) = SFORE(FS,XC,TC);
    POPSX(TC) = SIM_POP(RUN,MILESTONYR);
    TFHPOP_X(FH,T) = TFHPOP(FH,XC,T);
    TFHPOPMY(FH,MILESTONYR) = TFHPOP(FH,XC,MILESTONYR)/POPSX(MILESTONYR);
    YHE(MILESTONYR) = HFORE('hhd-he',XC,MILESTONYR);

    TRAMOD = SIM_TRAMOD(RUN);

* write new demand driver values to demand workbook

  if(SFORE_X('COM','2050'),
    execute_unload "EnergyDem.gdx" POP SFORE_X GDP_FSX TRAMOD TFHPOPMY YHE MFHHT PAMS
    execute 'gdxxrw.exe i=MCDem.gdx o=SATIM\DMD_PRJ.xlsx index=index_G2E!a6';
  );

