# Fotomosaico


Use make para compilar o programa

make clean para limpar arquivos temporários 

make purge para limpar temporários e executável

executar com ./mosaico -i <Foto de entrada> -o <arquivo de saida> -p <diretorio de tiles>
  
  

O programa divide a foto original em diversas partes e para cada parte calcula a média da cor dominante, então substitui essa parte por uma tile com a média de cor 
dominante mais próxima. O resultado é um fotomosaico de tiles.
  
  
  
O programa aceita formatos de foto .ppm tanto P3 como P6, fotos P6 tem melhor eficiência pois são arquivos binários. 
Para melhor efeito de mosaico use fotos de qualidade 4k para cima.
