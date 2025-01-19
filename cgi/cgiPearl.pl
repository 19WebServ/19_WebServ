#!/usr/bin/perl
use strict;
use warnings;
use utf8;
use CGI;

my $cgi = CGI->new;

# Generation de la page HTML
print <<'HTML';
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Generateur de Phrase</title>
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

        .form-container {
            margin-top: 2rem;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 10px;
        }

        input, button {
            padding: 10px;
            font-size: 1rem;
            border: 1px solid #ccc;
            border-radius: 5px;
        }

        button {
            background: #4CAF50;
            color: white;
            font-weight: bold;
            cursor: pointer;
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
        function generateSentence() {
            var subject = document.getElementById('subject').value || 'Le chat';
            var verb = document.getElementById('verb').value || 'mange';
            var complement = document.getElementById('complement').value || 'une pomme';

            var phrases = [
                subject + ' ' + verb + ' ' + complement + '.',
                subject + ' adore ' + verb + ' ' + complement + '.',
                'Parfois, ' + subject + ' ' + verb + ' aussi ' + complement + '.',
                'Il arrive que ' + subject + ' ' + verb + ' tranquillement ' + complement + '.',
                subject + ' n\'a jamais cesse de ' + verb + ' ' + complement + '.',
                subject + ' aime bien ' + verb + ' ' + complement + ' tous les jours.',
                'Un jour, ' + subject + ' a decide de ' + verb + ' ' + complement + '.',
                subject + ' ne peut pas s\'empecher de ' + verb + ' ' + complement + '.',
                'Chaque matin, ' + subject + ' commence par ' + verb + ' ' + complement + '.',
                subject + ' trouve du plaisir a ' + verb + ' ' + complement + '.',
                'Pourquoi ' + subject + ' veut-il toujours ' + verb + ' ' + complement + ' ?',
                subject + ' ne sait pas comment arreter de ' + verb + ' ' + complement + '.',
                'Hier encore, ' + subject + ' etait en train de ' + verb + ' ' + complement + '.',
                'Tout le monde sait que ' + subject + ' adore ' + verb + ' ' + complement + '.',
                subject + ' pretend ne pas aimer ' + verb + ' ' + complement + ', mais c\'est faux.',
                'Dans ses reves, ' + subject + ' ' + verb + ' ' + complement + ' sans arret.',
                'Meme sous la pluie, ' + subject + ' continue de ' + verb + ' ' + complement + '.',
                subject + ' essaye toujours de mieux ' + verb + ' ' + complement + '.',
                'Il parait que ' + subject + ' ne sait pas bien ' + verb + ' ' + complement + '.',
                'Avec un peu d\'effort, ' + subject + ' pourrait ' + verb + ' ' + complement + ' encore mieux.'
            ];

            var randomSentence = phrases[Math.floor(Math.random() * phrases.length)];

            document.getElementById('generatedSentence').innerHTML = '<strong>' + randomSentence + '</strong>';
        }
    </script>
</head>
<body>
    <!-- Bannière -->
    <div class="banner">
        Generateur de Phrase
    </div>

    <!-- Menu -->
    <nav>
        <ul>
            <li><a href="/">Accueil</a></li>
            <li><a href="/televers">Televers</a></li>
            <li><a href="/cgi">CGI</a></li>
            <li><a href="/redirect">Redirect</a></li>
        </ul>
    </nav>

    <!-- Contenu principal -->
    <main>
        <h2>Entrez des mots pour generer une phrase aleatoire</h2>
        <div class="form-container">
            <input type="text" id="subject" placeholder="Saisis un sujet" required>
            <input type="text" id="verb" placeholder="Saisis un verbe" required>
            <input type="text" id="complement" placeholder="Saisis un complement" required>
            <button onclick="generateSentence()">Generer une phrase</button>
        </div>
        <h2>Phrase generee :</h2>
        <p id="generatedSentence"></p>
    </main>

    <!-- Pied de page -->
    <footer>
        &copy; 2024 Mon Site Web. Tous droits reserves.
    </footer>
</body>
</html>
HTML