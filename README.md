# Projeto final - TR2 2024.1

Projeto acessível em: https://github.com/JabutiPatriota/tr2_project/tree/main
O presente projeto visa implementar um carro autônomo, cujos comandos (ir para frente, para trás, virar à esquerda e virar à direita) podem ser enviados a partir de requisições http (por meio de qualquer navegador de internet conectado à mesma rede do carro).

## Funcionamento

Antes de ligar o carro pela primeira vez, é essencial configurar o código fonte com o nome da rede Wi-Fi que será utilizada e com a respectiva senha (linhas 7 e 8 do código fonte), para, em seguida, compilá-lo e fazer o upload na placa NodeMCU ESP8266. Com isso, ainda com a placa conectada ao computador, será impresso no monitor serial o endereço IP que pode ser utilizado por qualquer outro aparelho conectado à rede para enviar comandos ao carro. Feito isso, o carro pode ser desconectado do computador e ligado, ao que o LED piscará duas vezes seguidas tão logo a placa se conecte ao Wi-Fi em questão. Em seguida, basta conectar-se (utilizando um computador ou aparelho celular, por exemplo) ao endereço IP informado pela própria placa por meio do monitor serial, e enviar comandos para que o carro se mova (ou pare, se estiver se movendo). Ao se deparar com um anteparo, ele irá convergir à direita até que a distância captada pelo sensor seja maior que sete centímetros (7 cm), conforme demonstrado no vídeo IMG_5852.MOV. Conforme o carro se movimenta, os dados lidos pelo sensor de proximidade e a taxa RSSI de conexão medida pela placa são enviados ao servidor da plataforma ThingSpeak, conforme fotos ilustrativas nesse repositório.

## Implementação

A implementação foi toda feita no mesmo arquivo, ˜projeto_tr2_miguel_luna.ino˜. O código é utilizado tanto para controlar os motores por meio da shield L298N, como para fazer as medições do sensor de proximidade e enviá-las para o ThingSpeak (juntamente com os dados de RSSI). O mesmo código, ainda, realiza ˜printscreen" da página HTML mostrada ao usuário para que comande o carro. A placa realiza esse procedimento por meio da biblioteca ESP8266WiFi, a qual instancia um servidor na porta 80 e instancia um cliente com um endereço específico que o acessa (esse que é mostrado ao usuário para que o utilize). Ao receber requisições, a placa analisa o endpoint recebido (há um endpoint para cada botão) e, a partir dele, envia comandos aos motores do carro.

## Faltou

Por mais que pesquisasse intensamente, não fui capaz de descobrir como ter acesso aos logs da placa para conseguir monitorar os pacotes transmitidos, recebidos e perdidos, bem como a taxa de latência e retransmissão.

## Referências

Em geral, o código, bem como as ideias de melhoria, foram retiradas dos próprios exemplos da biblioteca da plataforma arduino, bem como das "issues" no fórum da plataforma arduino. Além disso, foi utilizada a plataforma "ThingSpeak", na qual os dados podem ser vistos. A comunicação com tal plataforma foi feita com base na "lib" "ThingSpeak.h". A implementação da comunicação entre a placa NodeMcu e a shield de controle dos motores foi inspirada no site "https://www.instructables.com/".
