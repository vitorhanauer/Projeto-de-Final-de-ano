#ifndef __DEBOUNCE__H__
#define __DEBOUNCE__H__
#include <Arduino.h>

class deb
{

public:
    struct tempo
    {
        unsigned long marca;
        byte intervalo;
    };

    struct dadosDebounce
    {
        byte pino;
        byte fechado;
        byte leitura;
        byte estado;

        tempo temp;
    };

    struct dadosBotao
    {
        byte ultimaLeitura;
        bool pressionado;
        bool solto;
        byte estado;

        dadosDebounce dados;
    };
    void info(byte pino, byte intervalo, byte fechado, struct dadosBotao &p);
    bool pressionar(struct dadosBotao &p);
    bool soltar(struct dadosBotao &p);
    dadosBotao botao;

private:
    bool debounce(struct dadosDebounce &p);
    byte lerBotao(struct dadosBotao &p);
};

class Teclado : public deb
{

public:
    void iniTeclado();
    char varredura();

private:
};

deb::dadosBotao teclas[12];
const byte col[3] = {7, 8, 9};
const byte lin[4] = {10,11, 12, 13};

void Teclado::iniTeclado()
{
    byte j = 0;
    for (byte i = 0; i != 12; i++)
    {
        if (j == 3)
            j = 0;
        info(col[j], 50, 0, teclas[i]);
        j++;
    }
}
char Teclado::varredura()
{

    char valor = 0;

    digitalWrite(lin[0], 0);
    digitalWrite(lin[1], 1);
    digitalWrite(lin[2], 1);
    digitalWrite(lin[3], 1);

    if (pressionar(teclas[0]))
        return '1';
    if (pressionar(teclas[1]))
        return '2';
    if (pressionar(teclas[2]))
        return '3';

    digitalWrite(lin[0], 1);
    digitalWrite(lin[1], 0);
    digitalWrite(lin[2], 1);
    digitalWrite(lin[3], 1);

    if (pressionar(teclas[3]))
        return '4';
    if (pressionar(teclas[4]))
        return '5';
    if (pressionar(teclas[5]))
        return '6';

    digitalWrite(lin[0], 1);
    digitalWrite(lin[1], 1);
    digitalWrite(lin[2], 0);
    digitalWrite(lin[3], 1);

    if (pressionar(teclas[6]))
        return '7';
    if (pressionar(teclas[7]))
        return '8';
    if (pressionar(teclas[8]))
        return '9';

    digitalWrite(lin[0], 1);
    digitalWrite(lin[1], 1);
    digitalWrite(lin[2], 1);
    digitalWrite(lin[3], 0);

    if (pressionar(teclas[9]))
        return '*';
    if (pressionar(teclas[10]))
        return '0';
    if (pressionar(teclas[11]))
        return '#';

    return valor;
}

void deb::info(byte pino, byte intervalo, byte fechado, struct dadosBotao &p)
{
    p.dados.pino = pino;
    p.dados.fechado = fechado;
    p.dados.temp.intervalo = intervalo;
}

bool deb::debounce(struct dadosDebounce &p)
{
    p.leitura = digitalRead(p.pino);

    switch (p.estado)
    {

    case 0: // Aberto Firme
        if (p.leitura == p.fechado)
        {
            p.temp.marca = millis();
            p.estado = 1;
        }
        else
        {
            p.temp.marca = 0;
            p.estado = 0;
        }
        return false;
        break;

    case 1: // Aberto Transitando
        if (p.leitura == p.fechado && (millis() - p.temp.marca) > p.temp.intervalo)
        {
            p.estado = 2;
        }
        else if (p.leitura != p.fechado)
        {
            p.estado = 0;
        }
        return false;
        break;

    case 2: // Fechado Firme
        if (p.leitura != p.fechado)
        {
            p.temp.marca = millis();
            p.estado = 3;
        }
        else
        {
            p.temp.marca = 0;
            p.estado = 2;
        }
        return true;
        break;

    case 3: // Fechado Transitando
        if (p.leitura != p.fechado && (millis() - p.temp.marca) > p.temp.intervalo)
        {
            p.estado = 0;
        }
        else if (p.leitura == p.fechado)
        {
            p.estado = 2;
        }
        return true;
        break;
    }
}

byte deb::lerBotao(struct dadosBotao &p)
{

    byte leitura = debounce(p.dados);

    switch (p.estado)
    {

    case 0:
        if (leitura == 1)
        {
            p.pressionado = true;
            p.estado = 1;
        }
        return 0;
        break;

    case 1:
        if (leitura == 1)
            p.estado = 2;
        else
            p.estado = 3;
        return 1;
        break;

    case 2:
        if (leitura == 0)
        {
            p.estado = 3;
            p.solto = true;
        }
        return 1;
        break;

    case 3:
        if (leitura == 0)
            p.estado = 0;
        else
            p.estado = 1;
        return 3;
        break;
    }
}

bool deb::pressionar(struct dadosBotao &p)
{
    lerBotao(p);
    if (p.pressionado)
    {
        p.pressionado = false;
        return true;
    }
    else
        return false;
}

bool deb::soltar(struct dadosBotao &p)
{
    lerBotao(p);
    if (p.solto)
    {
        p.solto = false;
        return true;
    }
    else
        return false;
}

#endif //!__DEBOUNCE__H__