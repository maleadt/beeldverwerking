function opl_6_lena( )
%lena
lena=imread('images/lena.jpg');

figure, imshow(lena);

% Opstellen van de matrix om de morphologische operatie op toe te passen.
lenaMatrix=[ 1,1 ; 1,1 ];
% Toepassen van de erode en dilate
lenaerosie=imerode(lena, lenaMatrix);
lenadilatie=imdilate(lena, lenaMatrix);
% Onderstaande toont de morphologische gradient, het geeft het contrast
% rond een bepaalde pixel terug, dit zijn dus eigenlijk de randen die
% aangegeven worden
figure, imshow(lenadilatie-lenaerosie);


%Eerst open dan close voor smooth te maken
lena_open = imopen(lena, lenaMatrix);
lena_smooth = imclose(lena_open, lenaMatrix);
figure,imshow(lena_smooth);

%Guassian filter toepassen:
gauss = fspecial('gaussian',[7 7],0.8);
figure,imshow(imfilter(lena, gauss));

end

