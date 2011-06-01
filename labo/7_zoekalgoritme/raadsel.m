function raadsel( )
    % U2 geeft een concert dat over 17 minuten begint, maar ze moeten eerst allevier een brug oversteken om er te geraken. In het begin staan ze allevier aan dezelfde kant van de brug. Het is nacht en ze hebben maar één zaklamp. Er kunnen maximaal twee mensen tegelijk oversteken. Of er nu één, of twee oversteken, de zaklamp moet altijd mee. De zaklamp moet dus steeds heen en weer meegenomen worden. Sommige bandleden worden al wat ouder en hebben dus een langere tijd nodig om de brug over te steken. Als er twee mensen tegelijk wandelen, bepaalt de langzaamste de tijd nodig om de brug over te steken: Bono heeft 1 minuut nodig om over te steken, Edge 2, Adam 5 en Larry 10 minuten. Als Bono en Larry als eersten samen oversteken, zijn er 10 minuten verstreken als ze aan de overkant aankomen. Als Larry dan terugkeert, met de zaklamp, zijn er in totaal 20 minuten verstreken, en zijn ze dus te laat voor het concert. Er is geen trukjes voor de oplossing zoals zwemmen of gooien met de zaklamp. De vraag is natuurlijk: hoe doen ze het?
    clear all

    % Elk bandlid geraakt in een bepaalde tijd over de brug
    bono = 1;
    edge = 2;
    adam = 5;
    larry = 10;
    costs = [bono,edge,adam,larry];

    % Kies een goede representatie:
    % iedereen start op de linkeroever
    linksst = [1,2,3,4];
    % En niemand op de rechter
    rechtsst = [];

    % Creëer de zoekboom:
    % eerste stap: bepaal alle mogelijke correcte combinaties: voeg alle duo's toe die samen kunnen oversteken
    nodes = {};
    posschildren = nchoosek(1:4,2); % Stelt alle mogelijke combinaties van 2 elementen uit een verzameling van 4 op
    for i = 1:size(posschildren,1),
        rechts = linksst(posschildren(i,:));% Wie gaat er naar rechts
        links = linksst(setdiff(1:4,posschildren(i,:)));% wie blijft er links over
        cost = max(costs(posschildren(i,:)));% hoe lang doen ze erover?
        if cost < 17, % indien oplossing nog mogelijk is, voeg dan een node toe
            nodes = {nodes{1:end},{links,rechts,cost,rechts}};
        end
    end

    % search
    solfound = 0;
    while solfound == 0 & length(nodes) > 0,    
        currnode = nodes{1}; % kies de eerste node uit de lijst
        if length(currnode{2}) == 4,
            solfound = 1;
            result = currnode;
            break;
        end
        for j = 1:length(currnode{2}),
            rechts = currnode{2};
            naarlinks = rechts(j);
            links = [currnode{1}(1:end),naarlinks];
            intermcost = currnode{3} + costs(naarlinks);
            rechts(j) = [];
            if intermcost <= 17,
                % Nu gaan er twee terug naar de rechteroever over de brug gaan
                posschildren = nchoosek(1:length(links),2);
                for k = 1:size(posschildren,1),
                    naarrechts = links(posschildren(k,:));
                    currrechts = [rechts,naarrechts];
                    currlinks = links;
                    currlinks(posschildren(k,:)) = [];
                    intermcost = intermcost + max(costs(naarrechts));
                    if intermcost <= 17,
                        oversteken = [currnode{4};naarlinks,0;naarrechts];
                        nodes = {nodes{1:end},{currlinks,currrechts,intermcost,oversteken}};
                    end
                end
            end
        end
        nodes = nodes(2:end);
    end

    % Toon de resultaten
    if solfound == 1,
        result;
        for i = 1:4,
            result{i};
        end
    end

    % Vraag: Zou jij een andere representatie kiezen?
    % Antwoord: We zouden de zoekboom kunnen opbouwen met
    % knoop objecten die data bevatten.
    
    % Vraag: Welke zoekmethode wordt er in de code gebruikt, breadth of
    % depth first?
    % Antwoord: diepte eerst.    
end