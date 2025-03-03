#!/usr/bin/perl
use strict;
use warnings;
use utf8;

# Lire les variables d'environnement
my $method = $ENV{'REQUEST_METHOD'} || "GET";
my $query_string = $ENV{'QUERY_STRING'} || "";

# Fonction pour extraire les paramètres d'une chaîne query
sub parse_query {
    my ($query) = @_;
    my %params;
    foreach my $pair (split /&/, $query) {
        my ($key, $value) = split /=/, $pair, 2;
        next unless defined $key and defined $value;
        $value =~ tr/+/ /;
        $value =~ s/%([a-fA-F0-9]{2})/chr(hex($1))/eg;
        $params{$key} = $value;
    }
    return %params;
}

# Extraire les paramètres
my %params = parse_query($query_string);
my $subject = $params{'subject'} || '';
my $verb = $params{'verb'} || '';
my $complement = $params{'complement'} || '';

# Verifier si tous les champs sont remplis
if ($subject eq '' || $verb eq '' || $complement eq '') {
    print <<"HTML";
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Entree des mots</title>
    <style>
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
            text-align: center;
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
        .container {
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1);
            display: inline-block;
            margin-top: 2rem;
        }
        input[type="text"] {
            padding: 8px;
            margin: 10px;
            border-radius: 5px;
            border: 1px solid #ccc;
        }
        input[type="submit"] {
            padding: 10px 20px;
            background-color: #28a745;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #218838;
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
</head>
<body>
    <div class="banner">Generateur de phrases</div>
    <nav>
        <ul>
            <li><a href="/">Accueil</a></li>
            <li><a href="/cgi">CGI</a></li>
            <li><a href="/dirList">Directory listing</a></li>
        </ul>
    </nav>
    <div class="container">
        <h1>Veuillez entrer trois mots :</h1>
        <form action="/cgi/cgiPearl.pl" method="get">
            <label for="subject">Sujet :</label>
            <input type="text" name="subject" required><br>
            <label for="verb">Verbe :</label>
            <input type="text" name="verb" required><br>
            <label for="complement">Complement :</label>
            <input type="text" name="complement" required><br>
            <input type="submit" value="Generer">
        </form>
    </div>
    <footer>&copy; 2024 Mon Site Web. Tous droits reserves.</footer>
</body>
</html>
HTML
    exit;
}

# Generer une phrase aleatoire
my @phrases = (
    "$subject $verb $complement.",
    "$subject adore $verb $complement.",
    "Parfois, $subject $verb aussi $complement.",
    "$subject aime bien $verb $complement tous les jours.",
    "Un jour, $subject a decide de $verb $complement.",
    "$subject ne peut pas s'empecher de $verb $complement.",
    "Hier encore, $subject etait en train de $verb $complement.",
    "Avec un peu d'effort, $subject pourrait $verb $complement encore mieux."
);
my $random_sentence = $phrases[int(rand(@phrases))];

# Generer la reponse HTML
print <<"HTML";
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Phrase generee</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; }
        .banner { background-color: #4CAF50; color: white; padding: 1rem; font-size: 1.5rem; }
        .container { background: white; padding: 20px; border-radius: 10px; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); display: inline-block; margin-top: 2rem; }
        footer { background: #333; color: white; text-align: center; padding: 1rem; position: fixed; bottom: 0; width: 100%; }
    </style>
</head>
<body>
    <div class="banner">Generateur de phrases</div>
    <div class="container">
        <h1>Votre phrase generee :</h1>
        <p><strong>$random_sentence</strong></p>
        <a href="/cgi/cgiPearl.pl" class="btn">Recommencer</a>
    </div>
    <footer>&copy; 2024 Mon Site Web. Tous droits reserves.</footer>
</body>
</html>
HTML
