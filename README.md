# Raven 2022

![image](https://user-images.githubusercontent.com/41454550/208022004-ef6890ee-5cdc-42ec-9fc3-4fb06310f063.png)
![image](https://user-images.githubusercontent.com/41454550/208022281-f83c702a-c804-46b9-8c16-76f35afc827a.png)

## A

### Modification de Weapon_RocketLauncher
Ajout de règles dans InitializeFuzzyModule pour avoir 5 sets de règles logiques.

## B

### Modification de Raven_WeaponSystem

Ajout d'une méthode InitializeFuzzyModule pour définir les variables flou permettant de rendre plus réel la précision des agents.
Ajout / Modification de la méthode AddNoizeToAim afin de modifier en fonction de la variable Defuzzifier Accuracy défini précédemment la visé.

## C

### Création de la class Raven_Team 
Permet de créer des équipes (Modifier NumTeams dans Params.in pour plus d'équipes, ORIGINALITE).

Fonctionnement La target de la team est défini par rapport au leader. Il s'agit du bot le plus proche de lui. Les bot sont notifiés via le message (Msg_TeamTarget) de la cible de l'équipe.

### Modification de Raven_Game

#### Création de la méthode AddTeams 
Pour créer les équipes au lancement du jeu. Un bot point de spawn d'armes est placé aléatoirement sur la map pour chaque équipe (ORIGINALITE et DIFFICLE).

#### Modification de AddBots
Répartition des bots dans les équipes de façon dynamique.
Création de la class Trigger_TeamWeaponGiver pour permettre aux agents mourants de déposer leurs armes au point de spawn d'arme de son équipe.

## D

### Création d'une class Raven_Learner 
Hérite de Raven_Bot, variante qui permet de rendre un agent apprenant (activer l'apprentissage en passant Bot_Learner a 1 dans Params.ini).

### Création d'un singleton Neural dans Raven_Learner 
Permet de démarrer un thread d'apprentissage et de mettre à jour les échantillons d'apprentissage pour tous les bots apprenant.

### Création d'un message Msg_ShootHit
Permet d'appeler OnShootIt après que le bot tire et que sont projectile touche ou non sa cible. Le Bot_Learner exploite cette méthode pour mettre à jour les données d'apprentissage.

En effet nous avons choisis que le bot apprenne à tirer en fonction de si oui ou non un agent a déjà été dans la même situation et a réussi à toucher sa cible, et non si il a tirer. Ce qui permet de maximiser sa précision.

## E

### Création d'un nouveau but composite, Goal_FollowTeam
Permet aux agents de suivre leur équipe. La désirabilité de ce but est définie par la class FollowTeamGoal_Evaluator.

## F

### F.1 

#### Création de l'arme GrenadeLauncher

Ajout de la class Weapon_GrenadeLaucher.
Ajout de la class Projectile_Grenade.

### F.2 

#### Création du but composite Goal_FollowTeam
Permet aux membres d'une équipe de se positionner les uns des autres. Ils peuvent ainsi protéger leur leader.

### F.3
Une stratégie d'attaque guidée a été implémentée à l'aide des éléments cités précédemment à savoir, Le but composite Goal_FollowTeam et la stratégie de définition de la target d'une team.
En effet, les membres d'une équipe vont essayer de se regrouper, et vont tous essayer d'attaquer l'agent le plus proche de leur leader. Le nombre faisant la force. (ORIGINALITÉ).

### F.4
### F.5
