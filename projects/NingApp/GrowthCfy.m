function GrowthCmty()

FNm = 'AWorkDir\GrowthStat2-Comment-rnd1k.tab';

Samples = 500;
Skip=50;

%%% attributes
%Attrs = [5:24]; % all
%Attrs = [5:24]; % motifs
%Attrs = [5,6,7,8]; % at half time: nodes, edges, WCC, CCF
Attrs = [1,2,3,4]; % at full time: nodes, edges, WCC, CCF

RawData = dlmread(FNm, '\t', 1,1);
RawData = [RawData(:, 1)./RawData(:, 5), RawData]; % append growth column
%RawData = [
RawData = sortrows(RawData); % sort by growth
fprintf('\n**********************************************\nRawData has %d rows, %d cols\n', size(RawData,1), size(RawData,2));

Data = [ RawData(1+Skip:Samples+Skip,:); RawData(end-Samples+1-Skip:end-Skip, :)];
Data = Data(:, 1+Attrs); % select attributes
%Data = Data ./ (sum(Data')' * ones(1, size(Data,2))); % norm motifs
%Data = log(1+Data);

Class =  [zeros(Samples, 1); ones(Samples, 1)];
fprintf('Data has %d rows, %d cols\n', size(Data,1), size(Data,2));
fprintf('Low  growth: %.2f -- %.2f\n', RawData(1+Skip, 1), RawData(Samples+Skip,1));
fprintf('High growth: %.2f -- %.2f\n', RawData(end-Samples+1-Skip, 1), RawData(end-Skip,1));

%RawData(:,1)

TrDatSel = rand(length(Class), 1);
TrDat = Data(TrDatSel>0.5, :);
TrCls = Class(TrDatSel>0.5, :);
TsDat = Data(TrDatSel<0.5, :);
TsCls = Class(TrDatSel<0.5, :);
%TsDat = TrDat; TsCls = TrCls;
fprintf('Training data: %d examples\n', size(TrDat,1));
fprintf('Testing data:  %d examples\n', size(TsDat,1));

[Model, dev,stats] = glmfit(TrDat, TrCls, 'binomial', 'link', 'logit', 'constant', 'on');
Pred = glmval(Model, TsDat, 'logit');
[X,Y,THRE,AUC,OPTROCPT,SUBY,SUBYNAMES] = perfcurve(TsCls, Pred, 1);
Accuracy = (sum(TsCls(Pred>0.5))+sum(1-TsCls(Pred<0.5))) / length(TsCls);
Accuracy
AUC
ModepParams=Model(1:end)
stats.p

%R=corr(RawData);
%[R(:,1), sort(R(:,1))]



%{
size(d)

slowG = d(1+Skip:Samples+Skip, TakeAttrs);
fastG = d(end-Samples+1-Skip:end-Skip, TakeAttrs);

%slowG =  (d(1:Samples, 14) * ones(1,length(TakeAttrs))) .* d(1:Samples, TakeAttrs);
%fastG = (d(end-Samples+1:end, 14) * ones(1,length(TakeAttrs))) .* d(end-Samples+1:end, TakeAttrs);

Dat = [slowG;fastG];
Class = [zeros(Samples, 1); ones(Samples, 1)];
size(Dat)
%[C,err,P,logp,coeff] = classify(Dat, Dat, Class, 'linear');
[Model, dev,stats] = glmfit(Dat, Class, 'binomial', 'link', 'logit', 'constant', 'on');
Pred = glmval(Model, Dat, 'logit');
[X,Y,THRE,AUC,OPTROCPT,SUBY,SUBYNAMES] = perfcurve(Class,Pred, 1);
Accuracy = (sum(Class(Pred>0.5))+sum(1-Class(Pred<0.5))) / length(Class)
c=corr([Dat,Class]);
Corr=[c(:,end), sort(c(:,end))]
stats.p(2:end)
%}
end