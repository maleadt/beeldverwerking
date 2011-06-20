function texturevector = textureanalysis(imfile);

% Textureanalysis.m
% Reads in the image 'imfile', asks which region must be investigated and
% returns a featurevector 'texturevector' as result with the following
% elements: [gemiddelde grijswaarde, gemiddeld contrast, relatieve smoothness,
% genormaliseerd derde moment, uniformiteit, entropie]
%
% KTeelen, februari 2008

im = imread(imfile);
disp('Choose a region by selecting the upper left and the lower right corner:')
figure, imshow(im),
pts = ginput(2);
region = im(pts(1,2):pts(2,2),pts(1,1):pts(2,1));
figure, imshow(region)

% Histogram
figure, imhist(region)
numbins = 255;
histbin = 0:numbins;
histbin = histbin/numbins;
histval = imhist(region(:));
histval = histval/sum(histval(:)); % normalisation

% Compute n moments
n = 3;
mom = zeros(n,1);
% First moment = mean
meanval = histbin*histval;
histbin = histbin - meanval;% center variables about mean
mom(1) = meanval*numbins;
for i = 2:n,
    mom(i) = ((histbin*numbins).^i)*histval
end

% average gray value =
avgr = mom(1);
% average contrast = sqrt of 2nd moment = 
avctr = sqrt(mom(2));
% smoothness =
varn = mom(2)/numbins^2;
smoo = 1 - 1/(1+varn);
% Third moment normalized = 
thmn = mom(3)/numbins^2;
% uniformity = 
unif = sum(histval.^2);
% entropy =
entr = -sum(histval .* (log2(histval + eps)));

% texture parameters
texturevector = [avgr,avctr,smoo,thmn,unif,entr]
