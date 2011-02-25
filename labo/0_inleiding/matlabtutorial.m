%%%%%%%%%%%%%%%%%%%
% Matlab-tutorial %
%%%%%%%%%%%%%%%%%%%

% Maak gebruik van de matlab help! : typ help in de workspace
help
% Kijk ook eens onder het tabblad 'Help' in de workspace of druk op F1:
% hier kan je alle commando's bekijken en zoeken! De help functie in Matlab 
% is erg uitgebreid. De commando help geeft meestal zonder al te veel problemen 
% de nodige informatie over een gegeven commando omdat je hierarchisch door 
% de Matlab commando's kan zoeken en de meeste functies een herkenbare naam 
% hebben.

% % duiden op commentaar
% Alle lijnen afgesloten door ; worden niet getoond in de workspace

% Variabelen en Matrices definiëren
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% definitie van een scalair. De waarde wordt dadelijk toegekend aan een
% variabele
a = 1

% Speciale getalnotaties:
b = 12e3 % Dit moet je zonder spatie typen
c = eps % eps is het kleinste getal dat kan voorgesteld worden (= de precisie van de computer)
d = Inf % oneindig

% De werkruimte van Matlab bevat alle gedefinieerde scalairen, vectoren en
% matrices. De commando om een matrix te definiëren heeft de vorm 
% [ e11 e12 ... e1k; e21 e22 ... e2k; ... ; ej1 ej2 ... ejk ]
% voor een matrix van grootte j x k. 
V = [1,2,3,4,5,6,7] % V is een rijvector met 7 elementen
W = [9;8;7] % W is een kolomvector met drie elementen

A = [ 11 12 13 14; 21 22 23 24; 31 32 33 34 ]
% A is een matrix met drie rijen en 4 kolommen.

% Opmerking: Matlab variabelen en commando's zijn case-sensitive!
% Let er ook voor op om variabelen geen naam van een Matlab-functie te
% geven.

% De nieuwe waarde van A wordt gelijk op het scherm gezet. Door pijl-omhoog 
% wordt de voorgaande commando weer op de opdracht regel gezet. Voeg een puntkomma 
% achter de commando om het niet naar het scherm te schrijven. 

