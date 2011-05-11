function [ output ] = globalthresholding_3( )
image=imread('images/sonnet.jpg');
%Convolueer het beeld met gemiddelde-operator (m.b.v. fspecial en imfilter).
conv_beeld = imfilter(image,fspecial('average', [6, 6]), 'replicate');

%Trek het origineel van het geconvolueerde beeld af.
verschil = conv_beeld - image;


%Segmenteer het verschilbeeld met drempelwaarde 0.
cte=4;
binary=im2bw(verschil-cte,0);

% Waarom resulteert deze aanpassing in een verbetering van het resultaat?
% ???

figure,imshow(not(binary));
end

