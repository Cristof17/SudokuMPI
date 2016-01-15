Rotsching Cristofor 333CA

Citirea din fisier o fac astfel:
Citesc cate o linie din fisierul de intrare, contorizez cate linii am citit pana
la un anumit moment, si compar numarul de linii citite cu rangul procesului.
Daca fac match, atunci parsez linia si o returnez prin intermediul functiei 
parseInputAsArray();

In topologia locala unui proces apelez functia combineMatrixAdiacenta() pentru
agregarea vectorului de vecini a unui proces la matricea de topologie locala.

Pentru impementarea crearii topologiei folosind mesaje, din functia main apelez
functia createTopologyUsingMessages() care intoarce matricea de topologie pentru
nodul curent. Mesajele sunt trimise respectand algoritmul de trimitere a mesajelor
prezentat in curs. Astfel, pentru orice nod care transmite mesaje de tip sondaj 
si asteapta raspuns de tip ECHO , daca acesta primeste raspuns de tip Sonda, 
va trimite ECHO EMPTY + taie legatura catre acel nod. Pentru primirea oricarui 
mesaj apelez functia MPI_Recv cu ANY_SOURCE si ANT_TAG, urmand sa fac verificarea
tipului de mesaj prin o structura status trimisa functiei Receive.

In momentul in care primesc un mesaj de ECHO VALID tot ce fac este sa fac LOGIC OR
intre matricea primita si matricea de topologie curenta, pe care o voi trimite mai departe
catre parinte (Parintele este considerat procesul care a trims prima oara mesajul de tip
sonda).

Dupa ce am create matricea de topologie a nodului 0, initiatorul schimbului de mesaje,
trimit topologia acestuia prin functia MPI_Broadcast catre celelalte noduri. In acest 
moment toate nodurile isi creaza tabela de rutare, vector de noduri, cu ajutorul careia
vor comunica cu celelalte noduri. Algoritmul din spatele crearii tabelei de rutare este
urmatorul: Procesul i se uita in tabela de topologie pe linia i. Pe coloana j unde se
gaseste valoare 1, acesta va introduce in coloana j din tabela de rutare valoarea j.
Apoi mai inspecteaza odata tabela de rutare, cauta spatiile necompletate, cerceteaza 
legaturile nodurilor de sub el, sa vada daca acestea pot ajunge la nodul respectiv.
In caz ca nu gaseste niciun nod copil prin care sa poata trimite solutiile partiale,
acesta va trimite mesajele catre parintele sau. Functia care creaza tabela de rutare
este createRoutingVector();

Dupa ce a fost creata matricea de topologie, un nod citeste bucata sa din tabela de sudoku,
folosind functia getSudokuFragment()

Fiecare nod contine patru matrici cu ajutorul carora creeaza sudoku. Solutii este matricea 
in care sunt generate solutiile sudoku de dimensiunea sqrt(topologie sudoku fisier). Aux
reprezinta solutiile continute de matricea solutii, inglobate intr-o matrice de dimensiunea
topologiei sudoku din fisier. Primite reprezinta al treilea tablou tridimensional in care sunt
tinute minte matricile primite de la copii. Fiecare rezultat din aux este combinat cu fiecare
matrice de sudoku venita de la copii, care mai departe este validata, iar daca este validata este
introdusa in tabloul tridimensional deTrimis care contine solutiile valide de sudoku.
De mentionat este ca cele 4 tablouri sunt reprezentare ca un vector, deoarce combinatiile de
sudoku trebuiesc memorate intr-o zona continua de memorie.