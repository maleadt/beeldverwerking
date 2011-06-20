function [allposs] = listpossibilities(config,row,col);
      
occ = union(config(row,:),config(:,col));
lf = 1 + 3*mod(floor((row - 1)/3), 3);
uf = 1 + 3*mod(floor((col - 1)/3), 3);
configpart = config([lf, lf + 1, lf + 2],[uf, uf + 1, uf + 2]);
occ = union(occ,configpart(:));
allposs = setdiff(1:9, occ);
  