% Een variabele (gedefinieerd ad prompt blijft in de Matlab ruimte bestaan totdat 
% die expliciet verwijderd wordt. De inhoud van de ruimte kan bekeken worden met 
% de commando's who en whos: whos toont naast de namen van bestaande matrices ook 
% hun grootte: rijen, kolommen en het geheugenruimte dat ze beslaan.
whos

% Je kan aan alle variabelen meerdere namen toewijzen. 
C = A; 
whos
% A wordt hierdoor niet veranderd. Door C = A uit te voeren gaat C verwijzen naar 
% een nieuwe matrix met de grootte en inhoud van A.

% matrices initialiseren zonder de waardes te kennen kan bv met de commando's 
% eye(n, m), ones(n, m) en zeros(n, m), die respectievelijk de n x m eenheids-, 
% enen- en nulmatrix. 
D = eye(5)
E = eye(5, 2)
F = ones(3, 2)
G = zeros(6)
% Andere, specifieke matrices worden gegenereerd door rand, magic, hilbert
H = rand(2,3) % generatie van een 2x3 matrix met random getallen tussen 0 en 1

% Matrices kunnen uit het geheugen gehaald worden met de opdracht clear. 
clear A 
whos
% merk op dat C blijft bestaan
% clear zonder argumenten verwijdert alle matrices uit het geheugen.

% Ook aan een gedeelte van een bestaande matrix kan een naam gekoppeld worden. 
% Ieder element van een matrix heeft een label dat gebaseerd is op de naam van 
% de matrix en de positie van het element daarin. Het element in rij i en kolom j 
% van matrix A kan bekomen worden door A(i,j). 
V = [1,2,3,4,5,6,7]
v = V(2) % selecteer het tweede element uit V
w = V(3:4) % selecteer het derde tot het vierde element uit V

A = [ 11 12 13 14; 21 22 23 24; 31 32 33 34 ]
d = A(1, 2) % wijs het getal op positie (1, 2) in matrix A aan d toe. 

% Matlab maakt geen onderscheid tussen scalairen, vectoren en matrices: voor 
% Matlab is alles een matrix, scalairen zijn matrices met grootte 1 x 1.

% Het definiëren van vectoren en deel-matrices uit een matrix kan met behulp 
% van de dubbele-punt. 
Arij1 = A(1, :) % creëer een 1 x 4 rijvector. 
% Arij1 bevat de hele eerste rij van A; de dubbele punt staat voor een reeks 
% elementen.
Akolom2 = A(:, 2) % definieer een kolomvector met elementen uit de 2e kolom van A.
% Deze reeks kan ook nader gespecifieerd worden:
Akolom1_23 = A(1, 2:3)
% De resulterend rijvector heeft lengte twee en bevat elementen [A12, A13]. 
% x:y staat dus voor de reeks posities x tot en met y. 
H = A(1:2, 3:4) 
% H is een deelmatrix van A met de elementen [A13, A14; A23, A24].

% Zo kan je ook op een makkelijke manier een vector aanmaken met oplopende
% element-waardes
J = 3:9

% Strings kan je ook aanmaken en bewerken in matlab
hstr = 'Hello World!'
iname = input('What is your name?   ','s');
disp(strcat('Your name is  ',iname))

% Error messages, zoals bv.
error('Better solution needed!')
% zorgen ervoor dat de uitvoer in Matlab stopt en de foutboodschap wordt
% weergegeven.

% Matrix-bewerkingen
%%%%%%%%%%%%%%%%%%%%

% Matlab voert bewerkingen uit volgens de regels van matrix algebra.
A = [ 1 2 3 4; 2 3 1 2; 4 3 3 2; 1 0 9 7 ];
B = [ 1 2; 3 4 ];
C = [ 1 2; 3 5 ];
D = [ 1 1 1 1 ];
E = [1; 1; 2; 2];

% Eenvoudige rekenkundige bewerkingen op matrices:
F = B + C
F = B - C
F = E * D
F = D * E
F = A * E
F = D * A
F = [1; 2; 3; 4] * D
% De bewerking definieert telkens een nieuwe matrix F als resultaat van 
% de bewerkingen op bestaande of ter plekke gedefinieerde matrices.

% Matlab geeft een foutmelding als een illegale operatie wordt geprobeerd
E * A
A - C
A(1:2,1:2) - C

% De getransponeerde van een matrix wordt verkregen door het aanhalingsteken '.
% Probeer 
A'
E'

inv(A) % de inverse van A

% Vermenigvuldigen en delen kan ook elementsgewijs worden uitgevoerd door de .* en ./ operatoren:
A .* 3
E ./ 2
D .* (D * E)
A.^2

% Complexe bewerkingen en commando's (zoals bv. eig voor eigenwaarden en -vectoren) hebben
% de vorm [out1, out2, ... ] = f(in1, in2, ...). De inputs in de ronde haakjes zijn argumenten 
% voor de functie, de outputs tussen de vierkante haakjes zijn de namen die toegewezen zullen 
% worden aan matrices die de functie tijdens zijn uitvoering definieert, meestal op grond van 
% de argumenten. 
A = [1 2 3; 4 2 1];
[r, c] = size(A) % het aantal rijen en kolommen wordt respectievelijk aan r en c toegekend.

% Trigonometrische functies:
sin(pi) % LET OP: alle hoeken in matlab worden in radialen uitgedrukt
% cos, tan, asin, acos, atan

% Andere functies:
exp(2)
log(10) % natuurlijke logaritmes
rem(8,2) % rest
% andere functies:
abs(-1)
sign(-1)
sqrt(16)
round(3.1416) % Kijk ook eens naar floor en ceil

% Al deze functies zijn ook toepasbaar op vectoren en matrices:
[1,2] == [3,2]
sin([1:10:90]*360/(2*pi))
sqrt(exp(2:4))

% Vector functies:
max(A)
max(max(A))
max(A(:))
% Kijk ook eens naar min, sum, prod, sort, median, mean, std, any, all
mean(A)
mean(A(:))

% Programma's in .m-files
%%%%%%%%%%%%%%%%%%%%%%%%%
% Het is mogelijk en eenvoudig je eigen functies te schrijven voor Matlab. Die beschouwt 
% tekstbestanden met de extensie .m alsin te lezen en uit te voeren commando's. 
% Matlab voorziet een editor om m-files te schrijven
edit

% Hieronder volgt de code voor een m-file test.m. Geef je functie ook dezelfde naam als
% de m-file, want Matlab kijkt alleen naar de bestandsnaam als hij een programma 
% zoekt dat hoort bij een gegeven commando. De volgende code kan je
% copy-pasten naar de editor en opslaan als test.m (vergeet de commentaar
% niet te verwijderen: selecteer alles en typ Ctrl-T).
% function [som, verschil] = test(a, b)
% %
% % [som, verschil] = test(a, b). Een test functie.
% %
% som = a + b;
% verschil = a - b;
% % einde van de functie (en de m-file)

% De eerste regel van ieder m.file heeft de vorm function [out1, out2, ...] = functieNaam(in1, in2, ...). 
% Deze regel vertelt Matlab hoe de functie vanuit de commando regel aangeroepen kan worden. Binnen de 
% m-file kunnen de namen in1, in2, ... gebruikt worden, die pas bij het uitvoeren een concrete waarde 
% krijgen via de gespecificeerde argumenten.

% Interne variabelen (alle variabelen buiten de inputs en outputs in een m-file zijn lokaal gedefinieerd: 
% ze bestaan alleen binnen de ruimte die Matlab tijdelijk toekent aan de functie. Als de functie klaar is, 
% zijn de interne variabelen niet meer aanwezig.

% Voordat de nieuwe functie kan opgeroepen worden, moet Matlab weten dat deze bestaat en in welke directory
% het bijbehorende tekstbestand zit. Wanneer een functie vanuit de commando regel wordt aangeroepen zoekt 
% Matlab een m-file met die naam op in het path en de working/actieve directory. Matlab duidt in de menubalk 
% de working directory aan en er is browse knop waarmee een andere actieve directory gekozen kan worden. 

% Het path kan aangepast worden onder het tabblad 'File' en dan 'Set Path...' . Als je een nieuwe directory 
% in het path opslaat heeft Matlab beschikking tot alle scripts in de .m bestanden in het path als het de
% volgende keer wordt geopend.

% Zet ; achter elke regel van je functie, anders worden ze allemaal getoond wat vertragend kan werken
% Laat ze weg voor eventueel gewenste uitvoer. Uitvoer naar de workspace krijg je ook met de commando's
% disp of fprintf gegeven worden. Voeg bv volgende regel toe in je m-file:
fprintf('Het eerste getal is %f, het tweede %d.\n', a, b);
% Het eerste argument van fprintf is een string die op scherm gezet moeten worden. Alle strings 
% moeten tussen enkele aanhalingstekens staan, dat geldt ook voor bestandsnamen en directories. Sommige tekens 
% in een string hebben een speciale betekenis. De string '\n' staat voor de computer bijvoorbeeld voor één 
% teken, niet een "\" en een "n": '\n' is de "end-of-line" karakter die ervoor zorgt dat volgende karakters 
% naar de volgende regel op het scherm gestuurd worden. De alfanumerieke teken "\" zelf is gerepresenteerd als 
% '\\'. De '%d' en '%f' tekens staan voor de waarden van variabelen die na de string als argument mee zijn gegeven. 
% De % houdt in dat de waarde van de respectievelijke variabele moet worden afgedrukt, de d en f geven aan in wat 
% voor vorm niet-gehele of grote getallen precies worden weergegeven (met en zonder de wetenschappelijke (E) notatie 
% respectievelijk).


% Program flow
%%%%%%%%%%%%%%%

% If heeft, zoals de alle flow control commando's, een andere vorm dan de meeste functies, namelijk
% 
% if expressie,
%     % commando's
% end;

% expressie staat voor een logische expressie (waar of onwaar, 0 of 1), bv. iets als a == b. Logische 
% expressies maken gebruik van de volgende boolean operatoren:
1 == 0 % is-gelijk % Let er vooral op dat het teken voor de relatie is-gelijk bestaat uit TWEE is-gelijk tekens.
1 ~= 0 % is-ongelijk
1 < 0  % kleiner-dan
1 <= 1 % kleiner-danof gelijk
1 > 0  % groter-dan
1 >= 0 % groter-dan of gelijk 

% Er bestaan ook operatoren op expressies als geheel, waardoor complexe expressies gevormd kunnen worden die 
% nog steeds of waar, of onwaar kunnen zijn. De haakjes in de voorbeelden zijn niet strikt noodzakelijk, maar 
% ze werken verduidelijkend en voorkomen fouten.
~(1 == 0) % NOT
(1 == 0) & (2 == 2) % AND
(1 == 0) | (2 == 2) % OR



% De opdracht else is een uitbreiding van de functionaliteit van if. Een if - else structuur wordt als volgt gevormd:
% 
% if expressie,
%     set van commando's 1
% else,
%     set van commando's 2
% end;
% Als de expressie onwaar is, wordt de tweede set commando's uitgevoerd.
% Geneste if-structuren zijn ook mogelijk.

% Voorbeeld: 
if 1 > 0,
    disp('true');
else
    disp('false');
end

% Het while commando voert dezelfde set commando's herhaaldelijk uit,
% zolang aan een bepaalde voorwaarde wordt voldaan:
% while expressie,
%     commando-set
% end;

% Voorbeeld:
index = 1;
while index < 4,
    fprintf('%f\n', index);
    index = index + 1;
end;

% While lussen moetmoetne eindig zijn: de lus moet onder de juiste voorwaarden daadwerkelijk ophouden. 
% Een while opdracht kan makkelijk in een oneindige lus terecht komen. Zulke lussen kun je stoppen door 
% in Matlab op ctrl - c te drukken. Ctrl - c stopt alle functies.

% De functie break zorgt ervoor dat een while lus onmiddelijk verlaten wordt, zonder dat eventueel resterende 
% delen van de voorwaardelijke commando set worden uitgevoerd. 

% De laatste flow control opdracht is for. For is bedoeld om een index een bepaalde reeks waarden af te laten
% tellen en een set opdrachten uit te voeren voor ieder van die waarden:
% for index = a:b,
%     % opdrachten
% end;

% voorbeeld: 
fac = 1;
for index = 1:9,
    fac = fac * index
end;

% De for-opdracht kan de teller ook met andere waarden verhogen. De vorm is dan: for index = a:d:b, waarbij de 
% waarden a, a + d, a + 2 * d, ... worden aangenomen. Vervang bijvoorbeeld 1:9 in het voorgaande met 1:2:9 en 
% bekijk het resultaat. 



% Graphics
%%%%%%%%%%
% 2D plots kan je genereren mbv. plot
x = -4:0.1:4;
y = sin(x);
figure, plot(x,y),title('y = sin(x)'), xlabel('x'), ylabel('sin(x)')
% figure maakt een nieuw venster aan waarin het plot gemaakt wordt.
% De annotaties gebeuren met title, xlabel, text, gtext, ...
% Verschillende punt- en lijntypes kan je bekomen door variaties op
% plot(x,y,'rd-'). Probeer dat zelf eens uit, net als de functies subplot,
% hold on, hold off, ...

% 3D plots kan je maken met de functie mesh en meshgrid
xx = -2:0.1:2;
yy = xx;
[x,y] = meshgrid(xx,yy);
z = exp(-x.^2 - y.^2);
figure, mesh(z)
% Bekijk zelf het effect van commando's als plot3, mesh, surf, ...


% Om de matlabsessie te beëindigen, typ je quit of exit aan de prompt in de
% workspace.
% exit
