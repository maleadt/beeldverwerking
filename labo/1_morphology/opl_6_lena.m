function opl_6_lena( )
%lena
lena=imread('images/lena.jpg');
% Opstellen van de matrix om de morphologische operatie op toe te passen.
lenaMatrix=[ 1,1 ; 1,1 ];
% Toepassen van de erode en dilate
lenaerosie=imerode(lena, lenaMatrix);
lenadilatie=imdilate(lena, lenaMatrix);
% Onderstaande toont de morphologische gradient.
figure, imshow(lenadilatie-lenaerosie);

end

