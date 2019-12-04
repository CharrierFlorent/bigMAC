# bigMAC
Les systèmes de résolution de CSP s’appuient traditionnellement sur des algorithme d’exploration d’arborescences de recherche basés sur la notion de « backtracking » (cf. FC, RFL, MAC…). On va explorer une piste originale qui consiste à découper les domaines en couples de valeurs. Ainsi, à chaque nœud développé dans l’arbre de recherche, plutôt que de choisir une valeur dans le domaine de la variable à affecter, on choisira un couple de valeurs pour cette variable. Cette nouvelle approche conduit alors à explorer un espace de recherche dont la taille sera ramenée à O(d^n/2^n). 

Cette approche fonctionne grace à un resultat sur les CSP qui nous dis que tout CSP binaire bivalent peut être résolu en temps polynomial si ce CSP est SPC (Arc-Consistent + Path-Consistent)

## Installation & Utilisation
Codé en C. Compile sous gcc.

make pour compiler le projet.

./bigmac <nb variables> <taille max domaine> <densité> <durete> <option> pour executer le programme sur un csp généré aléatoirement
  
./bigmac -f <path/to/file> <option> pour executer le programme sur un fichier
  
  Option : -v : Verbal, affiche differentes informations sur la résolution

## Contribution

## Auteurs

## License

## Remerciements
