# Mandelbrot Fractal

Trabalho do Grau B da disciplina Processamento de Alto Desempenho. Unisinos, 2020.

O objetivo da aplicação é calcular de forma distribuída o fractal de mandelbrot, com uso de várias threads através da biblioteca POSIX Threads (Pthreads).

A aplicação enquadra-se no padrão SIMD (Single Instruction Multiple Data).

## Como funciona

A ideia é dividir a imagem do fractal em vários setores, definidos pelas constantes `DIV_HEIGHT` e `DIV_WIDTH`, que definem quantas divisões verticais e horizontais serão consideradas para a setorização, respectivamente.

A quantidade de setores define quantas threads serão utilizadas.

Cada thread executará o mesmo método, este sendo `threadFunction`, que irá realizar o cálculo do fractal de acordo com o setor pelo qual a thread atual é responsável.

Ao calcular os pixels, cada um recebe a cor adequada devido a um array de cores previamente calculado.

Ao final, todos os setores, que são armazenados em um array, são gravados em um arquivo PPM sequencialmente, formando a imagem completa.

### Valores default, alteráveis em `MandelbrotFractal.h`

* `IMG_HEIGHT` = `5000`, define a altura da imagem PPM gerada
* `IMG_WIDTH` = `5000`, define a largura da imagem PPM gerada
* `DIV_HEIGHT` = `10`, define a quantidade de divisões verticais
* `DIV_WIDTH` = `10`, define a quantidade de divisões horizontais
* `MAX_ITERATIONS` = `100`, define quantas iterações serão feitas para cada setor, ou seja, para o plot do fractal

## Compilar e Executar

Para compilar, utilizar o comando:

```
	g++ ./src/*.cpp -I . -lpthread
```

Para executar, basta chamar via console o binário gerado pelo compilador:

```
	./a.out
```
