function [ output_args ] = pca( input_args )
% PCA op een kleurenbeeld
im = imread(input_args);
im = im2double(im);
imresh = reshape(im,size(im,1)*size(im,2),3);
featurevecs = imresh;
covfeat = cov(featurevecs);
% Compute eigenvalues en eigenvectors
[V,D] = eig(covfeat); % V = vectors, D = values
d = diag(D); % rearrange
[d,idx] = sort(d);
d = flipud(d);
idx = flipud(idx);
D = diag(d);
V = V(:,idx);
% Compute the Hotelling transform
A = V';
mx = sum(featurevecs,1)/size(featurevecs,1);
Mx = repmat(mx,size(featurevecs,1),1);
Y = A*((featurevecs - Mx)');
imres = zeros(size(im));
for i = 1:3,
imres(:,:,i) = reshape(Y(i,:),size(im,1),size(im,2));
end
figure,
subplot(221), imshow(im),
subplot(222), imshow(imres(:,:,1))
subplot(223), imshow(imres(:,:,2))
subplot(224), imshow(imres(:,:,3))
end
