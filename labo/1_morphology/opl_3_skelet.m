function opl_3_skelet()
%gewoon erode doen om 1 pixel te verkleinen, dan van vorig aftrekken, enkel
%border blijft dan nog over
leaf=not(imread('images/leaf.bmp'));
leaf_erodeDisk =strel('disk',1);
%Verkleinen
leaf_erode = imerode(leaf,leaf_erodeDisk);
%Vergroten
leaf_dilate = imdilate(leaf,leaf_erodeDisk);
%Veschil tonen, je kan ook gewoon het verschil: origineel - leaf_erode
%tonen, dan is de rand smaller
leaf_result = leaf_dilate - leaf_erode;
figure, imshow(not(leaf_result)), title('origineel - erode');

%bwmorph geeft meteen de border
leaf_bwmorph = bwmorph(leaf,'remove');
figure, imshow(not(leaf_bwmorph)), title('morph');

[B,L] = bwboundaries(leaf,'noholes'); %L bevat nu de continue regios, 
%dus dat is de boundary
figure, imshow(not(L)), title('bwboundaries');

end

