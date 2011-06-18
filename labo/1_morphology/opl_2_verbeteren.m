function opl_2_verbeteren( )
%LABO_OPL2 Summary of this function goes here
%   Detailed explanation goes here

%Text afbeelding inlezen en inverteren
text=not(im2bw(imread('images/text.bmp')));
%textDilate=[0 1 0 ;1 1 1;0 1 0];
textSquare = strel('square', 2);

% Dilate uitvoeren op de tekst om de tekst duidelijker (voller) te maken.
text_verbetering = imdilate(text,textSquare);

%Door de imclose worden de letters meer samenhangend (aan elkaar hangen
%van losse stukken van 1 letter)
text_verbetering = imclose(text_verbetering, textSquare);

%Resultaat terug inverteren alvorens te tonen
%figure,imshow(not(text)),figure,imshow(not(text_verbetering));


finger=not(imread('images/fingerprint.bmp'));
fingerDisk=strel('disk',1,4);

% We kunnen de randen duidelijker maken door gebruik te maken van erode
finger_verbetering = imerode(finger, fingerDisk);

% Indien er nog te veel ruis rond de afdruk zou staan kan deze weggewerkt
% worden via imopen:
%finger_verbetering = imopen(finger_verbetering, strel('square', 2));

figure, imshow(not(finger)), title('Origineel');
figure, imshow(not(finger_verbetering)), title('Enhanced');

% 1mm dikte van de vingerafdrukken adhv bwmorph, omvormen naar een skelet
finger_1mm = bwmorph(finger_verbetering, 'skel', Inf);
figure, imshow(not(finger_1mm)), title('1mm');




% Om nog smoother te maken van de beelden, kunnen we gebruik maken van 
% gaussianfilter
%fingerGaus=fspecial('gaussian',[5 5],0.5);
%figure, imshow(imfilter(finger,fingerGaus));

end

