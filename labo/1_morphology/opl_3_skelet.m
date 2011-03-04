function opl_3_skelet()
%LABO_OPL3 Summary of this function goes here
%   Detailed explanation goes here

%gewoon erode doen om 1 pixel te verkleinen, dan van vorig aftrekken, enkel
%border blijft dan nog over
leaf=imread('images/leaf.bmp');
se=strel('disk',1);
figure, imshow(leaf - imerode(leaf,se));

%bwmorph geeft meteen de border
leaf2 = bwmorph(leaf,'remove');
figure, imshow(leaf2);

figure,
[B,L] = bwboundaries(leaf,'noholes'); %L bevat nu de continue regios, 
%dus dat is de boundary
imshow(L);

end

