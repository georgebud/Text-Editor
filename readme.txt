Codul scris reprezinta implementarea, cu ajutorul listelor, a unui editor de text pe baza unor comenzi de tipul: inserare, stergere, 
pozitionare, inlocuire, undo, redo.
Pentru stocarea textului citit de la tastatura am folosit liste dublu inlantuite, avand aspectul unei matrice stocand pe coloane fiecare 
caracter citit, iar la aparitia caracterului newline (ENTER) crearea unei noi linii.
Pentru acestea m-am folosit de structurile column si line si de structurile cursor si coordinates pentru parcurgere.
Pentru comenzile undo si redo folosesc o stiva in care adaug o structura (Operation) ce contine tipul operatiei, linia pe care se afla
cursorul in acel moment, doua campuri char pe care le folosesc in functie de comenzi, coordonata pe care se afla cursorul inainte sa se
produca operatia, respectiv coordonata pe care se afla cursorul dupa ce s-a efectuat operatia.

Programul porneste prin citirea de siruri de la tastatura si verifica daca sirul introdus a fost '::i'. 
La citirea sirului '::i' se trece din modul inserare in modul comanda, sau din modul comanda in modul inserare, dupa caz. Daca nu este
'::i' atunci suntem in faza de creare a listei cu fiecare caracter asezat intr-un nod. De asemenea textul introdus il voi salva intr-un
sir pe care il voi adauga in stiva atunci cand se trece in modul de comanda. Cand se tasteaza prima data '::i' se trece din modul insert
in modul comanda si adaug in stiva tipul operatiei, textul introdus pana in acel moment, coordonata de inceput a cursorului, cea finala
si numarul liniei curente. Acum se introduc comenzi si se proceseaza lista creata cu ajutorul functiei 'process_command'. Cand se face
din nou trazitia prin '::i', trecand in modul insert, se vor actualiza coordonatele, de inceput si final, pentru a relua procesul
initial. 

Pentru a verifica daca a fost introdus '::i' folosesc functia process_line. 
Cand nu este introdus ma aflu in modul de inserare de la tastatura (introduc text si creez noduri sau introduc comenzi si procesez lista),
altfel trec in modul comanda sau inserare, dupa caz.

Lista se creeaza pe baza functiei createLine. 
In cazul in care cursorul se afla pe un nod fara valoare (cand introduc text la inceputul programului sau cand introduc text dupa ultimul
caracter din lista) creez noduri in care adaug fiecare caracter iar la final adaug o linie noua la inceputul careia se va afla cursorul si
leg acest nod de ultimul nod al liniei precedente. In cazul in care mut cursorul pe un nod cu valoare si vreau sa introduc text, voi crea
o noua linie inaintea liniei pe care am pozitionat cursorul, la final updatand cursorul.

In functia process_command verific ce comenzi au fost introduse si apelez functia respectiva care face modificari asupra listei, apoi
adaug in stiva tipul operatiei (comanda), textul care a fost sters (sau NULL daca operatia a realizat doar modificari asupra cursorului,
sau cuvantul de inlocuit in cazul replace), coordonatele de inainte si dupa efectuarea operatiei, linia curenta pe care s-a realizat
operatia si cuvantul nou care inlocuieste cuvantul vechi - in cazul replace (sau NULL altfel) si apelez functia updateCounter care la
fiecare 5 comenzi salveaza outputul intr-un fisier dat ca parametru executabilului in linia de comanda. Cazul b - backspace, sterge un
caracter dinaintea cursorului; cazul q - quit, opreste programul (inchide bucla while din main); cazul gc - go to character, muta
cursorul pe coloana si linia specificate, daca linia nu este data in comanda atunci se va muta cursorul pe coloana specificata si linia
curenta; cazul dl - delete line, sterge linia specificata de comanda; cazul d - delete, sterge un numar de caractere de la cursor spre
dreapta, daca numarul de caractere nu este specificat de comanda atunci se va sterge un singur caracter (cel pe care se afla initial
cursorul); cazul s - save, deschide un fisier de tipul write only si salveaza outputul in acest fisier (care este dat ca parametru
executabilului); cazul u - undo, sunt luate informatiile unei operatii salvate in stiva si se realizeaza operatiile inverse acesteia,
de fiecare data dupa ce se apeleaza undo, pointerul de la varful stivei se va muta cu o operatie mai jos (care va fi acum noul varf);
cazul r - redo, inversul operatiei undo, varful stivei va fi din nou cel de dinainte de undo si se vor lua informatiile de la operatia
din varf pentru realizarea modificarilor; cazul re- replace, comanda este data impreuna cu inca 2 siruri de caractere (cuvinte),
functia are rolul de a inlocui primul cuvant din lista cu al doilea cuvant, in cazul in care primul cuvant se gaseste in lista.
De fiecare data dupa executarea unor comenzi se va face update la coordonate cat si la cursor.

Tin sa mentionez ca primesc punctele pe Testul 45 local, insa nu si pe vmchecker.
