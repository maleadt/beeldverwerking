function opl_1_testen( )
bestandsnaam = 'images/squarecircle.bmp';
img = im2bw(imread(bestandsnaam), 0.5);

%Maken ascii voorstelling van de figuren in een matrix
%se = strel('ball', 50,50);
%se = strel('line',20,150);
se = strel('disk',10);
%se = strel('diamond',30)

%Pixels toevoegen aan de rand van de objecten, het object gaat naar de vorm
%van het structural element (se)
img_adjusted = imdilate(not(img), se);

%Pixels verwijderen aan de rand van de objecten, het object gaat naar de vorm
%van het structural element (se)
%img_adjusted = imerode(not(img), se);

%dit is erode met erna dilate (met 2x hetzelfde structural element)
%dit is bv handig om kleine objecten van de image te verwijderen maar
%wel de grotere objecten in de image behouden (inclusief vorm)
%hiervoor gebruik je dan een structural element die groot genoeg is om de
%kleine te verwijderen elementen te omvatten, maar toch nog klein genoeg is
%om de grote niet te wijzigen
%img_adjusted = imopen(not(img), se);

%dit is dilate met erna erode (met 2x hetzelfde structural element)
%img_adjusted = imclose(not(img), se);

imshow(img), title('origineel')
figure, imshow(not(img_adjusted)), title('aangepast')
figure, imshow(abs(img - not(img_adjusted))), title('verschil')

end

