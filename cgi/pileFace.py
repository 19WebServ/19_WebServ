#!/usr/bin/python3

import os
import random
import urllib.parse

# Détermination de la méthode HTTP
method = os.environ.get("REQUEST_METHOD", "GET")

# 🔥 Récupération des données dans l'environnement (pas de stdin)
query_string = os.environ.get("QUERY_STRING", "")

# 🔥 Décodage des paramètres (GET ou POST)
params = urllib.parse.parse_qs(query_string)
choix_utilisateur = params.get("choix", [None])[0]

# Vérification si un choix a été fait
if choix_utilisateur in ["Pile", "Face"]:
    resultat = random.choice(["Pile", "Face"])
    message = "🎉 Tu as gagné !" if choix_utilisateur == resultat else "😢 Tu as perdu..."
else:
    resultat = ""
    message = "Faites votre choix et lancez le jeu !"

while True:
    print("coucou")

# Contenu HTML amélioré
html = f"""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Pile ou Face</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            text-align: center;
            background-color: #f4f4f4;
            margin: 50px;
        }}
        .container {{
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
        }}
        h1 {{
            color: #333;
        }}
        p {{
            font-size: 18px;
            color: #555;
        }}
        .buttons {{
            margin-top: 20px;
        }}
        button {{
            font-size: 18px;
            padding: 10px 20px;
            margin: 5px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: 0.3s;
        }}
        .pile {{
            background-color: #007bff;
            color: white;
        }}
        .face {{
            background-color: #28a745;
            color: white;
        }}
        .reload {{
            background-color: #dc3545;
            color: white;
        }}
        button:hover {{
            opacity: 0.8;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>🎲 Pile ou Face</h1>
        <p>{message}</p>
        <p>{("🪙 Résultat : <strong>" + resultat + "</strong>") if resultat else ""}</p>
        <div class="buttons">
            <form action="/cgi/pileFace.py" method="get">
                <button type="submit" name="choix" value="Pile" class="pile">Pile</button>
                <button type="submit" name="choix" value="Face" class="face">Face</button>
            </form>
        </div>
        <br>
        <button class="reload" onclick="window.location.href='/cgi'">🔄 Back to CGI</button>
    </div>
</body>
</html>
"""

print(html)