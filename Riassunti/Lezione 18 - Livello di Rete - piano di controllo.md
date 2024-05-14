
# Funzioni del livello di rete

- <font color="redf">Inoltro:</font> spostare i pacchetti dall'ingresso del router all'uscita del router appropriata (_Piano dei Dati_)
- <font color="redf">Instradamento:</font> determinare il percorso seguito dai pacchetti dalla sorgente alla destinazione (_Piano di Controllo_)

Due approcci alla strutturazione del piano di controllo della rete:
▪ controllo per router (tradizionale)
▪ controllo logicamente centralizzato (software defined networking)

## Piano di Controllo del router

I singoli componenti dell'algoritmo di instradamento in ogni router interagiscono nel piano di controllo.![[Pasted image 20240513191536.png]]

## Piano di controllo Software-Defined-Network

 Un controllore remoto calcola le tabelle di inoltro e le installa nei router![[Pasted image 20240513191638.png]]


# Algoritmi di Instradamento

<font color="red">Obiettivo</font> degli algoritmi di instradamento: determinare percorsi o cammini "buoni" tra le sorgenti e i destinatari attraverso la rete di router

- <font color="Greeen">Percorso:</font> sequenza di router che i pacchetti attraversano dall'host di origine all'host di destinazione (ovviamente, presupponendo l'attraversamento di un collegamento tra un nodo e il successivo)

- <font color="Greeen">"Buono":</font> costo” minimo (anche se occorre considerare ulteriori vincoli, spesso determinati da policy degli amministratori delle reti)

-  <font color="Greeen">Instradamento:</font> uno dei "primi 10" problemi nelle reti!![[Pasted image 20240513192055.png]]


# Astrazione: grafo

![[Pasted image 20240513192127.png]]

Grafo. G = (_N_,_E_)
_N_: insieme di router = {u , v, w, x, y, z}
_E_: insieme di collegamenti = {(u,v), (u,x), (v,x), (v,w), (x,w), (x,y), (w,y), (w,z), (y,z)}

$\large{c_{a,b}}$: costo del collegamento diretto che connette a e b. 
Esempio: $\large{c_{w,z} = 5}$ ( z è adiacente o vicino), $\large{c_{u,z}} = \infty$ 

costo definito dall'operatore di rete: potrebbe essere sempre 1, o proporzionale alla lunghezza fisica di un collegamento (ritardo di propagazione), o inversamente correlato alla larghezza di banda, o inversamente correlato alla congestione.
Il costo di un percorso è uguale alla somma dei costi dei collegamenti attraversati.


# Classificazione degli algoritmi di instradamento 

- Quanto velocemente cambiano i percorsi?
	- <font color="red">Statici:</font> i percorsi cambiano lentamente nel tempo (magari con un intervento umano)
	- <font color="red">Globali:</font> calcolo logicamente centralizzato o replicato sui tutti router, basato sulla conoscenza _completa_ della topologia e del costo dei collegamenti
		- algoritmi “_link state_”
	- <font color="red">Decentralizzati:</font> processo di calcolo iterativo, basato sullo scambio di informazioni tra vicini
		- inizialmente i router conoscono solo il costo dei collegamenti ai loro vicini
		- algoritmi “distance vector”
	- <font color="red">Dinamici:</font> i percorsi cambiano più velocemente
		- aggiornamenti periodici o in risposta a cambiamenti del costo dei collegamenti (e delle topologia)

- <font color="yellow">Sensibili al carico:</font> il costo dei collegamenti riflette il livello corrente di congestione (es. correlato al ritardo di accodamento)
- <font color="yellow">Insensibili al carico:</font> il costo dei collegamenti non riflette il livello corrente (o recente) di congestione

# Instradamento "link-state": algoritmo di Dijkstra

- _Centralizzato_: la topologia della rete e il costo dei collegamenti sono noti a tutti i nodi
	- Informazioni ottenute attraverso un algoritmo di “link state broadcast”
	- tutti i nodi hanno le stesse informazioni
- calcola i percorsi di costo minimo da un nodo (“sorgente”) a tutti gli altri nodi
	- dà la tabella di inoltro per quel nodo
- _iterativo_: dopo k iterazioni, conosce il cammino di costo minimo verso k destinazioni

- **notazione**:
	- $\large{C_{x,y}}$ : costo del collegamento diretto dal nodo x al nodo y; 
	  = ∞ se non sono vicini diretti 
	- $\large{D(v)}$ : stima _corrente_ del costo minimo del percorso dalla sorgente alla destinazione v
	- $\large{p(v)}$ : immediato predecessore di v lungo il percorso a costo minimo dall'origine a v
	- $\large{N'}$ : sottoinsieme di nodi contenente tutti (e solo) i nodi v per cui il percorso a costo minimo dall'origine a v è _definitivamente_ noto
	![[Pasted image 20240513193624.png]]

### Esempio di Dijkstra

![[Pasted image 20240513193704.png]]![[Pasted image 20240513193733.png]]![[Pasted image 20240513193753.png]]![[Pasted image 20240513193809.png]]![[Pasted image 20240513193828.png]]![[Pasted image 20240513193845.png]]![[Pasted image 20240513193903.png]]![[Pasted image 20240513193919.png]]![[Pasted image 20240513193933.png]]![[Pasted image 20240513193951.png]]
![[Pasted image 20240513194009.png]]
![[Pasted image 20240513194040.png]]![[Pasted image 20240513194110.png]]

- **Discussione**: 
	- <font color="red">Complessità algoritmica:</font> _n_ nodi (senza contare l'origine)
		- ciascuna delle _n_ iterazioni: deve controllare tutti i nodi, _w_, non in _N_ per determinare quello avente il costo minimo: n nodi, n – 1 nodi, …, 1
		- complessità $O(n^2)$
		- sono possibili implementazioni più efficienti: $O(nlogn)$ usando uno _heap_
	- <font color="red">Complessità dei messaggi:</font> _n_ nodi
		- ogni router deve trasmettere in broadcast le proprie informazioni sullo stato dei collegamenti agli altri _n_ router
		- algoritmi di broadcasting efficienti (e interessanti!): $O(n)$ attraversamenti dei collegamenti per diffondere un messaggio di broadcasting da una sorgente
		- il messaggio di ogni router attraversa $O(n)$ collegamenti: complessità dei messaggi complessiva: $O(n^2)$

- quando i costi dei collegamenti dipendono dal volume di traffico, sono possibili **oscillazioni dei percorsi**.
- scenario di esempi:
	-  instradamento verso _a_: i nodi _b_, _d_ e _c_ trasmettono rispettivamente con tasso 1, 1, e (<1)
	- Il costo dei collegamenti è direzionale dipendente dal traffico         
![[Pasted image 20240513194611.png]]


# Algoritmo distance vector

Basato sulla equazione di Bellman-Ford (BF) (programmazione dinamica): 
Sia $\large{d_x(y)}$: il costo del percorso di costo minimo da _x_ a _y_. Allora: $\large{d_x(y) = min_v(c_{x,v}+d_v(y))}$ 

Dove:
- $min_v$ : è calcolato su tutti i vicini _v_ di _x_
- $c_{x,v}$ : costo diretto del collegamento da _x_ a _v_
- $d_v(y)$ : costo del cammino minimo da _v_ a _y_

### Esempio Bellman-Ford

Si supponga che i nodi vicini di _u_, _x_, _v_, _w_, sappiano che per la destinazione _z_:![[Pasted image 20240513195346.png]]il nodo che raggiunge il minimo (x) è l'hop successivo sul percorso a costo minimo stimato verso la destinazione (z)

<font color="green">Idea chiave:</font>
- di tanto in tanto, ogni nodo invia ai vicini il proprio vettore delle distanze (stimate), distance vector in inglese
- quando x riceve un DV da un qualsiasi vicino, aggiorna la propria DV utilizzando l'equazione B-F: $$\large{D_x(y) \leftarrow min_v (c_{x,v})+D_v(y)}$$Per ogni nodo $\large{\forall y \in N}$
  - sotto certe condizioni minori e naturali, la stima $D_x(y)$ _converge_ _verso l'effettivo costo minimo_ $d_x(y)$

![[Pasted image 20240514100952.png]]
<font color="yellow">Iterativo asincrono:</font> ciascuna iterazione locale causata:
- cambiamento del costo del collegamento locale
- messaggio di aggiornamento del DV da un vicino
<font color="yellow">Distribuito, auto-terminante:</font> ciascun nodo notifica i vicini solo quando la sua DV cambia
- i vicini notificano i loro vicini - solo se necessario.
- nessuna notifica ricevuta, nessuna azione intrapresa!

### Distance Vector: esempio
![[Pasted image 20240514101218.png]]
![[Pasted image 20240514101238.png]]![[Pasted image 20240514101258.png]]![[Pasted image 20240514101316.png]]![[Pasted image 20240514101331.png]]![[Pasted image 20240514101346.png]]![[Pasted image 20240514101402.png]]![[Pasted image 20240514101425.png]]![[Pasted image 20240514101442.png]]![[Pasted image 20240514101459.png]]![[Pasted image 20240514101523.png]]![[Pasted image 20240514101542.png]]![[Pasted image 20240514101611.png]]

# Vettore delle distanze: cambiamento del costo dei collegamenti 

![[Pasted image 20240514101713.png]]

 - un nodo rileva la modifica del costo del collegamento locale
- aggiorna le informazioni di instradamento, ricalcola il DV locale
- se il DV cambia, avvisa i vicini
- _Le buone notizie viaggiano velocemente_:
	- $t_0$ : _y_ rileva la variazione del costo del collegamento, aggiorna il suo DV, informa i suoi vicini.
	- $t_1$ : _z_ riceve l'aggiornamento da _y_, aggiorna la sua DV, calcola il nuovo costo minimo per _x_ e invia ai suoi vicini la sua DV.
	- $t_2$ : _y_ riceve l'aggiornamento di _z_ e aggiorna il proprio DV. I costi minimi di _y_ non cambiano, quindi _y_ non invia un messaggio a _z_.
- _Le cattive notizie viaggiano lentamente_:
	- ![[Pasted image 20240514102637.png]]
	- $t_0$: _y_ vede che il collegamento diretto con _x_ ha un nuovo costo 60, ma _z_ ha detto che ha un percorso di costo 5. Pertanto, _y_ calcola “il mio nuovo costo verso _x_ sarà 6, tramite _z_"; notifica _z_ del nuovo costo 6 verso _x_.
	- $t_1$: _z_ apprende che il percorso verso _x_ tramite _y_ ha il nuovo costo 6, pertanto _z_ calcola “il mio nuovo costo verso _x_ sarà 7 tramite _y_", notifica _y_ del nuovo costo di 7 verso _x_.
	- $t_2$: _y_ apprende che il percorso verso _x_ tramite _z_ ha il nuovo costo 7, pertanto _y_ calcola “il mio nuovo costo verso _x_ sarà 8 tramite _y_", notifica _z_ del nuovo costo di 8 verso _x_.
	- $t_3$: _z_ apprende che il percorso verso _x_ tramite _y_ ha il nuovo costo 8, pertanto _z_ calcola “il mio nuovo costo verso _x_ sarà 9 tramite _y_", notifica _y_ del nuovo costo di 9 tramite _x_.
… così fino a $t_{45}$ quando _z_ non consideri il collegamento diretto con _x_ più conveniente del percorso tramite _y_, rompendo l'instradamento ciclico e nell'iterazione successiva anche la stima della distanza di _y_ converge. 

![[Pasted image 20240514103125.png]]
![[Pasted image 20240514103233.png]]
![[Pasted image 20240514103341.png]]![[Pasted image 20240514103356.png]]![[Pasted image 20240514103410.png]]![[Pasted image 20240514103436.png]]