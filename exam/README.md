# Compilation et exécution

Il n'y a pas de compilation, c'est un script python.
Pour l'exécution : `python3 exam.py`.

# Réponses aux questions
Dans chacune des questions, si le contraire n'est pas précisé, on se place dans le cas N = 24.
De plus, par simplification, on appellera simplement h la fonction de hachage utilisée, même si elle dépend de N.

## Question 1
Quel que soit le x<sub>0</sub> de départ, au bout de 2<sup>N</sup> itérations on a  2<sup>N</sup>+1 éléments dans la suite, tous  dans {0,...,2<sup>N</sup>-1} donc au moins 2 d'entre eux sont égaux, notons les x<sub>i</sub> et x<sub>j</sub> (i<j) et (x<sub>k</sub>)<sub>k $\geq$ i</sub> est periodique de periode divisant (j-i). En effet pour k $\geq$ i, x<sub>k</sub> = x<sub>i+q(j-i)+r</sub> = x<sub>i+r</sub>  où q(j-i)+r correspond à la division euclidienne de (k-i) par (j-i).

## Question 2
En partant de x<sub>0</sub> = 0 et en itérant l'algorithme de Floyd (dit "du lièvre et de la tortue"), on trouve une collision pour x<sub>i</sub> = x<sub>2i</sub> =  16685296 (avec i =  2232)

## Question 3
Pour trouver la periode à partir de la collision précédente on part du x<sub>i</sub> correspondant à la collision trouvée à la [question 2](README.md#Question%202), et on itère jusqu'à trouver un x<sub>j</sub> tel que  x<sub>i</sub> = x<sub>j</sub>. On pose alors l = j - i.
La periode trouvée ici est de l =  1116. (En partant de x<sub>0</sub> = 0 )

## Question 4
La fonction de hachage utilisée n'utilise que les N premiers bits de SHA256 donc trouver une collision revient à trouver une collision sur les N premiers bits de SHA256.
Dans cette question, pour tous les cas suivants on pose  x<sub>0</sub> = 0.
On part à chaque fois de x<sub>0</sub> et on itère jusqu'à trouver x<sub>i</sub> $\neq$ x<sub>i+l</sub> avec h(x<sub>i</sub>) =  h(x<sub>i+l</sub>).
### Cas N=24
l =  1116
x<sub>i</sub> =  16678005
x<sub>i+l</sub> =  12629779
h(x<sub>i</sub>) =  h(x<sub>i+l</sub>) =  14712708
### Cas N=32
l =  21113
x<sub>i</sub> =  4188217710
x<sub>i+l</sub> =  110360111
h(x<sub>i</sub>) =  h(x<sub>i+l</sub>) =  4045203601
### Cas N=40
l =  991353
x<sub>i</sub> =  152922203826
x<sub>i+l</sub> =  845850135587
h(x<sub>i</sub>) =  h(x<sub>i+l</sub>) =  1038512769756
### Cas N=48
l =  8991387
x<sub>i</sub> =  198256891920989
x<sub>i+l</sub> =  227635672087753
h(x<sub>i</sub>) =  h(x<sub>i+l</sub>) =  278035943461396

## Question 5
On applique ici les algorithmes des questions précédentes à la différence près qu'on utilise h composé avec g au lieu de juste h.
On teste différentes valeurs de x<sub>0</sub> jusqu'à avoir une collision entre M0 et M1 (en effet avoir une collision entre deux fois M0 ou deux fois M1 est assez inutile). 
Mon programme a trouvé une collision en partant de x<sub>0</sub> =  4924023.
La collision a lieu pour :
x<sub>i</sub> =  12476919
x<sub>i+l</sub> =  7529636
h(g(x<sub>i</sub>)) =  10501133
h(g(x<sub>i</sub>)) =  10501133
M<sub>i</sub> = g(x<sub>i</sub>) = AlicecERtiFIEQuelleDoitDIXMILLEDEUXCENTSEUROSETTRENTECENTIMESABOB
M<sub>i+l</sub> = g(x<sub>i+l</sub>) = aliCEcErtiFIeQUeLlEDoITNEUFEUROSETTRENTECENTIMESABOB
h(M<sub>i</sub>) =  10501133
h(M<sub>i+l</sub>) =  10501133









