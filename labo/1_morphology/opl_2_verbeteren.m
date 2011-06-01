function opl_2_verbeteren( )
%LABO_OPL2 Summary of this function goes here
%   Detailed explanation goes here

%??????
%text=not(im2bw(imread('images/text.bmp')));
%textDilate=[0 1 0 ;1 1 1;0 1 0];
% Dilate uitvoeren op de tekst doormiddel van b om de tekst duidelijker te
% maken.
%figure,imshow(not(text)),figure,imshow(not(imdilate(text,textDilate)));

%????
%finger=imread('images/fingerprint.bmp');
%se=strel('disk',1,4);
% We kunnen de randen smoother maken door gebruik te maken van erode of
%figure, imshow(finger);
%figure, imshow(not(imerode(not(finger),se)));

% door gebruik te maken van een gaussiaanse filter.
%fingerGaus=fspecial('gaussian',[5 5],0.5);
%figure, imshow(finger);
%figure, imshow(imfilter(finger,fingerGaus));




end

