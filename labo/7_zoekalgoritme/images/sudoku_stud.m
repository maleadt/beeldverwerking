% create random puzzles or look some up in de newspapers
p = [reshape(randperm(9),3,3),reshape(randperm(9),3,3),reshape(randperm(9),3,3);
    reshape(randperm(9),3,3),reshape(randperm(9),3,3),reshape(randperm(9),3,3);
    reshape(randperm(9),3,3),reshape(randperm(9),3,3),reshape(randperm(9),3,3)];
n = 40;
blanks = randint(1,n,81)+1;
p(blanks) = 0;
randompuzzle = p

simple = [9, 0, 0, 6, 0, 4, 7, 0, 0;
      4, 5, 0, 0, 0, 3, 0, 0, 1;
      1, 0, 0, 8, 5, 0, 0, 6, 0;
      0, 0, 0, 3, 0, 7, 0, 4, 0;
      8, 0, 7, 0, 9, 2, 0, 0, 0;
      0, 0, 2, 0, 0, 5, 3, 0, 0;
      2, 0, 0, 0, 0, 0, 5, 0, 6;
      0, 6, 0, 0, 0, 8, 0, 0, 0;
      0, 1, 5, 9, 0, 0, 8, 0, 7];
  
medium = [0, 0, 8, 0, 2, 0, 0, 0, 1;
      0, 0, 0, 0, 0, 1, 0, 0, 5;
      0, 2, 0, 9, 0, 7, 0, 4, 0;
      0, 0, 0, 8, 0, 0, 0, 5, 3;
      4, 0, 0, 1, 0, 0, 2, 0, 0;
      0, 6, 0, 4, 3, 9, 0, 8, 0;
      9, 7, 0, 0, 5, 0, 0, 0, 8;
      3, 0, 6, 0, 0, 0, 0, 9, 4;
      0, 0, 0, 0, 0, 0, 0, 0, 2];
  
advanced = [0, 0, 8, 0, 0, 0, 0, 0, 0;
      0, 9, 2, 0, 0, 0, 4, 0, 0;
      0, 6, 0, 0, 3, 0, 0, 8, 0;
      0, 0, 0, 0, 0, 0, 0, 1, 5;
      0, 0, 6, 0, 0, 1, 3, 0, 0;
      0, 0, 4, 7, 0, 5, 0, 0, 2;
      0, 0, 0, 0, 0, 7, 0, 9, 0;
      0, 0, 0, 0, 9, 0, 7, 4, 1;
      0, 5, 0, 3, 0, 0, 0, 0, 0];

  
% Choose a puzzle
puzzle = advanced;

% zoek de lege velden
[es_x,es_y] = find(puzzle == 0);
disp('Occupancy ratio: '),(81-length(es_x))/81

% creëer een lijst met alle mogelijkheden voor elk leeg veld in de set
% allpos:
allpos= {};
for i = 1:length(es_x),
    allpos = {allpos{1:end}, listpossibilities(puzzle,es_x(i),es_y(i))};
end
allpos

% How many configuration are still possible for this puzzle?
disp('Number of configurations to be verified in worst case: ')
len = 1;
for i = 1:length(allpos),
    len = len*length(allpos{i});
end
len

% Solve the puzzle:
confcount = 0;
itemstoprocess = [puzzle(:)'];
solved = 0;
while ~solved,
    % If no children are left, stop the search
    if size(itemstoprocess,1) == 0,
        disp('No solutions left.');
    else
        % else get the first element out of the list and compute the new
        % children and add them to the list
        father = reshape(itemstoprocess(1,:),9,9);
        itemstoprocess = itemstoprocess(2:end,:);
        
        % vul aa nmet je eigen code om het zoekproces te voltooien....
        
    end
end
