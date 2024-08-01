Tema 4 PCOM

Giuglan Catalin-Ionut
325CB

Pentru rezolvarea temei am pornit de la scheletul laboratorului 9 
(https://gitlab.cs.pub.ro/pcom/pcom-laboratoare-public/-/tree/master/lab9), din care
am preluat fisierele buffer.c, buffer.h, helpers.c, helpers.h, requests.c si
requests.h. De asemenea, am preluat si fisierele parson.c si parson.h de pe unul din
link-urile din enuntul temei (https://github.com/kgabis/parson).

Am folosit fisierele parson.c si parson.h pentru a crea obiecte JSON atunci
cand trimit date la server, si pentru a extrage informatii din raspunsurile
primite de la server.

In fisierul requests.c am implementat functiile deja existente in scheletul de
laborator, si am adaugat in plus functia char *compute_delete_request, pentru
a putea realiza comenzile de delete.

In fisierul client.c sunt implementate toate functiile pe care le folosesc pentru
a realiza comenzile din enuntul temei. Le voi descrie pe fiecare in parte:

-> is_logged_in() - verifica daca utilizatorul este logat sau nu. Daca sirul
"cookie" este gol, inseamna ca utilizatorul nu este logat si returneaza 0.
Altfel, returneaza 1.

-> has_access_to_library() - verifica daca utilizatorul are acces la biblioteca,
intr-o maniera asemanatoare cu is_logged_in(). Verifica daca sirul "token" este
gol, ceea ce inseamna ca utilizatorul nu are acces la biblioteca, si returneaza
0 sau 1 dupa caz.

-> get_response_code(char *response) - aceasta functie extrage codul de raspuns
HTTP dintr-un sir de caractere primit ca parametru. Functia cauta subsirul
"HTTP/1.1" in sirul primit, si returneaza codul de raspuns sub format int. In cazul
in care nu se gaseste subsirul asteptat, functia returneaza -1.

-> get_cookie(char *response) - functia cauta un cookie in raspunsul primit si,
daca gaseste unul il salveaza in variabila globala "cookie".

-> remove_underscores(char *string) - functia "scoate" toate caracterele
underscore ("_") dintr-un sir de caractere

-> get_token(char *response) - functie similara cu get_cookie, extrage
token-ul din raspunsul primit si il salveaza in variabila globala "token".

-> handle_register(int sockfd, char *route, char *content_type) - functie care
gestioneaza comanda "register". Utilizatorul introduce username si parola,
care sunt trimise printr-un request de tip POST la server. In functie de
raspunsul primit, se afiseaza un mesaj corespunzator.

-> handle_login(int sockfd, char *route, char *content_type) - functie care
gestioneaza comanda "login". Utilizatorul introduce username si parola,
si asemanator cu handle_register, se trimite un request de tip POST la server.
In final utilizatorul este sau nu logat, in functie de raspunsul primit.
Daca este logat, se salveaza cookie-ul primit.

-> handle_enter_library(int sockfd, char *route) - in aceasta functie ma ocup
de comanda "enter_library". Verifica daca utilizatorul este logat, si daca
da, trimite un request de tip GET la server pentru a primi acces la biblioteca.
Daca accesul este permis, se salveaza token-ul primit si se afiseaza mesajul 
corespunzator. Daca nu, se afiseaza un mesaj de eroare.

-> handle_get_books(int sockfd, char *route) - aceasta functie solicita
serverului lista de carti din biblioteca prin trimiterea unui request de tip
GET. Daca raspunsul indica faptul ca utilizatorul are acces la biblioteca,
se afiseaza lista de carti. Altfel, se afiseaza un mesaj de eroare.

-> handle_get_book(int sockfd, char *route, char *id) - functioneaza asemanator
cu "handle_get_books", doar ca in functia asta se trimite un request pentru
a primi informatii despre o carte specifica, folosind id-ul primit.

-> handle_add_book(int sockfd, char *route, char *content_type) - in aceasta
functie utilizatorul introduce informatiile despre o carte, care sunt trimise
printr-un request de tip POST la server. In functie de raspunsul primit de la
server, cartea a fost sau nu adaugata si se afiseaza un mesaj corespunzator.

-> handle_delete_book(int sockfd, char *route, char *id) - functie care gestioneaza
stergera unei carti specifice din biblioteca, folosind id-ul primit ca parametru.
Se trimite un request de tip DELETE la server si in functie de raspuns, cartea a fost
sau nu stearsa.

-> handle_logout(int sockfd, char *route) - in aceasta functie se trimite o cerere de
tip GET la server pentru a deconecta utilizatorul. Daca se reuseste deconectarea,
se reseteaza cookie-ul si token-ul pentru urmatoarea sesiune si utilizatorul este
deconectat, afisand un mesaj corespunzator.

-> main - in main sunt gestionate comenzile introduse de utilizator prin intermediul
unui "if/else", si in functie de comanda introdusa se apeleaza functia corespunzatoare.
Pentru comenzile care necesita interactiunea cu serverul, conexiunea este deschisa
si inchisa inainte si dupa apelarea functiei corespunzatoare. Programul se termina
cand utilizatorul introduce comanda "exit", caz in care se inchide conexiunea si
se iese din "while" folosind "break". In cazul in care utilizatorul introduce o
comanda care nu exista, se afiseaza un mesaj de eroare.