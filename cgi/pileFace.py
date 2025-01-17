#!/usr/bin/python3

# Ajout du header HTTP obligatoire
# print("Content-type: text/html\n")

# Contenu HTML avec le même style que tes autres pages
html = """<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Pile ou Face</title>
    <style>
        /* Styles communs */
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: Arial, sans-serif;
        }

        body {
            line-height: 1.6;
            color: #333;
            background-color: #f4f4f9;
        }

        .banner {
            background-color: #4CAF50;
            color: white;
            text-align: center;
            padding: 1rem;
            font-size: 1.5rem;
        }

        nav ul {
            list-style: none;
            background: #444;
            display: flex;
            justify-content: center;
            padding: 0.5rem 0;
        }

        nav ul li {
            margin: 0 1rem;
        }

        nav ul li a {
            color: #fff;
            text-decoration: none;
            padding: 0.5rem 1rem;
            display: block;
        }

        nav ul li a:hover {
            background: #555;
        }

        main {
            padding: 2rem;
            text-align: center;
        }

        .game-container {
            margin-top: 2rem;
        }

        .result {
            margin-top: 1.5rem;
            font-size: 2rem;
            font-weight: bold;
            color: #333;
        }

        .message {
            font-size: 1.5rem;
            margin-top: 1rem;
        }

        .button-container {
            margin-top: 1.5rem;
        }

        button {
            padding: 1rem 2rem;
            font-size: 1.5rem;
            font-weight: bold;
            background: #4CAF50;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 0 10px;
        }

        button:hover {
            background: #45a049;
        }

        footer {
            background: #333;
            color: white;
            text-align: center;
            padding: 1rem;
            position: fixed;
            bottom: 0;
            width: 100%;
        }
    </style>
    <script>
        function jouer(choixUtilisateur) {
            let resultat = Math.random() < 0.5 ? "Pile" : "Face";
            let message = (choixUtilisateur === resultat) ? "Tu as gagné ! 🎉" : "Tu as perdu... 😢";

            document.getElementById("resultat").innerText = "Résultat : " + resultat;
            document.getElementById("message").innerText = message;

            // Changer les boutons après le choix
            document.getElementById("buttons").innerHTML = `
                <button onclick="reessayer()">Réessayer</button>
            `;
        }

        function reessayer() {
            document.getElementById("resultat").innerText = "";
            document.getElementById("message").innerText = "Faites votre choix et lancez le jeu !";
            document.getElementById("buttons").innerHTML = `
                <button onclick="jouer('Pile')">Pile</button>
                <button onclick="jouer('Face')">Face</button>
            `;
        }
    </script>
</head>
<body>
    <!-- Bannière -->
    <div class="banner">
        Jouez à Pile ou Face !
    </div>

    <!-- Menu -->
    <nav>
        <ul>
            <li><a href="/">Accueil</a></li>
            <li><a href="/televers">Téléverser</a></li>
            <li><a href="/cgi">CGI</a></li>
            <li><a href="/contact">Contact</a></li>
        </ul>
    </nav>

    <!-- Contenu principal -->
    <main>
        <h2>Faites votre choix :</h2>
        <div class="game-container">
            <div id="buttons">
                <button onclick="jouer('Pile')">Pile</button>
                <button onclick="jouer('Face')">Face</button>
            </div>
        </div>

        <div class="result" id="resultat"></div>
        <div class="message" id="message">Faites votre choix et lancez le jeu !</div>
    </main>

    <!-- Pied de page -->
    <footer>
        &copy; 2024 Mon Site Web. Tous droits réservés.
    </footer>
</body>
</html>
"""

print(html)