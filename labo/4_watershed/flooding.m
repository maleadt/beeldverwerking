function flooding( filename )
% Flooding algoritme

I = imread(filename);
if length(size(I)) > 2,
    imd = double(rgb2gray(I));
else
    imd = double(I);
end

imd = imfilter(imd,fspecial('gaussian',[3,3],1),'replicate');
[r,k] = size(imd);
f = [-1 -2 -1;0 0 0; 1 2 1];
imv1 = filter2(f, imd(:,:));
imh1 = filter2(f',imd(:,:));
gradmagn = sqrt(imv1.^2+imh1.^2);
figure, imshow(mat2gray(gradmagn)), title('Sobel-filtered image');

levels = gradmagn(:);
levels = round(levels);
levels = unique(levels);
levels = sort(levels);

drownthresh = 40;
ind = find(gradmagn < drownthresh);
labels = zeros(size(imd));
labels(ind) = 1;
labels = bwmorph(labels,'clean',1); 
labels = bwmorph(labels,'fill',1); 
labels = bwlabel(labels,4);

figure, imshow(label2rgb(labels)),title('first step - drown under threshold')

figure,
for lev = 1:25:length(levels),
    level = levels(lev);
    newsum = 1;
    oldsum = 0;
    while newsum~=oldsum
        oldsum = newsum;
        labeled = labels~=0;
        labeledgrow = bwmorph(labeled,'dilate');
        kandidates = labeledgrow & ~labeled;
        chosen = kandidates & gradmagn <= level;

        up = [zeros(1,k); labels(1:end-1,:)];
        labels(up~=0 & chosen) = up(up~=0 & chosen);
        left = [zeros(r,1),labels(:,1:end-1)];
        labels(left~=0 & chosen) = left(left~=0 & chosen);
        right = [labels(:,2:end) , zeros(r,1)];
        labels(right~=0 & chosen) = right(right~=0 & chosen);
        down = [labels(2:end,:) ; zeros(1,k)];
        labels(down~=0 & chosen) = down(down~=0 & chosen);
        newsum = sum(labels(:));
    end
    %     imshow(label2rgb(labels)), title('Tussentijds gelabelde en gesegmenteerde beeld');
    %     pause(0.01)
    r*k - sum(sum(labels~=0))
end
figure, imshow(label2rgb(labels)), title('Het gelabelde en gesegmenteerde beeld');
figure, imshow(I), title('Het originele beeld')

end
