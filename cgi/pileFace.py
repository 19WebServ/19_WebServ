#!/usr/bin/python3

import os
import sys
import random
import urllib.parse  # Remplace cgi.parse_qs

# Détermination de la méthode HTTP
method = os.environ.get("REQUEST_METHOD", "GET")

# Récupération des données selon la méthode
choix_utilisateur = None

if method == "GET":
    query_string = os.environ.get("QUERY_STRING", "")
    params = urllib.parse.parse_qs(query_string)  # ✅ Fix ici
    choix_utilisateur = params.get("choix", [None])[0]

elif method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    post_data = sys.stdin.read(content_length) if content_length > 0 else ""
    params = urllib.parse.parse_qs(post_data)  # ✅ Fix ici aussi
    choix_utilisateur = params.get("choix", [None])[0]

# Vérification si un choix a été fait
if choix_utilisateur in ["Pile", "Face"]:
    resultat = random.choice(["Pile", "Face"])
    message = "Tu as gagné ! 🎉" if choix_utilisateur == resultat else "Tu as perdu... 😢"
else:
    # Aucune sélection, on affiche juste les boutons
    resultat = ""
    message = "Faites votre choix et lancez le jeu !"

# Envoi du header HTTP obligatoire
# print("Content-type: text/html\r\n")

# Contenu HTML simplifié pour test rapide
html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Pile ou Face</title>
</head>
<body>
    <h1>Pile ou Face</h1>
    <p>{message}</p>
    <form action="/cgi/pileFace.py" method="get">
        <button type="submit" name="choix" value="Pile">Pile</button>
        <button type="submit" name="choix" value="Face">Face</button>
    </form>
    <p>{("Résultat : " + resultat) if resultat else ""}</p>
</body>
</html>
"""

print(html)