# Utilização
## Execução
Para execução basta compilar e excutar o bináriogerado informando uma interface de rede.
   ```sh 
   gcc main.c && sudo ./a.out <IF_NET>
   ```
## Visualização das estatisticas
A visualização das estatísticas foi feita utilizando html e css para que fosse possível visualizar no navegador, para isso algumas dependencias são necessárias
   - Requisitos:
      - [nodejs](https://nodejs.org/en/)
      - [browsersync](https://www.browsersync.io/) 
      -
      ```sh
	  npm install -g browser-sync
	  ```
      - [bower](https://bower.io/)
      
      ```sh
	  npm install -g bower
	  ```
   - Instalar dependencias da visualização:
   
      ```sh
      bower install
      ```
   - Visualização com atualizalção a cada segundo
   
   ```sh
    browser-sync start --server --files "*.html" --index "main.html"
    ```
